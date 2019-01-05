#include "stdafx.h"
#include "OnlineUpdatePlayerNum.h"

OnlineReqtUpdatePlayerNum::OnlineReqtUpdatePlayerNum(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineReqtUpdatePlayerNum::Send(int roomCode, const char *players)
{
	std::string query;

	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "players=[%s]", players);

	return SendRequest(query);
}

void OnlineReqtUpdatePlayerNum::OnRequestSuccess(Json::Value &resp)
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

int OnlineReqtUpdatePlayerNum::GetFunctionID() const
{
	return OnlineFunctionUpdatePlayerNum;
}

const char *OnlineReqtUpdatePlayerNum::GetFunctionName() const
{
	return "update_player_num";
}

const char *OnlineReqtUpdatePlayerNum::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineReqtUpdatePlayerNum * OnlineReqtUpdatePlayerNum::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineReqtUpdatePlayerNum(observer);
}

const OnlineRespUpdatePlayerNum& OnlineReqtUpdatePlayerNum::GetResponse()
{
	return m_resp;
}