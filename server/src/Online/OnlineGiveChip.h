#ifndef _ONLINE_REQUEST_GIVE_CHIP_H_
#define _ONLINE_REQUEST_GIVE_CHIP_H_

#include "Online/Common/OnlineComponent.h"

struct OnlineRespGiveChip
{
	
};

class OnlineGiveChip : public OnlineComponent
{
public:
	static OnlineGiveChip * CreateRequest(OnlineObserver *observer);

	bool Send(int from, int to, int chip);
	const OnlineGiveChip &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineGiveChip(OnlineObserver *observer);

	OnlineRespGiveChip m_resp;
};

#endif