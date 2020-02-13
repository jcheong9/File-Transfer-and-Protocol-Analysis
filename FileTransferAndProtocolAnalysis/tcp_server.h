#pragma once
#include "common.h"


#define WM_SOCKET (WM_USER + 1)

void serverMainTCP(PVOID portParma);
void disconnectSocketServerTCP(SOCKET siServer);
void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);
DWORD WINAPI WorkerThread(LPVOID lpParameter);
