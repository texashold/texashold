#ifndef _ONLINE_REQUEST_GET_TOTAL_PLAYER_NUM_H_
#define _ONLINE_REQUEST_GET_TOTAL_PLAYER_NUM_H_

#include "Online/Common/OnlineComponent.h"

typedef int OnlineRespGetTotalGamePlayerNum;

class OnlineGetTotalGamePlayerNum : public OnlineComponent
{
public:
	static OnlineGetTotalGamePlayerNum * CreateRequest(OnlineObserver *observer);

	bool Send();
	const OnlineRespGetTotalGamePlayerNum &GetResponse(){return m_resp;}
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineGetTotalGamePlayerNum(OnlineObserver *observer);

	OnlineRespGetTotalGamePlayerNum m_resp;
};

#endif