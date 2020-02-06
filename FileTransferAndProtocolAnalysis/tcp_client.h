#pragma once

#define SERVER_TCP_PORT			5150	// Default port
#define BUFSIZE					1024	
#include "common.h"



void tcp_client(PVOID network);
int tcpSentPacket(SOCKET* sd, LPCSTR fileData);
void disconnectSocket(SOCKET* sd);