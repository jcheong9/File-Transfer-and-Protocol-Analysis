#include "tcp_server.h"

typedef struct _SOCKET_INFORMATION {
	BOOL RecvPosted;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
	_SOCKET_INFORMATION* Next;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

LPSOCKET_INFORMATION GetSocketInformation(SOCKET s);
LPSOCKET_INFORMATION SocketInfoList;

NETWORK* pp;

void serverMain(PVOID network)
{
	pp = (NETWORK*)network;
	MSG msg;
	DWORD Ret;
	SOCKET Listen;
	SOCKADDR_IN InternetAddr;
	HWND Window;
	WSADATA wsaData;
	
	if ((Window = MakeWorkerWindow()) == NULL)
		return;
	

	// Prepare echo server
	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);

	}

	if ((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("socket() failed with error %d\n", WSAGetLastError());

	}

	WSAAsyncSelect(Listen, Window, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());

	}

	if (listen(Listen, 5))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());

	}

	// Translate and dispatch window messages for the application thread

	while (Ret = GetMessage(&msg, NULL, 0, 0))
	{
		if (Ret == -1)
		{
			printf("GetMessage() failed with error %d\n", GetLastError());

		}
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}


LRESULT CALLBACK tcpCallBack(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SOCKET Accept;
	LPSOCKET_INFORMATION SocketInfo;
	DWORD RecvBytes;
	DWORD Flags;
	char buff[100];
	SYSTEMTIME st;
	GetSystemTime(&st);

	
	if (uMsg == WM_SOCKET)
	{
		if (WSAGETSELECTERROR(lParam))
		{
			
			sprintf_s(buff, "Socket failed with error %d\n", WSAGETSELECTERROR(lParam));

			MessageBox(hwnd, buff, TEXT(""), MB_OK);
			FreeSocketInformation(wParam);
		}
		else
		{
			switch (WSAGETSELECTEVENT(lParam))
			{
			case FD_ACCEPT:

				if ((Accept = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
				{
					sprintf_s(buff, "accept() failed with error %d\n", WSAGetLastError());

					MessageBox(hwnd, buff, TEXT(""), MB_OK);
					break;
				}

				// Create a socket information structure to associate with the
				// socket for processing I/O.

				CreateSocketInformation(Accept);

				sprintf_s(buff, "Socket number %d connected\n", (int)Accept);

				MessageBox(hwnd, buff, TEXT(""), MB_OK);

				WSAAsyncSelect(Accept, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

				break;

			case FD_READ:
				if(pp->beginTime == NULL)
					pp->beginTime = clock();
				SocketInfo = GetSocketInformation(wParam);

				// Read data only if the receive buffer is empty.

				if (SocketInfo->BytesRECV != 0)
				{
					SocketInfo->RecvPosted = TRUE;
					return 0;
				}
				else
				{
					SocketInfo->DataBuf.buf = SocketInfo->Buffer;
					SocketInfo->DataBuf.len = DATA_BUFSIZE;

					Flags = 0;
					if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes,
						&Flags, NULL, NULL) == SOCKET_ERROR)
					{
						if (WSAGetLastError() != WSAEWOULDBLOCK)
						{
							//printf("WSARecv() failed with error %d\n", WSAGetLastError());
							sprintf_s(buff, "WSARecv() failed with error %d\n", WSAGetLastError());

							MessageBox(hwnd, buff, TEXT(""), MB_OK);
							
							FreeSocketInformation(wParam);
							return 0;
						}
					}
					else // No error so update the byte count
					{
						SocketInfo->BytesRECV = 0;

						if (strcmp(SocketInfo->DataBuf.buf, "end") != 0) {
							processReceiveData(SocketInfo->DataBuf.buf);
						}
						else {
							writeToFile();
						}

						pp->endTime = float(clock() - pp->beginTime);	 //mill sec
						memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);
					}
				}

				break;

			case FD_CLOSE:

				SocketInfo = GetSocketInformation(wParam);
				//MessageBox(hwnd, SocketInfo->Buffer, TEXT("Server"), MB_OK);
				//printf("Buffer rev %s\n", SocketInfo->Buffer);
				sprintf_s(buff, "Closing socket %d\n", (int)wParam);
				writeToFile();
				//MessageBox(hwnd, buff, TEXT(""), MB_OK);
				FreeSocketInformation(wParam);
				break;
			}
		}
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
		sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return;
	}

	// Prepare SocketInfo structure for use.

	SI->Socket = s;
	SI->RecvPosted = FALSE;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SI->Next = SocketInfoList;

	SocketInfoList = SI;
}

LPSOCKET_INFORMATION GetSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION* SI = SocketInfoList;

	while (SI)
	{
		if (SI->Socket == s)
			return SI;

		SI = SI->Next;
	}

	return NULL;
}

void FreeSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION* SI = SocketInfoList;
	SOCKET_INFORMATION* PrevSI = NULL;

	while (SI)
	{
		if (SI->Socket == s)
		{
			if (PrevSI)
				PrevSI->Next = SI->Next;
			else
				SocketInfoList = SI->Next;

			closesocket(SI->Socket);
			GlobalFree(SI);
			return;
		}

		PrevSI = SI;
		SI = SI->Next;
	}
}

HWND MakeWorkerWindow(void)
{
	WNDCLASS wndclass;
	CHAR* ProviderClass = (TCHAR *)"AsyncSelect";
	HWND Window;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = (WNDPROC)tcpCallBack;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = ProviderClass;

	if (RegisterClass(&wndclass) == 0)
	{
		printf("RegisterClass() failed with error %d\n", GetLastError());
		return NULL;
	}

	// Create a window.

	if ((Window = CreateWindow(
		ProviderClass,
		"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		NULL,
		NULL)) == NULL)
	{
		printf("CreateWindow() failed with error %d\n", GetLastError());
		return NULL;
	}

	return Window;
}

int writeToFile() {
	//string filepath = convert(uploadData->filePath); // convert LPCST to std::string

	OPENFILENAME ofn;
	char file_name[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pp->hwnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = "*.txt\0";
	ofn.nFilterIndex = 1;

	//open save dialog box
	GetSaveFileName(&ofn);

	HANDLE hFile = CreateFile(TEXT(ofn.lpstrFile),      // name of the write
								FILE_APPEND_DATA,       // open for appending
								0,                      // do not share
								NULL,                   // default security
								CREATE_ALWAYS,          // overwrite existing
								FILE_ATTRIBUTE_NORMAL,  // normal file
								NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to open/create file
		return 2;
	}

	// Write data to the file

	std::string strText; // For C use LPSTR (char*) or LPWSTR (wchar_t*)
	if (pp->data != NULL) {
		strText = convert(pp->data);
	}
	else {
		strText = "Failed to recieved";
	}


	DWORD bytesWritten;
	WriteFile(
		hFile,            // Handle to the file
		strText.c_str(),  // Buffer to write
		strText.size(),   // Buffer size
		&bytesWritten,    // Bytes written
		nullptr);         // Overlapped

	 // Close the handle once we don't need it.
	CloseHandle(hFile);
	return 1;
}

void processReceiveData(LPSTR data) {
	if (pp->data == NULL)
		pp->data = data;
	LPCSTR str1 = "";
	std::string(pp->data).append(data);
}
