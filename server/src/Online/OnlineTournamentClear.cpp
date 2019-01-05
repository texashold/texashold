#include "stdafx.h"
#include "OnlineTournamentClear.h"

OnlineTournamentClear::OnlineTournamentClear(OnlineObserver *observer)
: OnlineComponent(observer)
{

}

bool OnlineTournamentClear::Send(const char *results)
{
	std::string query;
	
	return SendRequest(results);
}

void OnlineTournamentClear::OnRequestSuccess(Json::Value &resp)
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
		Json::Value jav = resp["profits"];
		if(jav.isArray())
		{
			for(int i = 0; i < (int)jav.size(); i++)
			{
				Json::Value jv = jav[i];
				if(jv.isObject())
				{
					Profit profit;
					char  profitStr[64] = "";
					GetJsonValue(jv, "player_id", profit.playerID, false);
					GetJsonValue(jv, "profit", profitStr, false);

					if(profit.playerID <= 0)
					{
						continue;
					}
					if(*profitStr == '\0')
					{
						continue;
					}
					profit.profit = atol(profitStr);

					GetJsonValue(jv, "credit", profit.credit, false);
					GetJsonValue(jv, "exp", profit.exp, false);

					m_resp.profits.push_back(profit);
				}
			}
		}

		if (m_observer)
		{
			m_observer->OnRequestSuccess(this, GetFunctionID());
		}
	}
}

int OnlineTournamentClear::GetFunctionID() const
{
	return OnlineFunctionTournamentClear;
}

const char *OnlineTournamentClear::GetSimulateResult() const
{
// #ifdef _DEBUG
// 	return "{\"state\":1,\"profits\":[{\"exp\":56,\"profit\":400000,\"player_id\":165045,\"credit\":3},{\"exp\":56,\"profit\":125000,\"player_id\":165069,\"credit\":2}]}";
// #else
	return "{\"state\":1}";
//#endif
}

const char *OnlineTournamentClear::GetFunctionName() const
{
	return "tournament_clear";
}

OnlineTournamentClear * OnlineTournamentClear::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineTournamentClear(observer);
}

const OnlineRespTournamentClear& OnlineTournamentClear::GetResponse()
{
	return m_resp;
}