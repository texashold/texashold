#include "stdafx.h"
#include "OnlineKeepAlive.h"

OnlineKeepALive::OnlineKeepALive(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineKeepALive::Send(int roomCode)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d", roomCode);

	return SendRequest(query);
}

void OnlineKeepALive::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		const Json::Value json_value = resp["notice"];
		if(json_value.isString())
		{
			m_resp.notice = json_value.asString();
			XLOG_DEBUG("Find notice:%s", m_resp.notice.c_str());
		}
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineKeepALive::GetFunctionID() const
{
	return OnlineFunctionKeepAlive;
}

const char *OnlineKeepALive::GetFunctionName() const
{
	return "room_beat";
}

OnlineKeepALive * OnlineKeepALive::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineKeepALive(observer);
}