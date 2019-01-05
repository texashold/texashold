#ifndef __ONLINE_COMPONENT_H__
#define __ONLINE_COMPONENT_H__

#include "Http/XHttpConnection.h"
#include "OnlineObserver.h"
#include "Online/OnlineRequestManager.h"
#include "Online/OnlineDefines.h"
#include "jsoncpp/include/json/json.h"
#include <string>
#include <vector>

using std::vector;
using std::string;

class OnlineReqtLogin;
class OnlineComponent: public XObject, public XHttpConnectionObserver
{
	friend class OnlineTranslate;
public:
	OnlineComponent(OnlineObserver *observer);
	virtual ~OnlineComponent();	

	void Close();
	bool IsInProcess();

	//Interface from XHttpConnectionObserver
	virtual void OnConnectionSuccess(XHttpConnection * httpCon, const void *data, int dataLen);
	virtual void OnConnectionFailed(XHttpConnection * httpCon, int error_code);

	//Register observer
	void RegisterObserver( OnlineObserver * observer );
	const OnlineObserver *GetObserver() const;

	//User data
	void SetIParam1(int iParam1);
	void SetIParam2(int iParam2);
	void SetPParam(void *pParam);
	int  GetIParam1()const;
	int  GetIParam2()const;
	void*GetPParam()const;
	void ResetParam();

	static OnlineRequestManager *GetOnlineReqtMgr();
protected:
	//Pure virtual interface, must be implemented in derived class
	virtual void        OnRequestSuccess(Json::Value &resp) = 0;
	virtual int         GetFunctionID() const = 0;
	virtual const char *GetFunctionName() const = 0;
	virtual const char *GetSimulateResult() const;

	//json util
	bool GetJsonValue(const Json::Value &json, const char *key, int &out, bool required = true);
	bool GetJsonValue(const Json::Value &json, const char *key, char *out, bool required = true);

	void AppendFormat(std::string &dest, const   char   *format, ...);

	//Request action
	bool SendRequest(std::string query, bool post = false);

	//Observer
	OnlineObserver * m_observer;
private:
	//Request manager
	static OnlineRequestManager *m_onlineReqtMgr;

	//Http Engine
	XHttpConnection * m_httpEngine;

	bool      m_pureHTTPRequest;



	//User data
	int              m_iParam1;
	int              m_iParam2;
	void            *m_pParam;


	//server addr
	const char	  *m_addr;


	//request
	std::string      m_query;
	bool			 m_bPost;


		
};


#endif