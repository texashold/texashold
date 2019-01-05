#ifndef _ONLINE_REQUEST_START_CONFIRM_
#define _ONLINE_REQUEST_START_CONFIRM_

#include "Online/Common/OnlineComponent.h"

struct OnlineRespStartConfirm
{
	
};

class OnlineStartConfirm : public OnlineComponent
{
public:
	static OnlineStartConfirm * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, unsigned short port, unsigned short udpPort);
	const OnlineStartConfirm &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineStartConfirm(OnlineObserver *observer);

	OnlineRespStartConfirm m_resp;
};

#endif