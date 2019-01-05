#ifndef _ONLINE_REQUEST_LOCK_ROOM_
#define _ONLINE_REQUEST_LOCK_ROOM_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespLockRoom
{

};

class OnlineLockRoom : public OnlineComponent
{
public:
	static OnlineLockRoom * CreateRequest(OnlineObserver *observer);

	bool Send(int playerID, int roomCode);
	const OnlineRespLockRoom &GetResponse();

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineLockRoom(OnlineObserver *observer);

	OnlineRespLockRoom m_resp;
};

#endif