#ifndef _ONLINE_REQUEST_UPDATE_PLAYER_INFO_
#define _ONLINE_REQUEST_UPDATE_PLAYER_INFO_

#include "Online/Common/OnlineComponent.h"
#include <string>

struct OnlineRespUpdatePlayerInfo
{
	std::string name;
	int  honor;
	std::string avatar;
	int  skin;
	int  sex;

};

class OnlineUpdatePlayerInfo : public OnlineComponent
{
public:
	static OnlineUpdatePlayerInfo * CreateRequest(OnlineObserver *observer);

	bool Send(int playerId);
	const OnlineRespUpdatePlayerInfo &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineUpdatePlayerInfo(OnlineObserver *observer);

	OnlineRespUpdatePlayerInfo m_resp;
};

#endif