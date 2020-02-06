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
#include "tcp_client.h"
#include "tcp_server.h"
#include "common.h"

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
#define ID_PACKETSIZE_1024B				113
#define ID_PACKETSIZE_4096B				113
#define ID_PACKETSIZE_20KB				113
#define ID_PACKETSIZE_60KB				113

//Declaration Functions application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int connect(HWND hwnd, LPCSTR fileData);
int upload_file(HWND hwnd, NETWORK* data);
void sentFile();
void disconnect(HWND hwnd);
LPCSTR packetizeSize();




