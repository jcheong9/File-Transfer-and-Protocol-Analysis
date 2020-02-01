#pragma once

#define SERVER_TCP_PORT			7000	// Default port
#define BUFSIZE					1024	
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

void tcp_client(HWND hwnd);