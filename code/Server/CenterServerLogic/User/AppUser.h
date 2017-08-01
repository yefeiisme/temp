#ifndef __APP_USER_H_
#define __APP_USER_H_

#include "User.h"

class CAppUser : public CUser
{
public:
	CAppUser();
	~CAppUser();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CAppUser::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[256];
private:
};

#endif
