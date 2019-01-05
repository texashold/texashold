#ifndef _ONLINE_REQUEST_KEEP_ALIVE_
#define _ONLINE_REQUEST_KEEP_ALIVE_

#include "Online/Common/OnlineComponent.h"
#include <string>

struct OnlineRespKeepAlive
{
	std::string notice;
};

class OnlineKeepALive : public OnlineComponent
{
public:
	static OnlineKeepALive * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode);
	const OnlineRespKeepAlive &GetResponse(){return m_resp;}
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineKeepALive(OnlineObserver *observer);

	OnlineRespKeepAlive m_resp;
};

#endif