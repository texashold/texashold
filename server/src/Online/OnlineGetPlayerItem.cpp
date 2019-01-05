#include "stdafx.h"
#include "OnlineGetPlayerItem.h"

OnlineGetPlayerItem::OnlineGetPlayerItem(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineGetPlayerItem::Send(int playerID)
{
	std::string query;

	AppendFormat(query, "userid=%d", playerID);

	return SendRequest(query);
}

void OnlineGetPlayerItem::OnRequestSuccess(Json::Value &resp)
{
	int ret = resp["state"].asInt();
	if (ret != OnlineNoError)
	{
		m_observer->OnRequestFailed(this, GetFunctionID(), ret);
	}
	else
	{
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

		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
}

int OnlineGetPlayerItem::GetFunctionID() const
{
	return OnlineFunctionGetPlayerItem;
}

const char *OnlineGetPlayerItem::GetFunctionName() const
{
	return "check_equip";
}

OnlineGetPlayerItem * OnlineGetPlayerItem::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineGetPlayerItem(observer);
}