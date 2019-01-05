#ifndef _ONLINE_REQUEST_UPDATE_PLAYER_NUM_
#define _ONLINE_REQUEST_UPDATE_PLAYER_NUM_

#include "Online/Common/OnlineComponent.h"

struct OnlineRespUpdatePlayerNum
{

};

class OnlineReqtUpdatePlayerNum : public OnlineComponent
{
public:
	static OnlineReqtUpdatePlayerNum * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, const char *players);
	const OnlineRespUpdatePlayerNum &GetResponse();

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineReqtUpdatePlayerNum(OnlineObserver *observer);

	OnlineRespUpdatePlayerNum m_resp;
};

#endif