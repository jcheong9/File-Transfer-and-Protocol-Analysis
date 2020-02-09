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

#define WM_FAILED_CONNECT        101
#define SERVER_PORT				5150	// Default port
#define BUFSIZE					1024	
#define DATA_BUFSIZE			8192

using namespace std;

typedef struct _SOCKET_INFORMATION {
	OVERLAPPED Overlapped;
	SOCKET Socket;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

typedef struct _SOCKET_INFORMATIONUDP {
	OVERLAPPED Overlapped;
	SOCKET Socket;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATIONUDP, * LPSOCKET_INFORMATIONUDP;

typedef struct
{
	int selectServerClient = 0; //0 is server, 1 is client

} PORTPARMA;

typedef struct NETWORK{
	int packetMessage = 0;		//0 no packet message
	int selectedProtocal = 0;   //0 is tcp, 1 is udp
	int connected = 1;			//0 is disconnect, 1 is connect
	int numPackets = 10;
	int packSize;
	int uploaded = 0;
	struct sockaddr_in server;
	HWND hwnd = NULL;
	LPSTR ip;
	LPCSTR data;
	TCHAR filePath[100];
	SOCKET sdClient;
	LPSOCKET_INFORMATION siServer;
	LPSOCKET_INFORMATIONUDP siServerUDP;
	DWORD numByteRead = 0;
} NETWORK, * NETWORK_STRUC;

//Funtion Prototype
int writeToFile(LPSTR data, NETWORK_STRUC networkStruct);
string convert(LPCSTR str);
int loadSaveFile(LPSTR data, NETWORK_STRUC networkStruct);

