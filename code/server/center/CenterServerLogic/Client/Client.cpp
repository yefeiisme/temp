#include "stdafx.h"
#include "Client.h"

CClient::CClient()
{
	m_pClientConn	= nullptr;
	m_uUniqueID		= 0;
	m_uIndex		= 0;
	m_uAccountID	= 0;
}

CClient::~CClient()
{
}

//=====================================================
// 功能：虚函数，用于其它子类继承实现
//
void CClient::DoAction()
{
}
