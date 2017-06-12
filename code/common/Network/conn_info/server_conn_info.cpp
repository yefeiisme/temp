#include "server_conn_info.h"
#include "IFileLog.h"

CServerConnInfo::pfnStateFunc CServerConnInfo::m_pfnStateFunc[SERVER_CONN_STATE_MAX] =
{
	&CServerConnInfo::OnIdle,
	&CServerConnInfo::OnConnect,
	&CServerConnInfo::OnWaitLogicExit,
};

CServerConnInfo::CServerConnInfo() : CTcpConnection()
{
}

CServerConnInfo::~CServerConnInfo()
{
}

const char *CServerConnInfo::GetIP()
{
	sockaddr_in	tagClientAddr;

	socklen_t nAddrLen = sizeof(tagClientAddr);

	if (getpeername(m_nSock, (sockaddr*)&tagClientAddr, &nAddrLen))
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("%s:%d, CNetLink::reinit Client[%4u] getpeername Error[%d]\n", __FILE__, __LINE__, m_uConnID, WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("%s:%d, CNetLink::reinit Client[%4u] getpeername Error[%d]\n", __FILE__, __LINE__, m_uConnID, errno);
#elif defined(__APPLE__)
#endif
		return nullptr;
	}

	char	*strIP = inet_ntoa(tagClientAddr.sin_addr);
	if (nullptr == strIP)
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("%s:%d, CNetLink::reinit Client[%4u] inet_ntoa Error[%d]\n", __FILE__, __LINE__, m_uConnID, WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("%s:%d, CNetLink::reinit Client[%4u] inet_ntoa Error[%d]\n", __FILE__, __LINE__, m_uConnID, errno);
#elif defined(__APPLE__)
#endif
		return nullptr;
	}

	return strIP;
}

void CServerConnInfo::OnIdle()
{
}

void CServerConnInfo::OnConnect()
{
}

void CServerConnInfo::OnWaitLogicExit()
{
}
