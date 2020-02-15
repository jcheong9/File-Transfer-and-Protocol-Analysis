/*---------------------------------------------------------------------------------------
--	SOURCE FILE:	udp_client.c - A simple UDP client program.
--
--	PROGRAM:		uclnt.exe
--
--	FUNCTIONS:		void udp_client(PVOID network)
--
--	DATE:			January 30, 2020
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNERS:		Jameson Cheong
--
--	PROGRAMMERS:	Jameson Cheong
--
--	NOTES:
--	The program will send a UDP packet to a user specifed server.
--  The server can be specified using a fully qualified domain name or and
--	IP address. The first packet will sent a time stamp. If user did not upload file, the 
--  following packet is fill with 'a' with the specified packet size provide by the user. 
--  If user specified a file, the file will be sent in a packet. 
---------------------------------------------------------------------------------------*/
#include "udp_client.h"


void udp_client(PVOID network)
{
		NETWORK* networkStruct = (NETWORK*)network;
		int	data_size = DEFLEN, port ;
		int	server_len, client_len;
		SOCKET sd;
		char* host;
		struct	hostent* hp;
		struct	sockaddr_in server, client;
		SYSTEMTIME stStartTime;
		WSADATA stWSAData;
		WORD wVersionRequested = MAKEWORD(2, 2);
		char buffer[64];
		LPSTR messageHeader;
		long num = (networkStruct->packSize) + 1;
		int numFile;
		LPSTR message = new TCHAR[num];

		host = networkStruct->ip;
		port = atoi(networkStruct->port);

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

		sprintf_s(buffer, "Port aasigned is %d\n", ntohs(client.sin_port));
		MessageBox(networkStruct->hwnd, buffer, TEXT("Client"), MB_OK);

		if (data_size > MAXLEN)
		{
			fprintf(stderr, "Data is too big\n");
			exit(1);
		}

		server_len = sizeof(server);

		// Get the start time
		GetSystemTime(&stStartTime);

		//sent header packets of the time
		memset(buffer, 0, 64);
		GetSystemTime(&stStartTime);
		if (networkStruct->uploaded) { // check if the file is uploaded
			sprintf_s(buffer, "`%d~\r\n", getTimeConvertToMil(stStartTime));
		}
		else {
			sprintf_s(buffer, "%d~\r\n", getTimeConvertToMil(stStartTime));
		}

		messageHeader = buffer;

		if (sendto(sd, messageHeader, strlen(messageHeader), 0, (struct sockaddr*) & server, server_len) == SOCKET_ERROR)
		{
			perror("sendto failure");
			_endthread();
		}
		//sent packets size
		memset(message, 0, num);
		memset(message, 'a', num);	

		if (networkStruct->uploaded) {
			string strFile = convert(networkStruct->data);
			numFile = strFile.length() + 1;
			LPSTR uploadMessage = new TCHAR[numFile];
			memset(uploadMessage, 0, numFile);
			strncpy(uploadMessage, strFile.c_str(),numFile);
			if (sendto(sd, uploadMessage, numFile, 0, (struct sockaddr*) & server, server_len) == SOCKET_ERROR)
			{
				perror("sendto failure");
				_endthread();
			}
			delete[] uploadMessage;
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

		MessageBox(networkStruct->hwnd, "Transmission Completed, Closing socket.", TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		
		delete[] message;
		closesocket(sd);
		WSACleanup();
		_endthread();
	}

