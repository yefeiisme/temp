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
// ���ܣ��麯����������������̳�ʵ��
//
void CClient::DoAction()
{
}
