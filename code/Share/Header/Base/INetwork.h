#ifndef __I_NETWORK_H_
#define __I_NETWORK_H_

class ITcpConnection
{
public:
	virtual bool		IsConnect() = 0;												// �������߼����ж��Ƿ�������״̬
	virtual const void	*GetPack(unsigned int &uPackLen) = 0;							// ��ȡ���յ����������ݰ�
	virtual bool		PutPack(const void *pPack, unsigned int uPackLen) = 0;			// �����������ݰ�
	virtual void		ShutDown() = 0;													// �Ͽ�������1.�����ⲿ���߼��㣩��������ĶϿ����ӣ�2.�����ⲿ���߼��㣩�յ�����Ͽ�����Ϣ���������Ӧ���������֪ͨ�������������Ͽ���
	virtual const char	*GetConnectToIP() = 0;
};

class IServerNetwork
{
public:
	virtual void		Stop() = 0;			// �˳�ʱ���ã����������߳��˳���
	virtual bool		IsExited() = 0;		// ����ȷ�������߳��Ƿ��Ѿ��������˳�
	virtual void		Release() = 0;		// �ͷ��ڴ�ռ�
};

class IClientNetwork
{
public:
	virtual void		Stop() = 0;																					// �˳�ʱ���ã����������߳��˳���
	virtual bool		IsExited() = 0;																				// ����ȷ�������߳��Ƿ��Ѿ��������˳�
	virtual void		Release() = 0;																				// �ͷ��ڴ�ռ�
	virtual bool		ConnectTo(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex) = 0;		// ���ӷ�����
	virtual bool		ConnectToUrl(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex) = 0;	// ���ӷ�����
};

typedef void(*CALLBACK_SERVER_EVENT)(void *lpParam, ITcpConnection *pTcpConnection);
typedef void(*CALLBACK_CLIENT_EVENT)(void *lpParam, ITcpConnection *pTcpConnection, const void *pTarget);
typedef void(*pfnConnectEvent)(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);

IServerNetwork *CreateServerNetwork(
	const unsigned short usPort,				// �˿ں�
	void *lpParam,								// �ص������Ĳ���
	pfnConnectEvent pfnConnectCallBack,			// ���ӳɹ���Ļص�����
	const unsigned int uConnectionNum,			// ���������
	const unsigned int uSendBufferLen,			// ÿ�����ӷ��ͻ������Ĵ�С
	const unsigned int uRecvBufferLen,			// ÿ�����ӽ��ջ������Ĵ�С
	const unsigned int uTempSendBufferLen,		// ����Ͱ��Ĵ�С
	const unsigned int uTempRecvBufferLen,		// �����հ��Ĵ�С
	const unsigned int uSleepTime				// �̵߳�Sleepʱ��
	);
IClientNetwork *CreateClientNetwork(
	const unsigned int uConnectionNum,			// ���������
	const unsigned int uSendBufferLen,			// ÿ�����ӷ��ͻ������Ĵ�С
	const unsigned int uRecvBufferLen,			// ÿ�����ӽ��ջ������Ĵ�С
	const unsigned int uTempSendBufferLen,		// ����Ͱ��Ĵ�С
	const unsigned int uTempRecvBufferLen,		// �����հ��Ĵ�С
	pfnConnectEvent pfnConnectCallBack,			// ���ӳɹ���Ļص�����
	void *lpParm,								// �ص������Ĳ���
	const unsigned int uSleepTime				// �̵߳�Sleepʱ��
	);

#endif
