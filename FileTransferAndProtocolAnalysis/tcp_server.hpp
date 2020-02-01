#pragma once
#include "application.hpp"

#include <windowsx.h>
#include <stdio.h>
#include <conio.h>

#define PORT 5150
#define DATA_BUFSIZE 8192

typedef struct _SOCKET_INFORMATION {
	BOOL RecvPosted;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
	_SOCKET_INFORMATION* Next;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

#define WM_SOCKET (WM_USER + 1)
void serverMain();
void CreateSocketInformation(SOCKET s);
LPSOCKET_INFORMATION GetSocketInformation(SOCKET s);
void FreeSocketInformation(SOCKET s);

HWND MakeWorkerWindow(void);
LRESULT CALLBACK tcpCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LPSOCKET_INFORMATION SocketInfoList;