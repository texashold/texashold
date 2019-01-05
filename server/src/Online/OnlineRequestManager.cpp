#include "stdafx.h"
#include "OnlineRequestManager.h"
#include "Common/OnlineComponent.h"
#include "XApplication.h"
#include "XGame.h"
#include "Online/Common/OnlineObserver.h"

OnlineRequestManager::OnlineRequestManager()
{
	XApplication::Instance()->AddChild(this);
}

OnlineRequestManager::~OnlineRequestManager()
{

}

void OnlineRequestManager::PushRequest(OnlineComponent *request)
{
	AddChild(request);

	std::list<OnlineComponent*>::iterator it = m_requestQueue.begin();
	for(; it != m_requestQueue.end(); ++it)
	{
		if(*it == request)
		{
			return;
		}
	}
	m_requestQueue.push_back(request);
}

void OnlineRequestManager::CancelRequest(OnlineComponent *request)
{
	std::list<OnlineComponent*>::iterator it = m_requestQueue.begin();
	for(; it != m_requestQueue.end(); ++it)
	{
		if(*it == request)
		{
			request->Close();
			m_requestQueue.erase(it);
			XL_DEL(request);
			return;
		}
	}
}

void OnlineRequestManager::CancelRequest(OnlineObserver *observer)
{
	std::list<OnlineComponent *>::iterator it = m_requestQueue.begin();
	for(; it != m_requestQueue.end(); ++it)
	{
		if((*it)->GetObserver() == observer)
		{
			(*it)->Close();
			//XL_DEL((*it));
			//m_requestQueue.erase(it);
			return;
		}
	}
}

void OnlineRequestManager::Update()
{
	XObject::Update();

	//Clear the not running request
	std::list<OnlineComponent*>::iterator it = m_requestQueue.begin();
	for(; it != m_requestQueue.end(); ++it)
	{
		if(!(*it)->IsInProcess())
		{
			OnlineComponent *request = *it;
			m_requestQueue.erase(it);
			XL_DEL(request);
			return;
		}
	}
}