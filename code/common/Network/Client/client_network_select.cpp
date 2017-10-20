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

	m_uSleepTime			= 0;

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_WriteSet);
	FD_ZERO(&m_ErrorSet);

	m_bRunning				= false;
	m_bExited				= false;
}

CClientNetwork::~CClientNetwork()
{
	if (m_pTcpConnection)
	{
		if (m_pTcpConnection->IsConnect())
		{
			m_pTcpConnection->Disconnect();
		}

		SAFE_DELETE(m_pTcpConnection);
	}

#if defined(WIN32) || defined(WIN64)
	WSACleanup();
#endif
}

bool CClientNetwork::Initialize(
	const unsigned int uSendBuffLen,
	const unsigned int uRecvBuffLen,
	const unsigned int uTempSendBuffLen,
	const unsigned int uTempRecvBuffLen,
	pfnConnectEvent pfnConnectCallBack,
	void *lpParm,
	const unsigned int uSleepTime,
	const unsigned char byPackHeadSize
	)
{
	if (nullptr == pfnConnectCallBack)
		return false;

	if (nullptr == lpParm)
		return false;

#if defined(WIN32) || defined(WIN64)
	WSADATA	wsaData;
	WORD	wVersionRequested	= MAKEWORD(2, 2);
	int		nError				= WSAStartup(wVersionRequested, &wsaData);

	if (nError != 0)
		return false;
#endif

	m_pTcpConnection	= new CClientConnInfo;
	if (nullptr == m_pTcpConnection)
		return false;

	if (!m_pTcpConnection->Initialize(0, uRecvBuffLen, uSendBuffLen, uTempRecvBuffLen, uTempSendBuffLen, byPackHeadSize))
		return false;

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

bool CClientNetwork::ConnectTo(char *pstrAddr, const unsigned short usPort)
{
	if (!m_pTcpConnection->IsIdle())
		return false;

	m_pTcpConnection->ConnectTo(pstrAddr, usPort);

	return true;
}

bool CClientNetwork::ConnectToUrl(char *pstrAddr, const unsigned short usPort)
{
	if (m_pTcpConnection->IsLogicConnected() || m_pTcpConnection->IsSocketConnected())
	{
		// 这个连接已经是Connect状态，不再处理建立连接的消息
		return false;
	}

	hostent	*pHost = gethostbyname(pstrAddr);
	if (nullptr == pHost)
		return false;

	// 后面还要将pHost转成IP地址
	// ...
	m_pTcpConnection->ConnectTo(pstrAddr, usPort);

	return true;
}

void CClientNetwork::ShutDown()
{
	m_pTcpConnection->ShutDown();
}

void CClientNetwork::OnClientIdle()
{
	// Do nothing.
}

void CClientNetwork::OnClientTryConnect()
{
	int	nNewSock = socket(AF_INET, SOCK_STREAM, 0);
	if (nNewSock < 0)
	{
		g_pFileLog->WriteLog("Create Socket[%d] Failed\n", nNewSock);

		m_pTcpConnection->Disconnect();

		return;
	}

	if (-1 == SetNoBlocking(nNewSock))
	{
		closesocket(nNewSock);

		m_pTcpConnection->Disconnect();

		return;
	}

	sockaddr_in	tagAddrIn;

	memset(&tagAddrIn, 0, sizeof(tagAddrIn));
	tagAddrIn.sin_family = AF_INET;
	tagAddrIn.sin_port = htons(m_pTcpConnection->GetConnectToPort());
	tagAddrIn.sin_addr.s_addr = inet_addr(m_pTcpConnection->GetConnectToIP());

	int nRet = connect(nNewSock, (sockaddr*)&tagAddrIn, sizeof(tagAddrIn));
	if (0 == nRet)
	{
		m_pTcpConnection->ReInit(nNewSock);

		m_pTcpConnection->Connected();

		m_pfnConnectCallBack(m_pFunParam, m_pTcpConnection, m_pTcpConnection->GetConnID());

		return;
	}

#if defined(WIN32) || defined(WIN64)
	if (WSAGetLastError() == WSAEWOULDBLOCK)
#elif defined(__linux)
	if (EINPROGRESS == errno)
#elif defined(__APPLE__)
#endif
	{
		m_pTcpConnection->ReInit(nNewSock);

		m_pTcpConnection->WaitConnectOK();

		m_pfnConnectCallBack(m_pFunParam, m_pTcpConnection, m_pTcpConnection->GetConnID());

		return;
	}

	closesocket(nNewSock);
}

void CClientNetwork::OnClientWaitConnect()
{
	timeval	timeout = {0, 0};

	FD_ZERO(&m_WriteSet);

	FD_SET(m_pTcpConnection->GetSock(), &m_WriteSet);

	if (select(1024, nullptr, &m_WriteSet, nullptr, &timeout) <= 0)
		return;

	if (!FD_ISSET(m_pTcpConnection->GetSock(), &m_WriteSet))
		return;

	int			nError	= 0;
	socklen_t	nLen	= sizeof(nError);
#if defined(WIN32) || defined(WIN64)
	getsockopt(m_pTcpConnection->GetSock(), SOL_SOCKET, SO_ERROR, (char*)&nError, &nLen);
#elif defined(__linux)
	getsockopt(m_pTcpConnection->GetSock(), SOL_SOCKET, SO_ERROR, &nError, &nLen);
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
		m_pTcpConnection->Disconnect();

		m_pfnConnectCallBack(m_pFunParam, nullptr, m_pTcpConnection->GetConnID());

		return;
	}

	m_pTcpConnection->Connected();

	m_pfnConnectCallBack(m_pFunParam, m_pTcpConnection, m_pTcpConnection->GetConnID());
}

void CClientNetwork::OnClientConnect()
{
	timeval	timeout = { 0, 0 };

	FD_ZERO(&m_ReadSet);
	FD_ZERO(&m_WriteSet);
	FD_ZERO(&m_ErrorSet);

	FD_SET(m_pTcpConnection->GetSock(), &m_ReadSet);
	FD_SET(m_pTcpConnection->GetSock(), &m_WriteSet);
	FD_SET(m_pTcpConnection->GetSock(), &m_ErrorSet);

	if (select(1024, &m_ReadSet, &m_WriteSet, &m_ErrorSet, &timeout) <= 0)
		return;

	if (FD_ISSET(m_pTcpConnection->GetSock(), &m_ErrorSet))
	{
		m_pTcpConnection->Disconnect();

		return;
	}

	if (FD_ISSET(m_pTcpConnection->GetSock(), &m_ReadSet))
	{
		if (m_pTcpConnection->RecvData() == -1)
		{
			m_pTcpConnection->Disconnect();

			return;
		}
	}

	if (FD_ISSET(m_pTcpConnection->GetSock(), &m_WriteSet))
	{
		if (!m_pTcpConnection->SendData())
		{
			m_pTcpConnection->Disconnect();

			return;
		}
	}
}

void CClientNetwork::OnClientWaitLogicExit()
{
	if (m_pTcpConnection->IsLogicConnected())
		return;

	m_pTcpConnection->LogicDisconnect();
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
		(this->*m_pfnClientStateFunc[m_pTcpConnection->GetState()])();

		yield();
	}

	// 关闭已连接上的Connection
	if (m_pTcpConnection->IsSocketConnected())
	{
		m_pTcpConnection->Disconnect();
	}

	m_bExited	= true;
}

IClientNetwork *CreateClientNetwork(
	unsigned int uMaxSendBuff,
	unsigned int uMaxReceiveBuff,
	unsigned int uMaxTempSendBuff,
	unsigned int uMaxTempReceiveBuff,
	pfnConnectEvent pfnConnectCallBack,
	void *lpParm,
	const unsigned int uSleepTime,
	const unsigned char byPackHeadSize
	)
{
	CClientNetwork	*pClient = new CClientNetwork();
	if (nullptr == pClient)
		return nullptr;

	if (!pClient->Initialize(uMaxSendBuff, uMaxReceiveBuff, uMaxTempSendBuff, uMaxTempReceiveBuff, pfnConnectCallBack, lpParm, uSleepTime, byPackHeadSize))
	{
		pClient->Release();
		return nullptr;
	}

	return pClient;
}
