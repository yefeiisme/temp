#ifndef __NET_LINK_H_
#define __NET_LINK_H_

#include "INetwork.h"
#include "../NetworkHead.h"
#define MAX_IP_LEN	16

enum E_NET_LINK_STATE
{
	NET_LINK_STATE_DISCONNNECT = -1,
	NET_LINK_STATE_CONNECT,
	NET_LINK_STATE_WAI_CONNECT,
};

class CTcpConnection : public ITcpConnection
{
private:
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
	unsigned int				m_uTargetIndex;			// �������ӳɹ���Ļص�����ֻ��Ϊ�ͻ��˶������ӷ�������ʱ��Ч��

	unsigned short				m_usConnectToPort;				// ���ӵĶ˿�
	char						m_strConnectToIP[MAX_IP_LEN];	// ���ӵ�IP��ַ

	bool						m_bTcpConnected;		// ���������Ƿ�����״̬
	bool						m_bLogicConnected;		// �ⲿ�߼��Ƿ�����״̬
public:
	CTcpConnection();
	~CTcpConnection();

	inline void					SetConnectTarget(const unsigned int uIndex)
	{
		m_uTargetIndex	= uIndex;
	}

	inline unsigned int			GetConnectTarget() const
	{
		return m_uTargetIndex;
	}

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

	inline unsigned short		GetConnectToPort() const
	{
		return m_usConnectToPort;
	}

	inline const char			*GetConnectToIP()
	{
		return m_strConnectToIP;
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

	inline bool					ConnectTo(char (&strIP)[MAX_IP_LEN], const unsigned short usPort)
	{
		if (m_bLogicConnected)
			return false;

		strncpy(m_strConnectToIP, strIP, sizeof(m_strConnectToIP));
		m_strConnectToIP[sizeof(m_strConnectToIP)-1]	= '\0';

		m_usConnectToPort	= usPort;

		m_bLogicConnected	= true;

		return true;
	}

	bool						Initialize(const unsigned int uIndex, unsigned int uRecvBufferLen, unsigned int uSendBufferLen, unsigned int uTempRecvBufLen, unsigned int uTempSendBufLen);
	inline void					ReInit(const int nSocket)
	{
		m_pUnreleased	= m_pRecv = m_pPack = m_pRecvBuf;
		m_pFlush		= m_pSend = m_pSendBuf;
		m_nSock			= nSocket;
	}

	const char					*GetIP();

	int							RecvData();
	int							SendData();
	bool						PutPack(const void *pPack, unsigned int uPackLen);
	const void					*GetPack(unsigned int &uPackLen);

	inline void					ShutDown()
	{
		m_bLogicConnected	= false;
	}

	void						Disconnect();

	inline void					Connected()
	{
		m_bTcpConnected		= true;
		m_bLogicConnected	= true;
	}
};

#endif
