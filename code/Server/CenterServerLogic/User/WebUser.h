#ifndef __WEB_USER_H_
#define __WEB_USER_H_

#include "User.h"

class CWebUser : public CUser
{
public:
	CWebUser();
	~CWebUser();

	void					DoAction();
private:
	void					ProcessNetPack();

	void					RecvPing(const void *pPack, const unsigned int uPackLen);
private:
	typedef void			(CWebUser::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	static pfnProtocolFunc	m_ProtocolFunc[256];
private:
};

#endif
