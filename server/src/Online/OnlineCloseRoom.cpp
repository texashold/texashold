#include "stdafx.h"
#include "OnlineCloseRoom.h"

OnlineCloseRoom::OnlineCloseRoom(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineCloseRoom::Send(int roomCode)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d", roomCode);

	return SendRequest(query);
}

void OnlineCloseRoom::OnRequestSuccess(Json::Value &resp)
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

int OnlineCloseRoom::GetFunctionID() const
{
	return OnlineFunctionCloseRoom;
}

const char *OnlineCloseRoom::GetFunctionName() const
{
	return "close_room";
}

OnlineCloseRoom * OnlineCloseRoom::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineCloseRoom(observer);
}