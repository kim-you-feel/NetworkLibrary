#include "MyCLanServer.h"
#include "SerializeBuffer.h"
#include <iostream>
bool MY_CLANSERVER::OnConnectionRequest(SOCKADDR_IN clientAddr)
{
	return true;
}

void MY_CLANSERVER::OnAccept(unsigned long long sessionID)
{
}

void MY_CLANSERVER::OnRelease(unsigned long long sessionID)
{
}

void MY_CLANSERVER::OnMessage(unsigned long long sessionID, CPacket* packet)
{
	unsigned short tempLen = 0;
	unsigned long long tempPayload = 0;

	*packet >> tempLen;
	*packet >> tempPayload;

	//std::cout << "Session ID : " << sessionID << std::endl;
	/*std::cout << "Recv Length : " << tempLen << std::endl;*/
	/*std::cout << "Recv PayLoad : " << tempPayload << std::endl;*/

	
	
	packet->Clear();

	*packet << tempLen;
	*packet << tempPayload;

	SendPacket(sessionID, packet);
}

void MY_CLANSERVER::OnError(int errcode, char*)
{
}
