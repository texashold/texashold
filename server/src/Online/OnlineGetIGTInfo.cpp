#include "stdafx.h"
#include "OnlineGetIGTInfo.h"

OnlineGetIGTInfo::OnlineGetIGTInfo(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineGetIGTInfo::Send(int playerId)
{
	std::string query;
	
	AppendFormat(query, "player_id=%d", playerId);

	return SendRequest(query);
}

void OnlineGetIGTInfo::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		Json::Value json_value;
		GetJsonValue(resp, "task_id", m_resp.m_inGameTaskID, false);
		if(m_resp.m_inGameTaskID > 0)
		{
			if(!GetJsonValue(resp, "task_onlinetimeneed", m_resp.m_inGameTaskTime))
			{
				return;
			}
		}
		
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineGetIGTInfo::GetFunctionID() const
{
	return OnlineFunctionGetIGTInfo;
}

const char *OnlineGetIGTInfo::GetFunctionName() const
{
	return "online_task";
}

const char *OnlineGetIGTInfo::GetSimulateResult() const
{
	return "{\"state\":1,\"task_id\":2,\"task_onlinetimeneed\":0}";
}

OnlineGetIGTInfo * OnlineGetIGTInfo::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineGetIGTInfo(observer);
}

const OnlineRespGetIGTInfo& OnlineGetIGTInfo::GetResponse()
{
	return m_resp;
}