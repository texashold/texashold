#include "stdafx.h"
#include "OnlineGiveChip.h"

OnlineGiveChip::OnlineGiveChip(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineGiveChip::Send(int from, int to, int chip)
{
	std::string query;

	AppendFormat(query, "payuserId=%d&", from);
	AppendFormat(query, "getuserId=%d&", to);
	AppendFormat(query, "chip=%d", chip);

	return SendRequest(query);
}

void OnlineGiveChip::OnRequestSuccess(Json::Value &resp)
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

int OnlineGiveChip::GetFunctionID() const
{
	return OnlineFunctionGiveChip;
}

const char *OnlineGiveChip::GetFunctionName() const
{
	return "give_chip";
}

OnlineGiveChip * OnlineGiveChip::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineGiveChip(observer);
}