/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: winmain.cpp -	An application that uses basic Winsock 2 API database
--								lookup calls to get host or serivce information.
--
--
-- PROGRAM: Basic Window Socket Application
--
-- FUNCTIONS:
--				WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
--						LPSTR lspszCmdParam, int nCmdShow)
--				LRESULT CALLBACK WndProc(HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)
--
-- DATE: January 15, 2020
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

//Textbox handlers for send and receive
HWND textHwnd;
HWND textHwndRx;

//Handlers for the tables for send and receive
HWND hWndListView;
HWND hWndListViewRx;
HWND hInput2;
HWND inputPacketSizeLabel;
HWND hwndButton;
HWND radioBtnClient;
HWND radioBtnServer;
HWND radioBtnTCP;
HWND radioBtnUDP;
HWND radioBtnTenTimes;
HWND radioBtnHundredTimes;
HWND radioBtnPacketTrue;
HWND radioBtnPacketFalse;


HWND textHwndLabel4;
HWND textHwndLabel5;
HWND textHwndLabel2;


PORTPARMA portparma;
NETWORK network;

HDC hdc;
HANDLE serverThread = NULL;
HANDLE clientThread = NULL;

static unsigned k = 0;
static TCHAR Name[] = TEXT("Basic Window Socket");
static HBRUSH startBackGroundColor = CreateSolidBrush(RGB(255, 255, 255));

char str[255];	//output buffer
int X = 0, Y = 0; // Current coordinates


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: January 15, 2020
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
-- DATE: January 15, 2020
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
		CreateWindow("STATIC", "<<-- File Transfer and Protocal Analysis -->>", WS_CHILD | WS_VISIBLE | SS_CENTER,
			30, 10, 425, 20, hwnd, NULL, NULL, NULL);

		CreateWindow("STATIC", "Select Server or Client Mode: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 45, 190, 20, hwnd, NULL, NULL, NULL);
		
		radioBtnServer = CreateWindow(TEXT("BUTTON"), TEXT("Server"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 45, 80, 20, hwnd, (HMENU)ID_SERVER_BTN, NULL, NULL);
		
		radioBtnClient = CreateWindow(TEXT("BUTTON"), TEXT("Client"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 45, 60, 20, hwnd, (HMENU)ID_CLIENT_BTN, NULL, NULL);

		CreateWindow("STATIC", "Select Protocol: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 65, 130, 20, hwnd, NULL, NULL, NULL);

		radioBtnTCP = CreateWindow(TEXT("BUTTON"), TEXT("TCP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 65, 60, 20, hwnd, (HMENU)ID_TCP_BTN, NULL, NULL);

		radioBtnUDP = CreateWindow(TEXT("BUTTON"), TEXT("UDP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 65, 60, 20, hwnd, (HMENU)ID_UDP_BTN, NULL, NULL);

		CreateWindow("STATIC", "Select Packet Times: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 85, 150, 20, hwnd, NULL, NULL, NULL);

		radioBtnTenTimes = CreateWindow(TEXT("BUTTON"), TEXT("10"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 85, 60, 20, hwnd, (HMENU)ID_PACKET_TEN_TIMES_BTN, NULL, NULL);

		radioBtnHundredTimes = CreateWindow(TEXT("BUTTON"), TEXT("100"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 85, 60, 20, hwnd, (HMENU)ID_PACKETS_HUNDRED_TIMES_BTN, NULL, NULL);

		CreateWindow("STATIC", "Packetize Message: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 105, 150, 20, hwnd, NULL, NULL, NULL);

		radioBtnPacketFalse = CreateWindow(TEXT("BUTTON"), TEXT("False"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 105, 60, 20, hwnd, (HMENU)ID_PACKETS_FALSE, NULL, NULL);

		radioBtnPacketTrue = CreateWindow(TEXT("BUTTON"), TEXT("True"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 105, 60, 20, hwnd, (HMENU)ID_PACKETS_TRUE, NULL, NULL);

		CreateWindow("STATIC", "Enter the Packet Size: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 135, 150, 20, hwnd, NULL, NULL, NULL);

		inputPacketSizeLabel = CreateWindow("edit", "1024", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			205, 135, 250, 20, hwnd, NULL, NULL, NULL);
		
		textHwndLabel2 = CreateWindow("STATIC", "Enter the IP: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 165, 100, 20, hwnd, NULL, NULL, NULL);

		hInput2 = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			205, 165, 250, 20, hwnd, NULL, NULL, NULL);

/*
		textHwndLabel4 = CreateWindow("STATIC", "Status: ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 195, 100, 20, hwnd, NULL, NULL, NULL);

		textHwndLabel5 = CreateWindow("STATIC", "Not Connected ", WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			205, 195, 100, 20, hwnd, NULL, NULL, NULL);
		hwndButton = CreateWindow("BUTTON", "Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			350, 215, 100, 20, hwnd, (HMENU)ID_SEND_BTN, NULL, NULL);

*/

		
		SendMessage(radioBtnServer, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnPacketFalse, BM_SETCHECK, BST_CHECKED, 0);
		ShowWindow(hInput2, SW_HIDE);
		ShowWindow(textHwndLabel2, SW_HIDE);
		EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_DISCONNECT:
			if (network.connected) {
				disconnect(hwnd);
				processEndData();
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
			portparma.selectServerClient = 0;
			break;

		case ID_CLIENT_BTN:
			SendMessage(radioBtnServer, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnClient, BM_SETCHECK, BST_CHECKED, 0);
			ShowWindow(textHwndLabel2, SW_RESTORE);
			ShowWindow(hInput2, SW_RESTORE);
			portparma.selectServerClient = 1;
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
			network.numPackets = 10;
			break;

		case ID_PACKETS_HUNDRED_TIMES_BTN:
			SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnHundredTimes, BM_SETCHECK, BST_CHECKED, 0);
			network.numPackets = 100;
			break;

		case ID_PACKETS_TRUE:
			network.packetMessage = 1;
			SendMessage(radioBtnPacketFalse, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnPacketTrue, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case ID_PACKETS_FALSE:
			network.packetMessage = 0;
			SendMessage(radioBtnPacketFalse, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnPacketTrue, BM_SETCHECK, BST_UNCHECKED, 0);
			break;
		}
		break;
	case WM_FAILED_CONNECT:
		EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);
		EnableMenuItem(GetMenu(hwnd), ID_CONNECT, MF_ENABLED);
		break;

	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(255, 255, 255));
		return (INT_PTR)startBackGroundColor;

	case WM_DESTROY:		// message to terminate the program
		disconnect(hwnd);
		SendMessage(hwnd, NULL, FD_CLOSE, NULL);
		PostQuitMessage(0);
		break;

	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int connect(HWND hwnd, LPCSTR fileData) {

	if (checkPackInput()) {
		if (portparma.selectServerClient) {
			if (checkIpInput()) {
				if (network.selectedProtocal) { // 0 is tcp, 1 is UDP
					clientThread = (HANDLE)_beginthread(udp_client, 1, &network);
				}
				else {
					clientThread = (HANDLE)_beginthread(tcp_client, 1, &network);
				}
				return 1;
			}
			else {
				MessageBox(NULL, TEXT("Please enter packet size"), "", MB_OK);
			}
		}
		else {
			if (checkPackInput()) {

				if (network.selectedProtocal) {
					//udp server
					serverThread = (HANDLE)_beginthread(serverMainUDP, 1, &network);
				}
				else {
					serverThread = (HANDLE)_beginthread(serverMain, 1, &network);
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
	strcmp(uploadData->filePath, ofn.lpstrFile);

	MessageBox(NULL, uploadData->filePath, "", MB_OK);
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
	LPSTR buffer = (LPSTR)GlobalAlloc(GPTR, num);
	DWORD read;
	if (!ReadFile(file, buffer, fileSize, &read, NULL)) {
		MessageBox(NULL, TEXT("Failed to read the file"), "", MB_OK);
		return 0;
	}
	uploadData->data = (LPCSTR)buffer;
	MessageBox(NULL, TEXT(buffer), "", MB_OK);

	return 1;
}

void disconnect(HWND hwnd) {
	if (portparma.selectServerClient) {
		disconnectSocketClient(&network.sdClient);	//disconnect client
	}
	else {
		//disconnectSocketServer(network.siServer); //disconnect server
	}
}

int checkPackInput() {
	int inputPacketLength;
	TCHAR inputPacket[255];
	GetWindowText(inputPacketSizeLabel, str, 255);
	if (GetWindowTextLengthA(inputPacketSizeLabel) != 0) {
		inputPacketLength = GetWindowTextLengthA(inputPacketSizeLabel);
		GetWindowText(inputPacketSizeLabel, inputPacket, 255);
		network.packSize = atoi(inputPacket);
		return 1;
	}
	
	return 0;
}

int checkIpInput() {
	int inputIPLength;
	TCHAR inputIP[255];
	memset(inputIP, 0, 100);
	GetWindowText(hInput2, str, 255);
	if (GetWindowTextLengthA(hInput2) != 0) {
		inputIPLength = GetWindowTextLengthA(hInput2);
		GetWindowText(hInput2, inputIP, 255);
		network.ip = inputIP;
		return 1;
	}
	else {
	}
	return 0;
}

void processEndData() {
	char buff[100];

	writeToFile((LPSTR)("\r\n Number of Received Packets:\r\n"), &network);
	writeToFile(LPSTR(to_string(network.numPackRecv).c_str()), &network);
	sprintf_s(buff, "\r\nEnding Time from server %d\r\n", clock());
	LPSTR messageHeader2 = buff;
	writeToFile(messageHeader2, &network);
	//writeToFile(,network);
}