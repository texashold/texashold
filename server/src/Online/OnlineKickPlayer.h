#ifndef _ONLINE_REQUEST_KICK_PLAYER_
#define _ONLINE_REQUEST_KICK_PLAYER_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"
#include <string>

struct OnlineRespKickPlayer
{

};

class OnlineKickPlayer : public OnlineComponent
{
public:
	static OnlineKickPlayer * CreateRequest(OnlineObserver *observer);

	bool Send(int srcID, int dstID, int roomCode);
	const OnlineRespKickPlayer &GetResponse();

	const char *GetSrcName() const;
	const char *GetDstName() const;
	
	void SetSrcName(const char *srcName);
	void SetDstName(const char *dstName);
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineKickPlayer(OnlineObserver *observer);

	OnlineRespKickPlayer m_resp;

	std::string	m_srcName;
	std::string m_dstName;
};

#endif