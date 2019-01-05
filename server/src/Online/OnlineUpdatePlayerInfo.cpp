#include "stdafx.h"
#include "OnlineUpdatePlayerInfo.h"

OnlineUpdatePlayerInfo::OnlineUpdatePlayerInfo(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineUpdatePlayerInfo::Send(int playerId)
{
	std::string query;
	
	AppendFormat(query, "player_id=%d", playerId);

	return SendRequest(query);
}

void OnlineUpdatePlayerInfo::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		Json::Value json_value;
		char str[256] = "";

		if(!GetJsonValue(resp, "name", str))
		{
			return;
		}
		m_resp.name = str;

		GetJsonValue(resp, "honor", m_resp.honor, false);

		GetJsonValue(resp, "head", str, false);
		m_resp.avatar = str;

		GetJsonValue(resp, "skin", m_resp.skin, false);
		if (!GetJsonValue(resp, "sex", m_resp.sex, false))
		{
			m_resp.sex = 2;
		}
		
		
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineUpdatePlayerInfo::GetFunctionID() const
{
	return OnlineFunctionUpdatePlayerInfo;
}

const char *OnlineUpdatePlayerInfo::GetFunctionName() const
{
	return "update_player_info";
}

const char *OnlineUpdatePlayerInfo::GetSimulateResult() const
{
	return "{\"state\":1, \"name\":\"iPod touch\"}";
}

OnlineUpdatePlayerInfo * OnlineUpdatePlayerInfo::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineUpdatePlayerInfo(observer);
}

const OnlineRespUpdatePlayerInfo& OnlineUpdatePlayerInfo::GetResponse()
{
	return m_resp;
}