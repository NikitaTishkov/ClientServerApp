#include "../Inc/Client.h"

using std::cout;
using std::endl;

CClient::CClient(int iIDNew = 1, int iBufSizeNew = BUF_SIZE, int iPortNew = PORT)
{
    m_iID = iIDNew;
    m_iBufSize = iBufSizeNew;
    const char *iIP = IP;
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(iPortNew);
    Inet_pton(AF_INET, iIP, &m_siServerAddr.sin_addr);
    m_slSocketSize = sizeof(m_siServerAddr);
    cout << "Client setuped" << endl;
}

CClient::~CClient()
{

}

void CClient::SetID(int iIDNew)
{
    m_iID = iIDNew;
}

void CClient::SetClientFd(int iFdNew)
{
    m_iClientFd = iFdNew;
}

void CClient::SetServerFd(int iFdNew)
{
    m_iServerFd = iFdNew;
}

void CClient::SetBufSize(int iSizeNew)
{
    m_iBufSize = iSizeNew;
}

void CClient::SetSocketAddr( short SinFamilyNew, 
                    unsigned short SinPortNew, 
                    struct in_addr SinAddrNew )
{
    m_siServerAddr.sin_addr = SinAddrNew;
    m_siServerAddr.sin_family = SinFamilyNew;
    m_siServerAddr.sin_port = SinPortNew;
}

void CClient::SetSocketSize(socklen_t slSizeNew)
{
    m_slSocketSize = slSizeNew;
}

    /* Getters */
int CClient::GetID() const
{
    return m_iID;
}
int CClient::GetClientFd() const
{
    return m_iClientFd;
}
int CClient::GetServerFd() const
{
    return m_iServerFd;
}
int CClient::GetBufSize() const
{
    return m_iBufSize;
}
struct sockaddr_in CClient::GetSocketAddr() const 
{
    return m_siServerAddr;
}
socklen_t CClient::GetSocketSize() const
{
    return m_slSocketSize;
}