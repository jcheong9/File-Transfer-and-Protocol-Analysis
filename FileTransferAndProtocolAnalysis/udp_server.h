#pragma once
#include "common.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: udp_server.h - contains the declaration functions prototypes for UDP server
--								
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
-- This is a header file contains declaration functions prototypes for UDP server
----------------------------------------------------------------------------------------------------------------------*/
#define WIN32_LEAN_AND_MEAN

//function prototype
void serverMainUDP(PVOID network);
void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);
void disconnectSocketServerUDP(SOCKET si, WSAEVENT udpEvent);