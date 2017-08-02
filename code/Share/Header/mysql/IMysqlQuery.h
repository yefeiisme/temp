#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

class IMysqlQuery
{
public:
};

IMysqlQuery			*CreateMysqlQuery(
										char *pstrDBIP,
										char *pstrAccount,
										char *pstrPassword,
										char *pstrDBName,
										unsigned short usDBPort,
										char *pstrCharset,
										unsigned int uPingTime
										);

#endif
