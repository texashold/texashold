#ifndef _ONLINE_REQUEST_TOURNAMENT_SIT_
#define _ONLINE_REQUEST_TOURNAMENT_SIT_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespTournamentSit
{
	char m_name[K_USER_NAME_BUFFER_LEN];
	int  m_title;
	char m_avatar[K_USER_AVATAR_URL_LENGTH];
	char m_maxHand[11];
	int  m_skin;
	int  m_sex;
	int  m_itemID;
	int  m_vipID;
};

class OnlineTournamentSit : public OnlineComponent
{
public:
	static OnlineTournamentSit * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, int playerId);
	const OnlineRespTournamentSit &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineTournamentSit(OnlineObserver *observer);

	OnlineRespTournamentSit m_resp;
};

#endif