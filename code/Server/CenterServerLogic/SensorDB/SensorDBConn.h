#ifndef __SENSOR_DB_CONN_H_
#define __SENSOR_DB_CONN_H_

class CSensorDBConn
{
public:
	CSensorDBConn();
	~CSensorDBConn();

	bool						Initialize();
	void						Run();
private:
	typedef void				(CSensorDBConn::*pfnProtocolFunc)(const void *pPack, const unsigned int uPackLen);
	pfnProtocolFunc				m_ProtocolFunc[256];
};

#endif
