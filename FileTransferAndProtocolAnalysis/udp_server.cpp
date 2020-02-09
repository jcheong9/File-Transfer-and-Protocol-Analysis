#include "udp_server.h"

DWORD WINAPI WorkerThreadUDP(LPVOID lpParameter);

void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags);




WSAOVERLAPPED Overlapped;
NETWORK* networkStructUDP;
struct sockaddr_in SenderAddr;

LPSOCKET_INFORMATIONUDP SocketInfo;
int SenderAddrSize = sizeof(SenderAddr);

void serverMainUDP(PVOID network)
{
    networkStructUDP = (NETWORK*)network;
    WSADATA wsaData;
    WSABUF DataBuf;
    HANDLE ThreadHandle;
    DWORD ThreadId;

    SOCKET RecvSocket = INVALID_SOCKET;
    struct sockaddr_in RecvAddr;


    char RecvBuf[1024];
    int BufLen = 1024;
    DWORD BytesRecv = 0;
    char buff[100];
    DWORD Flags = 0;
    LPSTR messageHeader;
    string str;
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

    //create path of the save file to be logged
    if (!loadSaveFile((LPSTR)("Transmitting:\r\n"), networkStructUDP)) {
        MessageBox(networkStructUDP->hwnd, "Please load file or create new file to be logged.", TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }


    // Create a worker thread to service completed I/O requests. 

       /*
    if ((ThreadHandle = CreateThread(NULL, 0, WorkerThreadUDP, (LPVOID)SocketInfo->Overlapped.hEvent, 0, &ThreadId)) == NULL)
    {
        sprintf_s(buff, "CreateThread failed with error %d\n", GetLastError());
        MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
        PostMessage(networkStructUDP->hwnd, WM_FAILED_CONNECT, 0, 0);
        _endthread();
    }
    
 */

    //-----------------------------------------------
    // Call the recvfrom function to receive datagrams
    // on the bound socket.

    DataBuf.len = BufLen;
    DataBuf.buf = RecvBuf;
    wprintf(L"Listening for incoming datagrams on port=%d\n", SERVER_PORT);



    while (networkStructUDP->connected)
    {
        //ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;
        networkStructUDP->siServerUDP = SocketInfo;

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
                else {
                    sprintf_s(buff, "Socket %u connected\n", RecvSocket);
                    MessageBox(networkStructUDP->hwnd, buff, TEXT(""), MB_OK);
                    str = SocketInfo->DataBuf.buf;
                    n = str.find("~");
                    networkStructUDP->numByteRead = networkStructUDP->numByteRead + BytesRecv;
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

                }

               // wprintf(L"Finished receiving. Closing socket.\n");
            }

        }
    }
    MessageBox(networkStructUDP->hwnd, TEXT("Finished receiving. Closing socket.\n"), TEXT("Server"), MB_OK);

    //---------------------------------------------
    // When the application is finished receiving, close the socket.

    WSACloseEvent(SocketInfo->Overlapped.hEvent);
    closesocket(RecvSocket);
    wprintf(L"Exiting.\n");

    //---------------------------------------------
    // Clean up and quit.
    WSACleanup();
    return ;
}


void CALLBACK WorkerRoutineUDP(DWORD Error, DWORD BytesTransferred,
    LPWSAOVERLAPPED Overlapped, DWORD InFlags)
{
    DWORD SendBytes, RecvBytes;
    DWORD Flags;
    char buff[100];

    // Reference the WSAOVERLAPPED structure as a SOCKET_INFORMATION structure
    LPSOCKET_INFORMATIONUDP SI = (LPSOCKET_INFORMATIONUDP)Overlapped;

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
        //SI->BytesRECV = 0;

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
            networkStructUDP->numByteRead = networkStructUDP->numByteRead + RecvBytes;
            writeToFile((LPSTR)("\r\nReceiving Bytes Callback:\r\n"), networkStructUDP);
            writeToFile(LPSTR(to_string(networkStructUDP->numByteRead).c_str()), networkStructUDP);
            //empty
            sprintf_s(buff, "\r\nEnding Time from server %d\r\n", clock());
            LPSTR messageHeader2 = buff;
            writeToFile(messageHeader2, networkStructUDP);
        }
    }
}


DWORD WINAPI WorkerThreadUDP(LPVOID lpParameter)
{
    DWORD Flags = 0;
    WSAEVENT EventArray[1];
    int Index;
    DWORD RecvBytes;
    char buff[100];
    string str;
    int n;
    LPSTR messageHeader;
    char RecvBuf[1025];
    int BufLen = 1025;

    // Save the accept event in the event array.

    while (TRUE)
    {

        // Fill in the details of our accepted socket.

        //ZeroMemory(&(SocketInfo->Overlapped), sizeof(WSAOVERLAPPED));
        SocketInfo->Overlapped.hEvent = (WSAEVENT)lpParameter;
        SocketInfo->BytesSEND = 0;
        SocketInfo->BytesRECV = 0;
        SocketInfo->DataBuf.len = DATA_BUFSIZE;
        SocketInfo->DataBuf.buf = SocketInfo->Buffer;
        networkStructUDP->siServerUDP = SocketInfo;
       // memset(SocketInfo->DataBuf.buf, 0, DATA_BUFSIZE);

        Flags = 0;
        if (WSARecvFrom(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags,
             (SOCKADDR*)&SenderAddr, &SenderAddrSize, &(SocketInfo->Overlapped), NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                //printf("WSARecv() failed with error %d\n", WSAGetLastError());
                sprintf_s(buff, "WSARecv() failed with error %d\n", WSAGetLastError());
                MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
                return FALSE;
            }
            else {
                Index = WSAWaitForMultipleEvents(1, &SocketInfo->Overlapped.hEvent, TRUE, INFINITE, TRUE);
                if (Index == WSA_WAIT_FAILED) {
                    wprintf(L"WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
                    sprintf_s(buff, "WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
                    MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
                }

                Index = WSAGetOverlappedResult(SocketInfo->Socket, &SocketInfo->Overlapped, &SocketInfo->BytesRECV,
                    FALSE, &Flags);
                if (Index == FALSE) {
                    wprintf(L"WSArecvFrom failed with error: %d\n", WSAGetLastError());
                    sprintf_s(buff, "WSArecvFrom failed with error: %d\n", WSAGetLastError());
                    MessageBox(networkStructUDP->hwnd, buff, TEXT("Server"), MB_OK);
                }
                else {
                    // MessageBox(networkStructUDP->hwnd, TEXT("WORKED"), TEXT("Server"), MB_OK);
                }
            }
        }
        else {
            MessageBox(networkStructUDP->hwnd, TEXT("NO Error"), TEXT("Server"), MB_OK);
            /*
            sprintf_s(buff, "Socket %u connected\n", SocketInfo->Socket);
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
            
            */

            //MessageBox(networkStruct->hwnd, TEXT("NO Error"), TEXT("Server"), MB_OK);
        }



    }

    return TRUE;
}