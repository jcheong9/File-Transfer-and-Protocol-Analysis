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
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: common.h - contains the declaration functions
--								and libraries for this project
--
--
--
-- PROGRAM: File Transfer and Protocol Analysis Application
--
-- DATE: January 29, 2020
--
-- REVISIONS:
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- NOTES:
-- This is a header file contains declaration functions, constants ,structs and libraries.
----------------------------------------------------------------------------------------------------------------------*/
#define WM_FAILED_CONNECT        101
#define BUFSIZE					1024	
#define DATA_BUFSIZE			8192
#define DATA_BUFSIZEUDP			200000

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
	CHAR Buffer[DATA_BUFSIZEUDP];
	WSABUF DataBuf;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATIONUDP, * LPSOCKET_INFORMATIONUDP;

typedef struct NETWORK{
	int selectServerClient = 0;
	int packetMessage = 0;		//0 no packet message
	int selectedProtocal = 0;   //0 is tcp, 1 is udp
	int connected = 1;			//0 is disconnect, 1 is connect
	int timesPacketsSelection = 10;
	int packSize = NULL;
	int numPackRecv = 0;
	int uploaded = 0;			//0 not upload file
	long startTime = 0;
	long endTime = 0;
	struct sockaddr_in server;
	HWND hwnd = NULL;
	TCHAR ip[64];
	TCHAR port[64];
	LPCSTR data;
	TCHAR filePath[100];
	SOCKET sdServer = NULL;
	SOCKET AcceptSocket = NULL;
	WSAEVENT udpEvent = NULL;
	DWORD numByteRead = 0;
} NETWORK, * NETWORK_STRUC;

//Funtion Prototype
int writeToFile(LPSTR data, NETWORK_STRUC networkStruct);
string convert(LPCSTR str);
int loadSaveFile(LPSTR data, NETWORK_STRUC networkStruct);
long getTimeConvertToMil(SYSTEMTIME s1);
