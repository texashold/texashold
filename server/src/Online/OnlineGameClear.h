#ifndef _ONLINE_REQUEST_GAME_CLEAR_
#define _ONLINE_REQUEST_GAME_CLEAR_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"
#include <map>

struct OnlineRespGameClear
{
	std::map<int, OLRespGCPlayerInfo> playerChip;
};
struct NoticeDetail;
class OnlineGameClear : public OnlineComponent
{
public:
	static OnlineGameClear * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, const char *result, NoticeDetail& noticeDetail);
	const OnlineRespGameClear &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineGameClear(OnlineObserver *observer);

	OnlineRespGameClear m_resp;
};

#endif