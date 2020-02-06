#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
typedef struct
{
	HWND hwnd = NULL;
	int selectedProtocal = 0;   //0 is tcp, 1 is udp
	int selectServerClient = 0; //0 is server, 1 is client
	int uploaded = 0;
	int connected = 1; //0 is disconnect, 1 is connect
	int numPackets = 10;
} PORTPARMA;

typedef struct
{
	LPSTR packSize;
	LPCSTR data;
	LPCSTR filePath;
} DATASENT;