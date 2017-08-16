#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

class IMysqlQuery
{
public:
	virtual bool		SendDBRequest(const void *pPack, const unsigned int uPackLen) = 0;
	virtual const void	*GetDBRespond(unsigned int &uPackLen) = 0;
	virtual void		Stop() = 0;
	virtual bool		IsExit() = 0;
	virtual void		Release() = 0;
};

struct SMysqlRequest
{
	unsigned int	uParam1;
	unsigned int	uParam2;
	unsigned int	uParam3;

	unsigned char	byOpt;
};

struct SMysqlRespond
{
	unsigned int	uParam1;
	unsigned int	uParam2;
	unsigned int	uParam3;

	unsigned int	uRowCount;
	unsigned int	uColCount;
	int				nRetCode;

	unsigned char	byOpt;
};

struct SMysqlDataHead
{
	unsigned int	uOffset;
	unsigned int	uDataLen;
};

IMysqlQuery				*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
