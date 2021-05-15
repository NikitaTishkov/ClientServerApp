#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "ErrWrap.h"

#define IP "127.0.0.1"
#define BUF_SIZE 1024
#define PORT 12321

class CClient
{
private:
    int m_iID;
    int m_iClientFd;
    int m_iServerFd;
    

    /* Buffer setups */
    int m_iBufSize;
    char *m_c_strBuffer;

    /* Socket data types */
    struct sockaddr_in m_siServerAddr;
    socklen_t m_slSocketSize;
    
public:
    /* Constructors */
    CClient(int iIDNew = 1, 
            int iBufSizeNew = BUF_SIZE, 
            int iPortNew = PORT);

    ~CClient();

    /* Setters */
    void SetID(int iIDNew);
    void SetClientFd(int iFdNew);
    void SetServerFd(int iFdNew);
    void SetBufSize(int iSizeNew);

    void SetSocketAddr( short SinFamilyNew, 
                        unsigned short SinPortNew, 
                        struct in_addr SinZeroNew );

    void SetSocketSize(socklen_t slSizeNew);

    /* Getters */
    int GetID() const;
    int GetClientFd() const;
    int GetServerFd() const;
    int GetBufSize() const;
    struct sockaddr_in GetSocketAddr() const;
    socklen_t GetSocketSize() const;

};

#endif