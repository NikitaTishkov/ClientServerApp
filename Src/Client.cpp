#include "../Inc/Client.h"

using std::cout;
using std::endl;
using std::cin;
using std::cerr;

CClient::CClient()
{
    m_iID = 1;
    m_iBufSize = BUF_SIZE;
    const char *iIP = IP;
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(PORT);
    Inet_pton(AF_INET, iIP, &m_siServerAddr.sin_addr);
    m_slSocketSize = sizeof(m_siServerAddr);
    cout << "Client setuped" << endl;
}

CClient::CClient(int iIDNew)
{
    m_iID = iIDNew;
    m_iBufSize = BUF_SIZE;
    const char *iIP = IP;
    m_siServerAddr.sin_family = AF_INET;
    m_siServerAddr.sin_port = htons(PORT);
    Inet_pton(AF_INET, iIP, &m_siServerAddr.sin_addr);
    m_slSocketSize = sizeof(m_siServerAddr);
    cout << "Client setuped" << endl;
}

CClient::CClient(int iIDNew, int iBufSizeNew, int iPortNew)
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
    cout << "Client #" << m_iID << "shuts downs!" << endl;
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
struct sockaddr_in CClient::GetServerAddr() const 
{
    return m_siServerAddr;
}

struct sockaddr* CClient::GetServerPtrAddr() const
{
    return (struct sockaddr*)&m_siServerAddr;
}

socklen_t CClient::GetSocketSize() const
{
    return m_slSocketSize;
}

void CClient::ConnectionInit()
{
    /* Creating socket */
    m_iClientFd = Socket(AF_INET, SOCK_STREAM, 0);
        
    /* Cunnect current socket to the server */
    Connect(m_iClientFd, GetServerPtrAddr(), m_slSocketSize);
    m_c_strBuffer = new char[BUF_SIZE];
    cout << recv(m_iClientFd, m_c_strBuffer, m_iBufSize, 0) << endl; 
    cout << "Connection confirmed and Server responses: " << m_c_strBuffer << endl;
    if(strcmp("Connected", m_c_strBuffer) == 0)
    {
        /* Authorization */
        /* Sending to the server ID of current client */
        send(m_iClientFd, &m_iID, sizeof(int), 0);
    }
}

void CClient::ClientCycle()
{
    while(1)
    {
        cout << "Sending or Receiving..." << endl;
        cout << "S/R: ";
        char chIsSend;
        cin >> chIsSend;
        cout << endl;
        if(chIsSend == 'S' || chIsSend == 's')
        {
            int DestID;
            char chIsWCHAR;
            cout << "Set Destination Client ID" << endl;
            cin >> DestID;
            cout << "Do you want to send WCHAR" << endl;
            cout << "Y/N (Yes/Np): ";
            cin >> chIsWCHAR;
            cout << endl;
            if(chIsWCHAR == 'Y' || chIsWCHAR == 'y')
            {
                const wchar_t* wstrMsg;
                cout << "Set message text: ";
                std::wcin >> (wchar_t*)wstrMsg;
                cout << endl;
                SendWString(DestID, wstrMsg);
            }
            else if(chIsWCHAR == 'N' || chIsWCHAR == 'n')
            {
                const char* cstrMsg;
                cout << "Set message text: ";
                cin >> (char*)cstrMsg;
                cout << endl;
                SendString(DestID, cstrMsg);
            }
            else
            {
                cerr << "Wrong option" << endl;
            }
        }
        else if(chIsSend == 'R' || chIsSend == 'r')
        {
            /*TODO: Communication*/
            RecvMessage();
        }
        else
        {
            cerr << "Wrong option" << endl;
        }
    }
}

void CClient::SendString(int DestID, const char* c_strMessage)
{
    /* Send Source ID */
    send(m_iClientFd, &m_iID, sizeof(int), 0);
    /* Send Destination */
    send(m_iClientFd, &DestID, sizeof(int), 0);
    /* Specify WCHAR or CHAR message */
    bool IsWchar = false;
    send(m_iClientFd, &IsWchar, sizeof(bool), 0);
    /* Send Data */
    send(m_iClientFd, c_strMessage, BUF_SIZE, 0);
}

void CClient::SendWString(int DestID, const wchar_t* wc_strMessage)
{
    /* Send Source ID */
    send(m_iClientFd, &m_iID, sizeof(int), 0);
    /* Send Destination */
    send(m_iClientFd, &DestID, sizeof(int), 0);
    /* Specify WCHAR or CHAR message */
    bool IsWchar = true;
    send(m_iClientFd, &IsWchar, sizeof(bool), 0);
    /* Send Data */
    send(m_iClientFd, wc_strMessage, BUF_SIZE, 0);
}

void CClient::RecvMessage()
{
    int iSrcID;
    int iDestID;
    bool bISWchar;
    const char *c_strBuffer;
    const wchar_t *wc_strBuffer; 
    /* Receiving data */
    /* Recv. Source ID */
    recv(m_iClientFd, &iSrcID, sizeof(int), 0);
    /* Recv. Destination ID */
    recv(m_iClientFd, &iDestID, sizeof(int), 0);
    /* Recv. WCHAR flag */
    recv(m_iClientFd, &bISWchar, sizeof(bool), 0);
    /* Recveive and show text */
    if(bISWchar)
    {
        recv(m_iClientFd, &wc_strBuffer, BUF_SIZE, 0);
        std::wcout << L"#ID" << iSrcID << L" says" << wc_strBuffer << endl;
    }
    else
    {
        recv(m_iClientFd, &c_strBuffer, BUF_SIZE, 0);
        std::cout << "#ID" << iSrcID << " says" << c_strBuffer << endl;
    }
    
}

int main()
{
    CClient my_client;
    cout << "Set ID: ";
    int Client_ID;
    cin >> Client_ID;
    cout << endl;
    my_client.SetID(Client_ID);
    my_client.ConnectionInit();
    my_client.ClientCycle();
    return 0;
}