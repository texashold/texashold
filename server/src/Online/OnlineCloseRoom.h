#ifndef _ONLINE_REQUEST_CLOSE_ROOM_
#define _ONLINE_REQUEST_CLOSE_ROOM_

#include "Online/Common/OnlineComponent.h"

struct OnlineRespCloseRoom
{
	
};

class OnlineCloseRoom : public OnlineComponent
{
public:
	static OnlineCloseRoom * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode);
	const OnlineCloseRoom &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineCloseRoom(OnlineObserver *observer);

	OnlineRespCloseRoom m_resp;
};

#endif