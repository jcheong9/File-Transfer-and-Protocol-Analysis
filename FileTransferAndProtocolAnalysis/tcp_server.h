#pragma once
#include "common.h"


//8192
#define WM_SOCKET (WM_USER + 1)

void serverMain(PVOID portParma);
//int writeToFile(LPSTR data);
//void processReceiveData(LPSTR data);
//string convert(LPCSTR str);
//int loadSaveFile(LPSTR data);
void disconnectSocketServer(SOCKET siServer);

