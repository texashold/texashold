#include "stdafx.h"
#include "OnlineChangeDealer.h"

OnlineReqtChangeDealer::OnlineReqtChangeDealer(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineReqtChangeDealer::Send(int playerID, const char *name, int dealerID, tChip gameChip)
{
	m_playerName = name;
	SetIParam1(playerID);
	SetIParam2(dealerID);

	std::string query;
	
	AppendFormat(query, "pid=%d&", playerID);
	AppendFormat(query, "did=%d&", dealerID);
	AppendFormat(query, "game_chip=%lld", gameChip);

	return SendRequest(query);
}

void OnlineReqtChangeDealer::OnRequestSuccess(Json::Value &resp)
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

int OnlineReqtChangeDealer::GetFunctionID() const
{
	return OnlineFunctionChangeDealer;
}

const char *OnlineReqtChangeDealer::GetFunctionName() const
{
	return "change_dealer";
}

const char *OnlineReqtChangeDealer::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineReqtChangeDealer * OnlineReqtChangeDealer::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineReqtChangeDealer(observer);
}

const OnlineRespChangeDealer& OnlineReqtChangeDealer::GetResponse()
{
	return m_resp;
}

const char * OnlineReqtChangeDealer::GetPlayerName() const
{
	return m_playerName.c_str();
}	