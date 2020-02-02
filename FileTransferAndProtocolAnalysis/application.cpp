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
HWND hwndButton;
HWND radioBtnClient;
HWND radioBtnServer;
HWND radioBtnTCP;
HWND radioBtnUDP;
HWND radioBtnTenTimes;
HWND radioBtnHundredTimes;

HWND textHwndLabel;
HWND textHwndLabel2;
HWND textHwndLabel3;
HWND textHwndLabel4;
HWND textHwndLabel5;

PORTPARMA portparma;
UPLOADFILE uploadData;
HDC hdc;

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

	portparma.hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
		500, 400, NULL, NULL, hInst, NULL);

	ShowWindow(portparma.hwnd, nCmdShow);
	UpdateWindow(portparma.hwnd);
	//void WINAPI ThreadFuc(HWND hwnd, LPVOID n); //second thread
	// Create the message loop
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg); // translate keybpard messages
		DispatchMessage(&Msg); // dispatch message and return control to windows
	}

	return Msg.wParam;
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
	TCHAR str[256];
	TCHAR input2Text[256];
	int lengthInput2;
	//TCHAR * str1;

	//tcpCallBack( hwnd, Message, wParam, lParam);

	switch (Message)
	{
	case WM_CREATE: //creates the labels, text fields and buttons
		textHwndLabel = CreateWindow("STATIC", "<<-- File Transfer and Protocal Analysis -->>", WS_CHILD | WS_VISIBLE | SS_CENTER,
			30, 10, 425, 20, hwnd, NULL, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Select Server or Client Mode: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 45, 190, 20, hwnd, NULL, NULL, NULL);
		
		radioBtnServer = CreateWindow(TEXT("BUTTON"), TEXT("Server"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 45, 80, 20, hwnd, (HMENU)ID_SERVER_BTN, NULL, NULL);
		
		radioBtnClient = CreateWindow(TEXT("BUTTON"), TEXT("Client"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 45, 60, 20, hwnd, (HMENU)ID_CLIENT_BTN, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Select Protocol: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 65, 130, 20, hwnd, NULL, NULL, NULL);

		radioBtnTCP = CreateWindow(TEXT("BUTTON"), TEXT("TCP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 65, 60, 20, hwnd, (HMENU)ID_TCP_BTN, NULL, NULL);

		radioBtnUDP = CreateWindow(TEXT("BUTTON"), TEXT("UDP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 65, 60, 20, hwnd, (HMENU)ID_UDP_BTN, NULL, NULL);

		textHwndLabel3 = CreateWindow("STATIC", "Select Packet Times: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 85, 150, 20, hwnd, NULL, NULL, NULL);

		radioBtnTenTimes = CreateWindow(TEXT("BUTTON"), TEXT("10"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			245, 85, 60, 20, hwnd, (HMENU)ID_PACKET_TEN_TIMES_BTN, NULL, NULL);

		radioBtnHundredTimes = CreateWindow(TEXT("BUTTON"), TEXT("100"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 85, 60, 20, hwnd, (HMENU)ID_PACKETS_HUNDRED_TIMES_BTN, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Enter the IP: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 115, 100, 20, hwnd, NULL, NULL, NULL);

		hInput2 = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			205, 115, 250, 20, hwnd, NULL, NULL, NULL);

		textHwndLabel4 = CreateWindow("STATIC", "Status: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 145, 100, 20, hwnd, NULL, NULL, NULL);

		textHwndLabel5 = CreateWindow("STATIC", "Not Connected ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			205, 145, 100, 20, hwnd, NULL, NULL, NULL);

		hwndButton = CreateWindow("BUTTON", "Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			350, 215, 100, 20, hwnd, (HMENU)ID_ENTER_BTN, NULL, NULL);

		SendMessage(radioBtnServer, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_CHECKED, 0);
		ShowWindow(hInput2, SW_HIDE);
		ShowWindow(textHwndLabel2, SW_HIDE);
		EnableMenuItem(GetMenu(hwnd), ID_DISCONNECT, MF_DISABLED | MF_GRAYED);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CONNECT:
			//serverMain(hwnd);
			if (portparma.uploaded)
				connect(hwnd, uploadData.data);
			break;
		case ID_UPLOAD:
			portparma.uploaded = upload_file(hwnd, &uploadData);
			OutputDebugString(TEXT(uploadData.filePath));
			OutputDebugString(TEXT(uploadData.data));
			break;

		case ID_EXIT:
			SendMessage(0, NULL, FD_CLOSE, NULL);
			PostQuitMessage(0); //terminates the program
			break;

		case ID_ENTER_BTN: //execute functions when button is clicked
			GetWindowText(hInput2, str, 256);
			if (GetWindowTextLengthA(hInput2) != 0) {
				switch (portparma.selectedProtocal) {
					case 1:
						lengthInput2= GetWindowTextLengthA(hInput2);
						GetWindowText(hInput2, input2Text, 256); //get the HInput's text
						break;
					case 2:
						lengthInput2 = GetWindowTextLengthA(hInput2);
						GetWindowText(hInput2, input2Text, 256);
						
						break;
					default:

						break;
				}
			}
			else {
				SetWindowText(textHwnd, "Invalid input");
			}
			break;
		case ID_SERVER_BTN:
			//OutputDebugString("server");
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
			portparma.selectedProtocal = 0;
			break;

		case ID_UDP_BTN:
			SendMessage(radioBtnUDP, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnTCP, BM_SETCHECK, BST_UNCHECKED, 0);
			portparma.selectedProtocal = 1;
			break;

		case ID_PACKET_TEN_TIMES_BTN:
			SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnHundredTimes, BM_SETCHECK, BST_UNCHECKED, 0);
			break;

		case ID_PACKETS_HUNDRED_TIMES_BTN:
			SendMessage(radioBtnTenTimes, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnHundredTimes, BM_SETCHECK, BST_CHECKED, 0);
			break;

		}
		break;
	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(255, 255, 255));
		return (INT_PTR)startBackGroundColor;
	case WM_DESTROY:		// message to terminate the program
		SendMessage(NULL, NULL, FD_CLOSE, NULL);
		PostQuitMessage(0);
		break;
	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

void connect(HWND hwnd, LPCSTR fileData) {
	int inputIPLength;
	TCHAR inputIP[255];

	if (portparma.selectServerClient) {
		GetWindowText(hInput2, str, 255);
		if (GetWindowTextLengthA(hInput2) != 0) {
			inputIPLength = GetWindowTextLengthA(hInput2);
			GetWindowText(hInput2, inputIP, 255);
			if (portparma.selectedProtocal) {
				//udp client
			}
			else {
				tcp_client(portparma.hwnd, inputIP, fileData);
			}
		}
	}
	else {
		if (portparma.selectedProtocal) {
			//udp server
		}
		else {
			serverMain(hwnd);
		}
	}
}


int upload_file(HWND hwnd, UPLOADFILE* uploadData) {
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
	uploadData->filePath = ofn.lpstrFile;

	MessageBox(NULL, uploadData->filePath, "", MB_OK);
	ifstream infile(ofn.lpstrFile);
	string line;
	stringstream stream;
	string indexTxt;
	LPSTR sti;

	if (infile.is_open()) {
		while (!infile.eof()) {
			stream.clear();
			getline(infile, line);
		}
	}
	sti = (LPSTR)line.c_str();
	//uploadData->data = sti;
	//MessageBox(NULL, uploadData->data, "", MB_OK);

	HANDLE file = CreateFile(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD fileSize = GetFileSize(file, NULL);
	LPWSTR buffer = (LPWSTR)GlobalAlloc(GPTR, fileSize + 1);
	DWORD read;
	ReadFile(file, buffer, fileSize, &read, NULL);
	uploadData->data = (LPCSTR)buffer;
	MessageBox(NULL, (LPCSTR)buffer, "", MB_OK);
	return 1;
}
