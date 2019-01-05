#include "stdafx.h"
#include "OnlinePlayerStand.h"

OnlinePlayerStand::OnlinePlayerStand(OnlineObserver *observer)
: OnlineComponent(observer)
{
	m_bettedChip = 0;
}

bool OnlinePlayerStand::Send(int roomCode, int playerId, tChip bettedChip, bool holdPlace)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "player_id=%d&", playerId);
	AppendFormat(query, "chip=%lld&",bettedChip);
	AppendFormat(query, "hold_place=%d&",holdPlace?1:0);

	m_bettedChip = bettedChip;

	return SendRequest(query);
}

void OnlinePlayerStand::OnRequestSuccess(Json::Value &resp)
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

int OnlinePlayerStand::GetFunctionID() const
{
	return OnlineFunctionPlayerStand;
}

const char *OnlinePlayerStand::GetFunctionName() const
{
	return "player_stand";
}

OnlinePlayerStand * OnlinePlayerStand::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlinePlayerStand(observer);
}

const OnlineRespPlayerStand& OnlinePlayerStand::GetResponse()
{
	return m_resp;
}

tChip OnlinePlayerStand::GetBettedChip() const
{
	return m_bettedChip;
}