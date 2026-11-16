#include <Winsock2.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "MyCLanServer.h"

int main()
{
	MY_CLANSERVER testServer;

	testServer.Start(6000, 8, true, 65536);

	

	
	_getch();
	while (1)
	{
	}

	testServer.Stop();

	return 0;
}

