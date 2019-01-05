#ifndef _SERVER_ADDR_H_
#define _SERVER_ADDR_H_

#include <string>
#include <vector>
#include <map>


class OnlineTryInvalidAddr;
class ServerAddr : public XObject
{
private:
	ServerAddr(XObject *parent = NULL);
	virtual ~ServerAddr();

public:

	static ServerAddr *Instance();

	//Interface from XObject
	virtual void Update();

	char *GetAddr() const;
	void HandleFailedAddr(char *addr);
	void ResetFailedTime(char *addr);

	int GetValidAddrCount() const;
private:
	static ServerAddr *g_serverAddr;

	void LoadAddr();
	void MoveAddr(std::vector<char *> &from, std::vector<char *> &to, char *addr);

	void RemoveTryRequest(char *addr);

	enum SCHEDULE_TASK{
		SCHEDULE_TASK_RETRY_INVALID_ADDR = 2013,

	};

	void OnScheduleTimeout(int tid, int iParam, void *pParam, int overTime);

	std::vector<char *> m_validAddr;
	std::vector<char *> m_invalidAddr;
	std::map<char *, int> m_failedTime;
	std::map<char *, OnlineTryInvalidAddr *> m_invalidAddrTryRequests;
};

#endif