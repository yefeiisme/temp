#ifndef __DATA_CONNECTION_H_
#define __DATA_CONNECTION_H_

#include "ClientConnection.h"

class CDataConnection : public CClientConnection
{
public:
	CDataConnection();
	~CDataConnection();
private:
	void					ResetTimeOut();
	void					Disconnect();
	void					OnWaitLogin();
};

#endif
