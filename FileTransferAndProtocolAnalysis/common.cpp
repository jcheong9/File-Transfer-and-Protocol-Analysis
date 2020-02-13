#include "common.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: common.cpp -	This holds the common functions used through out the application.
--
--
-- PROGRAM: File Transfer and Protocol Analysis Application
--
-- FUNCTIONS:
--              int loadSaveFile(LPSTR data, NETWORK_STRUC networkStruct)
--              int writeToFile(LPSTR data, NETWORK_STRUC networkStruct)
--              string convert(LPCSTR str)
--              long getTimeConvertToMil(SYSTEMTIME t1) 
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
-- This file contains common wrapper functions that are intended to be easily portable
-- between application.
--
----------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: loadSaveFile
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int loadSaveFile(LPSTR data, NETWORK_STRUC networkStruct)
--
-- RETURNS: int
--
-- NOTES:
-- This function creates window for the user to save file.
--
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
-- FUNCTION: writeToFile
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: int writeToFile(LPSTR data, NETWORK_STRUC networkStruct)
--
-- RETURNS: int
--
-- NOTES:
-- This function writes the passed in data into the specified path from the NETWORK_STRUC.
--
----------------------------------------------------------------------------------------------------------------------*/
int writeToFile(LPSTR data, NETWORK_STRUC networkStruct) {

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
-- FUNCTION: convert
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: string convert(LPCSTR str)
--
-- RETURNS: string
--
-- NOTES:
-- This function converts LCSTR to string.
--
----------------------------------------------------------------------------------------------------------------------*/
string convert(LPCSTR str) {
    return std::string(str);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getTimeConvertToMil
--
-- DATE: January 29, 2020
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Jameson Cheong
--
-- PROGRAMMER: Jameson Cheong
--
-- INTERFACE: long getTimeConvertToMil(SYSTEMTIME t1)
--
-- RETURNS: long
--
-- NOTES:
-- This function convert SYSTEMTIME to milisecond and return it.
--
----------------------------------------------------------------------------------------------------------------------*/
long getTimeConvertToMil(SYSTEMTIME t1) {
    long d;

    d = t1.wSecond * 1000;
    d += t1.wMinute * 60 * 1000;
    d += t1.wMilliseconds;

    return d;
}