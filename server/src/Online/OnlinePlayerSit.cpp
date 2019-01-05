#include "stdafx.h"
#include "OnlinePlayerSit.h"

OnlinePlayerSit::OnlinePlayerSit(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlinePlayerSit::Send(int roomCode, int playerId, long long carryChip, int inviterID, const char *clientIp, const char *session,  int hasSeat)
{
	SetIParam1(roomCode);
	SetIParam2(playerId);

	std::string query;
	AppendFormat(query, "room_id=%d&", roomCode);
	AppendFormat(query, "player_id=%d&", playerId);
	AppendFormat(query, "player_chip=%lld&", carryChip);
	AppendFormat(query, "inviter_id=%d&", inviterID);
	AppendFormat(query, "has_seat=%d&", hasSeat);
	AppendFormat(query, "client_ip=%s&",  clientIp);
	AppendFormat(query, "player_sessionid=%s",  session);

	return SendRequest(query);
}

void OnlinePlayerSit::OnRequestSuccess(Json::Value &resp)
{
	if(GetIParam1() == -2)
	{
		//this is only a notify, do nothing
		XLOG_INFO("Notify server %d resit successfully", GetIParam2());
		return;
	}

	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
		if(!GetJsonValue(resp, "has_seat", m_resp.m_hasSeat))
		{
			return;
		}
		if (m_resp.m_hasSeat == 2) //reconnection
		{
			//do nothing
		}
		else 
		{
			if(!GetJsonValue(resp, "player_name", m_resp.m_playerName))
			{
				return;
			}

			if(!GetJsonValue(resp, "power", m_resp.m_cardPower))
			{
				return;
			}

			GetJsonValue(resp, "equipe_title", m_resp.m_equipeTitle, false);

			Json::Value items = resp["item"];
			if (items.isArray())
			{
				int num = items.size();
				for ( int i = 0; i < num; ++i)
				{
					Json::Value jv = items[i];
					if(jv.isObject())
					{
						int itemID = 0;
						int chip = 0;
						if (!GetJsonValue(jv, "itemid", itemID, false) || !GetJsonValue(jv, "itemchip", chip, false))
						{
							continue;
						}
						m_resp.items[itemID] = chip;
					}
				}
			}

			GetJsonValue(resp, "player_face", m_resp.m_equipeAvatar, false);
			GetJsonValue(resp, "best_card", m_resp.m_maxHand, false);
			GetJsonValue(resp, "task_id", m_resp.m_inGameTaskID, false);
			GetJsonValue(resp, "skin", m_resp.m_skin, false);
			if (!GetJsonValue(resp, "player_sex", m_resp.m_sex, false))
			{
				m_resp.m_sex = 2;
			}

			if(m_resp.m_inGameTaskID > 0)
			{
				if(!GetJsonValue(resp, "task_onlinetimeneed", m_resp.m_inGameTaskTime))
				{
					return;
				}
			}
		}
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlinePlayerSit::GetFunctionID() const
{
	return OnlineFunctionPlayerSit;
}

const char *OnlinePlayerSit::GetFunctionName() const
{
	return "player_sit";
}

const char *OnlinePlayerSit::GetSimulateResult() const
{
	return "{\"player_name\":\"iPod touch\",\"equipe_jewelry\":0,\"player_face\":\"MjAwMThfMw==\",\"equipe_item\":0,\"equipe_vip\":0,\"equipe_car\":0,\"equipe_house\":0,\"state\":1,\"task_id\":2,\"best_card\":\"0E0A080702\",\"equipe_title\":0,\"task_onlinetimeneed\":0,\"power\":100}";
}

OnlinePlayerSit * OnlinePlayerSit::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlinePlayerSit(observer);
}

const OnlineRespPlayerSit& OnlinePlayerSit::GetResponse()
{
	return m_resp;
}