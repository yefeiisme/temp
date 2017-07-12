#include <sys/epoll.h>
#include "../conn_info/server_conn_info.h"
#include "INetwork.h"
#include "server_network_epoll.h"
#include "IFileLog.h"
#include <thread>

#define MAX_EP 500
#define MAX_EP_WAIT (MAX_EP/100)

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

	m_nepfd					= 0;

	m_bRunning				= false;
	m_bExited				= false;
}

CServerNetwork::~CServerNetwork()
{
	if (m_pListenLink->IsSocketConnected())
	{
		DisconnectConnection(m_pListenLink);
	}

	for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
	{
		if (m_pTcpConnection[nIndex].IsSocketConnected())
		{
			DisconnectConnection(&m_pTcpConnection[nIndex]);
		}
	}

	SAFE_DELETE(m_pListenLink);
	SAFE_DELETE_ARR(m_pTcpConnection);

	closesocket(m_nepfd);
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

	if (!pClientConn.SendData())
	{
		// 网络层异常，关闭网络相关操作，等待逻辑层退出
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

int CServerNetwork::SetNoBlocking(CServerConnInfo *pTcpConnection)
{
	if (nullptr == pTcpConnection)
		return -1;

	int nFlags;
	if ((nFlags = fcntl(pTcpConnection->GetSock(), F_GETFL, 0)) < 0 || fcntl(pTcpConnection->GetSock(), F_SETFL, nFlags | O_NONBLOCK) < 0)
		return -1;

	epoll_event ev	={ 0 };

	ev.data.ptr	= pTcpConnection;
	ev.events	= EPOLLIN | EPOLLET | EPOLLPRI | EPOLLHUP | EPOLLERR;

	return epoll_ctl(m_nepfd, EPOLL_CTL_ADD, pTcpConnection->GetSock(), &ev);
}

void CServerNetwork::AcceptClient(const SOCKET nNewSocket)
{
	CServerConnInfo	*pNewLink = GetNewConnection();
	if (nullptr == pNewLink)
	{
		closesocket(nNewSocket);
		return;
	}

	pNewLink->ReInit(nNewSocket);

	pNewLink->Connected();

	if (-1 == SetNoBlocking(pNewLink))
	{
		RemoveConnection(pNewLink);

		g_pFileLog->WriteLog("[%s][%d] SetNoBlocking Sock[%d] errno=%d\n", __FILE__, __FILE__, nNewSocket, errno);

		return;
	}

	pNewLink->Connected();

	m_pfnConnectCallBack(m_pFunParam, pNewLink, pNewLink->GetConnID());
}

void CServerNetwork::DisconnectConnection(CServerConnInfo *pTcpConnection)
{
	int ret = epoll_ctl(m_nepfd, EPOLL_CTL_DEL, pTcpConnection->GetSock(), nullptr);
	pTcpConnection->Disconnect();
}

void CServerNetwork::RemoveConnection(CServerConnInfo *pTcpConnection)
{
	DisconnectConnection(pTcpConnection);

	AddAvailableConnection(pTcpConnection);
}

void CServerNetwork::ReadAction()
{
	epoll_event wv[MAX_EP_WAIT] ={ 0 };

	int nRetCount = epoll_wait(m_nepfd, wv, MAX_EP_WAIT, 0);

	if (nRetCount < 0)
		return;

	for (int nLoopCount = 0; nLoopCount < nRetCount; ++nLoopCount)
	{
		CServerConnInfo	*pNetLink = (CServerConnInfo*)wv[nLoopCount].data.ptr;
		if (pNetLink->GetSock() == m_pListenLink->GetSock())
		{
			sockaddr_in	client_addr;
			socklen_t	length = sizeof(client_addr);
			SOCKET		nNewSocket = INVALID_SOCKET;

			while ( -1 != (nNewSocket = accept(m_pListenLink->GetSock(), (sockaddr*)&client_addr, &length)))
			{
				AcceptClient(nNewSocket);
			}

			continue;
		}

		if (wv[nLoopCount].events & EPOLLIN)
		{
			if (pNetLink->RecvData() == -1)
			{
				DisconnectConnection(pNetLink);
			}
		}
		else if (wv[nLoopCount].events & EPOLLPRI)
		{
			// data incoming
			//do_read_pack( pNetLink->m_nSock );
		}
		else if (wv[nLoopCount].events & EPOLLHUP)
		{
			DisconnectConnection(pNetLink);
		}
		else if (wv[nLoopCount].events & EPOLLERR)
		{
			// error
			DisconnectConnection(pNetLink);
		}
	}
}

void CServerNetwork::ThreadFunc()
{
	while (m_bRunning)
	{
		ReadAction();

		for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
		{
			(this->*m_pfnConnStateFunc[m_pTcpConnection[nIndex].GetState()])(m_pTcpConnection[nIndex]);
		}

		yield();
	}

	// 关闭accept
	DisconnectConnection(m_pListenLink);

	// 关闭已连接上的Connection
	for (int nIndex = 0; nIndex < m_uMaxConnCount; ++nIndex)
	{
		if (m_pTcpConnection[nIndex].IsSocketConnected())
		{
			DisconnectConnection(&m_pTcpConnection[nIndex]);
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

	m_nepfd = epoll_create(MAX_EP);
	if (-1 == m_nepfd)
	{
		g_pFileLog->WriteLog("epoll_create failed!\n");
		return false;
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
	setsockopt(m_pListenLink->GetSock(), SOL_SOCKET, SO_REUSEADDR, (const void*)&nOnFlag, sizeof(nOnFlag));

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

	if (-1 == SetNoBlocking(m_pListenLink))
	{
		g_pFileLog->WriteLog("Set listen socket async failed! errno=%d\n", errno);
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
