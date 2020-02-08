#pragma once
#include "common.h"

#define PORT 5150
#define DATA_BUFSIZE 8192
//8192
#define WM_SOCKET (WM_USER + 1)

void serverMain(PVOID portParma);
int writeToFile(LPSTR data);
void processReceiveData(LPSTR data);
string convert(LPCSTR str);

