#ifndef __I_CLIENT_CONNECTION_H_
#define __I_CLIENT_CONNECTION_H_

class IClientConnection
{
public:
	virtual const void	*GetPack(unsigned int &uPackLen) = 0;					// 获取已收到的网络数据包
	virtual bool		PutPack(const void *pPack, unsigned int uPackLen) = 0;	// 发送网络数据包
	virtual void		ResetTimeOut() = 0;										// 重置超时等待的时间
};

#endif
