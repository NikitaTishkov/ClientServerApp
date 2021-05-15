#include "../Inc/Server.h"

using std::cout;
using std::endl;

CServer::CServer(int iBufSizeNew = BUF_SIZE, 
                 int iPortNew = PORT)
{
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(PORT);
    Inet_pton(AF_INET, IP, &m_siServerAddr.sin_addr);
    m_slSocketSize = sizeof(m_siServerAddr);
    m_aConnections = new int*[MAX_SOCKET_NUM];
    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        m_aConnections[i] = new int[2];
    }
}

void CServer::SetServerFd(int iFdNew)
{
    m_iServerFd = iFdNew;
}

void CServer::SetBufSize(int iSizeNew)
{
    m_iBufSize = iSizeNew;
}

void CServer::SetClientsNum(int iNumNew)
{
    m_iClientsNum = iNumNew;
}

void CServer::SetSocketAddr( short SinFamilyNew,
                             unsigned short SinPortNew,
                             struct in_addr SinAddrNew)
{
    m_siServerAddr.sin_addr = SinAddrNew;
    m_siServerAddr.sin_family = SinFamilyNew;
    m_siServerAddr.sin_port = SinPortNew;
}

void CServer::SetSocketSize(socklen_t slSizeNew)
{
    m_slSocketSize = slSizeNew;
}

void CServer::SetHighestSocket(int iNewFd)
{
    m_HighestSocketFd = iNewFd;
}

int CServer::GetServerFd() const
{
    return m_iServerFd;
}

struct sockaddr_in CServer::GetServerAddr() const 
{
    return m_siServerAddr;
}

int CServer::GetBufSize() const
{
    return m_iBufSize;
}
int CServer::GetClientsNum() const    
{
    return m_iClientsNum;
}

struct sockaddr* CServer::GetServerPtrAddr() const
{
    return (struct sockaddr*)&m_siServerAddr;
}

socklen_t CServer::GetSocketSize() const
{
    return m_slSocketSize;
}

int CServer::GetHighestSocket() const
{
    return m_HighestSocketFd;
}

void CServer::ServerInit()
{
    m_iServerFd = Socket(AF_INET, SOCK_STREAM, 0);
    SetHighestSocket(m_iServerFd);
    /* Binding server addr to it`s socket */
    Bind(m_iServerFd, (struct sockaddr*)&m_siServerAddr, m_slSocketSize);
    /* Listening connection requests */
    Listen(m_iServerFd, MAX_SOCKET_NUM);

}

void CServer::HandleNewConnection()
{
    int i;
    bool bIsConnectionSet = false;
    int iConnectionFd = Accept(m_iServerFd, 0, 0);
    for(i = 0; i < MAX_SOCKET_NUM && !bIsConnectionSet; i++)
    {
        if(m_aConnections[i][0] == 0)
        {
            m_aConnections[i][0] = iConnectionFd;
            bIsConnectionSet = true;
        }
    }
    if(!bIsConnectionSet)
    {
        const char *sResponse = "Server is too busy!\n";
        send(iConnectionFd, sResponse, strlen(sResponse), 0);
        close(iConnectionFd);
    }
    else
    {
        /* Client authorization */
        send(iConnectionFd, "Connected", 9, 0);
        struct MsgPack msgResponse;
        recv(iConnectionFd, &msgResponse, sizeof(msgResponse), 0);
        m_aConnections[i][1] = msgResponse.ClientID;
        cout << "Client #ID: " << m_aConnections[i][1] << " authorized" << endl;
        if(iConnectionFd > GetHighestSocket())
        {
            SetHighestSocket(iConnectionFd);
        }
    }
}

void CServer::SetSelectFds()
{
    /* Clear file descriptors array */
    FD_ZERO(&m_ReadableSockets);
    FD_ZERO(&m_WritableSockets);

    /* Add Server file descriptor */
    FD_SET(m_iServerFd, &m_ReadableSockets);
    FD_SET(m_iServerFd, &m_WritableSockets);

    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        if(m_aConnections[i] != 0)
        {
            FD_SET(m_aConnections[i][0], &m_ReadableSockets);
            FD_SET(m_aConnections[i][0], &m_WritableSockets);
        }
    }
}

