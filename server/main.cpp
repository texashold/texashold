#include "stdafx.h"
#include "XConsoleApplication.h"
#include "GameController.h"

#if PLATFORM_IS_LINUX
#include <signal.h>
void signal_handler(int sig) 
{ 
	XLOG_ERROR("\n\n*********** SIGNAL HANDLER *************");
	switch(sig)
	{
	case SIGILL:
		XLOG_ERROR("SIGILL received.");
		break;
	case SIGABRT:
		XLOG_ERROR("SIGABRT received.");
		break;
		//	case SIGIOT:
		//		XLOG_ERROR("SIGIOT received.");
		//		break;
	case SIGBUS:
		XLOG_ERROR("SIGBUS received.");
		break;
	case SIGFPE:
		XLOG_ERROR("SIGFPE received.");
		break;
	case SIGSEGV:
		XLOG_ERROR("SIGSEGV received.");
		break;
	default:
		XLOG_ERROR("Unknown signal received, ignore it!!!");
		return;
	}

	XLOG_ERROR("Cancel game.");

	//cancel the game
	XConsoleApplication *app = (XConsoleApplication*)XApplication::Instance();
	//add to crash report
	app->PrintCrash();
	app->ReportCrash();
// 	if(app)
// 	{
// 		if(app->GetGameController())
// 		{
// 			app->GetGameController()->Close();
// 			app->Run();
// 		}
// 	}

	exit(sig);
} 
#endif

bool ParseParam(const char *format, GameParam &gameParam)
{
	//memset(&gameParam, 0, sizeof(GameParam));

	if(sscanf(format, "game_type=%d," ,&gameParam.gameType) == 1)
	{
		//Find ,
		while(*format != ',')
		{
			if(*format == '\0')
			{
				return false;
			}
			format++;
		}

		//Skip ,
		format++;
	}
	else
	{
		gameParam.gameType = GameTypeNormal;
	}

	if(gameParam.gameType != GameTypeNormal && gameParam.gameType != GameTypeTournament)
	{
		return false;
	}

	if (GameTypeNormal == gameParam.gameType)
	{
		int argc = sscanf(format, "room_type=%d,room_code=%d,small_blind=%d,fast=%d,exp=%d,max_player=%d,min_carry=%d,max_carry=%d,ai_player=%d,mute_type=%d,must=%d,smallest_point=%d",
			&gameParam.roomType,
			&gameParam.room.roomCode,
			&gameParam.room.smallBlindBet,
			&gameParam.room.fastRoom,
			&gameParam.room.exp,
			&gameParam.room.maxPlayer,
			&gameParam.room.minCarry,
			&gameParam.room.maxCarry,
			&gameParam.room.initAiPlayer,
			&gameParam.room.muteType,
			&gameParam.room.preBet,
			&gameParam.room.smallestPoint);
		if(argc >= 9)
		{
			return gameParam.roomType <= 5;
		}
	}
	else if (GameTypeTournament == gameParam.gameType)
	{
		int argc = sscanf(format, "room_type=%d,room_code=%d,max_player=%d", &gameParam.roomType, &gameParam.room.roomCode, &gameParam.room.maxPlayer);
		if(argc >= 2)
		{
			return true;
		}
	}

	return false;
}

char g_pLaunchPath[512] = "";

int main(int argc, char* argv[])
{
#if PLATFORM_IS_LINUX
	signal(SIGILL, &signal_handler);
	signal(SIGABRT, &signal_handler);
	signal(SIGIOT, &signal_handler);
	signal(SIGBUS, &signal_handler);
	signal(SIGFPE, &signal_handler);
	signal(SIGSEGV, &signal_handler);
	signal(SIGPIPE,SIG_IGN);
#endif

	if(argc < 2)
	{
		printf("No parameter input. Exit.\n");
#if PLATFORM_IS_WIN32
		system("pause");
#endif
		return -1;
	}

	GameParam  gameParam;
	if(!ParseParam(argv[1], gameParam))
	{
		printf("Parse parameter failed:%s\n Exit.\n", argv[1]);
		return -2;
	}

	//Initialize launch path
	strcpy(g_pLaunchPath, argv[0]);
	size_t pos = strlen(g_pLaunchPath);

	while(pos > 0)
	{
		pos--;
		if(g_pLaunchPath[pos] == '/' || g_pLaunchPath[pos] == '\\')
		{
			break;
		}
	}
	g_pLaunchPath[pos+1] = '\0';

	XConsoleApplication app(argv[0], gameParam);
	app.Run();

	XLOG_INFO("\n");
	XLOG_INFO("Normal exit!");
#ifdef DEBUG_SELF_LAUNCH
	system("pause");
#endif
	return 0;
}