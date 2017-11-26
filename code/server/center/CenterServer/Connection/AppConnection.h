#ifndef __APP_CONNECTION_H_
#define __APP_CONNECTION_H_

#include "ClientConnection.h"

class CAppConnection : public CClientConnection
{
public:
	CAppConnection();
	~CAppConnection();

	void					Connect(ITcpConnection *pTcpConnection);
private:
	void					ResetTimeOut();
	void					Disconnect();
	void					OnWaitLogin();
};

#endif
