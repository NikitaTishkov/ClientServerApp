#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

using std::cout;
using std::endl;

int main()
{
    int iClientFd, iServerFd;
    int iPort = 12321;
    bool bIsExit = false;
    int iBufSize = 1024;
    char a_chBuffer[iBufSize];
    const char *chIp = "127.0.0.1";

    struct sockaddr_in siServerAddr;
    socklen_t slSocketSize;

    iClientFd = socket(AF_INET, SOCK_STREAM, 0);

    if(iClientFd < 0)
    {
        cout << "Error creating socket..." << endl;
        exit(1);
    }

    cout << "Client Socket created" << endl;

    siServerAddr.sin_family = AF_INET;
    siServerAddr.sin_port = htons(iPort);
    inet_pton(AF_INET, chIp, &siServerAddr.sin_addr);
    // Connecting socket server
    slSocketSize = sizeof(siServerAddr);
    if (connect(iClientFd, (struct sockaddr*)&siServerAddr, slSocketSize) == -1)
    {
        cout << "Connection to the server failed" << endl;
        //exit(1);
    }
    

    recv(iClientFd, a_chBuffer, iBufSize, 0);
    cout << "Connection confirmed" << endl;

    cout << "Enter # to end the connection" << endl;

    do
    {
        cout << "Client: ";
        do 
        {
            std::cin >> a_chBuffer;
            send(iClientFd, a_chBuffer, iBufSize, 0);
            if (*a_chBuffer == '#')
            {
                send(iClientFd, a_chBuffer, iBufSize, 0);
                *a_chBuffer = '*';
                bIsExit = true;
            }
        } while (*a_chBuffer != 42);

        cout << "Server: ";

        do
        {
            recv(iClientFd, a_chBuffer, iBufSize, 0);
            cout << a_chBuffer << " ";
            if (*a_chBuffer == '#')
            {
                *a_chBuffer = '*';
                bIsExit = true;
            }
        } while (*a_chBuffer != 42);

        cout << endl;
    } while (!bIsExit);

    cout << "Connection terminated..." << endl;
    cout << "Goodbye" << endl;

    close(iClientFd);

    return 0;
}