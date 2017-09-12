#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer()
{
	m_pClientConn	= nullptr;
	m_uUniqueID		= 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::DoAction()
{
	ProcessNetPack();
}

void CPlayer::ProcessNetPack()
{
	const void		*pPack		= nullptr;
	unsigned int	uPackLen	= 0;
	BYTE			byProtocol	= 0;

	while (nullptr != (pPack = m_pClientConn->GetPack(uPackLen)))
	{
		byProtocol	= *((BYTE*)pPack);

		if (byProtocol >= c2s_end)
		{
			g_pFileLog->WriteLog("[%s][%d] Player[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
			return;
		}

		(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);

		m_pClientConn->ResetTimeOut();
	};
}

void CPlayer::SendMsg(const void *pPack, const unsigned int uPackLen)
{
	m_pClientConn->PutPack(pPack, uPackLen);
}
