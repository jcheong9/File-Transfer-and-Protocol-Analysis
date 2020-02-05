#pragma once

#define SERVER_TCP_PORT			5150	// Default port
#define BUFSIZE					1024	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

typedef struct
{
	struct sockaddr_in server;
	SOCKET sd;
} NETWORK;

void tcp_client(HWND hwnd, TCHAR * ipAddress, LPCSTR fileData, NETWORK* network);
int tcpSentPacket(SOCKET* sd, LPCSTR fileData);
void disconnectSocket(SOCKET* sd);