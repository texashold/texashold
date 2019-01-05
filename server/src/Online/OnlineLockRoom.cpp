#include "stdafx.h"
#include "OnlineLockRoom.h"

OnlineLockRoom::OnlineLockRoom(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineLockRoom::Send(int playerID, int roomCode)
{
	std::string query;
	
	AppendFormat(query, "player_id=%d&", playerID);
	AppendFormat(query, "room_id=%d", roomCode);

	return SendRequest(query);
}

void OnlineLockRoom::OnRequestSuccess(Json::Value &resp)
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

int OnlineLockRoom::GetFunctionID() const
{
	return OnlineFunctionLockRoom;
}

const char *OnlineLockRoom::GetFunctionName() const
{
	return "lock_room";
}

const char *OnlineLockRoom::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineLockRoom * OnlineLockRoom::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineLockRoom(observer);
}

const OnlineRespLockRoom& OnlineLockRoom::GetResponse()
{
	return m_resp;
}
