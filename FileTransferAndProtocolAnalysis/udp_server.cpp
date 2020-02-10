#include "udp_server.h"




NETWORK* networkStructUDP;
int firstPacket = 1;
void serverMainUDP(PVOID network)
{
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);
    LPSOCKET_INFORMATIONUDP SocketInfo;
    networkStructUDP = (NETWORK*)network;
    WSADATA wsaData;
    WSABUF DataBuf;
    SOCKET RecvSocket = INVALID_SOCKET;
    struct sockaddr_in RecvAddr;

    //char RecvBuf[1024];
    //int BufLen = 1024;
    DWORD BytesRecv = 0;
    //char buff[100];
    DWORD Flags = 0;
    LPSTR messageHeader;
    string str;

    char buffer[64];
    int n;

    int err = 0;
    int rc;
    int retval = 0;

    //-----------------------------------------------
    // Initialize Winsock
    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc != 0) {
        /* Could not find a usable Winsock DLL */
        wprintf(L"WSAStartup failed with error: %ld\n", rc);
        return ;
    }

    if ((SocketInfo = (LPSOCKET_INFORMATIONUDP)GlobalAlloc(GPTR,
        sizeof(SOCKET_INFORMATIONUDP))) == NULL)
    {
        printf("GlobalAlloc() failed with error %d\n", GetLastError());
        return ;
    }

    // Make sure the Overlapped struct is zeroed out
    SecureZeroMemory((PVOID)&SocketInfo->Overlapped, sizeof(WSAOVERLAPPED));

    // Create an event handle and setup the overlapped structure.
    SocketInfo->Overlapped.hEvent = WSACreateEvent();
    if (SocketInfo->Overlapped.hEvent == NULL) {
        wprintf(L"WSACreateEvent failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return ;
    }
    //-----------------------------------------------
    // Create a receiver socket to receive datagrams
    RecvSocket = WSASocket(AF_INET,
        SOCK_DGRAM,
        IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (RecvSocket == INVALID_SOCKET) {
        /* Could not open a socket */
        wprintf(L"WSASocket failed with error: %ld\n", WSAGetLastError());
        WSACloseEvent(SocketInfo->Overlapped.hEvent);
        WSACleanup();
        return ;
    }
    //-----------------------------------------------
    // Bind the socket to any address and the specified port.
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(SERVER_PORT);
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    rc = bind(RecvSocket, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
    if (rc != 0) {
        /* Bind to the socket failed */
        wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
        WSACloseEvent(SocketInfo->Overlapped.hEvent);
        closesocket(RecvSocket);
        WSACleanup();
        return ;
    }

    //assigning RecvSocket to struct
    SocketInfo->Socket = RecvSocket;
    networkStructUDP->sdServer = RecvSocket;
    //create path of the save file to be logged
    if (!loadSaveFile((LPSTR)("Transmitting:\r\n"), networkStructUDP)) {
        MessageBox(networkStructUDP->hwnd, "Please load file or create new file to be logged.", TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        return;
    }

    while (networkStructUDP->connected)
    {
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;

        rc = WSARecvFrom(SocketInfo->Socket, 
            &(SocketInfo->DataBuf),
            1,
            &BytesRecv,
            &Flags,
            (SOCKADDR*)&SenderAddr,
            &SenderAddrSize, &SocketInfo->Overlapped, WorkerRoutineUDP);
        if (rc != 0) {
            err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                wprintf(L"WSARecvFrom failed with error: %ld\n", err);
                WSACloseEvent(SocketInfo->Overlapped.hEvent);
                closesocket(RecvSocket);
                WSACleanup();
                return;
            }
            else {
                rc = WSAWaitForMultipleEvents(1, &SocketInfo->Overlapped.hEvent, TRUE, INFINITE, TRUE);
                if (rc == WSA_WAIT_FAILED) {
                    wprintf(L"WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
                    retval = 1;
                }

                rc = WSAGetOverlappedResult(RecvSocket, &SocketInfo->Overlapped, &BytesRecv,
                    FALSE, &Flags);
                if (rc == FALSE) {
                    wprintf(L"WSArecvFrom failed with error: %d\n", WSAGetLastError());
                    retval = 1;
                }
            }

        }
    }
    MessageBox(networkStructUDP->hwnd, TEXT("Finished received. Closing socket.\n"), TEXT("Server"), MB_OK);
	PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
    //---------------------------------------------
    // When the application is finished receiving, close the socket.

    WSACloseEvent(SocketInfo->Overlapped.hEvent);
    closesocket(RecvSocket);
    if (SocketInfo != NULL) {
        GlobalFree(SocketInfo);
    }

    //---------------------------------------------
    // Clean up and quit.
    WSACleanup();
    _endthread();
}


void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);
    SYSTEMTIME stStartTime;
    LPSTR messageHeader;
    string str;
    string beginTimeFromClient;
    int n;
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    char buff[100];

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATIONUDP SI = (LPSOCKET_INFORMATIONUDP)Overlapped;

    if (Error != 0)
    {
        printf("I/O operation failed with error %d\n", Error);
        sprintf_s(buff, "I/O operation failed with error %d\n", Error);
        //MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
    }

    if (BytesTransferred == 0)
    {
        int n = SI->Socket;
        sprintf_s(buff, "Closing socket %u\n", n);
        //MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
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
        if (firstPacket) {
            str = SI->DataBuf.buf;
            n = str.find("~");

            memset(buff, 0, 64);
            sprintf_s(buff, "\r\Begining Time From Client:\r\n");
            writeToFile(buff, networkStructUDP);

            //process the header sent from client
            if (str.find("`") != -1) {
                str = SI->DataBuf.buf;
                int indexStr = n - 1;
                beginTimeFromClient = str.substr(1, indexStr);
                str = str.substr(1, SI->DataBuf.len - 1);
                writeToFile(SI->DataBuf.buf, networkStructUDP);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStructUDP);
            }
            else {
                beginTimeFromClient = str.substr(0, n);
                str = str.substr(0, n);
                memset(buff, 0, 64);
                strcpy(buff, str.c_str());
                messageHeader = buff;
                writeToFile(messageHeader, networkStructUDP);
                writeToFile((LPSTR)("\r\n----------------\r\n"), networkStructUDP);
            }
            networkStructUDP->startTime = stol(beginTimeFromClient);
            firstPacket = 0;
        }

        // Now that there are no more bytes to send post another WSARecv() request.

        Flags = 0;
        ZeroMemory(&(SI->Overlapped), sizeof(WSAOVERLAPPED));

        SI->DataBuf.len = DATA_BUFSIZE;
        SI->DataBuf.buf = SI->Buffer;
        if (WSARecvFrom(SI->Socket,  &(SI->DataBuf),  1, &RecvBytes, &Flags, (SOCKADDR*)&SenderAddr,
            &SenderAddrSize, &(SI->Overlapped), WorkerRoutineUDP))
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                printf("WSARecv() failed with error %d\n", WSAGetLastError());
                return;
            }
        }
        else {
            
            ++networkStructUDP->numPackRecv;
            networkStructUDP->numByteRead = networkStructUDP->numByteRead + RecvBytes;

            writeToFile((LPSTR)("\r\nReceiving Bytes Callback:\r\n"), networkStructUDP);
            writeToFile(LPSTR(to_string(networkStructUDP->numByteRead).c_str()), networkStructUDP);
            
            GetSystemTime(&stStartTime);
            networkStructUDP->endTime = getTimeConvertToMil(stStartTime);
            sprintf_s(buff, "\r\nEnding Time from server %dms\r\n", networkStructUDP->endTime);
            LPSTR messageHeader2 = buff;
            writeToFile(messageHeader2, networkStructUDP);
        }
    }
}

void disconnectSocketServerUDP(SOCKET si) {
    closesocket(si);
    WSACleanup();
}