#pragma once


#include "common.h"



void tcp_client(PVOID network);
int tcpSentPacket(SOCKET* sd, LPCSTR fileData);
void disconnectSocketClient(SOCKET* sd);