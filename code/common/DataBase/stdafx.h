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
#include "my_global.h"
#include "mysql.h"
using namespace std;

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

template<class T>
int	StrToArray(char *szStr, const char *szTag, T *pOutArray, int nMaxCount)
{
	int	nCount = 0;
	const char *pToken = strtok(szStr, szTag);

	while(pToken && nCount < nMaxCount)
	{
		*pOutArray = (T)atoi(pToken);
		pToken = strtok(NULL, szTag);
		++pOutArray;
		++nCount;
	}

	return nCount;
}

#endif
