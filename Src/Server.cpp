#include "../Inc/Server.h"

using std::cout;
using std::endl;
using std::cerr;

CServer::CServer()
{
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(PORT);
    Inet_pton(AF_INET, IP, &m_siServerAddr.sin_addr);
    m_iBufSize = BUF_SIZE;
    m_slSocketSize = sizeof(m_siServerAddr);
    m_aConnections = new int*[MAX_SOCKET_NUM];
    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        m_aConnections[i] = new int[2];
    }
}

CServer::CServer(int iBufSizeNew, int iPortNew)
{
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(iPortNew);
    Inet_pton(AF_INET, IP, &m_siServerAddr.sin_addr);
    m_iBufSize = iBufSizeNew;
    m_slSocketSize = sizeof(m_siServerAddr);
    m_aConnections = new int*[MAX_SOCKET_NUM];
    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        m_aConnections[i] = new int[2];
    }
}

CServer::~CServer()
{

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

void CServer::SetNonBlocking(int iSockFd)
{
	int opts;

	opts = fcntl(iSockFd,F_GETFL);
	if (opts < 0) {
		cerr << "fcntl(F_GETFL)" << endl;;
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if (fcntl(iSockFd,F_SETFL,opts) < 0) {
		cerr << "fcntl(F_SETFL)" << endl;
		exit(EXIT_FAILURE);
	}
	return;
}

void CServer::SendString(int iDestFd, int iSrcID, int iDestID, const char* c_strMessage)
{
    /* Send Source ID */
    send(iDestFd, &iSrcID, sizeof(int), 0);
    /* Send Destination */
    send(iDestFd, &iDestID, sizeof(int), 0);
    /* Specify WCHAR or CHAR message */
    bool IsWchar = false;
    send(iDestFd, &IsWchar, sizeof(bool), 0);
    /* Send Data */
    send(iDestFd, c_strMessage, BUF_SIZE, 0);
}

void CServer::SendWString(int iDestFd, int iSrcID, int iDestID, const wchar_t* wc_strMessage)
{
    /* Send Source ID */
    send(iDestID, &iSrcID, sizeof(int), 0);
    /* Send Destination */
    send(iDestID, &iDestID, sizeof(int), 0);
    /* Specify WCHAR or CHAR message */
    bool IsWchar = true;
    send(iDestID, &IsWchar, sizeof(bool), 0);
    /* Send Data */
    send(iDestID, wc_strMessage, BUF_SIZE, 0);
}

void CServer::HandleNewConnection()
{
    int i;
    bool bIsConnectionSet = false;
    int iConnectionFd;
    while(iConnectionFd < 0)
    {
        iConnectionFd = accept(m_iServerFd, 0, 0);
    }
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
        send(iConnectionFd, sResponse, BUF_SIZE, 0);
        close(iConnectionFd);
    }
    else
    {
        /* Client authorization */
        char *c_strConnResponse = new char[BUF_SIZE];
        c_strConnResponse = "Connected";
        send(iConnectionFd, c_strConnResponse, BUF_SIZE, 0);
        int iClientAuthID;
        recv(iConnectionFd, &iClientAuthID, sizeof(int), 0);
        cout << "ID: " << iClientAuthID << endl;
        if(iClientAuthID == 0)
        {
            cerr << "Authorization error" << endl;
            exit(EXIT_FAILURE);
        }
        m_aConnections[i][1] = iClientAuthID;
        cout << "Client #ID: " << m_aConnections[i][1] << " authorized" << endl;
        if(iConnectionFd > GetHighestSocket())
        {
            SetHighestSocket(iConnectionFd);
        }
        SetNonBlocking(m_iServerFd);
        SetNonBlocking(iConnectionFd);
    }
}

void CServer::SetSelectFds()
{
    /* Clear file descriptors array */
    //cout << "Try to zero" << endl;
    FD_ZERO(&m_ReadableSockets);
    FD_ZERO(&m_WritableSockets);
    //cout << "Zero Fdset" << endl;
    /* Add Server file descriptor */
    FD_SET(m_iServerFd, &m_ReadableSockets);
    FD_SET(m_iServerFd, &m_WritableSockets);
    //cout << "Added server fd to list" << endl;
    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        if(m_aConnections[i] != 0)
        {
            FD_SET(m_aConnections[i][0], &m_ReadableSockets);
            FD_SET(m_aConnections[i][0], &m_WritableSockets);
        }
    }
}

void CServer::ReadFromClients()
{
    /* If Server fd is ready - handling new connaction */
    if(FD_ISSET(m_iServerFd, &m_ReadableSockets))
    {
        //HandleNewConnection();
    }

    for(int i = 0; i < MAX_SOCKET_NUM; i++)
    {
        if(FD_ISSET(m_aConnections[i][0], &m_ReadableSockets))
        {
            MessageTranfer(i);
        }
    }
}

void CServer::MessageTranfer(int iSourceSocketIndex)
{
    int iDestID;
    int iSrcID;
    bool bIsWchar;
    const char *c_strBuffer;
    const wchar_t *wc_strBuffer;
    cout << "reading from Client #ID:" << m_aConnections[iSourceSocketIndex][1] << endl;
    /* Receiving Data */
    /* Recv. Source ID */
    recv(m_aConnections[iSourceSocketIndex][0], &iSrcID, sizeof(int), 0);
    /* Recv. Destination ID */
    recv(m_aConnections[iSourceSocketIndex][0], &iDestID, sizeof(int), 0);
    /* Recv. Is WCHAR flag */
    recv(m_aConnections[iSourceSocketIndex][0], &bIsWchar, sizeof(bool), 0);
    /* Recv. text */
    if(bIsWchar)
    {
        recv(m_aConnections[iSourceSocketIndex][0], &c_strBuffer, BUF_SIZE, 0);
    }
    else
    {
        recv(m_aConnections[iSourceSocketIndex][0], &wc_strBuffer, BUF_SIZE, 0);
    }
    bool bIsSended = false;
    for(int i = 0; i < MAX_SOCKET_NUM && !bIsSended; i++)
    {
        if(m_aConnections[i][1] == iDestID)
        {
            if(!FD_ISSET(m_aConnections[i][0], &m_WritableSockets))
            {
                cerr << "Destination is not ready" << endl;
            }
            else
            {
                if(bIsWchar)
                {
                    SendWString(m_aConnections[i][0], iSrcID, iDestID, wc_strBuffer);
                }
                else
                {
                    SendString(m_aConnections[i][0], iSrcID, iDestID, c_strBuffer);
                }
                bIsSended = true;
            }   
        }
    }

    if(!bIsSended)
    {
        /*TODO: Error Response*/
        std::cerr << "There is no client with #ID" << iDestID << endl;   
        const char* c_strFoundErr = "There is no client with such ID\n";
        //send(m_aConnections[iSourceSocketIndex][0], c_strFoundErr, strlen(c_strFoundErr), 0);
    }
}

void CServer::ServerCycle()
{
    int iSelectStat;
    cout << "Server cycle starts..." << endl;
    while (1)
    {
        //cout << "Server in Cycle" << endl;
        SetSelectFds();
        //cout << "SelectFds setted up" << endl;
        iSelectStat = select(GetHighestSocket(),
                             &m_ReadableSockets,
                             &m_WritableSockets,
                             0,
                             0);

        if(iSelectStat < 0)
        {
            cerr << "Error in using select()" << endl;
            exit(EXIT_FAILURE);
        }
        else if(iSelectStat = 0)
        {
            /* All Client are busy */
        }
        else
        {
            ReadFromClients();
        }

    }
    
}

int main()
{
    CServer my_server;
    my_server.ServerInit();
    my_server.HandleNewConnection();
    my_server.HandleNewConnection();
    my_server.ServerCycle();
    return 0;
}