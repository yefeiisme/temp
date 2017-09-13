#ifndef __I_WEB_CONNECTION_H_
#define __I_WEB_CONNECTION_H_

class IWebConnection
{
public:
	virtual const void	*GetPack(unsigned int &uPackLen) = 0;					// ��ȡ���յ����������ݰ�
	virtual bool		PutPack(const void *pPack, unsigned int uPackLen) = 0;	// �����������ݰ�
	virtual void		ResetTimeOut() = 0;										// ���ó�ʱ�ȴ���ʱ��
};

#endif
