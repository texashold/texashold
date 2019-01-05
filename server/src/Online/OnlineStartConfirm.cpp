#include "stdafx.h"
#include "OnlineStartConfirm.h"

OnlineStartConfirm::OnlineStartConfirm(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineStartConfirm::Send(int roomCode, unsigned short port, unsigned short udpPort)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "state=1&");
	AppendFormat(query, "port=%d&", port);
	AppendFormat(query, "udpport=%d", udpPort);

	return SendRequest(query);
}

void OnlineStartConfirm::OnRequestSuccess(Json::Value &resp)
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

int OnlineStartConfirm::GetFunctionID() const
{
	return OnlineFunctionStartConfirm;
}

const char *OnlineStartConfirm::GetFunctionName() const
{
	return "start_confirm";
}

OnlineStartConfirm * OnlineStartConfirm::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineStartConfirm(observer);
}