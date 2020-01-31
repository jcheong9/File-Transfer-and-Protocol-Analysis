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

#include <stdio.h>
#include <string>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

#define ID_CONNECT						103
#define ID_UPLOAD						104
#define ID_EXIT							105
#define ID_ENTER_BTN					106
#define ID_SERVER_BTN					107
#define ID_CLIENT_BTN					108
#define ID_TCP_BTN						109
#define ID_UDP_BTN						110


//Declaration Functions application
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void upload_file(HWND hwnd);


typedef struct
{
	HWND hwnd;

	int selection = 0;
} PORTPARMA;
