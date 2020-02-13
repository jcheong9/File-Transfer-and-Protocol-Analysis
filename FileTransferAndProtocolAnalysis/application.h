#pragma once
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: application.h - contains the declaration functions
--								and headers for this project
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
-- This is a header file contains declaration functions, defined menu items and headers.
----------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include "tcp_client.h"
#include "udp_client.h"
#include "tcp_server.h"
#include "udp_server.h"
#include "common.h"

//define id menubutton
#define ID_DISCONNECT					102
#define ID_CONNECT						103
#define ID_UPLOAD						104
#define ID_EXIT							105
#define ID_SEND_BTN						106		//obsolete
#define ID_SERVER_BTN					107
#define ID_CLIENT_BTN					108
#define ID_TCP_BTN						109
#define ID_UDP_BTN						110
#define ID_PACKET_TEN_TIMES_BTN			111
#define ID_PACKETS_HUNDRED_TIMES_BTN	112
#define ID_PACKETS_TRUE					113
#define ID_PACKETS_FALSE				114



//Declaration Functions application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int connect(HWND hwnd, LPCSTR fileData);
int upload_file(HWND hwnd, NETWORK* data);
void disconnect(HWND hwnd);
int checkIpInput();
int checkPackInput();
void processEndData();
int checkPortInput();





