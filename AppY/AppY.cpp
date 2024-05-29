#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "serverSocket.h"
#include "clientSocket.h"

using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define MATRIX_SIZE 256


class FullImageMatrix {

private:
    int sumMetrix;
    short* shortMat;
    static const int mult = 1000000;

public:
    
    FullImageMatrix(short * mat) {
        shortMat = mat;
        sumMetrix = 0;
        proccesMatrix();
    }

    ~FullImageMatrix() {
        delete[] shortMat;
    }

    void proccesMatrix() {
        //found the sum of matrix
        for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
            sumMetrix += shortMat[i];
        }
    }

    int sendShortMetrix(serverSocket* sock) {
        //send the matrix
        int size = MATRIX_SIZE * MATRIX_SIZE * sizeof(short);
        int iResult = sock->sendData((char*)shortMat, size);

        return iResult;
    }

    int sendSum(serverSocket* sock) {
        //send sum of matrix
        int* sum = new int;
        *sum = sumMetrix;
        int iResult = sock->sendData((char*)sum, sizeof(int));
        return iResult;
    }


};

short* recvMat(clientSocket* sock) {
    int bufSize = MATRIX_SIZE * MATRIX_SIZE * sizeof(short);
    char* recvbufMat = new char[bufSize];
    int iResult = sock->receiveData(recvbufMat, bufSize);


    if (iResult <= 0) {
        delete[] recvbufMat;
        return nullptr;
    }
     return (short*)recvbufMat;
}

int __cdecl main(int argc, char** argv)
{
    const char* clientPort = "27015";
    const char* serverPort = "27016";

    if (argc == 3) {
        serverPort = argv[1];
        clientPort = argv[2];
    }
    
    clientSocket* cliSock = new clientSocket(clientPort);
    serverSocket* servSock = new serverSocket(serverPort);
    // Receive until the peer closes the connection
    short* mat;
    while (true) {

        mat = recvMat(cliSock);

        if (!mat)
            break;

        FullImageMatrix* matrix = new FullImageMatrix(mat);

        matrix->sendShortMetrix(servSock);
        matrix->sendSum(servSock);

        delete matrix;

    };

    cliSock->shutdownConnection();

    return 0;
}