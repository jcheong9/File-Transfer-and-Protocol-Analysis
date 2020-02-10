#pragma once
#include "common.h"

//8192
#define WIN32_LEAN_AND_MEAN

void serverMainUDP(PVOID network);
void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);