#include "tcp_client.h"


void tcp_client(PVOID network) {
	SYSTEMTIME st;
	NETWORK* networkStruct = (NETWORK*)network;
	int port, err;
	struct hostent* hp;
	struct sockaddr_in server;
	char* host;
	WSADATA WSAData;
	WORD wVersionRequested;
	LPCSTR sbuf = "";
	char buff[100];
	GetSystemTime(&st);
	float beginTime;
	

	host = networkStruct->ip;	// Host name local host
	port = SERVER_TCP_PORT;


	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &WSAData);
	if (err != 0) //No usable DLL
	{
		printf("DLL not found!\n");
		MessageBox(networkStruct->hwnd, "DLL not found!\n", TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		_endthread();
	}

	// Create the socket
	if ((networkStruct->sdClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
		MessageBox(networkStruct->hwnd, "Cannot create socket\n", TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		_endthread();
	}

	// Initialize and set up the address structure
	memset((char*)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL)
	{
		//fprintf(stderr, "Unknown server address\n");
		MessageBox(networkStruct->hwnd, "Unknown server address\n", TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		_endthread();
	}
	else {

		// Copy the server address
		memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);
	}
	// Connecting to the server
	if (connect(networkStruct->sdClient, (struct sockaddr*) & server, sizeof(server)) == -1)
	{
		sprintf_s(buff, "Can't connect to server\n");
		MessageBox(networkStruct->hwnd, buff, TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		_endthread();
	}

	LPSTR message = new TCHAR[networkStruct->packSize + 1];

	int n;
	networkStruct->beginTime = clock();
	char buffer[64];
	memset(buffer, 0, 64);
	sprintf_s(buffer, "\r\nBegining Time %d\r\n", clock());
	LPSTR messageHeader = buffer;
	send(networkStruct->sdClient, messageHeader, strlen(messageHeader), 0);

	send(networkStruct->sdClient, "end", strlen("end"), 0);
	/*
	if (networkStruct->uploaded) {
		if (networkStruct->selectedProtocal) {
			//udp
		}
		else {
			n = tcpSentPacket(&(networkStruct->sdClient), networkStruct->data);
		}
		//disconnectSocket(&(networkStruct->sdClient));


	}
	else {
		//send(networkStruct->sdClient, buffer, strlen(buffer), 0);
		memset(message, 0, networkStruct->packSize);
		memset(message, 'a', networkStruct->packSize);
		//testt
		send(networkStruct->sdClient, messageHeader, strlen(messageHeader), 0);

		for (int i = 0; i < networkStruct->numPackets; i++) {
			if (send(networkStruct->sdClient, message, strlen(message), 0) == SOCKET_ERROR) {
				MessageBox(networkStruct->hwnd, "error with senting to socket", TEXT(""), MB_OK);
				send(networkStruct->sdClient, "end", strlen("end"), 0);
				_endthread();
			}
		}
		send(networkStruct->sdClient, "end", strlen("end"), 0);

	}
	
	memset(message, 0, networkStruct->packSize);
	delete[] message;
	
	*/

	MessageBox(networkStruct->hwnd, "Disconnect", TEXT("Client"), MB_OK);
	PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
	closesocket(networkStruct->sdClient);
	WSACleanup();
	_endthread();

}

int tcpSentPacket(SOCKET* sd, LPCSTR fileData) {
	int n = send(*sd, fileData, strlen(fileData), 0);
	return n;
}


void disconnectSocketClient(SOCKET* sd) {
	setsockopt(*sd, SOL_SOCKET, SO_LINGER, NULL, NULL);
	closesocket(*sd);
	WSACleanup();
}




