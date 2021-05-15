#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#include "ErrWrap.h"

#define IP "127.0.0.1"
#define BUF_SIZE 1024
#define PORT 12321
#define MAX_SOCKET_NUM 3

struct MsgPack 
{
    int DestinationID;
    int ClientID;
    bool IsWchar;
    const void* Data;
};

class CServer
{
private:
    /* Server File descriptor */
    int m_iServerFd;
    /* Sockets array for knowing to who we communicate */
    int **m_aConnections;
    /* Number of connected clients < MAX_SOCKET_NUM */
    int m_iClientsNum;
    /* Buffer setups */
    int m_iBufSize;
    char *m_c_strBuffer;

    /* Socket data types */
    struct sockaddr_in m_siServerAddr;
    socklen_t m_slSocketSize;

    /* Array of readable descriptors */
    fd_set m_ReadableSockets;
    /* Array of writable descriptors */
    fd_set m_WritableSockets;
    /* Highest Socket file descriptor */
    int m_HighestSocketFd;
    
public:
    
    /** Constructor
     * 
     * Set ups client prefernces before creating socket and connecting 
     * to the server
     * 
     */ 
    CServer( int iBufSizeNew = BUF_SIZE, 
             int iPortNew = PORT);

    ~CServer();

    /* Setters */
    void SetServerFd(int iFdNew);
    void SetBufSize(int iSizeNew);
    void SetClientsNum(int iNumNew);
    void SetSocketAddr( short SinFamilyNew, 
                        unsigned short SinPortNew, 
                        struct in_addr SinZeroNew );

    void SetSocketSize(socklen_t slSizeNew);
    void SetHighestSocket(int iNewFd);

    /* Getters */
    int GetServerFd() const;
    int GetBufSize() const;
    int GetClientsNum() const;
    struct sockaddr_in GetServerAddr() const;
    struct sockaddr* GetServerPtrAddr() const; 
    socklen_t GetSocketSize() const;
    int GetHighestSocket() const;

    /* Server unctionalities */

    void ServerInit();
    void HandleNewConnection();
    void SetSelectFds();

};

#endif