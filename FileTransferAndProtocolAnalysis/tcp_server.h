#pragma once
#include "common.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: tcp_server.h - contains the declaration functions prototypes for TCP server
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
-- This is a header file contains declaration functions prototypes for TCP server
----------------------------------------------------------------------------------------------------------------------*/
#define WM_SOCKET (WM_USER + 1)

void serverMainTCP(PVOID portParma);
void disconnectSocketServerTCP(SOCKET siServer);
void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);
DWORD WINAPI WorkerThread(LPVOID lpParameter);
