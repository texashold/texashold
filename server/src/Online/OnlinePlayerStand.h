#ifndef _ONLINE_REQUEST_PLAYER_STAND_
#define _ONLINE_REQUEST_PLAYER_STAND_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespPlayerStand
{

};

class OnlinePlayerStand : public OnlineComponent
{
public:
	static OnlinePlayerStand * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, int playerId, tChip bettedChip, bool holdPlace);
	const OnlineRespPlayerStand &GetResponse();
	tChip GetBettedChip() const;

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlinePlayerStand(OnlineObserver *observer);

	OnlineRespPlayerStand m_resp;

	tChip	m_bettedChip;
};

#endif