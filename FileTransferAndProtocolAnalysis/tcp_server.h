#pragma once
#include "common.h"

#define PORT 5150
#define DATA_BUFSIZE 8192
//8192
#define WM_SOCKET (WM_USER + 1)

void serverMain(PVOID portParma);
void CreateSocketInformation(SOCKET s);
void FreeSocketInformation(SOCKET s);
HWND MakeWorkerWindow(void);
LRESULT CALLBACK tcpCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int writeToFile(NETWORK* network);