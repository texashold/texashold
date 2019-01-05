#include "stdafx.h"
#include "OnlineSelectAIPlayerSit.h"

OnlineSelectAIPlayerSit::OnlineSelectAIPlayerSit(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineSelectAIPlayerSit::Send(int roomCode, long long minCarryChip)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "need_chip=%lld", minCarryChip);

	return SendRequest(query);
}

void OnlineSelectAIPlayerSit::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		Json::Value json_value;

		if(!GetJsonValue(resp, "aiid", m_resp.m_aiid))
		{
			return;
		}
		if(!GetJsonValue(resp, "ainame", m_resp.m_playerName))
		{
			return;
		}
		if(!GetJsonValue(resp, "power", m_resp.m_cardPower))
		{
			return;
		}

		GetJsonValue(resp, "aiface", m_resp.m_avatar, false);

		json_value = resp["aichip"];
		if(!json_value.isString())
		{
			XLOG_ERROR("No correct response of \"aichip\"");
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
			return;
		}
		m_resp.m_aichip = atol(json_value.asCString());

		json_value = resp["aivip"];
		if(!json_value.isInt())
		{
			XLOG_ERROR("No correct response of \"aivip\"");
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
			return;
		}
		m_resp.m_vipItem = json_value.asInt();

		json_value = resp["aiitem"];
		if(!json_value.isInt())
		{
			XLOG_ERROR("No correct response of \"aiitem\"");
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
			return;
		}
		m_resp.m_item = json_value.asInt();

		json_value = resp["aititle"];
		if(!json_value.isInt())
		{
			XLOG_ERROR("No correct response of \"aititle\"");
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
			return;
		}
		m_resp.m_title = json_value.asInt();

		json_value = resp["bestcard"];
		if(json_value.isString())
		{
			strcpy(m_resp.m_maxHand, json_value.asCString());
		}
		else
		{
			memset(m_resp.m_maxHand, 0, sizeof(m_resp.m_maxHand));
		}

		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineSelectAIPlayerSit::GetFunctionID() const
{
	return OnlineFunctionSelectAIPlayerSit;
}

const char *OnlineSelectAIPlayerSit::GetFunctionName() const
{
	return "aiplayersit";
}

const char *OnlineSelectAIPlayerSit::GetSimulateResult() const
{
	static char buffer[128];
	static int aiid = 1;
	sprintf(buffer, "{\"state\":1, \"aiid\":%d, \"best_card\":null, \"aichip\":\"400\", \"ainame\":\"Guest%d\", \"power\":100, \"aivip\":0, \"aiitem\":0, \"aititle\":0}", aiid, aiid);
	aiid++;
	return buffer;
}

OnlineSelectAIPlayerSit * OnlineSelectAIPlayerSit::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineSelectAIPlayerSit(observer);
}

const OnlineRespSelectAIPlayerSit& OnlineSelectAIPlayerSit::GetResponse()
{
	return m_resp;
}