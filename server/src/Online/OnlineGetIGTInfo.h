#ifndef _ONLINE_REQUEST_GET_IN_GAME_TASK_INFO_
#define _ONLINE_REQUEST_GET_IN_GAME_TASK_INFO_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespGetIGTInfo
{
	int  m_inGameTaskID;
	int  m_inGameTaskTime;
};

class OnlineGetIGTInfo : public OnlineComponent
{
public:
	static OnlineGetIGTInfo * CreateRequest(OnlineObserver *observer);

	bool Send(int playerId);
	const OnlineRespGetIGTInfo &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineGetIGTInfo(OnlineObserver *observer);

	OnlineRespGetIGTInfo m_resp;
};

#endif