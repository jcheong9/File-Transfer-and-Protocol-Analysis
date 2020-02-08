#include "tcp_server.h"

typedef struct _SOCKET_INFORMATION {
    OVERLAPPED Overlapped;
    SOCKET Socket;
    CHAR Buffer[DATA_BUFSIZE];
    WSABUF DataBuf;
    DWORD BytesSEND;
    DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);

DWORD WINAPI WorkerThread(LPVOID lpParameter);

SOCKET AcceptSocket;
NETWORK* pp;

void serverMain(PVOID network)
{
    pp = (NETWORK*)network;
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    WSAEVENT AcceptEvent;
    char buff[100];

    if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
    {
        printf("WSAStartup failed with error %d\n", Ret);
        sprintf_s(buff, "WSAStartup failed with error %d\n", Ret);
        MessageBox(pp->hwnd, buff, TEXT(""), MB_OK);
        WSACleanup();
        return;
    }

    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
        WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        printf("Failed to get a socket %d\n", WSAGetLastError());
        return;
    }

    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (PSOCKADDR)&InternetAddr,
        sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        printf("bind() failed with error %d\n", WSAGetLastError());
        return;
    }

    if (listen(ListenSocket, 5))
    {
        printf("listen() failed with error %d\n", WSAGetLastError());
        return;
    }

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
        return;
    }

    // Create a worker thread to service completed I/O requests. 

    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID)AcceptEvent, 0, &ThreadId)) == NULL)
    {
        printf("CreateThread failed with error %d\n", GetLastError());
        return;
    }

    while (TRUE)
    {
        AcceptSocket = accept(ListenSocket, NULL, NULL);

        if (WSASetEvent(AcceptEvent) == FALSE)
        {
            printf("WSASetEvent failed with error %d\n", WSAGetLastError());
            return;
        }
    }
}

DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;
    char buff[100];

    // Save the accept event in the event array.

    EventArray[0] = (WSAEVENT)lpParameter;

    while (TRUE)
    {
        // Wait for accept() to signal an event and also process WorkerRoutine() returns.

        while (TRUE)
        {
            Index = WSAWaitForMultipleEvents(1, EventArray, FALSE, WSA_INFINITE, TRUE);

            if (Index == WSA_WAIT_FAILED)
            {
                printf("WSAWaitForMultipleEvents failed with error %d\n", WSAGetLastError());
                return FALSE;
            }

            if (Index != WAIT_IO_COMPLETION)
            {
                // An accept() call event is ready - break the wait loop
                break;
            }
        }

        WSAResetEvent(EventArray[Index - WSA_WAIT_EVENT_0]);

        // Create a socket information structure to associate with the accepted socket.

        if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
            sizeof(SOCKET_INFORMATION))) == NULL)
        {
            printf("GlobalAlloc() failed with error %d\n", GetLastError());
            return FALSE;
        }

        // Fill in the details of our accepted socket.

        SocketInfo->Socket = AcceptSocket;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;
        memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
            &(SocketInfo->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                //printf("WSARecv() failed with error %d\n", WSAGetLastError());
                sprintf_s(buff, "WSARecv() failed with error %d\n", WSAGetLastError());
                MessageBox(pp->hwnd, buff, TEXT(""), MB_OK);
                return FALSE;
            }
        }
        else {
            
            //printf("Socket %d connected\n", AcceptSocket);
            pp->numByteRead = pp->numByteRead + RecvBytes;
            //memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);
            pp->endTime = float(clock() - pp->beginTime);	 //mill sec
            pp->data = SocketInfo->Buffer;
        }
        sprintf_s(buff, "Socket %d connected\n", AcceptSocket);
        MessageBox(pp->hwnd, buff, TEXT(""), MB_OK);

       
    }

    return TRUE;
}

void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD SendBytes, RecvBytes;
    DWORD Flags;

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        printf("I/O operation failed with error %d\n", Error);
    }

    if (BytesTransferred == 0)
    {
        printf("Closing socket %d\n", SI->Socket);
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        GlobalFree(SI);
        return;
    }

    // Check to see if the BytesRECV field equals zero. If this is so, then
    // this means a WSARecv call just completed so update the BytesRECV field
    // with the BytesTransferred value from the completed WSARecv() call.

    if (SI->BytesRECV == 0)
    {
        SI->BytesRECV = BytesTransferred;
        SI->BytesSEND = 0;
    }
    else
    {
        SI->BytesSEND += BytesTransferred;
    }

    if (0)
    {

        // Post another WSASend() request.
        // Since WSASend() is not gauranteed to send all of the bytes requested,
        // continue posting WSASend() calls until all received bytes are sent.

        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.buf = SI->Buffer + SI->BytesSEND;
        SI->DataBuf.len = SI->BytesRECV - SI->BytesSEND;

        if (WSASend(SI->Socket, &(SI->DataBuf), 1, &SendBytes, 0,
            &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSASend() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
    }
    else
    {
        SI->BytesRECV = 0;

        // Now that there are no more bytes to send post another WSARecv() request.

        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.len = DATA_BUFSIZE;
        SI->DataBuf.buf = SI->Buffer;

        if (WSARecv(SI->Socket, &(SI->DataBuf), 1, &RecvBytes, &Flags,
            &(SI->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else {
            pp->numByteRead = pp->numByteRead + RecvBytes;
            pp->endTime = float(clock() - pp->beginTime);	 //mill sec
            //writeToFile(LPSTR(to_string(pp->numByteRead).c_str()));
            string str = convert(pp->data).c_str();
            writeToFile((LPSTR)str.c_str());
        }
    }
}


int writeToFile(LPSTR data) {
    //string filepath = convert(uploadData->filePath); // convert LPCST to std::string

    //open save dialog box only if the file is not loaded
    if (!pp->loadFile) {
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

        GetSaveFileName(&ofn);
        pp->filePath = ofn.lpstrFile;
        pp->loadFile = 1;
    }

    HANDLE hFile = CreateFile(TEXT(pp->filePath),      // name of the write
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


void processReceiveData(LPSTR data) {
    LPSTR str1 = new TCHAR[strlen(data) + 1];
    memset(str1, 0, strlen(data));
    if (pp->data == NULL) {
        strcpy(str1, data);
        pp->data = str1;
    }
    else {
        std::string(pp->data).append(data);
        pp->data = str1;

    }
    //strcpy(pp.data,str1);
}

string convert(LPCSTR str) {
    return std::string(str);
}