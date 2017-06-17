#include "../conn_info/server_conn_info.h"
#include "INetwork.h"
#include "server_network_select.h"
#include "IFileLog.h"
#include <thread>

CServerNetwork::pfnStateFunc CServerNetwork::m_pfnConnStateFunc[SERVER_CONN_STATE_MAX] =
{
	&CServerNetwork::OnConnIdle,
	&CServerNetwork::OnConnConnect,
	&CServerNetwork::OnConnWaitLogicExit,
};

CServerNetwork::CServerNetwork()
{
	m_pfnConnectCallBack	= nullptr;
	m_pFunParam				= nullptr;

	m_pListenLink			= nullptr;

	m_pTcpConnection		= nullptr;
	m_pFreeConn				= nullptr;
	m_uMaxConnCount			= 0;
	m_uFreeConnIndex		= 0;

	m_uSleepTime			= 0;

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_ErrorSet);

	m_bRunning				= false;
	m_bExited				= false;
}

CServerNetwork::~CServerNetwork()
{
	if (m_pListenLink->IsSocketConnected())
	{
		m_pListenLink->Disconnect();
	}

	for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
	{
		if (m_pTcpConnection[nIndex].IsSocketConnected())
		{
			m_pTcpConnection[nIndex].Disconnect();
		}
	}

	SAFE_DELETE(m_pListenLink);
	SAFE_DELETE_ARR(m_pTcpConnection);

#if defined(WIN32) || (WIN64)
	WSACleanup();
#endif
}

void CServerNetwork::OnConnIdle(CServerConnInfo &pClientConn)
{
}

void CServerNetwork::OnConnConnect(CServerConnInfo &pClientConn)
{
	if (!pClientConn.IsLogicConnected())
	{
		// 逻辑层已经退出了，这里只需要TcpConnect关闭就OK了
		pClientConn.Disconnect();
		return;
	}

	if (pClientConn.SendData() == -1)
	{
		// 网络层异常，关闭网络相关操作，等待逻辑层退出
		pClientConn.Disconnect();
		return;
	}

	timeval	timeout = {0, 0};

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_ErrorSet);

	FD_SET(pClientConn.GetSock(), &m_ReadSet);
	FD_SET(pClientConn.GetSock(), &m_ErrorSet);

	if (select(1024, &m_ReadSet, nullptr, &m_ErrorSet, &timeout) <= 0)
		return;

	if (FD_ISSET(pClientConn.GetSock(), &m_ErrorSet))
	{
		pClientConn.Disconnect();
		return;
	}

	if (!FD_ISSET(pClientConn.GetSock(), &m_ReadSet))
		return;

	if (pClientConn.RecvData() == -1)
	{
		pClientConn.Disconnect();
		return;
	}
}

void CServerNetwork::OnConnWaitLogicExit(CServerConnInfo &pClientConn)
{
	if (pClientConn.IsLogicConnected())
		return;

	AddAvailableConnection(&pClientConn);

	pClientConn.LogicDisconnect();
}

void CServerNetwork::ProcessAccept()
{
	FD_ZERO(&m_ReadSet);

	FD_SET(m_pListenLink->GetSock(), &m_ReadSet);
	timeval	timeout = {0, 0};

	if (select(1024, &m_ReadSet, nullptr, nullptr, &timeout) <= 0)
		return;

	if (!FD_ISSET(m_pListenLink->GetSock(), &m_ReadSet))
		return;

	sockaddr_in	client_addr;
	socklen_t	length		= sizeof(client_addr);
	SOCKET		nNewSocket	= INVALID_SOCKET;
	while (INVALID_SOCKET != (nNewSocket = accept(m_pListenLink->GetSock(), (sockaddr*)&client_addr, &length)))
	{
		AcceptClient(nNewSocket);
	}
}

int CServerNetwork::SetNoBlocking(const SOCKET nSock)
{
#if defined(WIN32) || defined(WIN64)
	unsigned long ulNonBlock = 1;
	return ioctlsocket(nSock, FIONBIO, &ulNonBlock);
#elif defined(__linux)
	int nFlags = fcntl(nSock, F_GETFL, 0);
	if (nFlags < 0 || fcntl(nSock, F_SETFL, nFlags | O_NONBLOCK | O_ASYNC) < 0)
		return -1;

	return 0;
#elif defined(__APPLE__)
#endif
}

void CServerNetwork::AcceptClient(const SOCKET nNewSocket)
{
	if (-1 == SetNoBlocking(nNewSocket))
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("ioctlsocket() failed with error %d\n", WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("ERROR on rtsig the sock; errno=%d\n", errno);
#elif defined(__APPLE__)
#endif

		return;
	}

	CServerConnInfo	*pNewLink = GetNewConnection();
	if (nullptr == pNewLink)
	{
		closesocket(nNewSocket);
		return;
	}

	pNewLink->ReInit(nNewSocket);

	pNewLink->Connected();

	m_pfnConnectCallBack(m_pFunParam, pNewLink, pNewLink->GetConnID());
}

void CServerNetwork::ThreadFunc()
{
	while (m_bRunning)
	{
		ProcessAccept();

		for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
		{
			(this->*m_pfnConnStateFunc[m_pTcpConnection[nIndex].GetState()])(m_pTcpConnection[nIndex]);
		}

		yield();
	}

	// 关闭accept
	m_pListenLink->Disconnect();

	// 关闭已连接上的Connection
	for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
	{
		if (m_pTcpConnection[nIndex].IsSocketConnected())
		{
			m_pTcpConnection[nIndex].Disconnect();
		}
	}

	m_bExited	= true;
}

bool CServerNetwork::Initialize(
	const unsigned short usPort,
	void *lpParam,
	pfnConnectEvent pfnConnectCallBack,
	const unsigned int uConnectionNum,
	const unsigned int uSendBufferLen,
	const unsigned int uRecvBufferLen,
	const unsigned int uTempSendBufferLen,
	const unsigned int uTempRecvBufferLen,
	const unsigned int uSleepTime
	)
{
#if defined(WIN32) || defined(WIN64)
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		return false;
	}
#endif

	if (0 == m_uMaxConnCount)
		return false;

	if (0 == uRecvBufferLen)
		return false;

	if (0 == uSendBufferLen)
		return false;

	if (nullptr == lpParam)
		return false;

	if (nullptr == pfnConnectCallBack)
		return false;

	if (nullptr == lpParam)
		return false;

	m_uMaxConnCount			= uConnectionNum;
	m_pfnConnectCallBack	= pfnConnectCallBack;
	m_pFunParam				= lpParam;

	m_pListenLink = new CServerConnInfo;
	if (nullptr == m_pListenLink)
		return false;

	m_pTcpConnection = new CServerConnInfo[m_uMaxConnCount];
	if (nullptr == m_pTcpConnection)
		return false;

	m_pFreeConn = new CServerConnInfo*[m_uMaxConnCount];
	if (!m_pFreeConn)
		return false;

	for (unsigned int uIndex = 0; uIndex < m_uMaxConnCount; ++uIndex)
	{
		if (!m_pTcpConnection[uIndex].Initialize(uIndex, uRecvBufferLen, uSendBufferLen, uTempSendBufferLen, uTempRecvBufferLen))
			return false;

		m_pFreeConn[uIndex]	= &m_pTcpConnection[uIndex];
	}

	struct sockaddr_in tagAddr;
	memset(&tagAddr, 0, sizeof(tagAddr));
	tagAddr.sin_family		= AF_INET;
	tagAddr.sin_port		= htons(usPort);
	tagAddr.sin_addr.s_addr	= INADDR_ANY;

	// start listen
	SOCKET	nSock	= socket(AF_INET, SOCK_STREAM, 0);
	if (nSock < 0)
	{
		g_pFileLog->WriteLog("create listen socket failed!\n");
		return false;
	}

	m_pListenLink->SetSock(nSock);

	int nOnFlag = 1;
#if defined(WIN32) || defined(WIN64)
	setsockopt(m_pListenLink->GetSock(), SOL_SOCKET, SO_REUSEADDR, (const char*)&nOnFlag, sizeof(nOnFlag));
#elif defined(__linux)
	setsockopt(m_pListenLink->GetSock(), SOL_SOCKET, SO_REUSEADDR, (const void*)&nOnFlag, sizeof(nOnFlag));
#elif defined(__APPLE__)
#endif

	if (::bind(m_pListenLink->GetSock(), (const sockaddr *)&tagAddr, sizeof(tagAddr)))
	{
		g_pFileLog->WriteLog("bind listen socket failed!\n");
		return false;
	}

	if (listen(m_pListenLink->GetSock(), 4096))
	{
		g_pFileLog->WriteLog("listen socket failed!\n");
		return false;
	}

	if (-1 == SetNoBlocking(nSock))
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("Set listen socket async failed! errno=%d\n", WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("Set listen socket async failed! errno=%d\n", errno);
#elif defined(__APPLE__)
#endif
		return false;
	}

	m_uSleepTime	= uSleepTime;
	m_bExited		= false;
	m_bRunning		= true;

	std::thread	threadNetwork(&CServerNetwork::ThreadFunc, this);
	threadNetwork.detach();

	return true;
}

void CServerNetwork::Release()
{
	delete this;
}

IServerNetwork *CreateServerNetwork(
	unsigned short usPort,
	void *lpParam,
	pfnConnectEvent pfnConnectCallBack,
	unsigned int uConnectionNum,
	unsigned int uSendBufferLen,
	unsigned int uRecvBufferLen,
	unsigned int uTempSendBufferLen,
	unsigned int uTempRecvBufferLen,
	const unsigned int uSleepTime
)
{
	CServerNetwork	*pServer = new CServerNetwork();
	if (nullptr == pServer)
		return nullptr;

	if (!pServer->Initialize(usPort, lpParam, pfnConnectCallBack, uConnectionNum, uSendBufferLen, uRecvBufferLen, uTempSendBufferLen, uTempRecvBufferLen, uSleepTime))
	{
		pServer->Release();
		return nullptr;
	}

	return pServer;
}
