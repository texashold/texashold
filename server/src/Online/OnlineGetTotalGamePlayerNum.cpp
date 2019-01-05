#include "stdafx.h"
#include "OnlineGetTotalGamePlayerNum.h"

OnlineGetTotalGamePlayerNum::OnlineGetTotalGamePlayerNum(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineGetTotalGamePlayerNum::Send()
{
	std::string query;

	return SendRequest(query);
}

void OnlineGetTotalGamePlayerNum::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		Json::Value json_value;

		json_value = resp["needplayer"];
		if(!json_value.isInt())
		{
			XLOG_ERROR("No correct response of \"needplayer\"");
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
			return;
		}
		m_resp = json_value.asInt();
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineGetTotalGamePlayerNum::GetFunctionID() const
{
	return OnlineFunctionGetTotalGamePlayerNum;
}

const char *OnlineGetTotalGamePlayerNum::GetFunctionName() const
{
	return "need_player";
}

const char *OnlineGetTotalGamePlayerNum::GetSimulateResult() const
{
	return "{\"state\":1,\"needplayer\":10000}";
}

OnlineGetTotalGamePlayerNum * OnlineGetTotalGamePlayerNum::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineGetTotalGamePlayerNum(observer);
}