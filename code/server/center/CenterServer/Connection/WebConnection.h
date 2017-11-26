#ifndef __WEB_CONNECTION_H_
#define __WEB_CONNECTION_H_

#include "ClientConnection.h"

class CWebConnection : public CClientConnection
{
public:
	CWebConnection();
	~CWebConnection();

	void					Connect(ITcpConnection *pTcpConnection);
private:
	void					ResetTimeOut();
	void					Disconnect();
	void					OnWaitLogin();
};

#endif
