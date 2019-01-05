#include "stdafx.h"
#include "OnlineTranslate.h"

OnlineTranslate::OnlineTranslate(OnlineObserver *observer)
: OnlineComponent(observer)
{
}

bool OnlineTranslate::Send(const char* keyfrom, const char* key, const char *text)
{
	std::string url = "http://fanyi.youdao.com/openapi.do?keyfrom=";
	url += keyfrom;
	url += "&key=";
    url += key;
    url += "&type=data&doctype=json&version=1.1&only=translate&q=";
	char buf[512] = "";
    XHttpConnection::URLEncode(text, buf, sizeof(buf));
    url += buf;    

	return m_httpEngine->SendByGet(url.c_str());
}

const Json::Value & OnlineTranslate::GetResponse()
{
    return m_resp;
}

void OnlineTranslate::OnRequestSuccess(Json::Value &resp)
{
	if(resp.size() > 0 && resp["errorCode"].asInt() == 0)//resp.size() > 0, avoid the json value is a number, etc.
	{
		m_resp = resp;
		m_observer->OnRequestSuccess(this, GetFunctionID());
	}
	else
	{
		if(resp.size() > 0)
			XLOG_ERROR("Translate failed with error %d", resp["errorCode"].asInt());
		else
			XLOG_ERROR("Translate failed in header decode");
		m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
	}
}

int OnlineTranslate::GetFunctionID() const
{
	return OnlineFunctionTranslate;
}

const char *OnlineTranslate::GetFunctionName() const
{
	return "OnlineTranslate";
}

OnlineTranslate * OnlineTranslate::CreateRequest(OnlineObserver *observer)
{
	return XL_NEW OnlineTranslate(observer);
}