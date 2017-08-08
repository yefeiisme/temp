#ifndef __I_MYSQL_QUERY_H_
#define __I_MYSQL_QUERY_H_

class IMysqlQuery
{
public:
};

IMysqlQuery			*CreateMysqlQuery(const char *pstrSettingFile, const char *pstrSection);

#endif
