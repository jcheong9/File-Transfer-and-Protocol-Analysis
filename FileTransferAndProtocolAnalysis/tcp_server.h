#pragma once
#include "common.h"


//8192
#define WM_SOCKET (WM_USER + 1)

void serverMainTCP(PVOID portParma);
//int writeToFile(LPSTR data);
//void processReceiveData(LPSTR data);
//string convert(LPCSTR str);
//int loadSaveFile(LPSTR data);
void disconnectSocketServerTCP(SOCKET siServer);

