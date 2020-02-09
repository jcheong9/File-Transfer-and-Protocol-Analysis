#pragma once
#include "common.h"


#define SERVER_UDP_PORT		8000	// Default port
#define MAXLEN				65000   // Maximum Buffer length
#define DEFLEN				64		// Default Length



void udp_client(PVOID network);
int tcpSentPacket(SOCKET* sd, LPCSTR fileData);
void disconnectSocketClient(SOCKET* sd);