#ifndef __ONLINE_TRY_INVALID_ADDR_H__
#define __ONLINE_TRY_INVALID_ADDR_H__

#include "Http/XHttpConnection.h"
#include "Online/OnlineDefines.h"

class OnlineTryInvalidAddr: public XObject, public XHttpConnectionObserver
{
public:
	OnlineTryInvalidAddr(XObject *parent);
	virtual ~OnlineTryInvalidAddr();	

	void Close();
	bool IsInProcess();

	//Interface from XHttpConnectionObserver
	virtual void OnConnectionSuccess(XHttpConnection * httpCon, const void *data, int dataLen);
	virtual void OnConnectionFailed(XHttpConnection * httpCon, int error_code);

	const char *GetAddr() const;

	bool Send(const char *addr);

private:
	virtual const char *GetFunctionName() const;
	virtual const char *GetSimulateResult() const;


	//Http Engine
	XHttpConnection * m_httpEngine;

	bool      m_pureHTTPRequest;

	//server addr
	const char	  *m_addr;
		
};


#endif