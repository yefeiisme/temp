#include "conn_info.h"
#include "IFileLog.h"

CTcpConnection::CTcpConnection()
{
	m_pfnPutPack		= nullptr;
	m_pfnGetPack		= nullptr;

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

bool CTcpConnection::Initialize(const unsigned int uIndex, unsigned int uRecvBufferLen, unsigned int uSendBufferLen, unsigned int uTempRecvBufLen, unsigned int uTempSendBufLen, unsigned char byPackSize)
{
	if (0 == uRecvBufferLen || 0 == uSendBufferLen)
		return false;

	if (16 == byPackSize)
	{
		m_pfnPutPack	= &CTcpConnection::Put16Pack;
		m_pfnGetPack	= &CTcpConnection::Get16Pack;
	}
	else if (32 == byPackSize)
	{
		m_pfnPutPack	= &CTcpConnection::Put32Pack;
		m_pfnGetPack	= &CTcpConnection::Get32Pack;
	}
	else
	{
		return false;
	}

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
				g_pFileLog->WriteLog("[%s][%d] errno: %d\n", __FILE__, __LINE__, nError);
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
	return (this->*m_pfnGetPack)(uPackLen);
}

bool CTcpConnection::PutPack(const void* pPack, unsigned int uPackLen)
{
	return (this->*m_pfnPutPack)(pPack, uPackLen);
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

bool CTcpConnection::Put16Pack(const void *pPack, unsigned int uPackLen)
{
	if (!m_bLogicConnected)
		return false;

	char	*pPutStart	= m_pWritePtr;
	char	*pPutEnd	= m_pFlush;
	char	*pData		= (char*)pPack;

	// ���ĳ�����Ӱ�ͷ�Ĵ�С
	unsigned short		uTotalPackLen	= uPackLen + sizeof(unsigned short);
	uPackLen	+= sizeof(unsigned short);

	int	nEmptyLen	= pPutStart >= pPutEnd ? pPutEnd + m_uSendBufLen - pPutStart : nEmptyLen = pPutEnd - pPutStart;

	// ���������ˣ�����ʣ��ռ䲻������������ȵİ������ش�����Ҫ�ر����ӣ�
	if (uTotalPackLen >= nEmptyLen)
	{
		g_pFileLog->WriteLog("[%s][%d] CNetLink::putpack Client[%4u]: Send buffer overflow!!!\n", __FILE__, __LINE__, m_uConnID);
		return false;
	}

	int	nTailLen = m_pSendBuf + m_uSendBufLen - pPutStart;

	if (pPutEnd > pPutStart || uTotalPackLen < nTailLen)
	{// �����ˣ���β���ռ乻д�������
		*(unsigned short*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(unsigned short), pData, uPackLen);

		m_pWritePtr = pPutStart + uTotalPackLen;
	}
	else if (uTotalPackLen == nTailLen)
	{// β���ռ��������������
		*(unsigned short*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(unsigned short), pData, uPackLen);

		m_pWritePtr = m_pSendBuf;
	}
	else
	{
		if (nTailLen > sizeof(unsigned short))
		{// β���ռ䣬�������ɰ�ͷ���Լ����ְ��������
			*(unsigned short*)pPutStart = uTotalPackLen;
			memcpy(pPutStart + sizeof(unsigned short), pData, nTailLen - sizeof(unsigned short));
			memcpy(m_pSendBuf, pData + (nTailLen - sizeof(unsigned short)), uTotalPackLen - nTailLen);
			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
		else if (nTailLen == sizeof(unsigned short))
		{// β���ռ䣬���ÿ��������°�ͷ�ĳ���
			*(unsigned short*)pPutStart = uTotalPackLen;
			memcpy(m_pSendBuf, pData, uPackLen);
			m_pWritePtr	= m_pSendBuf + uPackLen;
		}
		else
		{// β���ռ�Ȱ�ͷС���򽫰�ͷ�����ݣ�дһ������β����ʣ�ಿ��д�ڻ�������ͷ��
			int	nLeftHeadData	= sizeof(unsigned short)-nTailLen;

			// �ȿ���һ���ְ�ͷ��β��
			memcpy(pPutStart, &uTotalPackLen, nTailLen);

			// �ٿ���ʣ�ಿ�ֵ�������ͷ��
			memcpy(m_pSendBuf, (char*)(&uTotalPackLen) + nTailLen, nLeftHeadData);

			// ��󿽱���������
			memcpy(m_pSendBuf + nLeftHeadData, m_pTempSendBuf, uPackLen);

			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
	}

	return true;
}

bool CTcpConnection::Put32Pack(const void *pPack, unsigned int uPackLen)
{
	if (!m_bLogicConnected)
		return false;

	char	*pPutStart	= m_pWritePtr;
	char	*pPutEnd	= m_pFlush;
	char	*pData		= (char*)pPack;

	// ���ĳ�����Ӱ�ͷ�Ĵ�С
	unsigned int	uTotalPackLen	= uPackLen + sizeof(unsigned int);
	uPackLen	+= sizeof(unsigned int);

	int	nEmptyLen	= pPutStart >= pPutEnd ? pPutEnd + m_uSendBufLen - pPutStart : nEmptyLen = pPutEnd - pPutStart;

	// ���������ˣ�����ʣ��ռ䲻������������ȵİ������ش�����Ҫ�ر����ӣ�
	if (uTotalPackLen >= nEmptyLen)
	{
		g_pFileLog->WriteLog("[%s][%d] CNetLink::putpack Client[%4u]: Send buffer overflow!!!\n", __FILE__, __LINE__, m_uConnID);
		return false;
	}

	int	nTailLen = m_pSendBuf + m_uSendBufLen - pPutStart;

	if (pPutEnd > pPutStart || uTotalPackLen < nTailLen)
	{// �����ˣ���β���ռ乻д�������
		*(unsigned int*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(unsigned int), pData, uPackLen);

		m_pWritePtr = pPutStart + uTotalPackLen;
	}
	else if (uTotalPackLen == nTailLen)
	{// β���ռ��������������
		*(unsigned int*)pPutStart = uTotalPackLen;

		memcpy(pPutStart + sizeof(unsigned int), pData, uPackLen);

		m_pWritePtr = m_pSendBuf;
	}
	else
	{
		if (nTailLen > sizeof(unsigned int))
		{// β���ռ䣬�������ɰ�ͷ���Լ����ְ��������
			*(unsigned int*)pPutStart = uTotalPackLen;
			memcpy(pPutStart + sizeof(unsigned int), pData, nTailLen - sizeof(unsigned int));
			memcpy(m_pSendBuf, pData + (nTailLen - sizeof(unsigned int)), uTotalPackLen - nTailLen);
			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
		else if (nTailLen == sizeof(unsigned int))
		{// β���ռ䣬���ÿ��������°�ͷ�ĳ���
			*(unsigned int*)pPutStart = uTotalPackLen;
			memcpy(m_pSendBuf, pData, uPackLen);
			m_pWritePtr	= m_pSendBuf + uPackLen;
		}
		else
		{// β���ռ�Ȱ�ͷС���򽫰�ͷ�����ݣ�дһ������β����ʣ�ಿ��д�ڻ�������ͷ��
			int	nLeftHeadData	= sizeof(unsigned int)-nTailLen;

			// �ȿ���һ���ְ�ͷ��β��
			memcpy(pPutStart, &uTotalPackLen, nTailLen);

			// �ٿ���ʣ�ಿ�ֵ�������ͷ��
			memcpy(m_pSendBuf, (char*)(&uTotalPackLen) + nTailLen, nLeftHeadData);

			// ��󿽱���������
			memcpy(m_pSendBuf + nLeftHeadData, m_pTempSendBuf, uPackLen);

			m_pWritePtr	= m_pSendBuf + uTotalPackLen - nTailLen;
		}
	}

	return true;
}

const void *CTcpConnection::Get16Pack(unsigned int &uPackLen)
{
	if (!m_bTcpConnected)
		return nullptr;

	char			*pPackStart	= m_pNextPack;
	char			*pPackEnd	= m_pRecv;
	unsigned short	uPackLength;

	// ������յ������ݳ��Ⱥͻ�����β������
	int nDataLen = pPackEnd >= pPackStart ? pPackEnd - pPackStart : m_uRecvBufLen + pPackEnd - pPackStart;
	int nTailLen = m_uRecvBufLen + m_pRecvBuf - pPackStart;

	// ���ݳ��Ȳ���һ����ͷ�ĳ��ȣ�ֱ�ӷ���
	if (nDataLen < sizeof(unsigned short))
		return nullptr;

	// ���е�������ݵĳ��ȿ϶��Ǵ��ڰ�ͷ�ĳ��ȡ�����β������ȷ���Ƿ��ƣ��Ա������ͷ����������ô��ȡ
	if (nTailLen > sizeof(unsigned short))
	{// β�����ȴ��ڰ�ͷ�ĳ��ȣ���ֱ�ӻ�ȡ��ͷ���ȣ���������ָ��ָ���ͷ����
		uPackLength	= *(unsigned short*)pPackStart;
		pPackStart	+= sizeof(unsigned short);
		nTailLen	= nTailLen - sizeof(unsigned short);
	}
	else if (nTailLen == sizeof(unsigned short))
	{// β�����ȵ��ڰ�ͷ�ĳ��ȣ������ˣ��Ȼ�ȡ��ͷ���ȣ���������ָ��ָ�򻺳���ͷ
		uPackLength	= *(unsigned short*)pPackStart;
		pPackStart	= m_pRecvBuf;
	}
	else
	{// β������С�ڰ�ͷ�ĳ��ȣ������ˣ���β���ͻ�����ͷ�����ݣ���������İ�ͷ����������ָ��ָ���ͷ����
		int	nLeftDataLen	= sizeof(unsigned short)-nTailLen;

		memcpy(&uPackLength, pPackStart, nTailLen);

		memcpy((char*)(&uPackLength) + nTailLen, m_pRecvBuf, nLeftDataLen);

		pPackStart		= m_pRecvBuf + nLeftDataLen;
	}

	// ��ͷ�������ֵ���������Ȱ�ͷ�Ĵ�С��С��˵�����������⣬�������������ݣ���m_pNextPackָ���ѽ��յ�����ָ�뵱ǰλ�ã��ر����ӣ���Ϊ���������ˣ�
	if (uPackLength <= sizeof(unsigned short))
	{
		m_pNextPack = m_pRecv;
		return nullptr;
	}

	// �ѽ��յ��Ļ��������ݳ��ȣ��Ȱ�ͷ��С��˵��ֻ�յ���������Ĳ������ݡ��ȷ��أ��Ƚ��������ٴ���
	if (uPackLength > nDataLen)
		return nullptr;

	// ���е������϶����յ���һ�������İ����Ƚ����ݳ��ȼ�����ͷ�ĳ��ȣ��Ա�ʾ����ĳ��ȣ�����Ҫ��������ݳ��ȣ�
	uPackLength -= sizeof(unsigned short);

	char *pRetBuf = nullptr;
	if (pPackEnd > pPackStart || uPackLength < nTailLen)
	{// û�л���
		m_pNextPack		= pPackStart + uPackLength;
		uPackLen		= uPackLength;
		m_pUnreleased	= pPackStart;
		pRetBuf			= pPackStart;
	}
	else if (uPackLength > nTailLen)
	{// ������
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
	{// û�л��ƣ����������ݳ��Ⱦ���ʣ���β������
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

const void *CTcpConnection::Get32Pack(unsigned int &uPackLen)
{
	if (!m_bTcpConnected)
		return nullptr;

	char			*pPackStart	= m_pNextPack;
	char			*pPackEnd	= m_pRecv;
	unsigned int	uPackLength;

	// ������յ������ݳ��Ⱥͻ�����β������
	int nDataLen = pPackEnd >= pPackStart ? pPackEnd - pPackStart : m_uRecvBufLen + pPackEnd - pPackStart;
	int nTailLen = m_uRecvBufLen + m_pRecvBuf - pPackStart;

	// ���ݳ��Ȳ���һ����ͷ�ĳ��ȣ�ֱ�ӷ���
	if (nDataLen < sizeof(unsigned int))
		return nullptr;

	// ���е�������ݵĳ��ȿ϶��Ǵ��ڰ�ͷ�ĳ��ȡ�����β������ȷ���Ƿ��ƣ��Ա������ͷ����������ô��ȡ
	if (nTailLen > sizeof(unsigned int))
	{// β�����ȴ��ڰ�ͷ�ĳ��ȣ���ֱ�ӻ�ȡ��ͷ���ȣ���������ָ��ָ���ͷ����
		uPackLength	= *(unsigned int*)pPackStart;
		pPackStart	+= sizeof(unsigned int);
		nTailLen	= nTailLen - sizeof(unsigned int);
	}
	else if (nTailLen == sizeof(unsigned int))
	{// β�����ȵ��ڰ�ͷ�ĳ��ȣ������ˣ��Ȼ�ȡ��ͷ���ȣ���������ָ��ָ�򻺳���ͷ
		uPackLength	= *(unsigned int*)pPackStart;
		pPackStart	= m_pRecvBuf;
	}
	else
	{// β������С�ڰ�ͷ�ĳ��ȣ������ˣ���β���ͻ�����ͷ�����ݣ���������İ�ͷ����������ָ��ָ���ͷ����
		int	nLeftDataLen	= sizeof(unsigned int)-nTailLen;

		memcpy(&uPackLength, pPackStart, nTailLen);

		memcpy((char*)(&uPackLength) + nTailLen, m_pRecvBuf, nLeftDataLen);

		pPackStart		= m_pRecvBuf + nLeftDataLen;
	}

	// ��ͷ�������ֵ���������Ȱ�ͷ�Ĵ�С��С��˵�����������⣬�������������ݣ���m_pNextPackָ���ѽ��յ�����ָ�뵱ǰλ�ã��ر����ӣ���Ϊ���������ˣ�
	if (uPackLength <= sizeof(unsigned int))
	{
		m_pNextPack = m_pRecv;
		return nullptr;
	}

	// �ѽ��յ��Ļ��������ݳ��ȣ��Ȱ�ͷ��С��˵��ֻ�յ���������Ĳ������ݡ��ȷ��أ��Ƚ��������ٴ���
	if (uPackLength > nDataLen)
		return nullptr;

	// ���е������϶����յ���һ�������İ����Ƚ����ݳ��ȼ�����ͷ�ĳ��ȣ��Ա�ʾ����ĳ��ȣ�����Ҫ��������ݳ��ȣ�
	uPackLength -= sizeof(unsigned int);

	char *pRetBuf = nullptr;
	if (pPackEnd > pPackStart || uPackLength < nTailLen)
	{// û�л���
		m_pNextPack		= pPackStart + uPackLength;
		uPackLen		= uPackLength;
		m_pUnreleased	= pPackStart;
		pRetBuf			= pPackStart;
	}
	else if (uPackLength > nTailLen)
	{// ������
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
	{// û�л��ƣ����������ݳ��Ⱦ���ʣ���β������
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
