#ifndef __NET_LINK_H_
#define __NET_LINK_H_

#include "INetwork.h"
#include "../NetworkHead.h"
#define MAX_IP_LEN	16

class CTcpConnection : public ITcpConnection
{
protected:
	/**********************���ͻ�����**********************/
	char						*m_pSendBuf;
	char						*m_pTempSendBuf;
	char						*m_pFlush;
	char						*m_pSend;
	unsigned int				m_uSendBufLen;
	unsigned int				m_uTempSendBufLen;
	/**********************���ͻ�����**********************/
	
	/**********************���ջ�����**********************/
	char						*m_pRecvBuf;
	char						*m_pTempRecvBuf;
	char						*m_pPack;
	char						*m_pRecv;
	char						*m_pUnreleased;
	unsigned int				m_uRecvBufLen;
	unsigned int				m_uTempRecvBufLen;
	/**********************���ջ�����**********************/

	SOCKET						m_nSock;
	unsigned int				m_uConnID;

	bool						m_bTcpConnected;		// ���������Ƿ�����״̬
	bool						m_bLogicConnected;		// �ⲿ�߼��Ƿ�����״̬
public:
	CTcpConnection();
	~CTcpConnection();

	inline SOCKET				GetSock()
	{
		return m_nSock;
	}

	inline void					SetSock(const SOCKET nSock)
	{
		m_nSock	= nSock;
	}

	inline unsigned int			GetConnID() const
	{
		return m_uConnID;
	}

	inline bool					IsSocketConnected()
	{
		return m_bTcpConnected;
	}

	inline bool					IsLogicConnected()
	{
		return m_bLogicConnected;
	}

	inline bool					IsConnect()
	{
		return m_bTcpConnected;
	}

	bool						Initialize(const unsigned int uIndex, unsigned int uRecvBufferLen, unsigned int uSendBufferLen, unsigned int uTempRecvBufLen, unsigned int uTempSendBufLen);
	inline void					ReInit(const int nSocket)
	{
		m_pUnreleased	= m_pRecv = m_pPack = m_pRecvBuf;
		m_pFlush		= m_pSend = m_pSendBuf;
		m_nSock			= nSocket;
	}

	virtual const char			*GetIP() = 0;

	int							RecvData();
	bool						SendData();
	bool						PutPack(const void *pPack, unsigned int uPackLen);
	const void					*GetPack(unsigned int &uPackLen);

	inline void					ShutDown()
	{
		m_bLogicConnected	= false;
	}

	virtual void				Disconnect();

	virtual inline void			Connected()
	{
		m_bTcpConnected		= true;
		m_bLogicConnected	= true;
	}
};

#endif
