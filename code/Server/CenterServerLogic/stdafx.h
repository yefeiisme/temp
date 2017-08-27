#ifndef __STDAFX_H_
#define __STDAFX_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commondefine.h"
#include <list>
#include <map>
#include <set>
#include <string>
#include <IFileLog.h>
using namespace std;

struct SMysqlRequest
{
	uint64	uClientID;
	UINT	uClientIndex;
	BYTE	byClientType;
	BYTE	byOpt;
};

struct SMysqlRespond
{
	uint64	uClientID;
	UINT	uClientIndex;
	UINT	uRowCount;
	UINT	uColCount;
	int		nRetCode;
	BYTE	byClientType;
	BYTE	byOpt;
};

#endif
