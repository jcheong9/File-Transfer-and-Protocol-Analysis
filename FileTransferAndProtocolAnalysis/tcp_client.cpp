#include "tcp_client.h"


void tcp_client(PVOID network) {
	SYSTEMTIME st;
	NETWORK* networkStruct = (NETWORK*)network;
	int port, err;
	struct hostent* hp;
	struct sockaddr_in server;
	char* host;
	WSADATA WSAData;
	SOCKET sdClient;
	WORD wVersionRequested;
	LPCSTR sbuf = "";
	char buff[100];
	
	float beginTime;
	long num = (networkStruct->packSize) + 1;
	LPSTR message = new TCHAR[num];
	char buffer[64];

	host = networkStruct->ip;	// Host name local host
	port = SERVER_PORT;


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
	if ((sdClient = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
	if (connect(sdClient, (struct sockaddr*) & server, sizeof(server)) == -1)
	{
		sprintf_s(buff, "Can't connect to server\n");
		MessageBox(networkStruct->hwnd, buff, TEXT("Client"), MB_OK);
		PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
		_endthread();
	}
	//trnasmit messages

	memset(buffer, 0, 64);
	GetSystemTime(&st);
	if (networkStruct->uploaded) {
		sprintf_s(buffer, "`%d~\r\n", getTimeConvertToMil(st));
	}
	else {
		sprintf_s(buffer, "%d~\r\n", getTimeConvertToMil(st));
	}

	LPSTR messageHeader = buffer;
	send(sdClient, messageHeader, strlen(messageHeader), 0);

	//sent packets
	memset(message, 0, networkStruct->packSize);
	memset(message, 'a', networkStruct->packSize);
	if (networkStruct->uploaded) {
		send(sdClient, networkStruct->data, strlen(networkStruct->data), 0);
	}
	else {
		for (int i = 0; i < networkStruct->timesPacketsSelection; i++) {
			if (send(sdClient, message, strlen(message), 0) == SOCKET_ERROR) {
				MessageBox(networkStruct->hwnd, "error with senting to socket", TEXT(""), MB_OK);
				
				_endthread();
			}
		}
	}

	closesocket(sdClient);
	WSACleanup();
	MessageBox(networkStruct->hwnd, "Transmition Ended. Closing socket.", TEXT("Client"), MB_OK);
	PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
	_endthread();
}

int tcpSentPacket(SOCKET* sd, LPCSTR fileData) {
	int n = send(*sd, fileData, strlen(fileData), 0);
	return n;
}


