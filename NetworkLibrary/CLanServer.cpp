#include <Winsock2.h>
#include <windows.h>
#include <iostream>
#include <utility>
#include <map>
#include <process.h>
#include "RingBuffer.h"
#include "SerializeBuffer.h"
#include <conio.h>
#include <list>
#include "CLanServer.h"
#include <numeric>

#pragma comment(lib, "Ws2_32.lib")

// STRUCT , CLASS

struct SESSION
{
	unsigned long long sessionID;
	SOCKET socket;
	CRingBuffer recvBuffer;
	CRingBuffer sendBuffer;
	OVERLAPPED recvOverlapped{};
	OVERLAPPED sendOverlapped{};
	unsigned int ioCount;
	bool isSend;
};

// FUNCTION

HANDLE CreateNewIOCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	return (CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0,
		dwNumberOfConcurrentThreads));
}

BOOL AssociateDeviceWithCompletionPort
(HANDLE hCompletionPort, HANDLE hDevice, void* dwCompletionKey)
{
	HANDLE h = CreateIoCompletionPort(hDevice, hCompletionPort, (unsigned long long)dwCompletionKey, 0);
	return (h == hCompletionPort);

}




// MAIN

bool CLANSERVER::Start(unsigned short port, unsigned int workerCnt, bool nagleFlag, unsigned short maxUserNum)
{
	// 네트워크 초기화
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddr;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;

	bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	listen(listenSocket, SOMAXCONN);

	this->workerCnt = workerCnt;

	// IOCP생성.
	iocpHandle = CreateNewIOCompletionPort(0);
	iocpWorkerThreadArr = new HANDLE[workerCnt];

	
	sessionArr = new SESSION*[maxUserNum];
	freeList = new std::vector<unsigned short>(maxUserNum);
	InitializeCriticalSection(&cs_freeList);


	std::iota(freeList->begin(), freeList->end(), unsigned short{ 0 });


	// Thread 생성.
	acceptThreadHandle = (HANDLE)_beginthreadex(
		nullptr,
		0,
		Entry_Accept_Thread,
		this,
		0,
		nullptr
	);

	
	for (int i = 0; i < workerCnt; i++)
	{
		iocpWorkerThreadArr[i] = (HANDLE)_beginthreadex(
			nullptr,
			0,
			Entry_IOCP_Worker_Thread,
			this,
			0,
			nullptr
		);
	}
	return true;

}

void CLANSERVER::Stop()
{
	
	closesocket(listenSocket);
	WaitForSingleObject(acceptThreadHandle, INFINITE);
	
	/*std::map<unsigned long long, SESSION*>::iterator iter = sessionMap.begin();
	for (; iter != sessionMap.end(); iter++)
	{
		closesocket(iter->second->socket);

	}*/

	for (int i = 0; i < workerCnt; i++)
	{
		PostQueuedCompletionStatus(iocpHandle, 0, 0, 0);
	}
	WaitForMultipleObjects(workerCnt, iocpWorkerThreadArr, TRUE, INFINITE);

	//iter = sessionMap.begin();
	//for (; iter != sessionMap.end(); iter++)
	//{
	//	DeleteSession(iter->second);
	//}

}

int CLANSERVER::GetSessionCount()
{
	return 0;
}

unsigned int _stdcall CLANSERVER::Entry_Accept_Thread(void* args)
{
	CLANSERVER* thisClass = static_cast<CLANSERVER*>(args);
	thisClass->Accept_Thread();

	return 0;
}

unsigned int _stdcall CLANSERVER::Entry_IOCP_Worker_Thread(void* args)
{
	CLANSERVER* thisClass = static_cast<CLANSERVER*>(args);
	thisClass->IOCP_Worker_Thread();
	return 0;
}

unsigned int _stdcall CLANSERVER::Accept_Thread()
{
	while (1)
	{
		WSABUF wsabuf[2];
		std::pair<unsigned long long, SESSION*> tempPair;
		SESSION* tempSession = new SESSION;
		SOCKET tempClientSocket;
		SOCKADDR_IN tempClientAddr;
		int tempLen = sizeof(tempClientAddr);

		tempClientSocket = accept(listenSocket, (sockaddr*)&tempClientAddr, &tempLen);

		if (tempClientSocket == INVALID_SOCKET)
		{
			// ERR
			std::cout << "Accept ERR" << std::endl;
			return 0;
		}

		if (OnConnectionRequest(tempClientAddr) == false)
		{
			std::cout << "Not Allow User" << std::endl;
			continue;
		}

		// freeList 동기화 필요.
		unsigned long long tempSessionID;


		EnterCriticalSection(&cs_freeList);
		unsigned short tempSessionIndex = freeList->back();
		freeList->pop_back();
		LeaveCriticalSection(&cs_freeList);

		// Session의 ID 는 Index와 sessionCount의 조합. 앞 2바이트는 ID 뒤 6바이트는 sessionCount.

		tempSessionID = (static_cast<unsigned long long>(tempSessionIndex) << 48) |
			(sessionCount & 0x0000FFFFFFFFFFFF);
		sessionCount++;



		tempSession->socket = tempClientSocket;
		tempSession->sessionID = tempSessionID;
		tempSession->ioCount = 1;
		tempSession->isSend = false;

		wsabuf[0].buf = tempSession->recvBuffer.GetRearBufferPtr();
		wsabuf[0].len = tempSession->recvBuffer.DirectEnqueueSize();
		/*wsabuf[1].buf = (char*)&tempSession->recvBuffer;
		wsabuf[1].len = tempSession->recvBuffer.getFreeSize() - tempSession->recvBuffer.DirectEnqueueSize();*/




		int retRecv = 0;
		unsigned long flags = 0;

		AssociateDeviceWithCompletionPort(
			iocpHandle,
			(HANDLE)tempSession->socket,
			(void*)tempSession
		);

		sessionArr[tempSessionIndex] = tempSession;

		retRecv = WSARecv(
			tempSession->socket,
			wsabuf,
			1,
			nullptr,
			&flags,
			&tempSession->recvOverlapped,
			nullptr
		);

		if (retRecv == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING)
			{
				// ERR
				std::cout << "Accept Thread retRecv Error : " << err << std::endl;
				return 0;
			}
		}

		
		OnAccept(tempSession->sessionID);

		
		
	}
	return 0;
}

unsigned int _stdcall CLANSERVER::IOCP_Worker_Thread()
{
	CPacket tempPacket;

	while (1)
	{
		DWORD transferred = 0;
		ULONG_PTR completionKey = 0;
		OVERLAPPED* tempOverlapped = nullptr;
		

		BOOL retGQCS = GetQueuedCompletionStatus(
			iocpHandle,
			&transferred,
			&completionKey,
			&tempOverlapped,
			INFINITE
		);

		if (tempOverlapped == nullptr)
		{
			if (!retGQCS)
			{
				// 완료 패킷을 꺼내지 못한 ERROR.
				std::cout << "FQCS False, NOT COMPLETE PACKET" << std::endl;
				return 0;
			}

			if (completionKey == 0 && transferred == 0)
			{
				// thread 종료 패킷 수신
				std::cout << "THREAD STOP RECV" << std::endl;
				return 0;
			}
			continue;
		}



		SESSION* tempSession = (SESSION*)completionKey;

		if (retGQCS == false)
		{
			// IO 취소 완료 통지
			if (GetLastError() == ERROR_OPERATION_ABORTED)
			{
				std::cout << "Decrement 298" << std::endl;
				if (InterlockedDecrement(&tempSession->ioCount) == 0)
				{
					// Release Session
					std::cout << "IO COMPLETE RECV DELETE" << std::endl;
					DeleteSession(tempSession);
				}
				continue;
			}
			else
			{
				// 다른 이유의 Err

			}
		}

		if (retGQCS == true && transferred == 0)
		{
			// FIN 수신
			CancelIoEx((HANDLE)tempSession->socket, nullptr);
			std::cout << "Decrement 318" << std::endl;
			if (InterlockedDecrement(&tempSession->ioCount) == 0)
			{
				// Release Session

				std::cout << "FIN Delete" << std::endl;
				DeleteSession(tempSession);
				continue;
			}
			continue;
		}
		
		if (tempOverlapped == &tempSession->recvOverlapped)
		{
			// recv 시 로직 처리. 현재는 Echo
			unsigned short* tempLen;

			tempSession->recvBuffer.MoveRear(transferred);
			while (1)
			{
				if (tempSession->recvBuffer.getUseSize() < 10)
				{
					break;
				}

				tempSession->recvBuffer.Dequeue(tempPacket.GetBufferPtr(), 10);
				tempPacket.MoveWritePos(10);
				OnMessage(tempSession->sessionID, &tempPacket);

			}

			if (RecvPost(tempSession) == false)
			{
				std::cout << "Decrement 351" << std::endl;
				if (InterlockedDecrement(&tempSession->ioCount) == 0)
				{
					std::cout << "RecvPost Delete Session" << std::endl;
					DeleteSession(tempSession);
					continue;
				}
			}

		}
		else
		{
			// send 시 로직 처리. 및 IO_COUNT 감소.
			tempSession->sendBuffer.MoveFront(transferred);
			
			/*std::cout << "Decrement 366" << std::endl;*/
			if (InterlockedDecrement(&tempSession->ioCount) == 0)
			{	
				std::cout << "Send IO_COUNT Delete Session : " << tempSession->ioCount << std::endl;
				DeleteSession(tempSession);
				continue;
			}

			InterlockedExchange8((char*)&tempSession->isSend, false);
			// 만약 sendBuffer에 데이터가 들어왔다면 SendPost를 다시 호출.
			SendPost(tempSession);

		}

	}


}

bool CLANSERVER::SendPost(SESSION* tempSession)
{

	// Error 가 아니면 true 반환.

	int retSend = 0;
	WSABUF wsaBuf_Send[2];

	if (InterlockedExchange8((char*)&tempSession->isSend, true) != false)
	{
		return true;
	}

	wsaBuf_Send[0].buf = tempSession->sendBuffer.GetFrontBufferPtr();
	wsaBuf_Send[0].len = tempSession->sendBuffer.DirectDequeueSize();
	/*wsaBuf_Send[1].buf = (char*)&tempSession->sendBuffer;
	wsaBuf_Send[1].len = tempSession->sendBuffer.getUseSize() - tempSession->sendBuffer.DirectDequeueSize();*/

	if (wsaBuf_Send[0].len == 0)
	{
		InterlockedExchange8((char*)&tempSession->isSend, false);
		return true;
	}

	InterlockedIncrement(&tempSession->ioCount);
	retSend = WSASend(tempSession->socket, wsaBuf_Send, 1, nullptr, 0, &tempSession->sendOverlapped, nullptr);
	if (retSend == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			std::cout << "WSASEND ERR" << err << std::endl;
			return false;
		}
	}

	

	return true;

}

bool CLANSERVER::RecvPost(SESSION* tempSession)
{
	unsigned long flags = 0;
	WSABUF wsaBuf_Recv[2];
	int retRecv = 0;

	wsaBuf_Recv[0].buf = tempSession->recvBuffer.GetRearBufferPtr();
	wsaBuf_Recv[0].len = tempSession->recvBuffer.DirectEnqueueSize();
	/*wsaBuf_Recv[1].buf = (char*)&tempSession->recvBuffer;
	wsaBuf_Recv[1].len = tempSession->recvBuffer.getFreeSize() - tempSession->recvBuffer.DirectEnqueueSize();*/



	retRecv = WSARecv(tempSession->socket, wsaBuf_Recv, 1, nullptr, &flags, &tempSession->recvOverlapped, nullptr);
	if (retRecv == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			// ERR
			std::cout << err << std::endl;
			return false;
		}
	}


	return true;
}

bool CLANSERVER::DeleteSession(SESSION* tempSession)
{
	closesocket(tempSession->socket);
	OnRelease(tempSession->sessionID);

	delete tempSession;
	return true;
}

bool CLANSERVER::Disconnect(unsigned long long sessionID)
{
	SESSION* tempSession;
	unsigned short sessionIndex = static_cast<unsigned short>(sessionID >> 48);
	tempSession = sessionArr[sessionIndex];
	
	CancelIoEx((HANDLE)tempSession->socket, NULL);

	return false;
}

bool CLANSERVER::SendPacket(unsigned long long sessionID, CPacket* packet)
{
	SESSION* tempSession;
	unsigned short sessionIndex = static_cast<unsigned short>(sessionID >> 48);
	tempSession = sessionArr[sessionIndex];
	
	tempSession->sendBuffer.Enqueue(packet->GetBufferPtr(), packet->GetDataSize());
	bool retSendPost = false;
	retSendPost = SendPost(tempSession);



	return retSendPost;
}
