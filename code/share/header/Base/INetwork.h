#ifndef __I_NETWORK_H_
#define __I_NETWORK_H_

class ITcpConnection
{
public:
	virtual bool		IsConnect() = 0;												// 用于在逻辑层判断是否是连接状态
	virtual const void	*GetPack(unsigned int &uPackLen) = 0;							// 获取已收到的网络数据包
	virtual bool		PutPack(const void *pPack, unsigned int uPackLen) = 0;			// 发送网络数据包
	virtual void		ShutDown() = 0;													// 断开操作：1.用于外部（逻辑层）主动发起的断开连接；2.用于外部（逻辑层）收到网络断开的消息，接收完对应的网络包后，通知网络层可以正常断开了
	virtual const char	*GetIP() = 0;
};

class IServerNetwork
{
public:
	virtual void		Stop() = 0;			// 退出时调用（用于网络线程退出）
	virtual bool		IsExited() = 0;		// 用于确认网络线程是否已经正常的退出
	virtual void		Release() = 0;		// 释放内存空间
};

class IClientNetwork
{
public:
	virtual void		Stop() = 0;																					// 退出时调用（用于网络线程退出）
	virtual bool		IsExited() = 0;																				// 用于确认网络线程是否已经正常的退出
	virtual void		Release() = 0;																				// 释放内存空间
	virtual bool		ConnectTo(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex) = 0;		// 连接服务器
	virtual bool		ConnectToUrl(char *pstrAddr, const unsigned short usPort, const unsigned int uIndex) = 0;	// 连接服务器
	virtual void		ShutDown(const unsigned int uIndex) = 0;
};

typedef void(*pfnConnectEvent)(void *pParam, ITcpConnection *pTcpConnection, const unsigned int uIndex);

IServerNetwork *CreateServerNetwork(
	const unsigned short usPort,				// 端口号
	void *lpParam,								// 回调函数的参数
	pfnConnectEvent pfnConnectCallBack,			// 连接成功后的回调函数
	const unsigned int uConnectionNum,			// 最大连接数
	const unsigned int uSendBufferLen,			// 每个连接发送缓冲区的大小
	const unsigned int uRecvBufferLen,			// 每个连接接收缓冲区的大小
	const unsigned int uTempSendBufferLen,		// 最大发送包的大小
	const unsigned int uTempRecvBufferLen,		// 最大接收包的大小
	const unsigned int uSleepTime,				// 线程的Sleep时间
	const unsigned char byPackHeadSize = 16		// 包头的大小，默认16位大小
	);
IClientNetwork *CreateClientNetwork(
	const unsigned int uConnectionNum,			// 最大连接数
	const unsigned int uSendBufferLen,			// 每个连接发送缓冲区的大小
	const unsigned int uRecvBufferLen,			// 每个连接接收缓冲区的大小
	const unsigned int uTempSendBufferLen,		// 最大发送包的大小
	const unsigned int uTempRecvBufferLen,		// 最大接收包的大小
	pfnConnectEvent pfnConnectCallBack,			// 连接成功后的回调函数
	void *lpParm,								// 回调函数的参数
	const unsigned int uSleepTime,				// 线程的Sleep时间
	const unsigned char byPackHeadSize = 16		// 包头的大小，默认16位大小
	);

#endif
