#include "stdafx.h"
#include "OnlineTournamentStart.h"

OnlineTournamentStart::OnlineTournamentStart(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineTournamentStart::Send(int roomCode, const char *players)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "players=[%s]", players);

	return SendRequest(query);
}

void OnlineTournamentStart::OnRequestSuccess(Json::Value &resp)
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

int OnlineTournamentStart::GetFunctionID() const
{
	return OnlineFunctionTournamentStart;
}

const char *OnlineTournamentStart::GetFunctionName() const
{
	return "tournament_start";
}

const char *OnlineTournamentStart::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineTournamentStart * OnlineTournamentStart::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineTournamentStart(observer);
}

const OnlineRespTournamentStart& OnlineTournamentStart::GetResponse()
{
	return m_resp;
}