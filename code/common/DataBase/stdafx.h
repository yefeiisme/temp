#ifndef __STDAFX_H_
#define __STDAFX_H_

#ifdef _DEBUG
#pragma warning (disable: 4512)
#pragma warning (disable: 4786)
#pragma warning (disable: 4018)
#pragma warning (disable: 4244)
#pragma warning (disable: 4554)
#pragma warning (disable: 4996)
#endif

#include <stdio.h>
#include <string.h>
#include "commondefine.h"
#include "IIniFile.h"
#include "IFileLog.h"
#include <list>
#include <map>
#include <string>
using namespace std;

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#define MAX_CALLBACK_DATA_LEN	128

struct SResultSetHead
{
	char	strCallBackDta[MAX_CALLBACK_DATA_LEN];
	WORD	wCallBackDataLen;
	BYTE	byResultCount;
};

struct SResultHead
{
	UINT	uRowCount;
	UINT	uColCount;
	UINT	uLen;
};

struct SMysqlDataHead
{
	UINT	uOffset;
	UINT	uDataLen;
};

#endif
