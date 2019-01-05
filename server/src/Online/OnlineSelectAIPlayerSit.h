#ifndef _ONLINE_REQUEST_SELECT_AI_PLAYER_SIT_
#define _ONLINE_REQUEST_SELECT_AI_PLAYER_SIT_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespSelectAIPlayerSit
{
	int  m_aiid;
	long long  m_aichip;

	char m_playerName[K_USER_NAME_BUFFER_LEN];
	int  m_vipItem;
	int  m_title;
	int  m_item;
	char m_avatar[K_USER_AVATAR_URL_LENGTH];
	char m_maxHand[11];

	int m_cardPower;
};

class OnlineSelectAIPlayerSit : public OnlineComponent
{
public:
	static OnlineSelectAIPlayerSit * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, long long minChip);
	const OnlineRespSelectAIPlayerSit &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineSelectAIPlayerSit(OnlineObserver *observer);

	OnlineRespSelectAIPlayerSit m_resp;
};

#endif