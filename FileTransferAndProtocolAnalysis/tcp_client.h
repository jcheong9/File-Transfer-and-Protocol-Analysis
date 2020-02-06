#pragma once

#define SERVER_TCP_PORT			5150	// Default port
#define BUFSIZE					1024	
#include "common.h"

typedef struct
{
	struct sockaddr_in server;
	SOCKET sd;
} NETWORK;

int tcp_client(HWND hwnd, TCHAR * ipAddress, LPCSTR fileData, NETWORK* network);
int tcpSentPacket(SOCKET* sd, LPCSTR fileData);
void disconnectSocket(SOCKET* sd);