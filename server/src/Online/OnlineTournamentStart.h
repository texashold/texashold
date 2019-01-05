#ifndef _ONLINE_REQUEST_TOURNAMENT_START_
#define _ONLINE_REQUEST_TOURNAMENT_START_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespTournamentStart
{

};

class OnlineTournamentStart : public OnlineComponent
{
public:
	static OnlineTournamentStart * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, const char *players);
	const OnlineRespTournamentStart &GetResponse();

protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineTournamentStart(OnlineObserver *observer);

	OnlineRespTournamentStart m_resp;
};

#endif