#include "stdafx.h"
#include "AppUser.h"

CAppUser::pfnProtocolFunc CAppUser::m_ProtocolFunc[256] =
{
	&CAppUser::RecvPing,
};

CAppUser::CAppUser() : CUser()
{
}

CAppUser::~CAppUser()
{
}

void CAppUser::DoAction()
{
	ProcessNetPack();
}

void CAppUser::ProcessNetPack()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;
	BYTE			byProtocol	= 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol	= *((BYTE*)pPack);

		if (byProtocol >= 256)
		{
			g_pFileLog->WriteLog("[%s][%d] App User[%u] Invalid Protocol[%hhu]\n", __FUNCTION__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CAppUser::RecvPing(const void *pPack, const unsigned int uPackLen)
{
}
