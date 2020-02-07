#include "tcp_client.h"


void tcp_client(PVOID network) {
	NETWORK* networkStruct = (NETWORK*)network;
	int port, err;
	//SOCKET sd;
	struct hostent* hp;
	struct sockaddr_in server;
	char* host;
	WSADATA WSAData;
	WORD wVersionRequested;
	//char* sbuf[BUFSIZE];
	LPCSTR sbuf = "";
	char buff[100];
	

	host = networkStruct->ip;	// Host name local host
	port = SERVER_TCP_PORT;


	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &WSAData);
	if (err != 0) //No usable DLL
	{
		printf("DLL not found!\n");
		MessageBox(networkStruct->hwnd, "Cannot create socket\n", TEXT(""), MB_OK);
		//exit(1);
	}

	// Create the socket
	if ((networkStruct->sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
		MessageBox(networkStruct->hwnd, "Cannot create socket\n", TEXT(""), MB_OK);
		//exit(1);
	}

	// Initialize and set up the address structure
	memset((char*)&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if ((hp = gethostbyname(host)) == NULL)
	{
		//fprintf(stderr, "Unknown server address\n");
		MessageBox(networkStruct->hwnd, "Unknown server address\n", TEXT(""), MB_OK);

	}
	else {

		// Copy the server address
		memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);
	}
	// Connecting to the server
	if (connect(networkStruct->sd, (struct sockaddr*) & server, sizeof(server)) == -1)
	{
		sprintf_s(buff, "Can't connect to server\n");
		MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
	}
	for (int i = 0; i < networkStruct->numPackets; i++) {
		if (send(networkStruct->sd, networkStruct->data, strlen(networkStruct->data), 0)) {
			OutputDebugString("Error sock send");
		}
	}


	closesocket(networkStruct->sd);
	WSACleanup();
	_endthread();
}

int tcpSentPacket(SOCKET* sd, LPCSTR fileData) {
	int n = send(*sd, fileData, strlen(fileData), 0);
	return n;
}
/*
int tcpSentMultiplePacket() {
	tcpSentPacket(SOCKET * sd, LPCSTR fileData)
}

*/

void disconnectSocket(SOCKET* sd) {
	setsockopt(*sd, SOL_SOCKET, SO_LINGER, NULL, NULL);
	closesocket(*sd);

}




