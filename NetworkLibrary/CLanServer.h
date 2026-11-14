#pragma once
#include <windows.h>
#include <map>
#include <vector>

class CPacket;

struct SESSION;

class CLANSERVER
{
public:
	bool Start(unsigned short port, unsigned int workerCnt, bool nagleFlag, unsigned int maxUserNum);	// 오픈 IP, Port, WorkerCnt, nagleOption, MaxUserNum
	void Stop();
	int GetSessionCount(); // Session과 Player는 다른 개념.

	bool Disconnect(unsigned long long sessionID);
	bool SendPacket(unsigned long long sessionID, CPacket* packet);

	virtual bool OnConnectionRequest(SOCKADDR_IN clientAddr) = 0;
	virtual void OnAccept(unsigned long long sessionID) = 0;
	virtual void OnRelease(unsigned long long sessionID) = 0;
	virtual void OnMessage(unsigned long long sessionID, CPacket* packet) = 0;
	virtual void OnError(int errcode, char*) = 0;



	int getAcceptTPS();
	int getRecvMessageTPS();
	int getSendMessageTPS();

private:

	static unsigned int _stdcall Entry_Accept_Thread(void* args);
	static unsigned int _stdcall Entry_IOCP_Worker_Thread(void* args);

	unsigned int _stdcall Accept_Thread();
	unsigned int _stdcall IOCP_Worker_Thread();
	
	bool SendPost(SESSION* tempSession);
	bool RecvPost(SESSION* tempSession);
	bool DeleteSession(SESSION* tempSession);

	unsigned long long sessionCount;
	unsigned int workerCnt;

	SOCKET listenSocket;
	HANDLE* iocpWorkerThreadArr;
	HANDLE acceptThreadHandle;
	HANDLE iocpHandle;

	SESSION** sessionArr;
	std::vector<unsigned long long>* freeList;

	CRITICAL_SECTION cs_freeList;


};