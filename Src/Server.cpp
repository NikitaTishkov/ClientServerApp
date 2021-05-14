#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>

#define MAX_SOCKET_NUM 1

using std::cout;
using std::endl;

int main()
{
    int iClientFd, iServerFd;
    int iPort = 12321;
    bool bIsExit = false;
    int iBufSize = 1024;
    char a_chBuffer[iBufSize];

    struct sockaddr_in siServerAddr;
    socklen_t slSocketSize;

    iClientFd = socket(AF_INET, SOCK_STREAM, 0);

    if (iClientFd < 0)
    {
        cout << "Establishing connection error!" << endl;
        exit(1);
    }

    cout << "Server Socket connection created..." << endl;

    siServerAddr.sin_family = AF_INET;
    siServerAddr.sin_port = htons(iPort);
    inet_pton(AF_INET, "127.0.0.1", &siServerAddr.sin_addr);


    if (bind(iClientFd, (struct sockaddr*)&siServerAddr, sizeof(siServerAddr)) < 0)
    {
        cout << "Error binding socket..." << endl;
        exit(1);
    }

    slSocketSize = sizeof(siServerAddr);
    cout << "Looking for clients..." << endl;

    //listening 

    listen(iClientFd, MAX_SOCKET_NUM);

    //accept client

    iServerFd = accept(iClientFd, (struct sockaddr*)&siServerAddr, &slSocketSize);

    if (iServerFd < 0)
    {
        cout << "Error on accepting client..." << endl;
        exit(1);
    }

    while (iServerFd > 0)
    {
        strcpy(a_chBuffer, "Server connected...\n");
        send(iServerFd, a_chBuffer, iBufSize, 0);

        cout << "Connected with client..." << endl;
        cout << "Enter # to end the connection" << endl;

        cout << "Client:" << endl;
        
        do 
        {
            recv(iServerFd, a_chBuffer, iBufSize, 0);
            cout << "Buffer" << " ";

            if (*a_chBuffer == '#')
            {
                *a_chBuffer = '*';
                bIsExit = true;
            }
        } while (*a_chBuffer != '*');

        do 
        {
            cout << "\nServer: ";
            do
            {
                std::cin >> a_chBuffer;
                send(iServerFd, a_chBuffer, iBufSize, 0);
                if (*a_chBuffer == '#')
                {
                    send(iServerFd, a_chBuffer, iBufSize, 0);
                    *a_chBuffer = '*';
                    bIsExit = true;
                }
            } while (*a_chBuffer != '*');

            cout << "Client: ";
            do
            {
                recv(iServerFd, a_chBuffer, iBufSize, 0);
                cout << a_chBuffer << " ";
                if (*a_chBuffer == '#')
                {
                    *a_chBuffer == '*';
                }
            } while (*a_chBuffer != '*');
        } while (!bIsExit);

        cout << "Connection terminated..." << endl;
        cout << "Goodbye..." << endl;
        bIsExit = false;
        exit(1);
    }

    close(iClientFd);
    return 0;
}