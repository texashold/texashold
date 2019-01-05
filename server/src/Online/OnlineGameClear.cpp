#include "stdafx.h"
#include "OnlineGameClear.h"
#include "GameController.h"

OnlineGameClear::OnlineGameClear(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineGameClear::Send(int roomCode, const char *result, NoticeDetail& noticeDetail)
{
	std::string query;
	
	AppendFormat(query, "room_id=%d", roomCode);
	AppendFormat(query, "&result=%s", result);

	AppendFormat(query, "&notice_type=%d", noticeDetail.type);
	AppendFormat(query, "&param0=%d", noticeDetail.userId);
	AppendFormat(query, "&param1=%lld", noticeDetail.param1);
	AppendFormat(query, "&param2=%d", noticeDetail.param2);
	return SendRequest(query);
}

void OnlineGameClear::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		if (m_observer)
		{
			m_observer->OnRequestFailed(this, GetFunctionID(), ret);
		}
	}
	else
	{
		Json::Value jav = resp["player_chip"];
		if(jav.isArray())
		{
			for(int i = 0; i < (int)jav.size(); i++)
			{
				Json::Value jv = jav[i];
				if(jv.isObject())
				{
					int   uid  = 0;
					int   power = K_DEFAULT_CARD_POWER;
					tChip chip = 0;
					char  chipStr[64] = "";
					GetJsonValue(jv, "userid", uid, false);
					GetJsonValue(jv, "chip", chipStr, false);
					GetJsonValue(jv, "power", power, false);

					//items
					Json::Value items = jv["item"];
					if (items.isArray())
					{
						int num = items.size();
						for ( int j = 0; j < num; ++j)
						{
							Json::Value jItem = items[j];
							if(jItem.isObject())
							{
								int itemID = 0;
								int chip = 0;
								if (!GetJsonValue(jItem, "itemid", itemID, false) || !GetJsonValue(jItem, "itemchip", chip, false))
								{
									continue;
								}
								m_resp.playerChip[uid].items[itemID] = chip;
							}
						}
					}

					if(uid <= 0)
					{
						continue;
					}
					if(*chipStr == '\0')
					{
						continue;
					}
					chip = atol(chipStr);
					m_resp.playerChip[uid].chip = chip;
					m_resp.playerChip[uid].power = power;

					XLOG_INFO("Update player all chip %lld for player %d", chip, uid);
				}
			}
		}
		if (m_observer)
		{
			m_observer->OnRequestSuccess(this, GetFunctionID());
		}
	}
}

int OnlineGameClear::GetFunctionID() const
{
	return OnlineFunctionGameClear;
}

const char *OnlineGameClear::GetFunctionName() const
{
	return "game_clear";
}

OnlineGameClear * OnlineGameClear::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineGameClear(observer);
}

const OnlineRespGameClear& OnlineGameClear::GetResponse()
{
	return m_resp;
}