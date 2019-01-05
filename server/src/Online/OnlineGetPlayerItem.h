#ifndef _ONLINE_REQUEST_GET_PLAYER_ITEM_H_
#define _ONLINE_REQUEST_GET_PLAYER_ITEM_H_

#include "Online/Common/OnlineComponent.h"
#include <map>

struct OnlineRespGetPlayerItem
{
	std::map<int, int> items;
};

class OnlineGetPlayerItem : public OnlineComponent
{
public:
	static OnlineGetPlayerItem * CreateRequest(OnlineObserver *observer);

	bool Send(int playerID);
	const OnlineRespGetPlayerItem &GetResponse(){return m_resp;}
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineGetPlayerItem(OnlineObserver *observer);

	OnlineRespGetPlayerItem m_resp;
};

#endif