#include "conn_info.h"
#include "IFileLog.h"

CTcpConnection::CTcpConnection()
{
	m_nSock				= INVALID_SOCKET;
	m_uConnID			= 0;

	m_pSendBuf			= nullptr;
	m_pTempSendBuf		= nullptr;
	m_pFlush			= nullptr;
	m_pWritePtr			= nullptr;
	m_uSendBufLen		= 0;
	m_uTempSendBufLen	= 0;

	m_pRecvBuf			= nullptr;
	m_pTempRecvBuf		= nullptr;
	m_pNextPack			= nullptr;
	m_pRecv				= nullptr;
	m_pUnreleased		= nullptr;
	m_uRecvBufLen		= 0;
	m_uTempRecvBufLen	= 0;

	m_bTcpConnected		= false;
	m_bLogicConnected	= false;
}

CTcpConnection::~CTcpConnection()
{
	SAFE_DELETE_ARR(m_pSendBuf);
	SAFE_DELETE_ARR(m_pRecvBuf);
	SAFE_DELETE_ARR(m_pTempRecvBuf);
	SAFE_DELETE_ARR(m_pTempSendBuf);
}

bool CTcpConnection::Initialize(const unsigned int uIndex, unsigned int uRecvBufferLen, unsigned int uSendBufferLen, unsigned int uTempRecvBufLen, unsigned int uTempSendBufLen)
{
	if (0 == uRecvBufferLen || 0 == uSendBufferLen)
		return false;

	m_uRecvBufLen	= uRecvBufferLen;
	m_uSendBufLen	= uSendBufferLen;

	m_pRecvBuf		= new char[m_uRecvBufLen];
	if (!m_pRecvBuf)
		return false;

	memset(m_pRecvBuf, 0, sizeof(char)*m_uRecvBufLen);

	m_pSendBuf		= new char[m_uSendBufLen];
	if (!m_pSendBuf)
		return false;

	memset(m_pSendBuf, 0, sizeof(char)*m_uSendBufLen);

	// init temp packet buffer, will used to hold a packet which will be 
	// returned to caller, when the pack wrapped in the buffer
	m_uTempRecvBufLen	= uTempRecvBufLen;
	m_pTempRecvBuf		= new char[m_uTempRecvBufLen];
	if (!m_pTempRecvBuf)
		return false;

	memset(m_pTempRecvBuf, 0, sizeof(char)*m_uTempRecvBufLen);

	m_uTempSendBufLen	= uTempSendBufLen;
	m_pTempSendBuf		= new char[m_uTempSendBufLen];
	if (!m_pTempSendBuf)
		return false;

	memset(m_pTempSendBuf, 0, sizeof(char)*m_uTempSendBufLen);

	m_uConnID	= uIndex;

	return true;
}

int CTcpConnection::RecvData()
{
	char	*pRecvStart	= m_pRecv;
	char	*pRecvEnd	= m_pUnreleased;
	int		nReadedBytes;
	int		nMaxReadBytes;

	if (pRecvStart >= pRecvEnd)
	{
		// buffer is empty or not wrapped
		// so, fill in the tail, and then, 
		// if the tail full, wrap to the head
		nMaxReadBytes	= m_pRecvBuf + m_uRecvBufLen - pRecvStart;
		nReadedBytes	= recv(m_nSock, pRecvStart, nMaxReadBytes, MSG_NOSIGNAL);

		if (nReadedBytes > 0 && nReadedBytes < nMaxReadBytes)
		{
			m_pRecv = pRecvStart + nReadedBytes;
			return nReadedBytes;
		}
		else if (nReadedBytes == nMaxReadBytes)
		{
			pRecvStart = m_pRecvBuf;
			// not return, fall down to next block to read more data
		}
		else if (nReadedBytes == 0)
		{
			return -1;
		}
		else if (errno != EAGAIN)
		{
#if defined(WIN32) || defined(WIN64)
			int	nError	= WSAGetLastError();
			if (WSAECONNRESET != nError)
#elif defined(__linux)
			int	nError	= errno;
			if (ECONNRESET != nError)
#elif defined(__APPLE__)
#endif
			{
				g_pFileLog->WriteLog("file: %s, line: %d, errno: %d\n", __FILE__, __LINE__, nError);
			}
			return -1;
		}
		else
		{
			return 0;
		}
	}

	nMaxReadBytes	= pRecvEnd - pRecvStart;
	nReadedBytes	= recv(m_nSock, pRecvStart, nMaxReadBytes, MSG_NOSIGNAL);

	if (nReadedBytes > 0 && nReadedBytes < nMaxReadBytes)
	{
		m_pRecv = pRecvStart + nReadedBytes;
		return nReadedBytes;
	}
	else if (nReadedBytes == nMaxReadBytes)
	{
		// Overflow
		g_pFileLog->WriteLog("ERROR on CNetLink::recvdata Client[%4u] recvbuffer overflow\n", m_uConnID);
		return -1;
	}
	else if (nReadedBytes == 0)
	{
		return -1;
	}
	else if (errno != EAGAIN)
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("file: %s, line: %d, errno: %d\n", __FILE__, __LINE__, WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("file: %s, line: %d, errno: %d\n", __FILE__, __LINE__, errno);
#elif defined(__APPLE__)
#endif
		return -1;
	}
	else
	{
		m_pRecv = pRecvStart;
		return 0;
	}
}

const void *CTcpConnection::GetPack(unsigned int &uPackLen)
{
	if (!m_bTcpConnected)
		return nullptr;
	
	char	*pPackStart	= m_pNextPack;
	char	*pPackEnd	= m_pRecv;
	NetHead	uPackLength;
	
	// 计算接收到的数据长度和缓冲区尾部长度
	int nDataLen = pPackEnd >= pPackStart ? pPackEnd - pPackStart : m_uRecvBufLen + pPackEnd - pPackStart;
	int nTailLen = m_uRecvBufLen + m_pRecvBuf - pPackStart;

	// 数据长度不够一个包头的长度，直接返回
	if (nDataLen < sizeof(NetHead))
		return nullptr;
	
	// 运行到这里，数据的长度肯定是大于包头的长度。根据尾部长度确定是否环绕，以便决定包头长度数据怎么获取
	if (nTailLen > sizeof(NetHead))
	{// 尾部长度大于包头的长度，则直接获取包头长度，并将数据指针指向包头后面
		uPackLength	= *(NetHead*)pPackStart;
		pPackStart	+= sizeof(NetHead);
		nTailLen	= nTailLen - sizeof(NetHead);
	}
	else if (nTailLen == sizeof(NetHead))
	{// 尾部长度等于包头的长度，环绕了，先获取包头长度，并将数据指针指向缓冲区头
		uPackLength	= *(NetHead*)pPackStart;
		pPackStart		= m_pRecvBuf;
	}
	else
	{// 尾部长度小于包头的长度，环绕了，从尾部和缓冲区头的数据，组成真正的包头，并将数据指针指向包头后面
		int	nLeftDataLen	= sizeof(NetHead) - nTailLen;

		memcpy(&uPackLength, pPackStart, nTailLen);

		// 这里要测试一下，看地址是否是按1个字节大小进行位移的
		// ...
		memcpy((&uPackLength)+nTailLen, m_pRecvBuf, nLeftDataLen);

		pPackStart		= m_pRecvBuf + nLeftDataLen;
	}
	
	// 包头里的数据值（包长）比包头的大小还小，说明数据有问题，不处理这条数据，将m_pNextPack指向已接收的数据指针当前位置（关闭连接，因为数据有误了）
	if (uPackLength <= sizeof(NetHead))
	{
		m_pNextPack = m_pRecv;
		return nullptr;
	}

	// 已接收到的缓冲区数据长度，比包头的小。说明只收到了这个包的部分数据。先返回，等接收完了再处理
	if (uPackLength > nDataLen)
		return nullptr;

	// 运行到这里，则肯定是收到了一个完整的包。先将数据长度减掉包头的长度，以表示包体的长度（真正要传输的数据长度）
	uPackLength -= sizeof(NetHead);
	
	char *pRetBuf = nullptr;
	if (pPackEnd > pPackStart || uPackLength < nTailLen)
	{// 没有环绕
		m_pNextPack		= pPackStart + uPackLength;
		uPackLen		= uPackLength;
		m_pUnreleased	= pPackStart;
		pRetBuf			= pPackStart;
	}
	else if (uPackLength > nTailLen)
	{// 环绕了
		if (nTailLen > m_uTempRecvBufLen || uPackLength > m_uTempRecvBufLen)
		{
			g_pFileLog->WriteLog("[%s][%d] Client[%4u]:Recv temp buff overflow tail_length = %d tmp_recvbuf_len = %u, pack_length = %u\n", __FILE__, __LINE__, m_uConnID, nTailLen, m_uTempSendBufLen, uPackLen);
			return nullptr;
		}
		memcpy(m_pTempRecvBuf, pPackStart, nTailLen);
		memcpy(m_pTempRecvBuf + nTailLen, m_pRecvBuf, uPackLength - nTailLen);
		m_pUnreleased	= pPackStart;
		m_pNextPack		= m_pRecvBuf + uPackLength - nTailLen;
		uPackLen		= uPackLength;
		pRetBuf			= m_pTempRecvBuf;
	}
	else if (uPackLength == nTailLen)
	{// 没有环绕，且正好数据长度就是剩余的尾部长度
		m_pNextPack		= m_pRecvBuf;
		uPackLen		= uPackLength;
		m_pUnreleased	= pPackStart;
		pRetBuf			= pPackStart;
	}
	else
	{
		return nullptr;
	}

	return pRetBuf;
}

bool CTcpConnection::PutPack(const void* pPack, unsigned int uPackLen)
{
	if (!m_bLogicConnected)
		return false;

	char	*pPutStart	= m_pWritePtr;
	char	*pPutEnd	= m_pFlush;
	char	*pData		= (char*)pPack;

	// 包的长度添加包头的大小
	NetHead		uTotalPackLen	= uPackLen + sizeof(NetHead);
	uPackLen	+= sizeof(NetHead);

	int	nEmptyLen	= pPutStart >= pPutEnd ? pPutEnd + m_uSendBufLen - pPutStart : nEmptyLen = pPutEnd - pPutStart;

	// 缓冲区满了，或者剩余空间不够放入这个长度的包，返回错误（需要关闭连接）
	if (uTotalPackLen >= nEmptyLen)
	{
		g_pFileLog->WriteLog("[%s][%d] CNetLink::putpack Client[%4u]: Send buffer overflow!!!\n", __FILE__, __LINE__, m_uConnID);
		return false;
	}

	int	nTailLen = m_pSendBuf + m_uSendBufLen - pPutStart;
	
	if (pPutEnd > pPutStart || uTotalPackLen < nTailLen)
	{// 环绕了，但尾部空间够写入这个包
		*(NetHead*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(NetHead), pData, uPackLen);

		m_pWritePtr = pPutStart + uTotalPackLen;
	}
	else if (uTotalPackLen == nTailLen)
	{// 尾部空间正好容纳这个包
		*(NetHead*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(NetHead), pData, uPackLen);

		m_pWritePtr = m_pSendBuf;
	}
	else
	{
		if (nTailLen > sizeof(NetHead))
		{// 尾部空间，可以容纳包头，以及部分包体的数据
			*(NetHead*)pPutStart = uTotalPackLen;
			memcpy(pPutStart + sizeof(NetHead), pData, nTailLen - sizeof(NetHead));
			memcpy(m_pSendBuf, pData + (nTailLen - sizeof(NetHead)), uTotalPackLen - nTailLen);
			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
		else if (nTailLen == sizeof(NetHead))
		{// 尾部空间，正好可以容纳下包头的长度
			*(NetHead*)pPutStart = uTotalPackLen;
			memcpy(m_pSendBuf, pData, uPackLen);
			m_pWritePtr	= m_pSendBuf + uPackLen;
		}
		else
		{// 尾部空间比包头小，则将包头的数据，写一部分在尾部，剩余部分写在缓冲区的头部
			int	nLeftHeadData	= sizeof(NetHead) - nTailLen;

			// 先拷贝一部分包头到尾部
			memcpy(pPutStart, &uTotalPackLen, nTailLen);

			// 这里要测试一下，看地址是否是按1个字节大小进行位移的
			// ...

			// 再拷贝剩余部分到缓冲区头部
			memcpy(m_pSendBuf, (&uTotalPackLen)+nTailLen, nLeftHeadData);

			// 最后拷贝包体数据
			memcpy(m_pSendBuf + nLeftHeadData, m_pTempSendBuf, uPackLen);

			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
	}
	return true;
}


bool CTcpConnection::SendData()
{
	char	*pFlushStart	= m_pFlush;
	char	*pFlushEnd		= m_pWritePtr;
	int		nMaxSendBytes;
	int		nSendedBytes;
	int		nTailLen = 0;

	// nothing to send
	if (pFlushStart == pFlushEnd)
		return true;

	if (pFlushStart < pFlushEnd)
	{
		nMaxSendBytes	= pFlushEnd - pFlushStart;
		nSendedBytes	= send(m_nSock, pFlushStart, nMaxSendBytes, MSG_NOSIGNAL);

		if (nSendedBytes == nMaxSendBytes)
		{
			m_pFlush = pFlushEnd;
			return true;
		}
		else if (nSendedBytes >= 0)
		{
			m_pFlush = pFlushStart + nSendedBytes;
			return true;
		}
	}
	else
	{
		nTailLen		= m_pSendBuf  + m_uSendBufLen - pFlushStart;
		nSendedBytes	= send(m_nSock, pFlushStart, nTailLen, MSG_NOSIGNAL);

		if (nSendedBytes == nTailLen)
		{
			if (pFlushEnd == m_pSendBuf)
			{
				m_pFlush = m_pSendBuf;
				return true;
			}
			// wrap to the head of buffer
			nMaxSendBytes	= pFlushEnd - m_pSendBuf;
			nSendedBytes	= send(m_nSock, m_pSendBuf, nMaxSendBytes, MSG_NOSIGNAL);

			if (nSendedBytes >= 0)
			{
				m_pFlush = m_pSendBuf + nSendedBytes;
				return true;
			}
		}
		else if (nSendedBytes >= 0)
		{
			m_pFlush = pFlushStart + nSendedBytes;
			return true;
		}
	}

	if (errno != EAGAIN)
	{
#if defined(WIN32) || defined(WIN64)
		g_pFileLog->WriteLog("file: %s, line: %d, errno: %d\n", __FILE__, __LINE__, WSAGetLastError());
#elif defined(__linux)
		g_pFileLog->WriteLog("file: %s, line: %d, errno: %d\n", __FILE__, __LINE__, errno);
#elif defined(__APPLE__)
#endif
		return false;
	}
	else
	{
		// should block, return and wait for next time
		return true;
	}
	
}

void CTcpConnection::Disconnect()
{
	m_bTcpConnected	= false;

	if (INVALID_SOCKET != m_nSock)
	{
		closesocket(m_nSock);
		m_nSock		= INVALID_SOCKET;
	}
}
