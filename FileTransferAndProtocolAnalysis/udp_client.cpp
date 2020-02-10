/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	udp_client.c - A simple UDP client program.
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
		int	i, j, server_len, client_len;
		SOCKET sd;
		char* host;
		struct	hostent* hp;
		struct	sockaddr_in server, client;
		SYSTEMTIME stStartTime;
		WSADATA stWSAData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		char buffer[64];


		host = networkStruct->ip;

		// Initialize the DLL with version Winsock 2.2
		if (WSAStartup(wVersionRequested, &stWSAData) != 0) {
			MessageBox(networkStruct->hwnd, "DLL not found!\n", TEXT("Client"), MB_OK);
			PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
			_endthread();
		}

		// Create a datagram socket
		if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		{
			perror("Can't create a socket\n");
			exit(1);
		}

		// Store server's information
		memset((char*)&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(port);

		if ((hp = gethostbyname(host)) == NULL)
		{
			fprintf(stderr, "Can't get server's IP address\n");
			exit(1);
		}
		//strcpy((char *)&server.sin_addr, hp->h_addr);
		memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);

		// Bind local address to the socket
		memset((char*)&client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(0);  // bind to any available port
		client.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sd, (struct sockaddr*) & client, sizeof(client)) == -1)
		{
			perror("Can't bind name to socket");
			exit(1);
		}
		// Find out what port was assigned and print it
		client_len = sizeof(client);
		if (getsockname(sd, (struct sockaddr*) & client, &client_len) < 0)
		{
			perror("getsockname: \n");
			exit(1);
		}

		printf("Port aasigned is %d\n", ntohs(client.sin_port));
		sprintf_s(buffer, "Port aasigned is %d\n", ntohs(client.sin_port));
		MessageBox(networkStruct->hwnd, buffer, TEXT("Client"), MB_OK);

		if (data_size > MAXLEN)
		{
			fprintf(stderr, "Data is too big\n");
			exit(1);
		}


		sprintf_s(buffer, "\r\nBegining Time From Client %d~\r\n", clock());
		LPSTR messageHeader = buffer;



		// transmit data
		server_len = sizeof(server);


		long num = (networkStruct->packSize) + 1;
		LPSTR message = new TCHAR[num];

		memset(buffer, 0, 64);
		if (networkStruct->uploaded) {
			send(networkStruct->sdClient, "`", strlen("`"), 0);
		}

		// Get the start time
		GetSystemTime(&stStartTime);

		//sent packets
		memset(buffer, 0, 64);
		GetSystemTime(&stStartTime);
		if (networkStruct->uploaded) {
			sprintf_s(buffer, "`%d~\r\n", getTimeConvertToMil(stStartTime));
		}
		else {
			sprintf_s(buffer, "%d~\r\n", getTimeConvertToMil(stStartTime));
		}

		messageHeader = buffer;
		send(networkStruct->sdClient, messageHeader, strlen(messageHeader), 0);
		if (sendto(sd, messageHeader, strlen(messageHeader), 0, (struct sockaddr*) & server, server_len) == SOCKET_ERROR)
		{
			perror("sendto failure");
			_endthread();
		}
		//sent packets
		memset(message, 0, networkStruct->packSize);
		memset(message, 'a', networkStruct->packSize);
		if (networkStruct->uploaded) {
			if (sendto(sd, networkStruct->data, strlen(networkStruct->data), 0, (struct sockaddr*) & server, server_len) == SOCKET_ERROR)
			{
				perror("sendto failure");
				_endthread();
			}
		}
		else {
			for (int i = 0; i < networkStruct->timesPacketsSelection; i++) {
				if (sendto(sd, message, strlen(message), 0, (struct sockaddr*) & server, server_len) == SOCKET_ERROR)
				{
					perror("sendto failure");
					MessageBox(networkStruct->hwnd, "sendto failure", TEXT(""), MB_OK);
					_endthread();
				}
			}
		}

		MessageBox(networkStruct->hwnd, "Transmition Ended. Closing socket.", TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);

		closesocket(sd);
		WSACleanup();
	}

