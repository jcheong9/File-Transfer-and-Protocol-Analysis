#include "common.h"
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

int loadSaveFile(LPSTR data, NETWORK_STRUC networkStruct) {
    //open save dialog box only if the file is not loaded

    OPENFILENAME ofn;
    char file_name[100];
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = networkStruct->hwnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "*.txt\0";
    ofn.nFilterIndex = 1;

    GetSaveFileName(&ofn);
    strncpy(networkStruct->filePath, ofn.lpstrFile, 100);
    if (strcmp(networkStruct->filePath, "") == 0) {
        return 0;
    }
    //string filepath = convert(uploadData->filePath); // convert LPCST to std::string
    HANDLE hFile = CreateFile(TEXT(networkStruct->filePath),      // name of the write
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

    strText = convert(data);

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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
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
int writeToFile(LPSTR data, NETWORK_STRUC networkStruct) {
    //string filepath = convert(uploadData->filePath); // convert LPCST to std::string
    HANDLE hFile = CreateFile(TEXT(networkStruct->filePath),      // name of the write
        FILE_APPEND_DATA,       // open for appending
        0,                      // do not share
        NULL,                   // default security
        OPEN_ALWAYS,          // overwrite existing
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    if (hFile == INVALID_HANDLE_VALUE)
    {
        // Failed to open/create file
        return 2;
    }

    // Write data to the file
    std::string strText; // For C use LPSTR (char*) or LPWSTR (wchar_t*)

    strText = convert(data);

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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
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
string convert(LPCSTR str) {
    return std::string(str);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
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
long getTimeConvertToMil(SYSTEMTIME t1) {
    long d;

    d = t1.wSecond * 1000;
    d += t1.wMinute * 60 * 1000;
    d += t1.wMilliseconds;

    return d;
}