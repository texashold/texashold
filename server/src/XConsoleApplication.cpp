/*
*   File name: XConsoleApplication.cpp
*
*  Created on: 2012.10.09
*      Author: CaiZhaoDong
*     Version: 1.0
*  Contact me: cichiy08110@hotmail.com
* Description: Implementation of class XConsoleApplication
*/

#include "stdafx.h"
#include "XConsoleApplication.h"
#include "XGame.h"
#include "Base/XInvalidObjectPool.h"
#include "IO/XFileStream.h"
#include "GameController.h"
#include "Item/Item.h"
#include "Server/Server.h"

#if PLATFORM_IS_LINUX
#include <execinfo.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

static const float kMaxFPS       = 30.f;


XConsoleApplication::XConsoleApplication(const char *exePath, GameParam &gameParam)
{
	m_gameParam = gameParam;
	m_nFrameTimeInterval = 0;
	m_exit = false;

	XFileStream::InitPathUtil();

	InitModulePath(exePath, gameParam);
	InitDebugOut();
	InitWinSock();


	//Load item attribute
	Item::LoadAttribute();

	m_gameController = XL_NEW GameController(this);

	if(GameTypeNormal == gameParam.gameType && gameParam.roomType <= 5)
	{
		m_gameController->CreateOnlineGame(m_gameParam.roomType,
			m_gameParam.room.fastRoom != 0,
			m_gameParam.room.maxPlayer, 
			m_gameParam.room.maxCarry,
			m_gameParam.room.minCarry,
			m_gameParam.room.smallBlindBet * 2,
			m_gameParam.room.roomCode,
			m_gameParam.room.initAiPlayer,
			m_gameParam.room.smallestPoint);
		m_gameController->GetServer()->SetMuteType(m_gameParam.room.muteType);
		m_gameController->GetServer()->SetPreBet(m_gameParam.room.smallBlindBet * m_gameParam.room.preBet);
	}
	else if(GameTypeTournament == gameParam.gameType)
	{
		m_gameController->CreateOnlineGame(m_gameParam.roomType,
			true,
			m_gameParam.room.maxPlayer,
			m_gameParam.roomType < 100?5000000:10000000,
			m_gameParam.roomType < 100?5000000:10000000,
			m_gameParam.roomType < 100?(2*10000):(2*25000),
			m_gameParam.room.roomCode,
			0);
	}
	else
	{
		XLOG_ERROR("Create game failed,game type %d, room type %d, Exit", gameParam.gameType, gameParam.roomType);
		Exit();
	}

	// add XInvalidObjectPool as Application's child to enable its update to support delay-delete feature
	AddChild(XInvalidObjectPool::Instance());
}

XConsoleApplication::~XConsoleApplication()
{
	XL_DEL(m_gameController);

	FreeDebugOut();
	FreeWinSock();
}

const char *XConsoleApplication::GetDeviceLanguage() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceName() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceToken() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceIdentifier() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceMode() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceType() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceMAC() const
{
	return NULL;
}

const char *XConsoleApplication::GetDeviceOSVersion() const
{
	return NULL;
}

const char *XConsoleApplication::GetApplicationVersion() const
{
	return NULL;
}

const char *XConsoleApplication::GetApplicationName() const
{
	return "TexasPokerServer";
}

int XConsoleApplication::GetFPS() const
{
	return 0;
}

int XConsoleApplication::GetFTI() const
{
	return m_nFrameTimeInterval;
}

int XConsoleApplication::GetScreenWidth() const
{
	return 0;
}

int XConsoleApplication::GetScreenHeight() const
{
	return 0;
}

int XConsoleApplication::GetViewportWidth() const
{
	return 0;
}

int XConsoleApplication::GetViewportHeight() const
{
	return 0;
}

bool XConsoleApplication::OpenURL(const char *url)
{
	return false;
}

void XConsoleApplication::Update()
{
	static unsigned long nLastTimestamp = XTime::CurTimestamp();
	unsigned long nCurTimestamp = XTime::CurTimestamp();
	m_nFrameTimeInterval = nCurTimestamp - nLastTimestamp;
	nLastTimestamp = nCurTimestamp;

	if(m_nFrameTimeInterval < 0)
	{
		m_nFrameTimeInterval = 33;
	}

	XApplication::Update();
}

int XConsoleApplication::Run()
{
	// Main Run Loop
	unsigned long nTickCounter = XTime::CurTimestamp();
	unsigned long nLastTime    =  nTickCounter;
	int   nFrameCount  = 0;

	while (!m_exit)
	{
		unsigned long curTime = XTime::CurTimestamp();
		if (curTime <= (nLastTime + (1000.0f / kMaxFPS) * nFrameCount)
			&& curTime >= nLastTime)
		{
#if PLATFORM_IS_WIN32
			Sleep(10);
#elif PLATFORM_IS_LINUX
			usleep(10000);
#endif
			continue;
		}

		Update();

		curTime = XTime::CurTimestamp();
		if(nTickCounter + 1000 < curTime || nTickCounter >= curTime)
		{
			nTickCounter = curTime;
			nLastTime    = nTickCounter;
			m_nFPS       = nFrameCount;
			nFrameCount = 0;
		}
		else
		{
			nFrameCount++;
		}
	}

	XLOG_INFO("\n");
	XLOG_INFO("Run loop is over!");
	return 0;
}

void XConsoleApplication::Exit()
{
	m_exit = true;
}

void XConsoleApplication::InitModulePath(const char *exePath, GameParam &gameParam)
{
	strcpy(m_pLogPath, exePath);
	RemoveFileName(m_pLogPath);
	
	sprintf(m_gameRecordPath, "%s/gameRecord/", m_pLogPath);

	XTime time(XTime::CurMSTime());
	// chat path
	sprintf(m_chatLogPath, "%s/chatLog/%d%02d%02d/", m_pLogPath, time.GetYear(), time.GetMonth(), time.GetDay());
	sprintf(m_voiceLogPath, "%s/voiceLog/%d%02d%02d/", m_pLogPath, time.GetYear(), time.GetMonth(), time.GetDay());
	// log path
	strcat(m_pLogPath, "Log/");
	// ensure the directory exist
	CreateFolder(m_gameRecordPath);
	CreateFolder(m_pLogPath);
	CreateFolder(m_chatLogPath);
	CreateFolder(m_voiceLogPath);

	//add file name
	char seq_id[64] = "";
	sprintf(seq_id, "%d_%d_%d%02d%02d_%02d%02d%02d.log", 
		gameParam.room.roomCode,
		gameParam.roomType,
		time.GetYear(), 
		time.GetMonth(),
		time.GetDay(),
		time.GetHour(),
		time.GetMinute(),
		time.GetSecond());
	strcat(m_pLogPath, seq_id);
}

void XConsoleApplication::ReportCrash()
{
	char pCrashLogPath[512] = "";
	sprintf(pCrashLogPath, m_pLogPath);
	int index = RemoveFileName(pCrashLogPath);
	strcat(pCrashLogPath, "crash_log/");
	// ensure the directory exist
	CreateFolder(pCrashLogPath);
	// add file name
	strcat(pCrashLogPath, m_pLogPath+index);

	//copy
	FILE *fsrc = fopen(m_pLogPath, "rb");
	if(fsrc == NULL)
	{
		XLOG_ERROR("Can't find log file to copy");
		return;
	}

	FILE *fdst = fopen(pCrashLogPath, "wb");
	if(fdst == NULL)
	{
		XLOG_ERROR("Can't open crash report file to record!");
		fclose(fsrc);
		return;
	}

	while (char c = fgetc(fsrc))
	{
		if(c == EOF)
		{
			break;
		}
		fputc(c, fdst);
	}


	fclose(fsrc);
	fclose(fdst);
}


void XConsoleApplication::PrintCrash()
{
	
	//crash trace
#if PLATFORM_IS_LINUX
	const int MAX_CALLSTACK_DEPTH = 32;    /* ��Ҫ��ӡ��ջ�������� */  
	void *traceback[MAX_CALLSTACK_DEPTH];  /* �����洢���ö�ջ�еĵ�ַ */  
	/* ���� addr2line ������Դ�ӡ��һ��������ַ���ڵ�Դ����λ��   
	* ���ø�ʽΪ�� addr2line -f -e /tmp/a.out 0x400618  
	* ʹ��ǰ��Դ�������ʱҪ���� -rdynamic -g ѡ��  
	*/  
//	char cmd[512] = "addr2line -f -e ";   
//	char *prog = cmd + strlen(cmd);   
	/* �õ���ǰ��ִ�г����·�����ļ��� */  
//	int r = readlink("/proc/self/exe",prog,sizeof(cmd)-(prog-cmd)-1);   
	/* popen��fork��һ���ӽ���������/bin/sh, ��ִ��cmd�ַ����е����  
	* ͬʱ���ᴴ��һ���ܵ������ڲ�����'w', �ܵ������׼���������ӣ�  
	* ������һ��FILE��ָ��fpָ���������Ĺܵ����Ժ�ֻҪ��fp��������д�κ����ݣ�  
	* ���ݶ��ᱻ��������׼���룬  
	* ������Ĵ����У��Ὣ���ö�ջ�еĺ�����ַд��ܵ��У�  
	* addr2line�����ӱ�׼�����еõ��ú�����ַ��Ȼ����ݵ�ַ��ӡ��Դ����λ�úͺ�������  
	*/  

//	FILE *fp = popen(cmd, "w"); 
	/* �õ���ǰ���ö�ջ�е����к�����ַ���ŵ�traceback������ */  
	int depth = backtrace(traceback, MAX_CALLSTACK_DEPTH);   
	for (int i = 0; i < depth; i++)   
	{   
		/* �õ����ö�ջ�еĺ����ĵ�ַ��Ȼ�󽫵�ַ���͸� addr2line */  
		//fprintf(fdst, "%p\n", traceback[i]);   
		XLOG_ERROR("%p", traceback[i]);
		/* addr2line �������յ���ַ�󣬻Ὣ������ַ���ڵ�Դ����λ�ô�ӡ����׼��� */  
	}  
//	pclose(fp); 
#endif


}

void XConsoleApplication::InitDebugOut()
{
	XLog::Create();
#ifdef DEBUG_SELF_LAUNCH
	return;
#endif

	if(XLog::Instance()->Redirect(m_pLogPath))
	{
		XLOG_INFO("Create log file OK!");
	}
	else
	{
		XLOG_INFO("Create log file Failed!");
	}
}

void XConsoleApplication::FreeDebugOut()
{
	XLog::Destroy();
}

void XConsoleApplication::InitWinSock()
{
#if PLATFORM_IS_WIN32
	WSADATA wsaData;

	// Initialize Winsock.
	WSAStartup( MAKEWORD(2,2), &wsaData );
#endif
}

void XConsoleApplication::FreeWinSock()
{
#if PLATFORM_IS_WIN32
	WSACleanup();
#endif
}

int XConsoleApplication::RemoveFileName(char *path)
{
	size_t index = strlen(path) - 1;
	while(index > 0 && path[index] != '\\' && path[index] != '/')
	{
		index--;
	}

	path[index + 1] = 0;
	return index + 1;
}

void XConsoleApplication::CreateFolder(const char *path)
{
#if PLATFORM_IS_WIN32
	char cmd[1024];
	sprintf(cmd, "if not exist %s md %s", path, path);
	//replace / with "\"
	for(int i = 0; cmd[i] != 0; i++)
	{
		if(cmd[i] == '/')
		{
			cmd[i] = '\\';
		}
	}
	system(cmd);
#elif PLATFORM_IS_LINUX
	//Check the dir exist
	char directory[512] = {0};
	int len = 1;
	while(true)
	{
		if(path[len] == '\0' || path[len] == '\\' || path[len] == '/')
		{
			strncpy(directory, path, len);
			if(access(directory, F_OK) != 0)
			{
				if(mkdir(directory, S_IRWXU) != 0)
				{
#ifndef DEBUG_STRESS_TEST
					XLOG_ERROR("Could not create directory %s", directory);
#endif
				}
			}

			if(path[len] == '\0')
			{
				break;
			}
		}
		len++;
	}
#endif
}

const GameParam &XConsoleApplication::GetGameParam() const
{
	return m_gameParam;
};

GameController *XConsoleApplication::GetGameController()
{
	return m_gameController;
}

int XConsoleApplication::GetGameType() const
{
	return m_gameParam.gameType;
}