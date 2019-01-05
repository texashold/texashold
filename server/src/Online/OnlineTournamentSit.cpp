#include "stdafx.h"
#include "OnlineTournamentSit.h"

OnlineTournamentSit::OnlineTournamentSit(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineTournamentSit::Send(int roomCode, int playerId)
{
	std::string query;

	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "player_id=%d", playerId);

	return SendRequest(query);
}

void OnlineTournamentSit::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		Json::Value json_value;

		if(!GetJsonValue(resp, "name", m_resp.m_name))
		{
			return;
		}
		GetJsonValue(resp, "title", m_resp.m_title, false);
		GetJsonValue(resp, "avatar", m_resp.m_avatar, false);
		GetJsonValue(resp, "best_card", m_resp.m_maxHand, false);
		GetJsonValue(resp, "skin", m_resp.m_skin, false);
		if (!GetJsonValue(resp, "sex", m_resp.m_sex, false))
		{
			m_resp.m_sex = 2;
		}
		GetJsonValue(resp, "item_id", m_resp.m_itemID, false);
		GetJsonValue(resp, "vip", m_resp.m_vipID, false);

		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineTournamentSit::GetFunctionID() const
{
	return OnlineFunctionTournamentSit;
}

const char *OnlineTournamentSit::GetFunctionName() const
{
	return "tournament_sit";
}

const char *OnlineTournamentSit::GetSimulateResult() const
{
	return "{\"player_name\":\"iPod touch\",\"title\":0,\"avatar\":\"MjAwMThfMw==\",\"best_card\":\"0E0A080702\",\"skin\":0,\"sex\":2,\"item_id\":0}";
}

OnlineTournamentSit * OnlineTournamentSit::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineTournamentSit(observer);
}

const OnlineRespTournamentSit& OnlineTournamentSit::GetResponse()
{
	return m_resp;
}