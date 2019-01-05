#include "stdafx.h"
#include "ServerAddr.h"
#include "IO/XFileStream.h"
#include <iostream>
#include <fstream>
#include "XApplication.h"
#include "OnlineTryInvalidAddr.h"


static const int K_MAX_ADDR_LENGTH					= 256;
static const int K_MAX_FAILED_TIMES					= 3;
static const int K_TRY_INVALID_ADDR_TIME_INTERVAL	= 3*60*1000;

ServerAddr* ServerAddr::g_serverAddr = NULL;

ServerAddr* ServerAddr::Instance()
{
	if (NULL == g_serverAddr)
	{
		g_serverAddr = XL_NEW ServerAddr();
	}
	return g_serverAddr;
}

ServerAddr::ServerAddr(XObject *parent) : XObject(parent)
{
	LoadAddr();
	ScheduleTask(SCHEDULE_TASK_RETRY_INVALID_ADDR, K_TRY_INVALID_ADDR_TIME_INTERVAL, 0, NULL, -1);
}

ServerAddr::~ServerAddr()
{
	//free valid addr
	std::vector<char *>::iterator it = m_validAddr.begin();
	for (; it != m_validAddr.end(); ++it)
	{
		XL_DEL_ARRAY(*it);
	}

	//free invalid addr
	it = m_invalidAddr.begin();
	for (; it != m_invalidAddr.end(); ++it)
	{
		XL_DEL_ARRAY(*it);
	}
}

void ServerAddr::Update()
{

	XObject::Update();
}

void ServerAddr::LoadAddr()
{

	FILE *f = fopen(XFileStream::PathInConfig("server.conf"), "rt");

	if(f == NULL)
	{
		XLOG_ERROR("Can't open server config file %s", XFileStream::PathInConfig("server.conf"));
		return;
	}

	char buffer[K_MAX_ADDR_LENGTH] = {0};
	while(!feof(f))
	{
		fscanf(f, "%s", buffer);
		char *addr = XL_NEW char[K_MAX_ADDR_LENGTH];
		strcpy(addr, buffer);
		m_validAddr.push_back(addr);
		m_failedTime[addr] = 0;
		XLOG_DEBUG("Load server addr %s", addr);
	}
	fclose(f);

}

char* ServerAddr::GetAddr() const
{
	if (m_validAddr.empty())
	{
		XLOG_ERROR("Can't find valid server addr!");
		return NULL;
	}
	return (char *)m_validAddr.front();;
}

void ServerAddr::HandleFailedAddr(char *addr)
{
	if (m_failedTime.find(addr) == m_failedTime.end())
	{
		XLOG_ERROR("Can't find server addr %s for HandleFailedAddr()", addr);
		return;
	}
	
	++m_failedTime[addr];

	if (m_failedTime[addr] == K_MAX_FAILED_TIMES)
	{
		XLOG_WARNING("Addr %s failed %d times, move to invalid list!", addr, K_MAX_FAILED_TIMES);
		MoveAddr(m_validAddr, m_invalidAddr, addr);
	}
	
}

void ServerAddr::ResetFailedTime(char *addr)
{
	if (m_failedTime.find((char *)addr) == m_failedTime.end())
	{
		XLOG_ERROR("Can't find server addr %s for ResetFailedTime()", addr);
		return;
	}

	m_failedTime[addr] = 0;

	RemoveTryRequest(addr);
	MoveAddr(m_invalidAddr, m_validAddr, addr);
}

void ServerAddr::MoveAddr(std::vector<char *> &from, std::vector<char *> &to, char *addr)
{
	std::vector<char *>::iterator it = from.begin();
	while(it != from.end())
	{
		if (*it == addr)
		{
			it = from.erase(it);
			to.insert(to.begin(), addr);
			break;
		}
		else
		{
			++it;
		}
	}
}

int ServerAddr::GetValidAddrCount() const
{
	return (int)m_validAddr.size();
}


void ServerAddr::OnScheduleTimeout(int tid, int iParam, void *pParam, int overTime)
{
	switch(tid)
	{
	case SCHEDULE_TASK_RETRY_INVALID_ADDR:
		std::vector<char *>::const_iterator it = m_invalidAddr.begin();
		for (; it != m_invalidAddr.end(); ++it)
		{
			std::map<char *, OnlineTryInvalidAddr *>::iterator iter = m_invalidAddrTryRequests.find(*it);
			if (iter != m_invalidAddrTryRequests.end())
			{
				if (!iter->second->IsInProcess())
				{
					XL_DEL(iter->second);
					m_invalidAddrTryRequests.erase(iter);
				}
				else
				{
					continue;
				}
			}
			OnlineTryInvalidAddr *request = XL_NEW OnlineTryInvalidAddr(this);
			request->Send(*it);
			m_invalidAddrTryRequests[*it] = request;
		}
		break;
	}
}

void ServerAddr::RemoveTryRequest(char *addr)
{
	std::map<char *, OnlineTryInvalidAddr *>::iterator it = m_invalidAddrTryRequests.find(addr);
	if (it != m_invalidAddrTryRequests.end())
	{
		XL_DEL(it->second);
		m_invalidAddrTryRequests.erase(it);
	}
}