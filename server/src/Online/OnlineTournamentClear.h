#ifndef _ONLINE_REQUEST_TOURNAMENT_CLEAR_
#define _ONLINE_REQUEST_TOURNAMENT_CLEAR_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"
#include <map>

struct Profit
{
	int playerID;
	tChip profit;
	int credit;
	int exp;
};


struct OnlineRespTournamentClear
{
	std::vector<Profit> profits;
};

class OnlineTournamentClear : public OnlineComponent
{
public:
	static OnlineTournamentClear * CreateRequest(OnlineObserver *observer);

	bool Send(const char *results);
	const OnlineRespTournamentClear &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineTournamentClear(OnlineObserver *observer);

	OnlineRespTournamentClear m_resp;
};

#endif