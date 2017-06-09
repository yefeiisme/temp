#include "stdafx.h"
#include "Player.h"
#include "../ServerConnection/ServerConnection.h"

CPlayer::CPlayer()
{
	m_pConnection	= nullptr;
	m_uUniqueID		= 0;
	m_nActionTime	= 0;
}

CPlayer::~CPlayer()
{
}

void CPlayer::DoAction()
{
}

void CPlayer::ProcessNetPack(const void *pPack, const unsigned int uPackLen)
{
	if (nullptr == pPack)
		return;

	BYTE			byProtocol	= 0;

	byProtocol	= *((BYTE*)pPack);

	if (byProtocol >= s2c_end)
	{
		g_pFileLog->WriteLog("[%s][%d] Player[%u] Invalid Protocol[%hhu]\n", __FILE__, __LINE__, m_uUniqueID, byProtocol);
		return;
	}

	(this->*m_ProtocolFunc[byProtocol])(pPack, uPackLen);
}
