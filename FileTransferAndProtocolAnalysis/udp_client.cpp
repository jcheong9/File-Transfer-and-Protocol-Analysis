/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	udp_clnt.c - A simple UDP client program.
--
--	PROGRAM:		uclnt.exe
--
--	FUNCTIONS:		Winsock 2 API
--
--	DATE:			January 6, 2008
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNERS:
--
--	PROGRAMMERS:	Aman Abdulla
--
--	NOTES:
--	The program will send a UDP packet to a user specifed server.
--  The server can be specified using a fully qualified domain name or and
--	IP address. The packet is time stamped and then sent to the server. The
--  response (echo) is also time stamped and the delay is displayed.
---------------------------------------------------------------------------------------*/
#include "udp_client.h"

struct	sockaddr_in  serverStrucUDP, clientStrucUDP;
void udp_client(PVOID network)
{
	NETWORK* networkStruct = (NETWORK*)network;
	int	data_size = DEFLEN, port = SERVER_PORT;
	int	j, server_len, client_len;
	SOCKET sd;
	char* host, sbuf[MAXLEN];
	struct	hostent* hp;

	SYSTEMTIME stStartTime;
	WSADATA stWSAData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	char buff[100];
	host = networkStruct->ip;
	LPSTR message = new TCHAR[25];

	// Initialize the DLL with version Winsock 2.2
	WSAStartup(wVersionRequested, &stWSAData);

	// Create a datagram socket
	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("Can't create a socket\n");
		exit(1);
	}

	// Store server's information
	memset((char*)&serverStrucUDP, 0, sizeof(serverStrucUDP));
	serverStrucUDP.sin_family = AF_INET;
	serverStrucUDP.sin_port = htons(port);

	if ((hp = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "Can't get server's IP address\n");
		sprintf_s(buff, "Can't get server's IP address\n");
		MessageBox(networkStruct->hwnd, buff, TEXT("Client"), MB_OK);
		_endthread();
	}
	//strcpy((char *)&server.sin_addr, hp->h_addr);
	memcpy((char*)&serverStrucUDP.sin_addr, hp->h_addr, hp->h_length);

	// Bind local address to the socket
	memset((char*)&clientStrucUDP, 0, sizeof(clientStrucUDP));
	clientStrucUDP.sin_family = AF_INET;
	clientStrucUDP.sin_port = htons(0);  // bind to any available port
	clientStrucUDP.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sd, (struct sockaddr*) & clientStrucUDP, sizeof(clientStrucUDP)) == -1)
	{
		perror("Can't bind name to socket");
		_endthread();
	}
	// Find out what port was assigned and print it
	client_len = sizeof(clientStrucUDP);
	if (getsockname(sd, (struct sockaddr*) & clientStrucUDP, &client_len) < 0)
	{
		perror("getsockname: \n");
		_endthread();
	}
	sprintf_s(buff, "Port aasigned is %d\n", ntohs(clientStrucUDP.sin_port));

	if (data_size > MAXLEN)
	{
		fprintf(stderr, "Data is too big\n");
		_endthread();
	}

	// Get the start time
	GetSystemTime(&stStartTime);

	// transmit data
	server_len = sizeof(serverStrucUDP);
	memset(message, 'a', 25);
	if (sendto(sd, message, 25, 0, (struct sockaddr*) & serverStrucUDP, server_len) == -1)
	{
		perror("sendto failure");
		_endthread();
	}


	closesocket(sd);
	WSACleanup();
	_endthread();
}

