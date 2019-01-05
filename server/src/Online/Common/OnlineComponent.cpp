#include "stdafx.h"
#include "OnlineComponent.h"
#include "IO/XFileStream.h"
#include "XConsoleApplication.h"
#include <stdarg.h>
#include "ServerAddr.h"
#include "GameController.h"

OnlineRequestManager* OnlineComponent::m_onlineReqtMgr = NULL;

/**
* OnlineComponent constructor. 
*/
OnlineComponent::OnlineComponent(OnlineObserver *observer)
{
	m_observer          = observer;

	m_iParam1 = 0;
	m_iParam2 = 0;
	m_pParam  = NULL;

	m_httpEngine = XL_NEW XHttpConnection(this, this);

	if(m_onlineReqtMgr == NULL)
	{
		m_onlineReqtMgr = XL_NEW OnlineRequestManager();
	}
	m_onlineReqtMgr->PushRequest(this);

	m_pureHTTPRequest = false;
	m_addr = NULL;

	m_bPost = false;
}

/**
* OnlineComponent destructor. 
*/
OnlineComponent::~OnlineComponent(void)
{	

}

void OnlineComponent::RegisterObserver( OnlineObserver * observer )
{
	m_observer = observer;
}

const OnlineObserver *OnlineComponent::GetObserver() const
{
	return m_observer;
}

void OnlineComponent::Close()
{
	m_httpEngine->Cancel();
}

bool OnlineComponent::IsInProcess()
{
	return m_httpEngine->IsProcessing();
}

void OnlineComponent::OnConnectionSuccess(XHttpConnection * httpCon, const void *data, int dataLen)
{
	const char *pResp = (const char *)data;

	Json::Reader json_reader;
	Json::Value  json_value;

	if(json_reader.parse(pResp, json_value))
	{
		OnRequestSuccess(json_value);
	}
	else
	{
		XLOG_ERROR("json parse failed!");
		if(m_observer)
		{
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
		}
	}

	ServerAddr::Instance()->ResetFailedTime((char *)m_addr);

	Close();
}

void OnlineComponent::OnConnectionFailed(XHttpConnection * httpCon, int error_code)
{

	if(error_code == X_HTTP_ERROR_TIMEOUT)
	{
		if (m_observer)
		{
			m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorTimeOut);
		}

	}
	else
	{
#if 0
		//m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorNetWork);
#else
		ServerAddr::Instance()->HandleFailedAddr((char *)m_addr);
		XLOG_WARNING("Connect failed, try to connect other addr ...");
		SendRequest(m_query, m_bPost);
		return;
#endif
	}

	ServerAddr::Instance()->ResetFailedTime((char *)m_addr);

	Close();
}

void OnlineComponent::AppendFormat(std::string &dest, const char *format, ...)
{
	va_list   vl; 
	static char   buffer[2048];     //根据实际情况定大小 

	va_start(vl,   format); 
	vsprintf(buffer,   format,   vl); 
	va_end(vl); 

	dest.append(buffer);
}

bool OnlineComponent::SendRequest(std::string query, bool post)
{

	m_addr = ServerAddr::Instance()->GetAddr();
	if (NULL == m_addr)
	{
		XConsoleApplication *app =  (XConsoleApplication *)XApplication::Instance();
		app->Exit();
		return false;
	}
	

	m_query = query;
	m_bPost = post;

	std::string url(m_addr);
	url.append(GetFunctionName());

	XLOG_DEBUG("");


#ifdef DEBUG_SELF_LAUNCH
	const char *result = GetSimulateResult();
	XLOG_DEBUG("Http Response:%s", result);
	OnConnectionSuccess(m_httpEngine, result, strlen(result+1));;
#else
	if(post)
	{
		XLOG_DEBUG("Send Post Request url:%s", url.c_str());
		XLOG_DEBUG("Send Post Request query:%s", query.c_str());
		XHttpRequestHeader httpHeader;
		httpHeader.SetHeader("Content-Length", query.length());
		m_httpEngine->SendByPost(url.c_str(), query.c_str(), query.length(), httpHeader);
	}
	else
	{
		query.insert(0, "?");
		query.insert(0, url);
		XLOG_DEBUG("Send Get Request:%s", query.c_str());
		m_httpEngine->SendByGet(query.c_str(), XHttpRequestHeader());
	}
#endif
	return true;
}

const char *OnlineComponent::GetSimulateResult() const
{
	return "{\"state\":1}";
}

bool OnlineComponent::GetJsonValue(const Json::Value &json, const char *key, int &out, bool required /* = true */)
{
	out = 0;

	const Json::Value json_value = json[key];
	if(json_value.isInt())
	{
		out = json_value.asInt();
		return true;
	}
	
	if(required)
	{
		XLOG_ERROR("No correct response of \"%s\"", key);
		m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
	}
	return false;
}

bool OnlineComponent::GetJsonValue(const Json::Value &json, const char *key, char *out, bool required /* = true */)
{
	*out = 0;

	const Json::Value json_value = json[key];
	if(json_value.isString())
	{
		strcpy(out, json_value.asCString());
		return true;
	}

	if(required)
	{
		XLOG_ERROR("No correct response of \"%s\"", key);
		m_observer->OnRequestFailed(this, GetFunctionID(), OnlinePErrorBadResponse);
	}
	return false;
}

OnlineRequestManager *OnlineComponent::GetOnlineReqtMgr()
{
	if(m_onlineReqtMgr == NULL)
	{
		m_onlineReqtMgr = XL_NEW OnlineRequestManager();
	}
	return m_onlineReqtMgr;
}

void OnlineComponent::SetIParam1(int iParam1)
{
	m_iParam1 = iParam1;
}

void OnlineComponent::SetIParam2(int iParam2)
{
	m_iParam2 = iParam2;
}

void OnlineComponent::SetPParam(void *pParam)
{
	m_pParam = pParam;
}

int OnlineComponent::GetIParam1() const
{
	return m_iParam1;
}

int OnlineComponent::GetIParam2() const
{
	return m_iParam2;
}

void *OnlineComponent::GetPParam() const
{
	return m_pParam;
}

void OnlineComponent::ResetParam()
{
	m_iParam1 = 0;
	m_iParam2 = 0;
	m_pParam  = NULL;
}