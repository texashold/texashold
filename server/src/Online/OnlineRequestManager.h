#ifndef _ONLINE_REQUEST_MANAGER_H_
#define _ONLINE_REQUEST_MANAGER_H_

#include <list>

class OnlineObserver;
class OnlineComponent;
class OnlineRequestManager : public XObject
{
public:
	OnlineRequestManager();
	~OnlineRequestManager();

	void PushRequest(OnlineComponent *request);
	void CancelRequest(OnlineComponent *request);
	void CancelRequest(OnlineObserver *observer);

	void Update();
private:
	std::list<OnlineComponent*>   m_requestQueue;
};
#endif