#include "tcp_client.h"



void tcp_client(HWND hwnd, TCHAR * ipAddress, LPCSTR fileData) {
	int n, ns, bytes_to_read;
	int port, err;
	SOCKET sd;
	struct hostent* hp;
	struct sockaddr_in server;
	char* host, * bp, rbuf[BUFSIZE], ** pptr;
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
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}

	// Copy the server address
	memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);

	// Connecting to the server
	if (connect(sd, (struct sockaddr*) & server, sizeof(server)) == -1)
	{
		sprintf_s(buff, "Can't connect to server\n");
		MessageBox(hwnd, buff, TEXT(""), MB_OK);

	}

	pptr = hp->h_addr_list;


	// Transmit data through the socket
	ns = send(sd, fileData, strlen(fileData), 0);
	//printf("Receive:\n");
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
}