#pragma once
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: application.h - contains the declaration functions
--								and libraries for this project
--
--
--
-- PROGRAM: Basic Window Socket Application
--
-- DATE: January 15, 2020
--
-- REVISIONS:
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- NOTES:
-- This is a header file contains declaration functions and libraries.
----------------------------------------------------------------------------------------------------------------------*/
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
#pragma comment(lib, "Ws2_32.lib")
#include "tcp_client.h"
#include "tcp_server.h"

#define ID_DISCONNECT					102
#define ID_CONNECT						103
#define ID_UPLOAD						104
#define ID_EXIT							105
#define ID_SEND_BTN						106
#define ID_SERVER_BTN					107
#define ID_CLIENT_BTN					108
#define ID_TCP_BTN						109
#define ID_UDP_BTN						110
#define ID_PACKET_TEN_TIMES_BTN			111
#define ID_PACKETS_HUNDRED_TIMES_BTN	112

typedef struct
{
	HWND hwnd = NULL;
	int selectedProtocal = 0;   //0 is tcp, 1 is udp
	int selectServerClient = 0; //0 is server, 1 is client
	int uploaded = 0;
	int connected = 1; //0 is disconnect, 1 is connect
	int numPackets = 100;
} PORTPARMA;

typedef struct
{
	LPCSTR data;
	LPCSTR filePath;
} UPLOADFILE;

typedef struct
{
	SOCKET sd;
} NETWORK;

//Declaration Functions application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int connect(HWND hwnd, LPCSTR fileData);
int upload_file(HWND hwnd, UPLOADFILE* data);
void sentFile();
void disconnect(HWND hwnd);




