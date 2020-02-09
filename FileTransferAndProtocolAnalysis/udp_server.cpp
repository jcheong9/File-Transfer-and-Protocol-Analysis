
#include "tcp_server.h"



void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);

DWORD WINAPI WorkerThreadUDP(LPVOID lpParameter);
int firstMessageUDP = 1;

SOCKET AcceptSocketUDP;
NETWORK* networkStructUDP;

void serverMainUDP(PVOID network)
{
    networkStructUDP = (NETWORK*)network;
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    WSAEVENT AcceptEvent;
    char buff[100];
    memset(buff, 0, 100);
    if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
    {
        sprintf_s(buff, "WSAStartup failed with error %d\n", Ret);
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        WSACleanup();
        _endthread();
    }


    if ((ListenSocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
        WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        sprintf_s(buff, "Failed to get a UDP socket %d\n", WSAGetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        _endthread();
    }


    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(PORT);

    if (bind(ListenSocket, (PSOCKADDR)&InternetAddr,
        sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        sprintf_s(buff, "bind() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    if (listen(ListenSocket, 5))
    {
        sprintf_s(buff, "listen() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        sprintf_s(buff, "WSACreateEvent() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    // Create a worker thread to service completed I/O requests. 

    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThreadUDP, (LPVOID)AcceptEvent, 0, &ThreadId)) == NULL)
    {
        sprintf_s(buff, "CreateThread failed with error %d\n", GetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    //create path of the save file to be logged
    if (!loadSaveFile((LPSTR)("Transmitting:\r\n"), networkStructUDP)) {
        MessageBox(networkStructUDP->hwnd, "Please load file or create new file to be logged.", TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }


    while (networkStructUDP->connected)
    {
        AcceptSocketUDP = accept(ListenSocket, NULL, NULL);

        if (WSASetEvent(AcceptEvent) == FALSE)
        {
            printf("WSASetEvent failed with error %d\n", WSAGetLastError());
            return;
        }
    }
}

DWORD WINAPI WorkerThreadUDP(LPVOID lpParameter)
{
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;
    char buff[100];
    string str;
    int n;
    LPSTR messageHeader;

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

        SocketInfo->Socket = AcceptSocketUDP;
        ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;
        networkStructUDP->siServer = SocketInfo;
        memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
            &(SocketInfo->Overlapped), WorkerRoutineUDP) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                //printf("WSARecv() failed with error %d\n", WSAGetLastError());
                sprintf_s(buff, "WSARecv() failed with error %d\n", WSAGetLastError());
                MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
                return FALSE;
            }
        }
        else {
            sprintf_s(buff, "Socket %u connected\n", AcceptSocketUDP);
            MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
            str = SocketInfo->DataBuf.buf;
            n = str.find("~");
            networkStructUDP->numByteRead = networkStructUDP->numByteRead + RecvBytes;
            char buffer[64];
            memset(buffer, 0, 64);
            sprintf_s(buffer, "\r\Server Recieved Time for first message: %d\r\n", clock());
            messageHeader = buffer;
            writeToFile(messageHeader, networkStructUDP);
            writeToFile((LPSTR)("\r\nReceived Data from:\r\n"), networkStructUDP);
            if (str.find("`") != -1) {
                writeToFile(SocketInfo->DataBuf.buf, networkStructUDP);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStructUDP);
            }
            else {

                str = str.substr(0, n);
                memset(buffer, 0, 64);
                strcpy(buffer, str.c_str());
                messageHeader = buffer;
                writeToFile(messageHeader, networkStructUDP);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStructUDP);
            }

            writeToFile((LPSTR)("\r\nReceiving Bytes:\r\n"), networkStructUDP);
            writeToFile(LPSTR(to_string(networkStructUDP->numByteRead).c_str()), networkStructUDP);

            memset(buffer, 0, 64);
            sprintf_s(buffer, "\r\nEnding Time from server %d\r\n", clock());
            LPSTR messageHeader2 = buffer;
            writeToFile(messageHeader2, networkStructUDP);

            //MessageBox(networkStruct->hwnd, TEXT("NO Error"), TEXT("Server"), MB_OK);
        }



    }

    return TRUE;
}

void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    char buff[100];

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        printf("I/O operation failed with error %d\n", Error);
        sprintf_s(buff, "I/O operation failed with error %d\n", Error);
        MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
    }

    if (BytesTransferred == 0)
    {
        //printf("Closing socket %d\n", SI->Socket);
        sprintf_s(buff, "Closing socket %u\n", SI->Socket);
        MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
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
            &(SI->Overlapped), WorkerRoutineUDP) == SOCKET_ERROR)
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
            &(SI->Overlapped), WorkerRoutineUDP) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else {
            //empty
        }
    }
}

int loadSaveFile(LPSTR data) {
    //open save dialog box only if the file is not loaded

    OPENFILENAME ofn;
    char file_name[100];
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = networkStructUDP->hwnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = "*.txt\0";
    ofn.nFilterIndex = 1;

    GetSaveFileName(&ofn);
    strncpy(networkStructUDP->filePath, ofn.lpstrFile, 100);
    if (strcmp(networkStructUDP->filePath, "") == 0) {
        return 0;
    }
    //string filepath = convert(uploadData->filePath); // convert LPCST to std::string
    HANDLE hFile = CreateFile(TEXT(networkStructUDP->filePath),      // name of the write
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
