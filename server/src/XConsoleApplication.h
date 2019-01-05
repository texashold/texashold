/*
*   File name: XConsoleApplication.h
*
*  Created on: 2012.10.09
*      Author: CaiZhaoDong
*     Version: 1.0
*  Contact me: cichiy08110@hotmail.com
* Description: Win32 specified XApplication
*/

#ifndef _X_CONSOLE_APPLICATION_H_
#define _X_CONSOLE_APPLICATION_H_

#include "XApplication.h"

#define K_DEFAULT_HTTP_SERVER_ADDRESS		  "http://127.0.0.1:8081/TexasPoker/"


//parameter for room
struct RoomParam
{
	int roomCode;
	int maxPlayer;
	int smallBlindBet;
	int fastRoom;
	int exp;
	int minCarry;
	int maxCarry;
	int initAiPlayer;
	int muteType;
	int preBet;
	int smallestPoint;
	RoomParam()
	{
		maxPlayer = 9;
		muteType = 0;
		preBet = 0;
		smallestPoint = 2;
	}
};


struct GameParam
{
	int gameType;		// 0: normal game, 1: tournament game
	int roomType;		// room level
	RoomParam room;
};

class GameController;
class XConsoleApplication : public XApplication
{
public:
	XConsoleApplication(const char *exePath, GameParam &gameParam);
	virtual ~XConsoleApplication();

	//General interface
	virtual const char *GetDeviceLanguage() const;
	virtual const char *GetDeviceName() const;
	virtual const char *GetDeviceToken() const;
	virtual const char *GetDeviceIdentifier() const;
	virtual const char *GetDeviceMode() const;
	virtual const char *GetDeviceType() const;
	virtual const char *GetDeviceMAC() const;
	virtual const char *GetDeviceOSVersion() const;

	//Application info
	virtual const char *GetApplicationVersion() const;
	virtual const char *GetApplicationName() const;
	virtual int         GetFPS() const;
	virtual int         GetFTI() const;
	virtual int         GetScreenWidth() const;
	virtual int         GetScreenHeight() const;
	virtual int         GetViewportWidth() const;
	virtual int         GetViewportHeight() const;

	//Application operation
	virtual bool        OpenURL(const char *url);

	//Interface from XObject
	virtual void Update();

	//Platform specified interface
	void InitModulePath(const char *exePath, GameParam &gameParam);
	void InitDebugOut();
	void FreeDebugOut();
	void InitWinSock();
	void FreeWinSock();
	void ReportCrash();
	void PrintCrash();

	int  Run();
	void Exit();

	const GameParam    &GetGameParam() const;
	GameController     *GetGameController();

	int GetGameType() const;

	void				CreateFolder(const char *path);
	const char*			GetGameRecordPath() { return m_gameRecordPath; }
	const char*			GetChatLogPath() { return m_chatLogPath; }
	const char*			GetVoiceLogPath() { return m_voiceLogPath; }
private:
	int         RemoveFileName(char *path);

	//FPS
	int          m_nFPS;
	int          m_nFrameTimeInterval;


	GameParam       m_gameParam;
	GameController *m_gameController;

	char         m_pLogPath[512];
	char         m_gameRecordPath[512];
	char         m_chatLogPath[512];
	char         m_voiceLogPath[512];

	//Exit
	bool           m_exit;
};

#endif