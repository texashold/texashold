#ifndef _ONLINE_REQUEST_UNLOCK_ROOM_
#define _ONLINE_REQUEST_UNLOCK_ROOM_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespUnlockRoom
{

};

class OnlineUnlockRoom : public OnlineComponent
{
public:
	static OnlineUnlockRoom * CreateRequest(OnlineObserver *observer);

	bool Send(int playerID, int roomCode);
	const OnlineRespUnlockRoom &GetResponse();

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineUnlockRoom(OnlineObserver *observer);

	OnlineRespUnlockRoom m_resp;
};

#endif