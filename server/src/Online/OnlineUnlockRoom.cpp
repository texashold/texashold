#include "stdafx.h"
#include "OnlineUnlockRoom.h"

OnlineUnlockRoom::OnlineUnlockRoom(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineUnlockRoom::Send(int playerID, int roomCode)
{
	std::string query;
	
	AppendFormat(query, "player_id=%d&", playerID);
	AppendFormat(query, "room_id=%d", roomCode);

	return SendRequest(query);
}

void OnlineUnlockRoom::OnRequestSuccess(Json::Value &resp)
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

int OnlineUnlockRoom::GetFunctionID() const
{
	return OnlineFunctionUnlockRoom;
}

const char *OnlineUnlockRoom::GetFunctionName() const
{
	return "unlock_room";
}

const char *OnlineUnlockRoom::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineUnlockRoom * OnlineUnlockRoom::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineUnlockRoom(observer);
}

const OnlineRespUnlockRoom& OnlineUnlockRoom::GetResponse()
{
	return m_resp;
}
