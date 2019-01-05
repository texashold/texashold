#ifndef _ONLINE_REQUEST_TOURNAMENT_EXIT_
#define _ONLINE_REQUEST_TOURNAMENT_EXIT_

#include "Online/Common/OnlineComponent.h"
#include "CSCommonDefine.h"

struct OnlineRespTournamentExit
{

};

class OnlineTournamentExit : public OnlineComponent
{
public:
	static OnlineTournamentExit * CreateRequest(OnlineObserver *observer);

	bool Send(int roomCode, int playerId);
	const OnlineRespTournamentExit &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineTournamentExit(OnlineObserver *observer);

	OnlineRespTournamentExit m_resp;
};

#endif