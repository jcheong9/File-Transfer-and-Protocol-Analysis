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
#pragma comment(lib, "Ws2_32.lib")

#define SERVER_TCP_PORT			7000	// Default port
#define BUFSIZE					1024	
#define ID_DISCONNECT					102
#define ID_CONNECT						103
#define ID_UPLOAD						104
#define ID_EXIT							105
#define ID_ENTER_BTN					106
#define ID_SERVER_BTN					107
#define ID_CLIENT_BTN					108
#define ID_TCP_BTN						109
#define ID_UDP_BTN						110
#define PORT 5150
#define DATA_BUFSIZE 8192

//Declaration Functions application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void upload_file(HWND hwnd);
LRESULT tcpCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void tcp_client(HWND hwnd);



#define WM_SOCKET (WM_USER + 1)
void serverMain(HWND hwnd);
void CreateSocketInformation(SOCKET s);

void FreeSocketInformation(SOCKET s);

HWND MakeWorkerWindow(void);
LRESULT CALLBACK tcpCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


typedef struct
{
	HWND hwnd;

	int selection = 0;
} PORTPARMA;
