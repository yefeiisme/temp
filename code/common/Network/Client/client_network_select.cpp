#include "INetwork.h"
#include "../NetworkHead.h"
#include "../conn_info/client_conn_info.h"
#include "client_network_select.h"
#include "IFileLog.h"
#include <thread>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

CClientNetwork::pfnStateFunc CClientNetwork::m_pfnClientStateFunc[CLIENT_CONN_STATE_MAX] =
{
	&CClientNetwork::OnClientIdle,
	&CClientNetwork::OnClientTryConnect,
	&CClientNetwork::OnClientWaitConnect,
	&CClientNetwork::OnClientConnect,
	&CClientNetwork::OnClientWaitLogicExit,
};

CClientNetwork::CClientNetwork()
{
	m_pfnConnectCallBack	= nullptr;
	m_pFunParam				= nullptr;

	m_pTcpConnection		= nullptr;

	m_uMaxConnCount			= 0;

	m_uSleepTime			= 0;

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_WriteSet);
	FD_ZERO(&m_ErrorSet);

	m_bRunning				= false;
	m_bExited				= false;
}

CClientNetwork::~CClientNetwork()
{
	for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
	{
		if (m_pTcpConnection[nIndex].IsConnect())
		{
			m_pTcpConnection[nIndex].Disconnect();
		}
	}

	SAFE_DELETE_ARR(m_pTcpConnection);
	m_uMaxConnCount	= 0;

#if defined(WIN32) || defined(WIN64)
	WSACleanup();
#endif
}

bool CClientNetwork::Initialize(
	const unsigned int uClientCount,
	const unsigned int uSendBuffLen,
	const unsigned int uRecvBuffLen,
	const unsigned int uTempSendBuffLen,
	const unsigned int uTempRecvBuffLen,
	pfnConnectEvent pfnConnectCallBack,
	void *lpParm,
	const unsigned int uSleepTime
	)
{
	if (nullptr == pfnConnectCallBack)
		return false;

	if (nullptr == lpParm)
		return false;

	m_uMaxConnCount	= uClientCount;

#if defined(WIN32) || defined(WIN64)
	WSADATA	wsaData;
	WORD	wVersionRequested	= MAKEWORD(2, 2);
	int		nError				= WSAStartup(wVersionRequested, &wsaData);

	if (nError != 0)
		return false;
#endif

	m_pTcpConnection	= new CClientConnInfo[m_uMaxConnCount];
	if (nullptr == m_pTcpConnection)
		return false;

	for (unsigned int uIndex = 0; uIndex < m_uMaxConnCount; ++uIndex)
	{
		if (!m_pTcpConnection[uIndex].Initialize(uIndex, uRecvBuffLen, uSendBuffLen, uTempRecvBuffLen, uTempSendBuffLen))
			return false;
	}

	m_pfnConnectCallBack	= pfnConnectCallBack;
	m_pFunParam				= lpParm;

	m_uSleepTime			= uSleepTime;
	m_bExited				= false;
	m_bRunning				= true;

	std::thread	threadNetwork(&CClientNetwork::ThreadFunc, this);
	threadNetwork.detach();

	return true;
}

void CClientNetwork::Release()
{
	delete this;
}

bool CClientNetwork::ConnectTo(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex)
{
	if (uIndex >= m_uMaxConnCount)
		return false;

	CClientConnInfo	&pClientConn = m_pTcpConnection[uIndex];
	if (!pClientConn.IsIdle())
		return false;

	pClientConn.ConnectTo(pstrAddr, usPort);

	return true;
}

bool CClientNetwork::ConnectToUrl(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex)
{
	if (uIndex >= m_uMaxConnCount)
		return false;

	CClientConnInfo	&pTcpConnection = m_pTcpConnection[uIndex];
	if (pTcpConnection.IsLogicConnected() || pTcpConnection.IsSocketConnected())
	{
		// 这个连接已经是Connect状态，不再处理建立连接的消息
		return false;
	}

	hostent	*pHost = gethostbyname(pstrAddr);
	if (nullptr == pHost)
		return false;

	// 后面还要将pHost转成IP地址
	// ...
	pTcpConnection.ConnectTo(pstrAddr, usPort);

	return true;
}

void CClientNetwork::ShutDown(const unsigned int uIndex)
{
	if (uIndex >= m_uMaxConnCount)
		return;

	m_pTcpConnection[uIndex].ShutDown();
}

void CClientNetwork::OnClientIdle(CClientConnInfo &pClientConn)
{
}

void CClientNetwork::OnClientTryConnect(CClientConnInfo &pClientConn)
{
	int	nNewSock = socket(AF_INET, SOCK_STREAM, 0);
	if (nNewSock < 0)
	{
		g_pFileLog->WriteLog("Create Socket[%d] Failed\n", nNewSock);

		pClientConn.Disconnect();

		return;
	}

	if (-1 == SetNoBlocking(nNewSock))
	{
		closesocket(nNewSock);

		pClientConn.Disconnect();

		return;
	}

	sockaddr_in	tagAddrIn;

	memset(&tagAddrIn, 0, sizeof(tagAddrIn));
	tagAddrIn.sin_family = AF_INET;
	tagAddrIn.sin_port = htons(pClientConn.GetConnectToPort());
	tagAddrIn.sin_addr.s_addr = inet_addr(pClientConn.GetConnectToIP());

	int nRet = connect(nNewSock, (sockaddr*)&tagAddrIn, sizeof(tagAddrIn));
	if (0 == nRet)
	{
		pClientConn.ReInit(nNewSock);

		pClientConn.Connected();

		m_pfnConnectCallBack(m_pFunParam, &pClientConn, pClientConn.GetConnID());

		return;
	}

#if defined(WIN32) || defined(WIN64)
	if (WSAGetLastError() == WSAEWOULDBLOCK)
#elif defined(__linux)
	if (EINPROGRESS == errno)
#elif defined(__APPLE__)
#endif
	{
		pClientConn.ReInit(nNewSock);

		pClientConn.WaitConnectOK();

		m_pfnConnectCallBack(m_pFunParam, &pClientConn, pClientConn.GetConnID());

		return;
	}

	closesocket(nNewSock);
}

void CClientNetwork::OnClientWaitConnect(CClientConnInfo &pClientConn)
{
	timeval	timeout = {0, 0};

	FD_ZERO(&m_WriteSet);

	FD_SET(pClientConn.GetSock(), &m_WriteSet);

	if (select(1024, nullptr, &m_WriteSet, nullptr, &timeout) <= 0)
		return;

	if (!FD_ISSET(pClientConn.GetSock(), &m_WriteSet))
		return;

	int			nError	= 0;
	socklen_t	nLen	= sizeof(nError);
#if defined(WIN32) || defined(WIN64)
	getsockopt(pClientConn.GetSock(), SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen);
#elif defined(__linux)
	getsockopt(pClientConn.GetSock(), SOL_SOCKET, SO_ERROR, &nError, &nLen);
#elif defined(__APPLE__)
#endif

	if (0 != nError)
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("Connnect Failed errno=%d\n", WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("Connnect Failed errno=%d\n", errno);
#elif defined(__APPLE__)
#endif
		pClientConn.Disconnect();

		m_pfnConnectCallBack(m_pFunParam, nullptr, pClientConn.GetConnID());

		return;
	}

	pClientConn.Connected();

	m_pfnConnectCallBack(m_pFunParam, &pClientConn, pClientConn.GetConnID());
}

void CClientNetwork::OnClientConnect(CClientConnInfo &pClientConn)
{
	timeval	timeout = { 0, 0 };

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_WriteSet);
	FD_ZERO(&m_ErrorSet);

	FD_SET(pClientConn.GetSock(), &m_ReadSet);
	FD_SET(pClientConn.GetSock(), &m_WriteSet);
	FD_SET(pClientConn.GetSock(), &m_ErrorSet);

	if (select(1024, &m_ReadSet, &m_WriteSet, &m_ErrorSet, &timeout) <= 0)
		return;

	if (FD_ISSET(pClientConn.GetSock(), &m_ErrorSet))
	{
		pClientConn.Disconnect();

		return;
	}

	if (FD_ISSET(pClientConn.GetSock(), &m_ReadSet))
	{
		if (pClientConn.RecvData() == -1)
		{
			pClientConn.Disconnect();

			return;
		}
	}

	if (FD_ISSET(pClientConn.GetSock(), &m_WriteSet))
	{
		if (!pClientConn.SendData())
		{
			pClientConn.Disconnect();

			return;
		}
	}
}

void CClientNetwork::OnClientWaitLogicExit(CClientConnInfo &pClientConn)
{
	if (pClientConn.IsLogicConnected())
		return;

	pClientConn.LogicDisconnect();
}

int CClientNetwork::SetNoBlocking(const SOCKET nSock)
{
#if defined(WIN32) || defined(WIN64)
	unsigned long ulNonBlock = 1;
	return (ioctlsocket(nSock, FIONBIO, &ulNonBlock) == SOCKET_ERROR);
#elif defined(__linux)
	int nFlags = fcntl(nSock, F_GETFL, 0);
	if (nFlags < 0 || fcntl(nSock, F_SETFL, nFlags | O_NONBLOCK | O_ASYNC) < 0)
		return -1;

	return 0;
#elif defined(__APPLE__)
#endif
}

void CClientNetwork::ThreadFunc()
{
	while (m_bRunning)
	{
		for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
		{
			(this->*m_pfnClientStateFunc[m_pTcpConnection[nIndex].GetState()])(m_pTcpConnection[nIndex]);
		}

		yield();
	}

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

IClientNetwork *CreateClientNetwork(
	unsigned int uLinkCount,
	unsigned int uMaxSendBuff,
	unsigned int uMaxReceiveBuff,
	unsigned int uMaxTempSendBuff,
	unsigned int uMaxTempReceiveBuff,
	pfnConnectEvent pfnConnectCallBack,
	void *lpParm,
	const unsigned int uSleepTime
	)
{
	CClientNetwork	*pClient = new CClientNetwork();
	if (nullptr == pClient)
		return nullptr;

	if (!pClient->Initialize(uLinkCount, uMaxSendBuff, uMaxReceiveBuff, uMaxTempSendBuff, uMaxTempReceiveBuff, pfnConnectCallBack, lpParm, uSleepTime))
	{
		pClient->Release();
		return nullptr;
	}

	return pClient;
}
