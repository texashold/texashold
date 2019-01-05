#ifndef _ONLINE_REQUEST_TRANSLATE_H_
#define _ONLINE_REQUEST_TRANSLATE_H_

#include "Online/Common/OnlineComponent.h"
#include "Online/jsoncpp/include/json/json.h"
#include "CSCommonDefine.h"

class OnlineTranslate : public OnlineComponent
{
public:
	static OnlineTranslate * CreateRequest(OnlineObserver *observer);

	bool Send(const char* keyfrom, const char* key, const char *text);
	const Json::Value &GetResponse();
protected:
	virtual void OnRequestSuccess(Json::Value &resp);
	virtual int  GetFunctionID() const;
	virtual const char *GetFunctionName() const;
private:
	//Constructor must be private, create instance with CreateQuest()
	OnlineTranslate(OnlineObserver *observer);

	Json::Value m_resp;
};

#endif