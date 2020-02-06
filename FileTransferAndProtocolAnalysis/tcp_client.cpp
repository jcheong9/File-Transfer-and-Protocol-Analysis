#include "tcp_client.h"


int tcp_client(HWND hwnd, TCHAR * ipAddress, LPCSTR fileData, NETWORK * network) {
	int port, err;
	//SOCKET sd;
	struct hostent* hp;
	struct sockaddr_in server;
	char* host, ** pptr;
	WSADATA WSAData;
	WORD wVersionRequested;
	//char* sbuf[BUFSIZE];
	LPCSTR sbuf = "";
	char buff[100];
	

	host = ipAddress;	// Host name local host
	port = SERVER_TCP_PORT;


	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &WSAData);
	if (err != 0) //No usable DLL
	{
		printf("DLL not found!\n");
		exit(1);
	}

	// Create the socket
	if ((network->sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
		exit(1);
	}

	// Initialize and set up the address structure
	memset((char*)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL)
	{
		//fprintf(stderr, "Unknown server address\n");
		MessageBox(hwnd, "Unknown server address\n", TEXT(""), MB_OK);
	}

	// Copy the server address
	memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);

	// Connecting to the server
	if (connect(network->sd, (struct sockaddr*) & server, sizeof(server)) == -1)
	{
		sprintf_s(buff, "Can't connect to server\n");
		MessageBox(hwnd, buff, TEXT(""), MB_OK);
		return 0;
	}

	return 1;
	// Transmit data through the socket
	//send(*sd, fileData, strlen(fileData), 0);
	//printf("Receive:\n");
	/*
	bp = rbuf;
	bytes_to_read = (int)strlen(fileData);

	// client makes repeated calls to recv until no more data is expected to arrive.
	while ((n = recv(sd, bp, bytes_to_read, 0)) < strlen(fileData))
	{
		bp += n;
		bytes_to_read -= n;
		if (n == 0)
			break;
	}
	//OutputDebugString(rbuf);
	MessageBox(hwnd, bp, TEXT("client"), MB_OK);
	closesocket(sd);
	WSACleanup();
	*/
}

int tcpSentPacket(SOCKET* sd, LPCSTR fileData) {
	int n = send(*sd, fileData, strlen(fileData), 0);
	disconnectSocket(sd);
	return n;
}

void disconnectSocket(SOCKET* sd) {
	closesocket(*sd);
	WSACleanup();
}