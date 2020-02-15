/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: winmain.cpp -	An application that uses basic Winsock 2 API database
--								lookup calls to get host or serivce information.
--
--
-- PROGRAM: File Transfer and Protocol Analysis Application
--
-- FUNCTIONS:
--				WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--						LPSTR lspszCmdParam, int nCmdShow)
--				LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)
--				int connect(HWND hwnd, LPCSTR fileData)
--				int upload_file(HWND hwnd, NETWORK* uploadData)
--				void disconnect(HWND hwnd)
--				int checkPackInput()
--				int checkIpInput()
--				int checkPortInput()
--				void processEndData()
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- NOTES:
-- This application provides three selections to perform WinSocket API database lookup.
-- The three selections are name address, service port, and port service. Once these lookup are
-- executed with the appropriate input(s), the host's information will be diaplayed on the screen.
----------------------------------------------------------------------------------------------------------------------*/
#pragma once

#include "application.h"
using namespace std;

//Handlers for the tables for send and receive
HWND hInput2;
HWND hInput3;
HWND inputPacketSizeLabel;
HWND hwndButton;
HWND radioBtnClient;
HWND radioBtnServer;
HWND radioBtnTCP;
HWND radioBtnUDP;
HWND radioBtnTenTimes;
HWND radioBtnHundredTimes;
HWND textHwndLabel2;
HWND textHwndLabel3;
NETWORK network;
HDC hdc;
LPSTR bufferGlobalAlloc;
static TCHAR Name[] = TEXT("Basic Window Socket");
static HBRUSH startBackGroundColor = CreateSolidBrush(RGB(255, 255, 255));
char str[255];	//output buffer

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: wWinMain
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--					LPSTR lspszCmdParam, int nCmdShow)
--
-- RETURNS: int
--
-- NOTES:
-- This function creates window and the user interface.
--
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI wWinMain(_In_ HINSTANCE hInst,_In_opt_ HINSTANCE hprevInstance,
	_In_ LPWSTR lspszCmdParam, _In_ int nCmdShow)
{
	MSG Msg;
	WNDCLASSEX Wcl;
	
	// Define a Window class
	Wcl.cbSize = sizeof(WNDCLASSEX);
	Wcl.style = 0; // default style
	Wcl.hIcon = LoadIcon(NULL, (LPCSTR)IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc; // window function
	Wcl.hInstance = hInst; // handle to this instance
	Wcl.hbrBackground = startBackGroundColor;
	Wcl.lpszClassName = Name; // window class name

	Wcl.lpszMenuName = TEXT("MYMENU");
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0;

	// Register the class
	if (!RegisterClassEx(&Wcl))
		return 0;

	network.hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		500, 400, NULL, NULL, hInst, NULL);

	CreateWindow("STATIC", "<<-- File Transfer and Protocal Analysis -->>", WS_CHILD | WS_VISIBLE | SS_CENTER,
		30, 10, 425, 20, network.hwnd, NULL, NULL, NULL);

	CreateWindow("STATIC", "Select Server or Client Mode: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 45, 190, 20, network.hwnd, NULL, NULL, NULL);

	radioBtnServer = CreateWindow(TEXT("BUTTON"), TEXT("Server"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		245, 45, 80, 20, network.hwnd, (HMENU)ID_SERVER_BTN, NULL, NULL);

	radioBtnClient = CreateWindow(TEXT("BUTTON"), TEXT("Client"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		350, 45, 60, 20, network.hwnd, (HMENU)ID_CLIENT_BTN, NULL, NULL);

	CreateWindow("STATIC", "Select Protocol: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 65, 130, 20, network.hwnd, NULL, NULL, NULL);

	radioBtnTCP = CreateWindow(TEXT("BUTTON"), TEXT("TCP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		245, 65, 60, 20, network.hwnd, (HMENU)ID_TCP_BTN, NULL, NULL);

	radioBtnUDP = CreateWindow(TEXT("BUTTON"), TEXT("UDP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		350, 65, 60, 20, network.hwnd, (HMENU)ID_UDP_BTN, NULL, NULL);

	CreateWindow("STATIC", "Select Packet Times: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 85, 150, 20, network.hwnd, NULL, NULL, NULL);

	radioBtnTenTimes = CreateWindow(TEXT("BUTTON"), TEXT("10"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		245, 85, 60, 20, network.hwnd, (HMENU)ID_PACKET_TEN_TIMES_BTN, NULL, NULL);

	radioBtnHundredTimes = CreateWindow(TEXT("BUTTON"), TEXT("100"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		350, 85, 60, 20, network.hwnd, (HMENU)ID_PACKETS_HUNDRED_TIMES_BTN, NULL, NULL);

	CreateWindow("STATIC", "Enter the Packet Size: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 135, 150, 20, network.hwnd, NULL, NULL, NULL);

	inputPacketSizeLabel = CreateWindow("edit", "1024", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		205, 135, 250, 20, network.hwnd, NULL, NULL, NULL);

	textHwndLabel3 = CreateWindow("STATIC", "Enter the Port: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 165, 100, 20, network.hwnd, NULL, NULL, NULL);

	hInput3 = CreateWindow("edit", "5150", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		205, 165, 250, 20, network.hwnd, NULL, NULL, NULL);

	textHwndLabel2 = CreateWindow("STATIC", "Enter the IP: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
		30, 195, 100, 20, network.hwnd, NULL, NULL, NULL);

	hInput2 = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		205, 195, 250, 20, network.hwnd, NULL, NULL, NULL);

	SendMessage(radioBtnServer, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
	SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_CHECKED, 0);
	ShowWindow(hInput2, SW_HIDE);
	ShowWindow(textHwndLabel2, SW_HIDE);	

	EnableMenuItem(GetMenu(network.hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);

	ShowWindow(network.hwnd, nCmdShow);
	UpdateWindow(network.hwnd);
	//void WINAPI ThreadFuc(HWND hwnd, LPVOID n); //second thread
	// Create the message loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg); // translate keybpard messages
		DispatchMessage(&Msg); // dispatch message and return control to windows
	}

	return (int)Msg.wParam;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)
--
-- RETURNS: LRESULT
--
-- NOTES:
-- This function process windows messages and define the behavior of this application.
--
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
	WPARAM wParam, LPARAM lParam)
{
	hdc = GetDC(hwnd);

	switch (Message)
	{
	case WM_CREATE: //creates the labels, text fields and buttons


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_DISCONNECT:
			if (network.connected) {
				disconnect(hwnd);
				processEndData();
				network.numPackRecv = 0;
				network.connected = 0;
			}
			EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);
			EnableMenuItem(GetMenu(hwnd), ID_CONNECT, MF_ENABLED);
			break;
			
		case ID_CONNECT:
			if (connect(hwnd, network.data)) {
				EnableMenuItem(GetMenu(hwnd), ID_CONNECT, MF_DISABLED | MF_GRAYED);
				EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_ENABLED );
				network.connected = 1;
			}
			break;
		case ID_UPLOAD:
			network.uploaded = upload_file(hwnd, &network);
			break;

		case ID_EXIT:
			SendMessage(hwnd, NULL, FD_CLOSE, NULL);
			disconnect(hwnd);
			PostQuitMessage(0); //terminates the program
			break;

		case ID_SERVER_BTN:
			SendMessage(radioBtnServer, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnClient, BM_SETCHECK, BST_UNCHECKED, 0);
			ShowWindow(hInput2, SW_HIDE);
			ShowWindow(textHwndLabel2, SW_HIDE);
			network.selectServerClient = 0;
			break;

		case ID_CLIENT_BTN:
			SendMessage(radioBtnServer, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnClient, BM_SETCHECK, BST_CHECKED, 0);
			ShowWindow(textHwndLabel2, SW_RESTORE);
			ShowWindow(hInput2, SW_RESTORE);

			network.selectServerClient = 1;
			break;

		case ID_TCP_BTN:
			SendMessage(radioBtnUDP, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
			network.selectedProtocal = 0;
			break;

		case ID_UDP_BTN:
			SendMessage(radioBtnUDP, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnTCP, BM_SETCHECK, BST_UNCHECKED, 0);
			network.selectedProtocal = 1;
			break;

		case ID_PACKET_TEN_TIMES_BTN:
			SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnHundredTimes, BM_SETCHECK, BST_UNCHECKED, 0);
			network.timesPacketsSelection = 10;
			break;

		case ID_PACKETS_HUNDRED_TIMES_BTN:
			SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnHundredTimes, BM_SETCHECK, BST_CHECKED, 0);
			network.timesPacketsSelection = 100;
			break;
		}
		break;
	case WM_FAILED_CONNECT:
		if (network.connected) {
			disconnect(hwnd);
			processEndData();
			network.numPackRecv = 0;
			network.connected = 0;
		}
		EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);
		EnableMenuItem(GetMenu(hwnd), ID_CONNECT, MF_ENABLED);
		break;

	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(255, 255, 255));
		return (INT_PTR)startBackGroundColor;

	case WM_DESTROY:		// message to terminate the program
		disconnect(hwnd);
		processEndData();
		SendMessage(hwnd, NULL, FD_CLOSE, NULL);
		PostQuitMessage(0);
		break;

	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connect
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int connect(HWND hwnd, LPCSTR fileData)
--
-- RETURNS: int
--
-- NOTES:
-- This function starts a thread for client or server.
--
----------------------------------------------------------------------------------------------------------------------*/
int connect(HWND hwnd, LPCSTR fileData) {
	if (checkPackInput()) {
		if (network.selectServerClient) {
			if (checkIpInput() && checkPortInput()) {
				if (network.selectedProtocal) { // 0 is tcp, 1 is UDP
					_beginthread(udp_client, 1, &network);
				}
				else {
					_beginthread(tcp_client, 1, &network);
				}
				return 1;
			}
			else {
				MessageBox(NULL, TEXT("Please enter ip and port size"), "", MB_OK);
			}
		}
		else {
			if (checkPortInput()) {
				if (network.selectedProtocal) {
					//udp server
					_beginthread(serverMainUDP, 1, &network);
				}
				else {
					_beginthread(serverMainTCP, 1, &network);
				}
			}
			return 1;
		}
	}
	else {
		MessageBox(NULL, TEXT("Please enter packet size"), "", MB_OK);
	}
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: upload_file
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int upload_file(HWND hwnd, NETWORK* uploadData)
--
-- RETURNS: int
--
-- NOTES:
-- This function process windows messages and define the behavior of this application.
--
----------------------------------------------------------------------------------------------------------------------*/
int upload_file(HWND hwnd, NETWORK* uploadData) {
	OPENFILENAME ofn;
	char file_name[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = "All files\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;

	GetOpenFileNameA(&ofn);
	int n = strcmp(uploadData->filePath, ofn.lpstrFile);

	ifstream infile(ofn.lpstrFile);
	string line;
	stringstream stream;
	string indexTxt;

	if (infile.is_open()) {
		while (!infile.eof()) {
			stream.clear();
			getline(infile, line);
		}
	}

	HANDLE file = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fileSize = GetFileSize(file, NULL);
	long num = fileSize + 1;
	if (bufferGlobalAlloc == NULL) {
		bufferGlobalAlloc = (LPSTR)GlobalAlloc(GPTR, num);
	}
	else {
		GlobalFree(bufferGlobalAlloc);
		bufferGlobalAlloc = (LPSTR)GlobalAlloc(GPTR, num);
	}
	DWORD read;
	if (!ReadFile(file, bufferGlobalAlloc, fileSize, &read, NULL)) {
		MessageBox(NULL, TEXT("Failed to read the file"), "", MB_OK);
		return 0;
	}
	uploadData->data = (LPCSTR)bufferGlobalAlloc;
	MessageBox(NULL, "Uploaded", "", MB_OK);
	return 1;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: disconnect
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: void disconnect(HWND hwnd)
--
-- RETURNS: void
--
-- NOTES:
-- This function disconnet the UDP or TCP server.
--
----------------------------------------------------------------------------------------------------------------------*/
void disconnect(HWND hwnd) {
	network.numByteRead = 0;
	if (!network.selectServerClient) {
		if (network.selectedProtocal) {
			disconnectSocketServerUDP(network.sdServer);
		}
		else {
			disconnectSocketServerTCP(network.sdServer); //disconnect server
		}
	}
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: checkPackInput
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int checkPackInput() 
--
-- RETURNS: int
--
-- NOTES:
-- This function validated the packet input field.
--
----------------------------------------------------------------------------------------------------------------------*/
int checkPackInput() {
	int inputPacketLength;
	TCHAR inputPacket[64];
	GetWindowText(inputPacketSizeLabel, str, 64);
	if (GetWindowTextLengthA(inputPacketSizeLabel) != 0) {
		inputPacketLength = GetWindowTextLengthA(inputPacketSizeLabel);
		GetWindowText(inputPacketSizeLabel, inputPacket, 64);
		network.packSize = atoi(inputPacket);
		return 1;
	}
	
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: checkIpInput
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int checkIpInput()
--
-- RETURNS: int
--
-- NOTES:
-- This function validated the ip input field.
--
----------------------------------------------------------------------------------------------------------------------*/
int checkIpInput() {
	int inputIPLength;
	TCHAR inputIP[64];
	memset(inputIP, 0, 64);
	GetWindowText(hInput2, str, 64);
	if (GetWindowTextLengthA(hInput2) != 0) {
		inputIPLength = GetWindowTextLengthA(hInput2);
		GetWindowText(hInput2, inputIP, 64);
		strcpy(network.ip, inputIP);
		//network.ip = inputIP;
		return 1;
	}
	else {
	}
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: checkPortInput
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int checkPortInput()
--
-- RETURNS: int
--
-- NOTES:
-- This function validated the port input field.
--
----------------------------------------------------------------------------------------------------------------------*/
int checkPortInput() {
	int inputIPLength;
	TCHAR inputPort[64];
	memset(inputPort, 0, 64);
	GetWindowText(hInput3, str, 64);
	if (GetWindowTextLengthA(hInput3) != 0) {
		inputIPLength = GetWindowTextLengthA(hInput2);
		GetWindowText(hInput3, inputPort, 64);
		strcpy(network.port, inputPort);
		//network.ip = inputIP;
		return 1;
	}
	else {
	}
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: processEndData
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: void processEndData() 
--
-- RETURNS: void
--
-- NOTES:
-- This function writes the received packets and the duration of transfer.
--
----------------------------------------------------------------------------------------------------------------------*/
void processEndData() {
	char buff[100];
	long duration = network.endTime - network.startTime;

	writeToFile((LPSTR)("\r\n----------------\r\n"), &network);
	writeToFile((LPSTR)("\r\n Number of Received Packets:\r\n"), &network);
	writeToFile(LPSTR(to_string(network.numPackRecv).c_str()), &network);

	sprintf_s(buff, "\r\nTransfer time from client to last message received from server: %dms\r\n", duration);
	LPSTR messageHeader2 = buff;
	writeToFile(messageHeader2, &network);
}