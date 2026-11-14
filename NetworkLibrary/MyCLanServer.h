#pragma once
#include "CLanServer.h"

class MY_CLANSERVER : public CLANSERVER
{


	virtual bool OnConnectionRequest(SOCKADDR_IN clientAddr);
	virtual void OnAccept(unsigned long long sessionID);
	virtual void OnRelease(unsigned long long sessionID);
	virtual void OnMessage(unsigned long long sessionID, CPacket* packet);
	virtual void OnError(int errcode, char*);



};


