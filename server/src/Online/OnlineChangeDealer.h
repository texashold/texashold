#ifndef _ONLINE_CHANGE_DEALER_
#define _ONLINE_CHANGE_DEALER_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"
#include <string>

struct OnlineRespChangeDealer
{

};

class OnlineReqtChangeDealer : public OnlineComponent
{
public:
	static OnlineReqtChangeDealer * CreateRequest(OnlineObserver *observer);

	bool Send(int playerID, const char *name, int dealerID, tChip gameChip);
	const OnlineRespChangeDealer &GetResponse();

	const char *GetPlayerName() const;

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineReqtChangeDealer(OnlineObserver *observer);

	OnlineRespChangeDealer m_resp;
	std::string m_playerName;
};

#endif