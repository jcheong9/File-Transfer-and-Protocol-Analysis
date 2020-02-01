//#include "application.h"
//
//#define SERVER_TCP_PORT			7000	// Default port
//#define BUFSIZE					1024		
//
//void tcp_client() {
//	int n, ns, bytes_to_read;
//	int port, err;
//	SOCKET sd;
//	struct hostent* hp;
//	struct sockaddr_in server;
//	char* host, * bp, rbuf[BUFSIZE], ** pptr;
//	WSADATA WSAData;
//	WORD wVersionRequested;
//	char sbuf[BUFSIZE] = "orem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lore";
//
//
//	host = (TCHAR *)"127.0.0.1";	// Host name local host
//	port = SERVER_TCP_PORT;
//
//
//	wVersionRequested = MAKEWORD(2, 2);
//	err = WSAStartup(wVersionRequested, &WSAData);
//	if (err != 0) //No usable DLL
//	{
//		printf("DLL not found!\n");
//		exit(1);
//	}
//
//	// Create the socket
//	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
//	{
//		perror("Cannot create socket");
//		exit(1);
//	}
//
//	// Initialize and set up the address structure
//	memset((char*)&server, 0, sizeof(struct sockaddr_in));
//	server.sin_family = AF_INET;
//	server.sin_port = htons(port);
//	if ((hp = gethostbyname(host)) == NULL)
//	{
//		fprintf(stderr, "Unknown server address\n");
//		exit(1);
//	}
//
//	// Copy the server address
//	memcpy((char*)&server.sin_addr, hp->h_addr, hp->h_length);
//
//	// Connecting to the server
//	if (connect(sd, (struct sockaddr*) & server, sizeof(server)) == -1)
//	{
//		fprintf(stderr, "Can't connect to server\n");
//		perror("connect");
//		exit(1);
//	}
//	printf("Connected:    Server Name: %s\n", hp->h_name);
//	pptr = hp->h_addr_list;
//	printf("\t\tIP Address: %s\n", inet_ntoa(server.sin_addr));
//	printf("Transmiting:\n");
//	memset((char*)sbuf, 0, sizeof(sbuf));
//	//getline(sbuf); // get user's text
//
//	// Transmit data through the socket
//	ns = send(sd, sbuf, BUFSIZE, 0);
//	printf("Receive:\n");
//	bp = rbuf;
//	bytes_to_read = BUFSIZE;
//
//	// client makes repeated calls to recv until no more data is expected to arrive.
//	while ((n = recv(sd, bp, bytes_to_read, 0)) < BUFSIZE)
//	{
//		bp += n;
//		bytes_to_read -= n;
//		if (n == 0)
//			break;
//	}
//	printf("%s\n", rbuf);
//	closesocket(sd);
//	WSACleanup();
//	exit(0);
//}