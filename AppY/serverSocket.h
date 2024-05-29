#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

class serverSocket
{

private:
    WSADATA wsaData;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    const char* port;

public:

    serverSocket(const char* portStr);

    void cleanup();

    int initializeWinsock();

    int createSocket();

    int setupListeningSocket();

    int acceptClient();

    int shutdownConnection();

    int sendData(char* sendbuf, int size);

};
