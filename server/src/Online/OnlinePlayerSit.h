#ifndef _ONLINE_REQUEST_PLAYER_SIT_
#define _ONLINE_REQUEST_PLAYER_SIT_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"
#include <map>

struct OnlineRespPlayerSit
{
	char m_playerName[K_USER_NAME_BUFFER_LEN];
	int  m_equipeTitle;
	std::map<int, int> items;
	char m_equipeAvatar[K_USER_AVATAR_URL_LENGTH];
	char m_maxHand[11];
	int  m_inGameTaskID;
	int  m_inGameTaskTime;
	int  m_cardPower;
	int  m_skin;
	int  m_sex;
      int  m_hasSeat;
};

class OnlinePlayerSit : public OnlineComponent
{
public:
	static OnlinePlayerSit * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, int playerId, long long carryChip, int inviterID, const char *clientIp, const char *session, int hasSeat);
	const OnlineRespPlayerSit &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlinePlayerSit(OnlineObserver *observer);

	OnlineRespPlayerSit m_resp;
};

#endif