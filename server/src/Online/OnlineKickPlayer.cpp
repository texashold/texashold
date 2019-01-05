#include "stdafx.h"
#include "OnlineKickPlayer.h"

OnlineKickPlayer::OnlineKickPlayer(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineKickPlayer::Send(int srcID, int dstID, int roomCode)
{
	std::string query;
	
	AppendFormat(query, "player_id=%d&", srcID);
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "kicked_id=%d", dstID);

	return SendRequest(query);
}

void OnlineKickPlayer::OnRequestSuccess(Json::Value &resp)
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

int OnlineKickPlayer::GetFunctionID() const
{
	return OnlineFunctionKickPlayer;
}

const char *OnlineKickPlayer::GetFunctionName() const
{
	return "kick_player";
}

const char *OnlineKickPlayer::GetSimulateResult() const
{
	return "{\"state\":1}";
}

OnlineKickPlayer * OnlineKickPlayer::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineKickPlayer(observer);
}

const OnlineRespKickPlayer& OnlineKickPlayer::GetResponse()
{
	return m_resp;
}

const char* OnlineKickPlayer::GetSrcName() const
{
	return m_srcName.c_str();
}

const char* OnlineKickPlayer::GetDstName() const
{
	return m_dstName.c_str();
}

void OnlineKickPlayer::SetSrcName(const char *srcName)
{
	m_srcName = srcName;
}

void OnlineKickPlayer::SetDstName(const char *dstName)
{
	m_dstName = dstName;
}