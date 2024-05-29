#include "serverSocket.h"

serverSocket::serverSocket(const char* portStr) {
    port = portStr;
    initializeWinsock();
    createSocket();
    setupListeningSocket();
    acceptClient();
};

void serverSocket::cleanup() {
    closesocket(ClientSocket);
    WSACleanup();
};

int serverSocket::initializeWinsock() {
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return -1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << endl;
        WSACleanup();
        return -1;
    }
    return 0;
}

int serverSocket::createSocket() {
    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }
    return 0;
}

int serverSocket::setupListeningSocket() {
    // Setup the TCP listening socket
    int iResult;
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        cleanup();
        return -1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << endl;
        cleanup();
        return -1;
    }
    return 0;
}

int serverSocket::acceptClient() {
    // Accept a client socket
    cout << "wait for the client connect" << endl;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed with error: " << WSAGetLastError() << endl;
        cleanup();
        return -1;
    }
    cout << "client connected" << endl;
    // No longer need server socket
    closesocket(ListenSocket);
    return 0;
}

int serverSocket::shutdownConnection() {
    // shutdown the connection since we're done
    int iResult;
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed with error: " << WSAGetLastError() << endl;
        cleanup();
        return 1;
    }
    return 0;
}

int serverSocket::sendData(char* sendbuf, int size) {
    //send sendbuf
    iSendResult = send(ClientSocket, sendbuf, size, 0);
    if (iSendResult == SOCKET_ERROR) {
        cout << "send failed with error: " << WSAGetLastError() << endl;
        cleanup();
        return -1;
    }
    cout << "Bytes sent: " << size << endl;
    return iSendResult;
}