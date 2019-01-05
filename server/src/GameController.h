#ifndef _GAME_CONTROLLER_H_
#define _GAME_CONTROLLER_H_

#include <map>
#include "Connection/XConnectionObserver.h"
#include "Online/Common/OnlineObserver.h"
#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

enum GameType
{
	GameTypeNormal = 0,
	GameTypeTournament,
};

enum NoticeType
{
	NoticeTypeHundredMillion = 1,
	NoticeTypeMoreThan6AllIn,
	NoticeType400TimesBigBlind,
	NoticeTypeWinWithFourOfAKind,
	NoticeTypeWinWithStraightFlush,
	NoticeTypeWinWithRoyalFlush
};

struct NoticeDetail
{
	NoticeDetail()
	{
		Reset();
	}

	void Reset()
	{
		type = 0;
		userId = 0;
		param1 = 0;
		param2 = 0;
	}
	int type;
	int userId;
	long long param1;
	int param2;
};
struct TournamentResult;
struct GameClearInfo;
class XConnection;
class Server;
class XSocketConnection;
class OnlineGetTotalGamePlayerNum;
class GameController : public XObject, public XConnectionObserver, public OnlineObserver
{
public:
	enum GameMode
	{
		GAME_MODE_NONE,
		GAME_MODE_SINGLE_PLAYER,
		GAME_MODE_ONLINE_GAME,
	};

	GameController(XObject *parent = NULL);
	~GameController();

	void         CreateOnlineGame(int roomLevel, bool fastRoom, int maxPlayerNum, int maxCarryMoney, int minCarryMoney, int minBetMoney, int roomCode, int AIPlayerNum, int smallestPoint = 2);
	
	Server      *GetServer() const;
	int			 GetMode() const;

	void         Update();

	//Interface from XConnectionObserver
	virtual int  OnDataReceived(XConnection *con, XMemoryBuffer &recvData);
	virtual void OnStateChanged(XConnection *con, int oldState, int newState);

	virtual void OnRequestSuccess(OnlineComponent *onlineComponent, int func_id );
	virtual void OnRequestFailed(OnlineComponent *onlineComponent, int func_id, int error_code );

	void         Close(int delay = 0);

	//Request to web server
	void         SendConfirmPresentChip(int from, int to, int chip);
	void         SendConfirmPlayerSit(int player, int seat, tChip playerChip, int inviter, const char *clientIp, const char *session, int hasSeat = 0);
	void         SendNotifyPlayerReSit(int player, tChip playerChip, int betChip);//notify java server to refresh ChipInGameCache
	void         SendConfirmPlayerStand(int player, tChip bettedChip, bool holdPlace = false);
	void         SendGetPlayerItem(int player);
	void		 SendGameClear(GameClearInfo *gameClearInfo);
	void         SendGetInGameTaskInfo(int playerID);
	void         SendKickPlayer(int srcID, int dstID, const char *srcName, const char *dstName);
	void         SendLockRoom(int playerID, int roomCode);
	void         SendUnlockRoom(int playerID, int roomCode);
	void		 SendTournamentSit(int playerID);
	void		 SendTournamentExit(int playerID);
	void		 SendTournamentStart(const char *players);
	void		 SendTournamentClear(int gameCount, std::vector<TournamentResult> &results);	//player eliminated
	void		 SendTournamentClear(int gameCount, int playerID, int rank);					//player exit
	void		 SendUpdatePlayerNum(const char *players);
	void		 SendUpdatePlayerInfo(int playerId);
	void		 SendChangeDealer(int playerId, const char *playerName, int dealerId, tChip gameChip);
	void		 SendTranslate(int from, const char* keyfrom, const char* key, const char *text);

	void         OnScheduleTimeout(int tid, int iParam, void *pParam, int overTime);

	void         RunRoomAI();

private:
	enum SCHEDULE_TASK{
		SCHEDULE_TASK_KEEP_ALIVE,
		SCHEDULE_TASK_CLOSE_IDLE_ROOM,
		SCHEDULE_TASK_AI_JOIN,
		SCHEDULE_TASK_CLOSE_ROOM,
		SCHEDULE_TASK_CHECK_1_AI_PLAYER,
		SCHEDULE_TASK_KICK_AI_PLAYER,
		SCHEDULE_TASK_RETRY_GAME_CLEAR,
		SCHEDULE_TASK_RETRY_CREATE_CONFIRM,
		SCHEDULE_TASK_DELAY_CLOSE,
	};

	void         ExitApp();
	void         ScheduleAIJoin(int timeDelay);

	void         HandleWebPushInvite(const Json::Value &json);
	void         HandleWebTournamentInvite(const Json::Value &json);
	void         HandleWebPushKickPlayer(const Json::Value &json);
	void         HandleWebPrivateNotice(const Json::Value &json);
	void         HandleWebSurprise(const Json::Value &json);

	Server      *m_server;

	GameMode     m_gameMode;

	XSocketConnection *m_listenConnection;
	XSocketConnection *m_webSrvConnection;
	unsigned short     m_listenPort;
	unsigned short     m_udpPort;

	//Keep ALive
	int          m_roomCode;
	int          m_roomLevel;
	int          m_bigBlindBet;
	int          m_keepAliveRetry;

	union RequestInfo
	{
		struct PlayerSitConform
		{
			int playerID;
			int seat;
			tChip playerChip;
		}playerSitInfo;

		struct PresentChipInfo
		{
			int src;
			int dest;
			int chip;
		}presentChipInfo;
	};

	std::map<OnlineComponent*, RequestInfo> m_requestInfoMap;

	//AI player
	int         m_aiPlayerToJoin;

	int         m_totalNeedGamePlayer;
	xtime_t     m_lastUpdateNeedPlayerNumTime;
	OnlineGetTotalGamePlayerNum *m_requetGetTGPN;

	//GameClear Retry
	std::string    m_gameClearUrl;
	NoticeDetail   m_noticeDetail;
	int            m_gameClearRetryCount;

	//Create room retry
	int            m_startConfirmRetry;
};

#endif