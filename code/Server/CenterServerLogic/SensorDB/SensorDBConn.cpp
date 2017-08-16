#include "stdafx.h"
#include "SensorDBConn.h"
#include "ICenterServer.h"

CSensorDBConn::CSensorDBConn()
{
	for (auto nIndex = 0; nIndex < sizeof(m_ProtocolFunc[0]) / sizeof(m_ProtocolFunc); ++nIndex)
	{
		m_ProtocolFunc[nIndex]	= nullptr;
	}
}

CSensorDBConn::~CSensorDBConn()
{
}

bool CSensorDBConn::Initialize()
{
	return true;
}

void CSensorDBConn::Run()
{
	unsigned int	uPakcLen	= 0;
	const void		*pPack		= g_ICenterServer.GetDBRespond(uPakcLen);

	if (nullptr == pPack)
		return;
}
