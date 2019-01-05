#include "stdafx.h"
#include "GameController.h"
#include "Connection/XSocketConnectionTCP.h"
#include "Connection/XSocketConnectionUDP.h"
#include "Connection/XNullConnection.h"
#include "Server/Server.h"
#include "XConsoleApplication.h"
#include "OnlineStartConfirm.h"
#include "OnlineCloseRoom.h"
#include "OnlinePlayerSit.h"
#include "OnlinePlayerStand.h"
#include "OnlineKeepAlive.h"
#include "OnlineGameClear.h"
#include "OnlineSelectAIPlayerSit.h"
#include "OnlineGetTotalGamePlayerNum.h"
#include "OnlineGetPlayerItem.h"
#include "OnlineGiveChip.h"
#include "OnlineGetIGTInfo.h"
#include "CSCommonDefine.h"
#include "IO/XMemoryStream.h"
#include "MessageDefine.h"
#include "OnlineKickPlayer.h"
#include "OnlineLockRoom.h"
#include "OnlineUnlockRoom.h"
#include "Server/ServerPlayer.h"
#include "OnlineTournamentSit.h"
#include "OnlineTournamentExit.h"
#include "OnlineTournamentStart.h"
#include "OnlineTournamentClear.h"
#include "OnlineUpdatePlayerNum.h"
#include "OnlineUpdatePlayerInfo.h"
#include "OnlineChangeDealer.h"
#include "OnlineTranslate.h"

#define K_MAX_IDLE_TIME                       (1 * 60 * 1000)
#define K_KEEP_ALIVE_TIME_INTERVAL            (30 * 1 * 1000)

static const int K_MAX_GAME_CLEAR_RETRY_TIME	= 5;

GameController::GameController(XObject *parent)
: XObject(parent)
{
	m_server           = NULL;
	//m_client         = NULL;
	m_gameMode         = GAME_MODE_NONE;

	m_listenConnection = NULL;
	m_webSrvConnection = NULL;
	m_listenPort       = 0;
	m_udpPort          = 0;

	m_roomCode         = 0;

	m_aiPlayerToJoin   = 0;

	m_totalNeedGamePlayer = 0;
	m_lastUpdateNeedPlayerNumTime = 0;
	m_requetGetTGPN    = NULL;
	
	m_gameClearRetryCount = 0;
	m_keepAliveRetry = 0;
	m_startConfirmRetry = 0;

	ScheduleTask(SCHEDULE_TASK_CLOSE_IDLE_ROOM, K_MAX_IDLE_TIME);
}

GameController::~GameController()
{

}

void GameController::CreateOnlineGame(int roomLevel, bool fastRoom, int maxPlayerNum, int maxCarryMoney, int minCarryMoney, int minBetMoney, int roomCode, int AIPlayerNum, int smallestPoint)
{
	m_gameMode = GAME_MODE_ONLINE_GAME;
	m_roomCode = roomCode;
	m_roomLevel = roomLevel;
	m_bigBlindBet = minBetMoney;

	// Create listen connection
	m_listenConnection = XL_NEW XSocketConnectionTCP(this);
	m_listenConnection->RegisterObserver(this);

	m_webSrvConnection = XL_NEW XSocketConnectionUDP(this);
	m_webSrvConnection->RegisterObserver(this);

#ifdef DEBUG_SELF_LAUNCH
	m_listenPort = 7890;
#else
	m_listenPort = 0;
#endif
	m_listenPort = m_listenConnection->CreateAsServer(m_listenPort);
	m_udpPort    = m_webSrvConnection->CreateAsServer(0);

	XLOG_INFO("Start room server ok, listening at port:%d, and udp at %d", m_listenPort, m_udpPort);

	// Create server
	m_server = XL_NEW Server(this);

	m_server->SetMaxPlayerNum(maxPlayerNum);
	m_server->SetMaxCarryMoney(maxCarryMoney);
	m_server->SetMinCarryMoney(minCarryMoney);
	m_server->SetMinBetMoney(minBetMoney);
	m_server->SetRoomCode(roomCode);
	m_server->SetRoomLevel(roomLevel);
	m_server->SetFastRoom(fastRoom);
	m_server->SetSmallestPoint(smallestPoint);

	// Start server
	m_server->Start();

	m_aiPlayerToJoin = XMath::Min(AIPlayerNum, maxPlayerNum);

	// Confirm
	OnlineStartConfirm::CreateRequest(this)->Send(roomCode, m_listenPort, m_udpPort);
}

Server* GameController::GetServer() const
{
	return m_server;
}

//Client* GameController::GetClient() const
//{
//	return m_client;
//}

int GameController::GetMode() const
{
	return m_gameMode;
}

void GameController::Update()
{
	XObject::Update();
}

void GameController::Close(int delay)
{
	if (delay > 0)
	{
		XLOG_INFO("Close room delay %d.", delay);
		ScheduleTask(SCHEDULE_TASK_DELAY_CLOSE, delay);
		return;
	}

	XLOG_INFO("Cancel Game...");
	OnlineCloseRoom::CreateRequest(this)->Send(m_roomCode);

}

void GameController::ExitApp()
{
	XConsoleApplication *app = (XConsoleApplication*)XApplication::Instance();
	app->Exit();
}

int GameController::OnDataReceived(XConnection *con, XMemoryBuffer &recvData)
{
	if(con == m_webSrvConnection)
	{
		static const int HEADER_SIZE = sizeof(short); // 2
		if(recvData.GetDataSize() >= HEADER_SIZE)
		{
			short totalData = 0;
			memcpy(&totalData, recvData.GetData(), HEADER_SIZE);
			totalData = XMath::RevertByteOrder(totalData);

			if(totalData <= 0 || recvData.GetDataSize() < totalData)
			{
				return 0;
			}

			char *pJsonStr = XL_NEW char[totalData];
			memcpy(pJsonStr, (const char*)recvData.GetData() + HEADER_SIZE, totalData - HEADER_SIZE);
			pJsonStr[totalData - HEADER_SIZE] = 0;

			XLOG_INFO("Recv web server request!");
			XLOG_INFO("Content:%s", pJsonStr);

			Json::Reader json_reader;
			Json::Value  json_value;

			if(json_reader.parse(pJsonStr, json_value))
			{
				if(json_value.isObject())
				{
					Json::Value json = json_value["id"];
					if(json.isString())
					{
						if(json.asString().compare("invite") == 0)
						{
							HandleWebPushInvite(json_value);
						}
						else if(json.asString().compare("tournament_invite") == 0)
						{
							HandleWebTournamentInvite(json_value);
						}
						else if(json.asString().compare("kick") == 0)
						{
							HandleWebPushKickPlayer(json_value);
						}
						else if(json.asString().compare("notice") == 0)
						{
							HandleWebPrivateNotice(json_value);
						}
						if(json.asString().compare("surprise") == 0)
						{
							HandleWebSurprise(json_value);
						}
					}
					else
					{
						XLOG_ERROR("No value for key \"id\"!");
					}
				}
				else
				{
					XLOG_ERROR("Json value is not a object!");
				}
			}
			else
			{
				XLOG_ERROR("Not Json String!");
			}
			XL_DEL_ARRAY(pJsonStr);

			recvData.RemoveData(totalData);
		}
	}
	return 1;
}

void GameController::OnStateChanged(XConnection *con, int oldState, int newState)
{
	if( newState == XSocketConnection::XCONSTATE_RECVCONNECTREQUEST)
	{
		XLOG_INFO("Client connection request received.");
		XSocketConnection *connection = m_listenConnection->Accept();
		if(connection == NULL)
		{
			XLOG_ERROR("Client connection create failed!");
			return;
		}

		if(m_gameMode == GAME_MODE_ONLINE_GAME)
		{
			m_server->AddConnection(connection);

			XLOG_INFO("Server children count %d", m_server->GetChildrenCount());
			XLOG_INFO("Client connected.");

			//disable idle time counter
			RemoveTask(SCHEDULE_TASK_CLOSE_IDLE_ROOM);
		}
	}
}

void GameController::OnRequestSuccess(OnlineComponent *onlineComponent, int func_id )
{
	switch(func_id)
	{
	case OnlineFunctionStartConfirm:
		XLOG_INFO("Start confirm OK!");
		// Start Keep Alive
		ScheduleTask(SCHEDULE_TASK_KEEP_ALIVE, K_KEEP_ALIVE_TIME_INTERVAL);

		//Schedule AI player to join
		if(m_aiPlayerToJoin > 0)
		{
			ScheduleAIJoin(XMath::Random(5000, 10000));
			m_aiPlayerToJoin--;
		}
		m_startConfirmRetry = 0;
		break;
	case OnlineFunctionKeepAlive:
		XLOG_INFO("Keep ALive OK!");
		m_keepAliveRetry = 0;
		ScheduleTask(SCHEDULE_TASK_KEEP_ALIVE, K_KEEP_ALIVE_TIME_INTERVAL);
		//Send notice
		{
			OnlineKeepALive *req = (OnlineKeepALive*)onlineComponent;
			if(!req->GetResponse().notice.empty())
			{
				m_server->SendNotice(-1, req->GetResponse().notice.c_str());
			}
		}
		break;
	case OnlineFunctionCloseRoom:
		XLOG_INFO("Close room OK!");
		ExitApp();
		break;
	case OnlineFunctionPlayerSit:
		{
			std::map<OnlineComponent*, RequestInfo>::iterator it = m_requestInfoMap.find(onlineComponent);
			if(it != m_requestInfoMap.end())
			{
				const OnlineRespPlayerSit &onlineResp = ((OnlinePlayerSit *)onlineComponent)->GetResponse();
				if(onlineResp.m_hasSeat == 2)//for reconnect
				{
                              m_server->PlayerReconnectSit(it->second.playerSitInfo.playerID);
                              XLOG_INFO("Player %d reconnect sit confirm OK!", it->second.playerSitInfo.playerID);
				}
				else
				{
					std::vector<ServerItem> items;
					std::map<int, int>::const_iterator itemIt = onlineResp.items.begin();
					for(; itemIt != onlineResp.items.end(); ++itemIt)
					{
						ServerItem item;
						item.itemID = itemIt->first;
						item.extraChipLimit = itemIt->second;
						items.push_back(item);
					}

					m_server->PlayerSit(it->second.playerSitInfo.playerID, 
						it->second.playerSitInfo.seat ,
						0,
						it->second.playerSitInfo.playerChip, 
						onlineResp.m_maxHand,
						onlineResp.m_playerName,
						onlineResp.m_equipeAvatar,
						onlineResp.m_cardPower,
						onlineResp.m_skin,
						onlineResp.m_equipeTitle,
						items,
						onlineResp.m_inGameTaskID,
						onlineResp.m_inGameTaskTime,
						false,
						onlineResp.m_sex);
					XLOG_INFO("Player %d sit confirm OK!", it->second.playerSitInfo.playerID);
					m_server->UpdatePlayerNum();
				}
				m_requestInfoMap.erase(it);

				RunRoomAI();
			}
			else
			{
				XLOG_WARNING("Unknown player sit confirm ok!");
			}
		}
		break;
	case OnlineFunctionPlayerStand:
		XLOG_INFO("Player stand up confirm OK!");
		m_server->UpdatePlayerNum();
		RunRoomAI();
		break;
	case OnlineFunctionGameClear:
		{
			XLOG_INFO("Game clear OK!");
			m_gameClearRetryCount = 0;
			OnlineGameClear *gameclear = (OnlineGameClear*)onlineComponent;
			m_server->GameClear(&gameclear->GetResponse().playerChip);
			RunRoomAI();
		}
		break;
	case OnlineFunctionSelectAIPlayerSit:
		{
			const OnlineRespSelectAIPlayerSit &onlineResp = ((OnlineSelectAIPlayerSit *)onlineComponent)->GetResponse();
			XLOG_INFO("Select ai player %d join the room.", onlineResp.m_aiid);
			if(!m_server->AddAIPlayer(XL_NEW XNullConnection(this), onlineResp.m_aiid, onlineResp.m_aichip, onlineResp.m_maxHand))
			{
				XLOG_ERROR("Add ai player failed!");
				OnlinePlayerStand::CreateRequest(this)->Send(m_roomCode, onlineResp.m_aiid, 0, false);
			}
			else
			{
				m_server->PlayerSit(onlineResp.m_aiid, 
					-1, 
					0,
					XMath::Minl(onlineResp.m_aichip, m_server->GetMaxCarryMoney()),
					onlineResp.m_maxHand,
					onlineResp.m_playerName,
					onlineResp.m_avatar,
					onlineResp.m_cardPower,
					0,
					onlineResp.m_title);
				RemoveTask(SCHEDULE_TASK_CLOSE_IDLE_ROOM);

				if(m_aiPlayerToJoin > 0)
				{
					ScheduleAIJoin(XMath::Random(5000, 10000));
					m_aiPlayerToJoin--;
				}
				else
				{
					RunRoomAI();
				}
			}
		}
		break;
	case OnlineFunctionGetTotalGamePlayerNum:
		XLOG_INFO("Get Total game player state OK!");
		m_lastUpdateNeedPlayerNumTime = XTime::CurMSTime();
		m_totalNeedGamePlayer = m_requetGetTGPN->GetResponse();
		m_requetGetTGPN = NULL;
		RunRoomAI();
		break;
	case OnlineFunctionGiveChip:
		{
			std::map<OnlineComponent*, RequestInfo>::iterator it = m_requestInfoMap.find(onlineComponent);
			if(it != m_requestInfoMap.end())
			{
				m_server->PlayerGiveChip(it->second.presentChipInfo.src, 
					it->second.presentChipInfo.dest,
					it->second.presentChipInfo.chip);
				m_requestInfoMap.erase(it);
			}
			else
			{
				XLOG_WARNING("Unkown response for give chip request!");
			}
		}
		break;
	case OnlineFunctionGetPlayerItem:
		{
			XLOG_INFO("Get player item info success!");
			const OnlineRespGetPlayerItem &resp = ((OnlineGetPlayerItem *)onlineComponent)->GetResponse();
			std::vector<ServerItem> items;
			std::map<int, int>::const_iterator it = resp.items.begin();
			for(; it != resp.items.end(); ++it)
			{
				ServerItem item;
				item.itemID = it->first;
				item.extraChipLimit = it->second;
				items.push_back(item);
			}
			m_server->SetPlayerItem(onlineComponent->GetIParam1(), items);
		}
		break;
	case OnlineFunctionGetIGTInfo:
		{
			XLOG_INFO("Get in game task info success!");
			OnlineGetIGTInfo *reqt = (OnlineGetIGTInfo*)onlineComponent;
			m_server->PlayerSitResponse(onlineComponent->GetIParam1(), 0, reqt->GetResponse().m_inGameTaskID,
				reqt->GetResponse().m_inGameTaskTime);
			break;
		}
	case OnlineFunctionKickPlayer:
		{
			XLOG_INFO("Kick Player success!");
			OnlineKickPlayer *reqt = (OnlineKickPlayer*)onlineComponent;
			m_server->KickPlayer(MSG_NO_ERROR, reqt->GetIParam1(), reqt->GetIParam2(),
				reqt->GetSrcName(), reqt->GetDstName());
			break;
		}
	case OnlineFunctionLockRoom:
		{
			XLOG_INFO("Lock room success!");
			m_server->LockRoom(MSG_NO_ERROR, onlineComponent->GetIParam1());
			break;
		}
	case OnlineFunctionUnlockRoom:
		{
			XLOG_INFO("Unlock room success!");
			m_server->UnlockRoom(MSG_NO_ERROR, onlineComponent->GetIParam1());
			break;
		}
	case OnlineFunctionTournamentSit:
		{
			const OnlineRespTournamentSit &onlineResp = ((OnlineTournamentSit *)onlineComponent)->GetResponse();
			m_server->TournamentSit(onlineComponent->GetIParam1(), 
				0,
				onlineResp.m_name, 
				onlineResp.m_title,
				onlineResp.m_avatar,
				onlineResp.m_maxHand,
				onlineResp.m_skin,
				onlineResp.m_sex,
				onlineResp.m_itemID,
				onlineResp.m_vipID);
			XLOG_INFO("Tournament sit success, player id %d!", onlineComponent->GetIParam1());
		}
		break;
	case OnlineFunctionTournamentExit:
		XLOG_INFO("Player exit tournament game ok!");
		break;
	case OnlineFunctionTournamentStart:
		XLOG_INFO("Tournament game start ok, room code %d", m_roomCode);
		m_server->StartNewGame();
		break;
	case OnlineFunctionTournamentClear:
		{
			m_gameClearRetryCount = 0;
			const OnlineRespTournamentClear &onlineResp = ((OnlineTournamentClear *)onlineComponent)->GetResponse();
			m_server->TournamentClear(onlineResp.profits);
			XLOG_INFO("Tournament game clear ok!");
		}
		break;
	case OnlineFunctionUpdatePlayerNum:
		XLOG_INFO("Update player num ok!");
		break;
	case OnlineFunctionUpdatePlayerInfo:
		{
			const OnlineRespUpdatePlayerInfo &onlineResp = ((OnlineUpdatePlayerInfo *)onlineComponent)->GetResponse();
			m_server->UpdatePlayerInfo(onlineComponent->GetIParam1(),onlineResp.name.c_str(), onlineResp.honor, onlineResp.avatar.c_str(), onlineResp.skin, onlineResp.sex);
			XLOG_INFO("Update player info success");
			break;
		}
	case OnlineFunctionChangeDealer:
		{
			OnlineReqtChangeDealer *reqt = (OnlineReqtChangeDealer *)onlineComponent;
			m_server->ChangeDealer(0, onlineComponent->GetIParam1(), reqt->GetPlayerName(), onlineComponent->GetIParam2());
			XLOG_DEBUG("Change dealer success");
			break;
		}
	case OnlineFunctionTranslate:
		{
			XLOG_INFO("Translate success!");
			OnlineTranslate *reqt = (OnlineTranslate*)onlineComponent;
			m_server->Translated(0, reqt->GetIParam1(), reqt->GetResponse());
		}
		break;
	default:
		XLOG_ERROR("Unknown online function %d success.", func_id);
		break;
	}
}

void GameController::OnRequestFailed(OnlineComponent *onlineComponent, int func_id, int error_code )
{
	switch(func_id)
	{
	case OnlineFunctionStartConfirm:
		XLOG_ERROR("Start confirm failed(%d), retry count = %d!", error_code, m_startConfirmRetry);
		if(error_code == OnlinePErrorNetWork && m_startConfirmRetry < 5)
		{
			ScheduleTask(SCHEDULE_TASK_RETRY_CREATE_CONFIRM, 5000);
		}
		else
		{
			Close();
		}
		break;
	case OnlineFunctionKeepAlive:
		XLOG_ERROR("Keep ALive failed(%d), retry count is %d!", error_code, m_keepAliveRetry);
		if((error_code == OnlinePErrorNetWork || error_code == OnlinePErrorTimeOut ) && m_keepAliveRetry < 2)
		{
			XLOG_INFO("Retry at next keep alive time!");
			m_keepAliveRetry++;
			ScheduleTask(SCHEDULE_TASK_KEEP_ALIVE, K_KEEP_ALIVE_TIME_INTERVAL);
		}
		else
		{
			Close();
		}
		break;
	case OnlineFunctionCloseRoom:
		XLOG_ERROR("Close room failed(%d)!", error_code);
		ExitApp();
		break;
	case OnlineFunctionPlayerSit:
		{
			std::map<OnlineComponent*, RequestInfo>::iterator it = m_requestInfoMap.find(onlineComponent);
			if(it != m_requestInfoMap.end())
			{
				if(error_code == 0) error_code = 1000;
				m_server->PlayerSit(it->second.playerSitInfo.playerID, it->second.playerSitInfo.seat, error_code);
				XLOG_INFO("Player %d sit confirm failed(%d)!", it->second.playerSitInfo.playerID, error_code);
				m_requestInfoMap.erase(it);
			}
			else
			{
				XLOG_WARNING("Unknown player sit confirm failed!");
			}
		}
		break;
	case OnlineFunctionPlayerStand:
		{
			OnlinePlayerStand *reqt = (OnlinePlayerStand *)onlineComponent;
			XLOG_ERROR("Player %d stand up confirm failed, chip:%lld, eror_code:%d", onlineComponent->GetIParam1(), reqt->GetBettedChip(), error_code);

			//add to crash report
			XConsoleApplication *app = (XConsoleApplication*)XApplication::Instance();
			app->ReportCrash();

			m_server->PlayerStandUpFailed(onlineComponent->GetIParam1(), reqt->GetBettedChip());

			if (4 == error_code) //chip lack
			{
				Close();
			}
			else
			{
				RunRoomAI();
			}
		}
		break;
	case OnlineFunctionGameClear:
		XLOG_ERROR("Game clear failed(%d)!", error_code);
		XLOG_INFO("Game clear retry count is:%d", m_gameClearRetryCount);
		if(error_code == OnlinePErrorNetWork && m_gameClearRetryCount < K_MAX_GAME_CLEAR_RETRY_TIME)
		{
			ScheduleTask(SCHEDULE_TASK_RETRY_GAME_CLEAR, 5000);
		}
		else
		{
			Close();
		}
		break;
	case OnlineFunctionSelectAIPlayerSit:
		XLOG_ERROR("Select ai player sit failed(%d)!", error_code);
		break;
	case OnlineFunctionGetTotalGamePlayerNum:
		XLOG_ERROR("Get total game player num failed(%d)!", error_code);
		m_requetGetTGPN = NULL;
		m_lastUpdateNeedPlayerNumTime = XTime::CurMSTime();
		RunRoomAI();
		break;
	case OnlineFunctionGiveChip:
		XLOG_ERROR("Give chip confirm failed(%d)", error_code);
		m_requestInfoMap.erase(onlineComponent);
		break;
	case OnlineFunctionGetPlayerItem:
		XLOG_ERROR("Get player item failed(%d)", error_code);
		break;
	case OnlineFunctionGetIGTInfo:
		XLOG_ERROR("Get in game task info failed(%d)", error_code);
		break;
	case OnlineFunctionKickPlayer:
		{
			XLOG_INFO("Kick Player failed!");
			OnlineKickPlayer *reqt = (OnlineKickPlayer*)onlineComponent;

			switch(error_code)
			{
			case 5:
				m_server->KickPlayer(MSG_ERROR_PROP_LACK, reqt->GetIParam1(), reqt->GetIParam2(),
					reqt->GetSrcName(), reqt->GetDstName());
				break;
			case 22:
				m_server->KickPlayer(MSG_ERROR_KICK_PLAYER_LIMIT, reqt->GetIParam1(), reqt->GetIParam2(),
					reqt->GetSrcName(), reqt->GetDstName());
				break;
			default:
				m_server->KickPlayer(MSG_ERROR_UNKNOWN, reqt->GetIParam1(), reqt->GetIParam2(),
					reqt->GetSrcName(), reqt->GetDstName());
				break;
			}
			break;
		}
	case OnlineFunctionLockRoom:
		{
			XLOG_INFO("Lock room failed!");
			switch(error_code)
			{
			case 4:
				m_server->LockRoom(MSG_ERROR_PROP_LACK, onlineComponent->GetIParam1());
				break;
			case 5:
				m_server->LockRoom(MSG_ERROR_ROOM_ALREADY_LOCKED, onlineComponent->GetIParam1());
				break;
			case 2:
			case 3:
			default:
				m_server->LockRoom(MSG_ERROR_UNKNOWN, onlineComponent->GetIParam1());
				break;
			}
			break;
		}
	case OnlineFunctionUnlockRoom:
		{
			XLOG_INFO("Unlock room failed!");
			m_server->UnlockRoom(MSG_ERROR_UNKNOWN, onlineComponent->GetIParam1());
			break;
		}
	case OnlineFunctionTournamentSit:
		{
			m_server->TournamentSit(onlineComponent->GetIParam1(), error_code);
			XLOG_INFO("Tournament sit failed, player id %d!", onlineComponent->GetIParam1());
		}
		break;
	case OnlineFunctionTournamentExit:
		XLOG_INFO("Player exit tournament game failed!");
		break;
	case OnlineFunctionTournamentStart:
		XLOG_INFO("Tournament game start failed!");
		Close();
		break;
	case OnlineFunctionTournamentClear:
		XLOG_ERROR("Tournament game clear failed(%d)!", error_code);
		XLOG_INFO("Tournament game clear retry count is:%d", m_gameClearRetryCount);
		if(error_code == OnlinePErrorNetWork && m_gameClearRetryCount < 5)
		{
			ScheduleTask(SCHEDULE_TASK_RETRY_GAME_CLEAR, 5000);
		}
		else
		{
			Close();
		}
		break;
	case OnlineFunctionUpdatePlayerNum:
		XLOG_INFO("Update player num failed!");
		break;
	case OnlineFunctionUpdatePlayerInfo:
		{
			XLOG_DEBUG("Update player info failed");
			break;
		}
	case OnlineFunctionChangeDealer:
		{
			m_server->ChangeDealer(error_code, onlineComponent->GetIParam1(), NULL, -1);
			XLOG_INFO("Change dealer failed web");
			break;
		}
	case OnlineFunctionTranslate:
		{
			m_server->Translated(error_code, onlineComponent->GetIParam1(), Json::Value());
		}
		break;
	default:
		XLOG_ERROR("Unknown online function %d failed(%d).", func_id, error_code);
		break;
	}
}

void GameController::SendConfirmPlayerSit(int player, int seat, tChip playerChip, int inviter, const char *clientIp, const char *session, int hasSeat)
{
	OnlinePlayerSit *reqtPlayerSit = OnlinePlayerSit::CreateRequest(this);
	RequestInfo requestInfo;
	requestInfo.playerSitInfo.playerID = player;
	requestInfo.playerSitInfo.seat = seat;
	requestInfo.playerSitInfo.playerChip = playerChip;

	m_requestInfoMap[reqtPlayerSit] = requestInfo;
	reqtPlayerSit->Send(m_roomCode, player, playerChip, inviter, clientIp, session, hasSeat);
}

void GameController::SendNotifyPlayerReSit(int player, tChip playerChip, int betChip)
{
	OnlinePlayerSit *reqtPlayerSit = OnlinePlayerSit::CreateRequest(this);
	reqtPlayerSit->Send(-2, player, playerChip, betChip, "", "", 0);
}

void GameController::SendConfirmPlayerStand(int player, tChip bettedChip, bool holdPlace)
{
	XASSERT(bettedChip >= 0, "Assert failed:betted chip must >= 0");
	OnlinePlayerStand *reqt = OnlinePlayerStand::CreateRequest(this);
	reqt->SetIParam1(player);
	reqt->Send(m_roomCode, player, bettedChip, holdPlace);
}

void GameController::SendConfirmPresentChip(int from, int to, int chip)
{
	OnlineGiveChip *reqtGiveChip = OnlineGiveChip::CreateRequest(this);

	RequestInfo requestInfo;
	requestInfo.presentChipInfo.src = from;
	requestInfo.presentChipInfo.dest = to;
	requestInfo.presentChipInfo.chip = chip;

	m_requestInfoMap[reqtGiveChip] = requestInfo;	
	reqtGiveChip->Send(from, to, chip);
}

void GameController::SendGetPlayerItem(int player)
{
	OnlineGetPlayerItem *reqt = OnlineGetPlayerItem::CreateRequest(this);
	
	reqt->SetIParam1(player);
	reqt->Send(player);
}

void GameController::SendGameClear(GameClearInfo *gameClearInfo)
{
	string &result = m_gameClearUrl;
	m_noticeDetail.Reset();
	result.clear();
	result.append("[");
	char str[128] = {0};
	int playerIndex = 0;
	int allInCount = 0;
	tChip maxChipChange = 0;
	int maxWinUserSeat = -1;
	for (int i= 0; i < MAX_PLAYER_NUM; ++i)
	{
		if (gameClearInfo[i].userId > 0)
		{
			if(playerIndex++ != 0)
			{
				result.append(",");
			}

			result.append("{");
			sprintf(str, "\"uid\":%d", gameClearInfo[i].userId);
			result.append(str);

			sprintf(str, ",\"win\":%d", gameClearInfo[i].result);
			result.append(str);

			sprintf(str, ",\"chip\":\"%lld\"", gameClearInfo[i].chipChange);
			result.append(str);

			//max hand
			if (strlen(gameClearInfo[i].maxHand) == 10)
			{
				sprintf(str, ",\"best_card\":\"%s\"", gameClearInfo[i].maxHand);
				result.append(str);
			}

			//exp
			sprintf(str, ",\"exp\":%d", gameClearInfo[i].exp);
			result.append(str);

			//win type
			if(gameClearInfo[i].result == GameResultWin
				|| gameClearInfo[i].result == GameResultDraw)
			{
				sprintf(str, ",\"wintype\":%d", gameClearInfo[i].winType + 1);
				result.append(str);
			}

			//allin
			if(gameClearInfo[i].allin != 0)
			{
				result.append(",\"allin\":1");
				allInCount++;
			}

			//kick card
			if(gameClearInfo[i].addKickCarduNum > 0)
			{
				sprintf(str, ",\"kickcard\":%d", gameClearInfo[i].addKickCarduNum);
				result.append(str);			
			}

			//lock card
			if(gameClearInfo[i].addLockCardNum > 0)
			{
				sprintf(str, ",\"lockcard\":%d", gameClearInfo[i].addLockCardNum);
				result.append(str);			
			}

			//item effect
			if (!gameClearInfo[i].items.empty())
			{
				result.append(",\"item\":[");
				std::vector<ServerItem>::const_iterator it = gameClearInfo[i].items.begin();
				for(; it != gameClearInfo[i].items.end(); ++it)
				{
					if (it != gameClearInfo[i].items.begin())
					{
						result.append(",");
					}
					sprintf(str, "{\"itemid\":%d,\"itemchip\":%d}", it->itemID, it->extraChipLimit);
					result.append(str);
				}
				result.append("]");
			}

			result.append("}");

			if(gameClearInfo[i].result == GameResultWin && gameClearInfo[i].chipChange > maxChipChange)
			{
				m_noticeDetail.userId = gameClearInfo[i].userId;
				maxChipChange = gameClearInfo[i].chipChange;
				maxWinUserSeat = i;
			}
		}
	}

	if(maxWinUserSeat >= 0)//ensure there's a winner
	{
		if(maxChipChange > 100000000)
		{
			m_noticeDetail.type = NoticeTypeHundredMillion;
			m_noticeDetail.param1 = maxChipChange;
		}
		else if(m_roomLevel >= 3 && allInCount > 6)
		{
			m_noticeDetail.type = NoticeTypeMoreThan6AllIn;
			m_noticeDetail.param1 = allInCount;
		}
		else if(m_roomLevel >= 3 && maxChipChange > m_bigBlindBet * 400)
		{
			m_noticeDetail.type = NoticeType400TimesBigBlind;
			m_noticeDetail.param1 = m_roomLevel;
			m_noticeDetail.param2 = (int)(maxChipChange / m_bigBlindBet);
		}
		else if(gameClearInfo[maxWinUserSeat].winType == CardPatternRoyalFlush)
		{
			m_noticeDetail.type = NoticeTypeWinWithRoyalFlush;
		}
		else if(maxChipChange > 0 && CardPatternFourOfAKind <= gameClearInfo[maxWinUserSeat].winType && gameClearInfo[maxWinUserSeat].winType <= CardPatternStraightFlush)
		{
			int type = -1;
			CardSet winnerCardSet = m_server->GetPlayerCurrentCardSet(maxWinUserSeat);
			CardSet cardset;
			for(int i = 0; i < MAX_PLAYER_NUM; i++)
			{
				if (i != maxWinUserSeat && gameClearInfo[i].userId > 0)
				{
					cardset = m_server->GetPlayerCurrentCardSet(i);
					if((gameClearInfo[i].result == GameResultWin || gameClearInfo[i].result == GameResultDraw) && cardset.GetScore() >= winnerCardSet.GetScore())//the winner is not the best cardset
					{
						break;
					}
					if(cardset.GetPattern() == winnerCardSet.GetPattern())//same pattern, winnerCardSet.GetPattern() == gameClearInfo[maxWinUserSeat].winType
					{
						type = gameClearInfo[maxWinUserSeat].winType;
						break;
					}
				}
			}
			if(type >= CardPatternFourOfAKind)
			{
				m_noticeDetail.type = NoticeTypeWinWithFourOfAKind + type - CardPatternFourOfAKind;
			}
		}
	}

	result.append("]");

	OnlineGameClear::CreateRequest(this)->Send(m_roomCode, result.c_str(), m_noticeDetail);
	m_gameClearRetryCount = 0;
}

void GameController::SendGetInGameTaskInfo(int playerID)
{
	OnlineGetIGTInfo *reqt = OnlineGetIGTInfo::CreateRequest(this);
	reqt->SetIParam1(playerID);
	reqt->Send(playerID);
}

void GameController::SendKickPlayer(int srcID, int dstID, const char *srcName, const char *dstName)
{
	OnlineKickPlayer *reqt = OnlineKickPlayer::CreateRequest(this);
	reqt->SetIParam1(srcID);
	reqt->SetIParam2(dstID);
	reqt->SetSrcName(srcName);
	reqt->SetDstName(dstName);
	reqt->Send(srcID, dstID, m_roomCode);
}

void GameController::SendLockRoom(int playerID, int roomCode)
{
	OnlineLockRoom *reqt = OnlineLockRoom::CreateRequest(this);
	reqt->SetIParam1(playerID);
	reqt->Send(playerID, roomCode);
}

void GameController::SendUnlockRoom(int playerID, int roomCode)
{
	OnlineUnlockRoom *reqt = OnlineUnlockRoom::CreateRequest(this);
	reqt->SetIParam1(playerID);
	reqt->Send(playerID, roomCode);
}

void GameController::SendTournamentSit(int playerID)
{
	OnlineTournamentSit *reqt = OnlineTournamentSit::CreateRequest(this);
	reqt->SetIParam1(playerID);
	reqt->Send(m_roomCode, playerID);
}

void GameController::SendTournamentExit(int playerID)
{
	OnlineTournamentExit::CreateRequest(this)->Send(m_roomCode, playerID);
}

void GameController::SendTournamentStart(const char *players)
{
	OnlineTournamentStart::CreateRequest(this)->Send(m_roomCode, players);
}

void GameController::SendTournamentClear(int gameCount, std::vector<TournamentResult> &results)
{
	char str[128] = {0};
	m_gameClearUrl.clear();
	std::string &result = m_gameClearUrl;

	sprintf(str, "room_id=%d&game_count=%d&results=[", m_roomCode, gameCount);
	result.append(str);

	std::vector<TournamentResult>::iterator it = results.begin();
	bool bFirst = true;
	for (; it != results.end(); ++it)
	{
		if (!bFirst)
		{
			result.append(",");
		}
		else
		{
			bFirst = false;
		}

		sprintf(str, "{\"player_id\":%d,\"exited\":%d,\"rank\":%d}", it->playerID, 0, it->rank);
		result.append(str);
	}
	result.append("]");

	OnlineTournamentClear::CreateRequest(this)->Send(result.c_str());
	m_gameClearRetryCount = 0;
}

void GameController::SendTournamentClear(int gameCount, int playerID, int rank)
{
	char str[128] = {0};
	std::string result;

	sprintf(str, "room_id=%d&game_count=%d&results=[", m_roomCode, gameCount);
	result.append(str);
	sprintf(str, "{\"player_id\":%d,\"exited\":%d,\"rank\":%d}", playerID, 1, rank);
	result.append(str);
	result.append("]");

	OnlineTournamentClear::CreateRequest(NULL)->Send(result.c_str());
}

void GameController::SendUpdatePlayerNum(const char *players)
{
	OnlineReqtUpdatePlayerNum::CreateRequest(this)->Send(m_roomCode, players);
}

void GameController::SendUpdatePlayerInfo(int playerId)
{
	OnlineUpdatePlayerInfo *reqt = OnlineUpdatePlayerInfo::CreateRequest(this);
	reqt->SetIParam1(playerId);
	reqt->Send(playerId);
}

void GameController::SendChangeDealer(int playerId, const char *playerName, int dealerId, tChip gameChip)
{
	OnlineReqtChangeDealer::CreateRequest(this)->Send(playerId, playerName, dealerId, gameChip);
}

void GameController::SendTranslate(int from, const char* keyfrom, const char* key, const char *text)
{
	OnlineTranslate *reqt = OnlineTranslate::CreateRequest(this);
	reqt->SetIParam1(from);
	reqt->Send(keyfrom,key,text);
}

void GameController::ScheduleAIJoin(int delayTime)
{
	if(m_server->IsLocked())
	{
		return;
	}
	ScheduleTask(SCHEDULE_TASK_AI_JOIN, delayTime);
	XLOG_INFO("Schedule task for joining ai player after %d ms.", delayTime);
}

void GameController::OnScheduleTimeout(int tid, int iParam, void *pParam, int overTime)
{
	switch(tid)
	{
	case SCHEDULE_TASK_CLOSE_IDLE_ROOM:
		XLOG_INFO("Timeout for schedule close idle room!");
		Close();
		break;
	case SCHEDULE_TASK_KEEP_ALIVE:
		XLOG_INFO("Timeout for schedule keep alive!");
		OnlineKeepALive::CreateRequest(this)->Send(m_roomCode);
		break;
	case SCHEDULE_TASK_AI_JOIN:
		XLOG_INFO("Timeout for schedule join ai player!");

		OnlineSelectAIPlayerSit::CreateRequest(this)->Send(m_roomCode, m_server->GetMinCarryMoney());

		break;
	case SCHEDULE_TASK_CLOSE_ROOM:
		XLOG_INFO("Timeout for schedule exit close room!");
		if(m_server->GetConnectionNum() <= 0)
		{
			Close();
		}
		break;
	case SCHEDULE_TASK_CHECK_1_AI_PLAYER:
		XLOG_INFO("Timeout for schedule check one ai player!");
		m_server->RandomKickAIPlayer();
		break;
	case SCHEDULE_TASK_KICK_AI_PLAYER:
		XLOG_INFO("Timeout for schedule kick an ai player!");
		m_server->RandomKickAIPlayer();
		break;
	case SCHEDULE_TASK_RETRY_CREATE_CONFIRM:
		XLOG_INFO("Retry create confirm!");
		OnlineStartConfirm::CreateRequest(this)->Send(m_roomCode, m_listenPort, m_udpPort);
		break;
	case SCHEDULE_TASK_RETRY_GAME_CLEAR:
		{
			XLOG_INFO("Retry game clear!");
			XConsoleApplication *app = (XConsoleApplication*)XApplication::Instance();
			m_gameClearRetryCount++;
			if (app->GetGameType() == GameTypeTournament)
			{
				OnlineTournamentClear::CreateRequest(this)->Send(m_gameClearUrl.c_str());
			}
			else
			{
				OnlineGameClear::CreateRequest(this)->Send(m_roomCode, m_gameClearUrl.c_str(), m_noticeDetail);
			}
		}
		break;
	case SCHEDULE_TASK_DELAY_CLOSE:
		XLOG_INFO("Delay close room!");
		Close();
		break;
	}
}

void GameController::RunRoomAI()
{

	XConsoleApplication *app = (XConsoleApplication*)XApplication::Instance();
	if (app->GetGameType() == GameTypeTournament)
	{
		XLOG_WARNING("Tournament can't run room ai!");
		return;
	}

	XLOG_INFO("Run Room AI...");

	RemoveTask(SCHEDULE_TASK_CLOSE_ROOM);
	RemoveTask(SCHEDULE_TASK_CHECK_1_AI_PLAYER);
	RemoveTask(SCHEDULE_TASK_KICK_AI_PLAYER);

	m_aiPlayerToJoin = 0;

	//检测空房间
	if(m_server->GetConnectionNum() <= 0)
	{
		XLOG_INFO("No connection in the room, schedule exit after 10 seconds.");
		ScheduleTask(SCHEDULE_TASK_CLOSE_ROOM, 10 * 1000);
		return;
	}

	//检测锁定房间
	//TO DO...

	//检查游戏玩家人数
	if(m_requetGetTGPN != NULL && m_requetGetTGPN->IsInProcess())
	{
		return;
	}
	if(XTime::CurMSTime() - m_lastUpdateNeedPlayerNumTime > 60 * 60 * 1000)
	{
		m_requetGetTGPN = OnlineGetTotalGamePlayerNum::CreateRequest(this);
		m_requetGetTGPN->Send();
		return;
	}

	//如果游戏玩家过多
	if(m_totalNeedGamePlayer < 0)
	{
		m_server->RandomKickAIPlayer();
		m_totalNeedGamePlayer++;
	}
	//如果游戏玩家没有过多
	else
	{
		int playerNum = m_server->GetPlayerNum();
		if(playerNum == 1)
		{
			if(m_server->HasAIPlayer())
			{
				ScheduleTask(SCHEDULE_TASK_CHECK_1_AI_PLAYER, XMath::Random(30 * 1000, 90 * 1000));
			}
			else
			{
				XLOG_INFO("One humen player, join an AI!");
				ScheduleAIJoin(XMath::Random(10000, 30000));
			}
		}
		else if(playerNum == 2)
		{
			if(m_aiPlayerToJoin <= 0 && XMath::Random(1, 100) < 50)
			{
				XLOG_INFO("2 player room, schedule join 1 ai player");
				ScheduleAIJoin(XMath::Random(20000, 60000));
			}
		}
		else if(playerNum >= m_server->GetMaxPlayerNum())
		{
			if(m_server->HasAIPlayer())
			{
				XLOG_INFO("Schedule kick an ai player in full room!");
				ScheduleTask(SCHEDULE_TASK_KICK_AI_PLAYER, XMath::Random(3 * 1000, 10 * 1000));
			}
		}
		else if(m_server->HasAIPlayer())
		{
			if(XMath::Random(1, 100) < 5)
			{
				XLOG_INFO("Exchange a AI player!");
				//kick a player
				m_server->RandomKickAIPlayer();

				//join a ai player
				ScheduleAIJoin(XMath::Random(5 * 1000, 10 * 1000));
			}
		}
	}
}

void GameController::HandleWebPushInvite(const Json::Value &json)
{
	int  srcPlayerID = 0;
	int  dstPlayer = 0;
	int  roomCode  = 0;
	int  roomSmallBlind = 0;

	XLOG_INFO("HandleWebPushInvite...");

	Json::Value json_value;

	json_value = json["uid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"uid\"");
		return;
	}
	srcPlayerID = json_value.asInt();

	json_value = json["pid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"pid\"");
		return;
	}
	dstPlayer = json_value.asInt();

	json_value = json["rid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"rid\"");
		return;
	}
	roomCode = json_value.asInt();

	json_value = json["sb"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"sb\"");
		return;
	}
	roomSmallBlind = json_value.asInt();

	json_value = json["name"];
	if(!json_value.isString())
	{
		XLOG_ERROR("No value for key \"name\"");
		return;
	}

	XLOG_INFO("GameController::Receive invitation for %d from %d in room %d", dstPlayer, srcPlayerID, roomCode);
	m_server->InvitePlayer(srcPlayerID, dstPlayer, json_value.asCString(), roomCode, roomSmallBlind);
}

void GameController::HandleWebTournamentInvite(const Json::Value &json)
{
	int  srcPlayerID = 0;
	int  dstPlayer = 0;
	int  field  = 0;
	int  efee = 0;

	XLOG_INFO("HandleWebTournamentInvite...");

	Json::Value json_value;

	json_value = json["uid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"uid\"");
		return;
	}
	srcPlayerID = json_value.asInt();

	json_value = json["pid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"pid\"");
		return;
	}
	dstPlayer = json_value.asInt();

	json_value = json["field"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"field\"");
		return;
	}
	field = json_value.asInt();

	json_value = json["efee"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"efee\"");
		return;
	}
	efee = json_value.asInt();

	json_value = json["name"];
	if(!json_value.isString())
	{
		XLOG_ERROR("No value for key \"name\"");
		return;
	}

	XLOG_INFO("GameController::Receive tournament invitation for %d from %d in field %d", dstPlayer, srcPlayerID, field);

	m_server->TournamentInvite(dstPlayer, json_value.asCString(), field, efee);
}

void GameController::HandleWebPushKickPlayer(const Json::Value &json)
{
	XLOG_INFO("HandleWebPushKickPlayer...");

	Json::Value pid = json["pid"];
	if(!pid.isInt())
	{
		XLOG_ERROR("No value for key \"pid\"");
		return;
	}
	int dstPlayer = pid.asInt();

	if (json["sid"].isNull())
	{
		XLOG_INFO("GameController::Force Kick player %d", dstPlayer);
		m_server->ForceKickPlayer(dstPlayer);
	}
	else
	{
		XLOG_INFO("GameController::GM Kick player %d", dstPlayer);
		m_server->KickPlayer(MSG_NO_ERROR, json["sid"].asInt(), dstPlayer, json["sname"].asCString(), json["pname"].asCString());
	}
}

void GameController::HandleWebPrivateNotice(const Json::Value &json)
{
	XLOG_INFO("HandleWebPrivateNotice...");
	Json::Value json_value;
	json_value = json["pid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"pid\"");
		return;
	}
	int dstPlayer = json_value.asInt();

	json_value = json["msg"];
	if(!json_value.isString())
	{
		XLOG_ERROR("No value for key \"msg\"");
		return;
	}

	m_server->SendNotice(dstPlayer, json_value.asCString());
}

void GameController::HandleWebSurprise(const Json::Value &json)
{
	XLOG_INFO("HandleWebSurprise...");

	Json::Value json_value;
	json_value = json["pid"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"pid\"");
		return;
	}
	int playerId = json_value.asInt();

	json_value = json["chip"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"chip\"");
		return;
	}
	tChip chip = json_value.asInt();

	json_value = json["item"];
	if(!json_value.isInt())
	{
		XLOG_ERROR("No value for key \"item\"");
		return;
	}
	int itemId = json_value.asInt();

	m_server->NoticeSurprise(playerId, chip, itemId);
}

