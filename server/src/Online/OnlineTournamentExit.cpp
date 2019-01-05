#include "stdafx.h"
#include "OnlineTournamentExit.h"

OnlineTournamentExit::OnlineTournamentExit(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineTournamentExit::Send(int roomCode, int playerId)
{
	std::string query;

	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "player_id=%d", playerId);

	return SendRequest(query);
}

void OnlineTournamentExit::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineTournamentExit::GetFunctionID() const
{
	return OnlineFunctionTournamentExit;
}

const char *OnlineTournamentExit::GetFunctionName() const
{
	return "tournament_exit";
}

OnlineTournamentExit * OnlineTournamentExit::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineTournamentExit(observer);
}

const OnlineRespTournamentExit& OnlineTournamentExit::GetResponse()
{
	return m_resp;
}