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

HWND textHwndLabel;
HWND textHwndLabel2;

PORTPARMA portparma;
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
		600, 400, NULL, NULL, hInst, NULL);

	ShowWindow(portparma.hwnd, nCmdShow);
	UpdateWindow(portparma.hwnd);
	void WINAPI ThreadFuc(HWND hwnd, LPVOID n); //second thread
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
	switch (Message)
	{
	case WM_CREATE: //creates the labels, text fields and buttons
		textHwndLabel = CreateWindow("STATIC", "<<< Client TCP >>>",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			30, 10, 525, 20, hwnd, NULL, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Select Server or Client Mode: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 45, 150, 20, hwnd, NULL, NULL, NULL);
		
		radioBtnServer = CreateWindow(TEXT("BUTTON"), TEXT("Server"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			205, 45, 100, 20, hwnd, (HMENU)ID_SERVER_BTN, NULL, NULL);
		
		radioBtnClient = CreateWindow(TEXT("BUTTON"), TEXT("Client"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 45, 100, 20, hwnd, (HMENU)ID_CLIENT_BTN, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Select Protocol: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 45, 150, 20, hwnd, NULL, NULL, NULL);

		radioBtnTCP = CreateWindow(TEXT("BUTTON"), TEXT("TCP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			205, 65, 100, 20, hwnd, (HMENU)ID_TCP_BTN, NULL, NULL);

		radioBtnUDP = CreateWindow(TEXT("BUTTON"), TEXT("UDP"), WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			350, 65, 100, 20, hwnd, (HMENU)ID_UDP_BTN, NULL, NULL);

		textHwndLabel2 = CreateWindow("STATIC", "Enter the IP: ",
			WS_VISIBLE | WS_CHILD | SS_LEFT | ES_READONLY,
			30, 120, 150, 20, hwnd, NULL, NULL, NULL);

		hInput2 = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
			205, 120, 350, 20, hwnd, NULL, NULL, NULL);

		hwndButton = CreateWindow("BUTTON", "Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			455, 215, 100, 20, hwnd, (HMENU)ID_ENTER_BTN, NULL, NULL);

		SendMessage(radioBtnServer, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
		ShowWindow(hInput2, SW_HIDE);
		ShowWindow(textHwndLabel2, SW_HIDE);

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CONNECT:
			//portparma.selection = 0;
			//ShowWindow(hInput2, SW_HIDE);
			//ShowWindow(textHwndLabel2, SW_HIDE);
			//SetWindowText(textHwndLabel, "host name -> IP OR IP -> host name");

			break;
		case ID_UPLOAD:
			upload_file(hwnd);
			//portparma.selection = 1;
			//ShowWindow(textHwndLabel2, SW_RESTORE);
			//ShowWindow(hInput2, SW_RESTORE);
			//SetWindowText(textHwndLabel, "service name/protocol -> port number");
			break;

		case ID_EXIT:
			PostQuitMessage(0); //terminates the program
			break;

		case ID_ENTER_BTN: //execute functions when button is clicked
			GetWindowText(hInput2, str, 256);
			if (GetWindowTextLengthA(hInput2) != 0) {
				switch (portparma.selection) {
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
			break;

		case ID_CLIENT_BTN:
			SendMessage(radioBtnServer, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnClient, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case ID_TCP_BTN:
			SendMessage(radioBtnUDP, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(radioBtnTCP, BM_SETCHECK, BST_CHECKED, 0);
			break;

		case ID_UDP_BTN:
			SendMessage(radioBtnUDP, BM_SETCHECK, BST_CHECKED, 0);
			SendMessage(radioBtnTCP, BM_SETCHECK, BST_UNCHECKED, 0);
			break;

		}


		break;
	case WM_CTLCOLORSTATIC:
		hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(255, 255, 255));
		return (INT_PTR)startBackGroundColor;
	case WM_DESTROY:		// message to terminate the program
		PostQuitMessage(0);
		break;
	default: // Let Win32 process all other messages
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

void upload_file(HWND hwnd) {
	OPENFILENAME ofn;
	char file_name[100];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;

	GetOpenFileNameA(&ofn);

}

