#include "tcp_server.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: tcp_server.cpp -	An application that uses basic Winsock 2 API database
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

SOCKET AcceptSocket;
NETWORK* networkStruct;

void serverMainTCP(PVOID network)
{
    networkStruct = (NETWORK*)network;
    WSADATA wsaData;
    SOCKET ListenSocket;
    SOCKADDR_IN InternetAddr;
    INT Ret;
    HANDLE ThreadHandle;
    DWORD ThreadId;
    WSAEVENT AcceptEvent;
    char buff[100];
    int port = atoi(networkStruct->port);
    int           ret;
    BOOL          bOpt;
    memset(buff, 0, 100);
    if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
    {
        sprintf_s(buff, "WSAStartup failed with error %d\n", Ret);
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        WSACleanup();
        _endthread();
    }

    if ((ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0,
        WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
    {
        sprintf_s(buff, "Failed to get a TCP socket %d\n", WSAGetLastError());
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        _endthread();
    }

    InternetAddr.sin_family = AF_INET;
    InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InternetAddr.sin_port = htons(port);
    bOpt = TRUE;
    ret = setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt,
        sizeof(bOpt));
    if (bind(ListenSocket, (PSOCKADDR)&InternetAddr,
        sizeof(InternetAddr)) == SOCKET_ERROR)
    {
        sprintf_s(buff, "bind() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    if (listen(ListenSocket, 5))
    {
        sprintf_s(buff, "listen() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    if ((AcceptEvent = WSACreateEvent()) == WSA_INVALID_EVENT)
    {
        sprintf_s(buff, "WSACreateEvent() failed with error %d\n", WSAGetLastError());
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    networkStruct->sdServer = ListenSocket;

    // Create a worker thread to service completed I/O requests. 

    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThread, (LPVOID)AcceptEvent, 0, &ThreadId)) == NULL)
    {
        sprintf_s(buff, "CreateThread failed with error %d\n", GetLastError());
        MessageBox(networkStruct->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }

    //create path of the save file to be logged
    if (!loadSaveFile((LPSTR)("Transmitting:\r\n"), networkStruct)) {
        MessageBox(networkStruct->hwnd, "Please load file or create new file to be logged.", TEXT(""), MB_OK);
        PostMessage(networkStruct->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }


    while (networkStruct->connected)
    {
        AcceptSocket = accept(ListenSocket, NULL, NULL);

        if (WSASetEvent(AcceptEvent) == FALSE)
        {
            printf("WSASetEvent failed with error %d\n", WSAGetLastError());
            return;
        }
    }
    closesocket(AcceptSocket);
    closesocket(ListenSocket);
    WSACleanup();
    _endthread();
}

DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
    DWORD Flags;
    LPSOCKET_INFORMATION SocketInfo = NULL;
    WSAEVENT EventArray[1];
    DWORD Index;
    DWORD RecvBytes;
    char buffer[64];
    string str;
    string beginTimeFromClient;
    int n;
    LPSTR messageHeader;

    // Save the accept event in the event array.

    EventArray[0] = (WSAEVENT)lpParameter;

    while (networkStruct->connected)
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
        //memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);

        Flags = 0;
        if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
            &(SocketInfo->Overlapped), WorkerRoutine) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                //printf("WSARecv() failed with error %d\n", WSAGetLastError());
                sprintf_s(buffer, "WSARecv() failed with error %d\n", WSAGetLastError());
                MessageBox(networkStruct->hwnd, buffer, TEXT("Server"), MB_OK);
                return FALSE;
            }
        }
        else {
            str = SocketInfo->DataBuf.buf;
            n = str.find("~");
            networkStruct->numByteRead = networkStruct->numByteRead + RecvBytes;

            memset(buffer, 0, 64);
            sprintf_s(buffer, "\r\Begining Time From Client:\r\n");
            writeToFile(buffer, networkStruct);

            //process the header sent from client
            if (str.find("`") != -1) {
                str = SocketInfo->DataBuf.buf;
                int indexStr = n - 1;
                beginTimeFromClient = str.substr(1, indexStr);
                str = str.substr(1, SocketInfo->DataBuf.len-1);
                writeToFile(SocketInfo->DataBuf.buf, networkStruct);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStruct);

            }
            else {
                beginTimeFromClient = str.substr(0, n);
                str = str.substr(0, n);
                memset(buffer, 0, 64);
                strcpy(buffer, str.c_str());
                messageHeader = buffer;
                writeToFile(messageHeader, networkStruct);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStruct);
            }
            networkStruct->startTime = stol(beginTimeFromClient);

        }

        sprintf_s(buffer, "Socket %d connected\n", (int)AcceptSocket);
        MessageBox(networkStruct->hwnd, buffer, TEXT(""), MB_OK);

       
    }
    if (SocketInfo != NULL) {
        GlobalFree(SocketInfo);
    }
    return TRUE;
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
void CALLBACK WorkerRoutine(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{   
    SYSTEMTIME stStartTime;
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    char buff[100];

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATION SI = (LPSOCKET_INFORMATION)Overlapped;

    if (Error != 0)
    {
        printf("I/O operation failed with error %d\n", Error);
        sprintf_s(buff, "I/O operation failed with error %d\n", Error);
        MessageBox(networkStruct->hwnd, buff, TEXT("Server"), MB_OK);
    }

    if (BytesTransferred == 0)
    {
        //printf("Closing socket %d\n", SI->Socket);
        int num = SI->Socket;
        sprintf_s(buff, "Closing socket %d\n", num);
        MessageBox(networkStruct->hwnd, buff, TEXT("Server"), MB_OK);
    }

    if (Error != 0 || BytesTransferred == 0)
    {
        closesocket(SI->Socket);
        //GlobalFree(SI);
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
            ++networkStruct->numPackRecv;
            networkStruct->numByteRead = networkStruct->numByteRead + RecvBytes;
            writeToFile((LPSTR)("\r\nReceived Bytes Callback:\r\n"), networkStruct);
            writeToFile(LPSTR(to_string(networkStruct->numByteRead).c_str()), networkStruct);

            GetSystemTime(&stStartTime);
            networkStruct->endTime = getTimeConvertToMil(stStartTime);
            sprintf_s(buff, "\r\nEnding Time from server %dms\r\n", networkStruct->endTime);
            LPSTR messageHeader2 = buff;
            writeToFile(messageHeader2, networkStruct);
        }
    }
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

void disconnectSocketServerTCP(SOCKET si) {
    closesocket(si);
    WSACleanup();
}
