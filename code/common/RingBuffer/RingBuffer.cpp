#include "string.h"
#include "IRingBuffer.h"
#include "RingBuffer.h"

#define RB_SPACE	(sizeof(void*)*2)

CRingBuffer::CRingBuffer()
{
	m_pBuffer	= nullptr;
	m_pRead		= nullptr;
	m_pWrite	= nullptr;
	m_pTemp		= nullptr;
	m_pNextRead	= nullptr;
	m_uBufLen	= 0;
	m_uMaxPack	= 0;
}

CRingBuffer::~CRingBuffer()
{
	delete []m_pBuffer;
	m_pBuffer	= nullptr;
	m_pRead		= nullptr;
	m_pWrite	= nullptr;
	m_pTemp		= nullptr;
	m_pNextRead	= nullptr;
	m_uBufLen	= 0;
	m_uMaxPack	= 0;
}

bool CRingBuffer::Initialize(const unsigned int uBufLen, const unsigned int uMaxPack)
{
	if (uBufLen <= (2 * uMaxPack + RB_SPACE))
		return false;

	m_pBuffer	= new char[uBufLen];
	if (nullptr == m_pBuffer)
		return false;

	memset(m_pBuffer, 0, uBufLen);

	m_uMaxPack	= uMaxPack;
	m_uBufLen	= uBufLen - (m_uMaxPack + RB_SPACE);	//����8�ֽ�
	m_pRead		= m_pBuffer;
	m_pWrite	= m_pBuffer;
	m_pTemp		= m_pBuffer + (m_uBufLen + RB_SPACE);
	m_pNextRead	= m_pBuffer;

	return true;
}

void CRingBuffer::Clear()
{
	m_pBuffer	= nullptr;
	m_pRead		= nullptr;
	m_pWrite	= nullptr;
	m_pTemp		= nullptr;
	m_pNextRead	= nullptr;
	m_uBufLen	= 0;
	m_uMaxPack	= 0;
}

bool CRingBuffer::Reinit()
{
	if (nullptr == m_pBuffer)
		return false;
	
	m_pRead		= m_pBuffer;
	m_pWrite	= m_pBuffer;
	m_pNextRead	= m_pBuffer;

	return true;
}
/*
 *	д������,������ָ��
 */
char* CRingBuffer::PutData(char *pRead, char *pWrite, char *pData, const unsigned int uDataSize)
{
	//�ұ߿ռ�
	unsigned int uRightMargin = m_uBufLen - (pWrite - m_pBuffer);

	if (pWrite >= pRead && uDataSize >= uRightMargin)
	{
		/*
		 *	û�л���,�����ұ߿ռ䲻��,��������
		 */
		memcpy(pWrite, pData, uRightMargin);
		pWrite = m_pBuffer;
		memcpy(pWrite, pData + uRightMargin, uDataSize - uRightMargin);
		pWrite += uDataSize - uRightMargin;
		
		if (pWrite > m_pBuffer + m_uBufLen)
			return nullptr;

		return pWrite;
	}

	//������,����û�л��ƣ������ұ߿ռ乻��
	memcpy(pWrite, pData, uDataSize);
	pWrite += uDataSize;

	if (pWrite > m_pBuffer + m_uBufLen)
		return nullptr;

	return pWrite;
}

unsigned CRingBuffer::GetDataLen(char *pRead, char *pWrite)
{
	//��ָ���ұ߿ռ�
	unsigned int uRightMargin = m_uBufLen - (pRead - m_pBuffer);

	if (pWrite < pRead && sizeof(unsigned int) > uRightMargin)
	{
		/*
		 *	�����ˣ��������ݳ��Ȳ�����ȡ
		 */
		unsigned int	uDataLen = 0;
		char			*ptr = (char*)&uDataLen;

		memcpy(ptr, pRead, uRightMargin);
		memcpy(ptr + uRightMargin, m_pBuffer, sizeof(unsigned int) - uRightMargin);

		return uDataLen;
	}
	
	return *((unsigned int*)pRead);
}

bool CRingBuffer::SndPack(const void *pData, const unsigned int uDataSize)
{
	if (nullptr == pData || 0 == uDataSize)
		return false;
	
	if (nullptr == m_pBuffer)
		return false;
	
	if (uDataSize > m_uMaxPack)
		return false;

	char	*pRead	= m_pRead;
	char	*pWrite	= m_pWrite;

	/*
	 *	�ж��Ƿ���
	 */
	////////////////////////////////////////////////////////
	unsigned int	uContentSize    = pWrite >= pRead/*û�л���*/ ? (pWrite - pRead) : m_uBufLen - (pRead - pWrite);

	if (uContentSize > m_uBufLen - 1)
		return false;

	unsigned int	uEmptySize      = m_uBufLen - uContentSize - 1;
	////////////////////////////////////////////////////////

	/*
	 *	û�ռ���
	 */
	if (uDataSize + sizeof(unsigned int) > uEmptySize)
		return false;

	//д�볤��
	pWrite = PutData(pRead, pWrite, (char*)&uDataSize, sizeof(unsigned int));

	if (pWrite == nullptr)
		return false;

	//д������
	pWrite = PutData(pRead, pWrite, (char*)pData, uDataSize);

	if (pWrite == nullptr)
		return false;

	//�޸�ָ��
	m_pWrite = pWrite;

	return true;
}

const void *CRingBuffer::RcvPack(unsigned int &uDataSize)
{
	uDataSize = 0;

	if (m_pBuffer == nullptr)
		return nullptr;

	//�ͷ���һ������
	m_pRead			= m_pNextRead;
	char	*pRead	= m_pRead;
	char	*pWrite	= m_pWrite;

	/*
	 *	�ж��Ƿ���
	 */
	////////////////////////////////////////////////////////
	unsigned int uContentSize	= pWrite >= pRead/*û�л���*/ ? pWrite - pRead : m_uBufLen - (pRead - pWrite);

	if (uContentSize > m_uBufLen - 1)
		return nullptr;
	////////////////////////////////////////////////////////

	if (uContentSize <= sizeof(unsigned int))
		return nullptr;

	uDataSize = GetDataLen(pRead, pWrite);

	if (uContentSize < uDataSize || uDataSize > m_uMaxPack)
	{
		/*
		 *	��Ӧ�ò��������
		 */
		uDataSize = 0;
		return nullptr;
	}

	/*
	 *	һ�𿽱�,�ܳ���
	 */
	unsigned int	uReadLen = uDataSize + sizeof(unsigned int);
	//��ָ���ұ߿ռ�
	unsigned int	uRightMargin = m_uBufLen - (pRead - m_pBuffer);

	if (pWrite < pRead && uReadLen >= uRightMargin)
	{
		/*
		 *	�����ˣ��������ݳ��Ȳ�����ȡ
		 */
		memcpy(m_pTemp, pRead, uRightMargin);

		memcpy(m_pTemp + uRightMargin, m_pBuffer, uReadLen - uRightMargin);

		if (uReadLen - uRightMargin > m_uBufLen)
		{
			uDataSize = 0;
			return nullptr;
		}
		//�޸�ָ��
		m_pNextRead = m_pBuffer + (uReadLen - uRightMargin);

		return (m_pTemp + sizeof(unsigned int));
	}
	else
	{
		//�޸�ָ��
		if (m_pNextRead + uReadLen > m_pBuffer + m_uBufLen)
		{
			uDataSize = 0;
			return nullptr;
		}

		m_pNextRead += uReadLen; 
		return (pRead + sizeof(unsigned int));
	}
}

void CRingBuffer::Release()
{
	delete this;
}

IRingBuffer	*CreateRingBuffer(const unsigned int uBufLen, const unsigned int uMaxPack)
{
	CRingBuffer	*pRingBuffer	= new CRingBuffer();
	if (nullptr == pRingBuffer)
		return nullptr;

	if (!pRingBuffer->Initialize(uBufLen, uMaxPack))
	{
		delete pRingBuffer;
		return nullptr;
	}

	return pRingBuffer;
}
