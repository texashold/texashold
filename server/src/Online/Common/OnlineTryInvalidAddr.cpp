#include "stdafx.h"
#include "OnlineTryInvalidAddr.h"
#include "XConsoleApplication.h"
#include "ServerAddr.h"


/**
* OnlineTryInvalidAddr constructor. 
*/
OnlineTryInvalidAddr::OnlineTryInvalidAddr(XObject *parent) : XObject(parent)
{
	m_httpEngine = XL_NEW XHttpConnection(this, this);

	m_pureHTTPRequest = false;
	m_addr = NULL;
}

/**
* OnlineTryInvalidAddr destructor. 
*/
OnlineTryInvalidAddr::~OnlineTryInvalidAddr()
{	
	Close();
}

void OnlineTryInvalidAddr::Close()
{
	m_httpEngine->Cancel();
}

bool OnlineTryInvalidAddr::IsInProcess()
{
	return m_httpEngine->IsProcessing();
}

void OnlineTryInvalidAddr::OnConnectionSuccess(XHttpConnection * httpCon, const void *data, int dataLen)
{
	XLOG_INFO("Try addr %s success.", m_addr);

	ServerAddr::Instance()->ResetFailedTime((char *)m_addr);
}

void OnlineTryInvalidAddr::OnConnectionFailed(XHttpConnection * httpCon, int error_code)
{
// 	if(error_code == X_HTTP_ERROR_TIMEOUT)
// 	{
// 		ServerAddr::Instance()->ResetFailedTime((char *)m_addr);
// 	}
	XLOG_INFO("Try addr %s failed, error_code %d.", m_addr, error_code);
}


bool OnlineTryInvalidAddr::Send(const char *addr)
{
	m_addr = addr;
	if (NULL == m_addr)
	{
		return false;
	}
	
	std::string url(m_addr);
	url.append(GetFunctionName());

#ifdef DEBUG_SELF_LAUNCH
	const char *result = GetSimulateResult();
	XLOG_DEBUG("Http Response:%s", result);
	OnConnectionSuccess(m_httpEngine, result, strlen(result+1));;
#else
	XLOG_DEBUG("Send Get Request:%s", url.c_str());
	m_httpEngine->SendByGet(url.c_str(), XHttpRequestHeader());
#endif
	return true;
}

const char* OnlineTryInvalidAddr::GetFunctionName() const
{
	return "checkConn";
}

const char *OnlineTryInvalidAddr::GetSimulateResult() const
{
	return "1";
}

const char* OnlineTryInvalidAddr::GetAddr() const
{
	return m_addr;
}