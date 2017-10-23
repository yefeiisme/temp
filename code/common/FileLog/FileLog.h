#ifndef _CFS__FILELOGS___H____
#define _CFS__FILELOGS___H____

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

#define LOG_ROOT_DIRECTORY	128
#define LOG_DIRECTORY_LEN	256
#define LOG_FILE_NAME_LEN	256
#define LOG_FILE_CONTENT	1024*1024		// �ļ�д������ֵ
#define MAXSIZE_CURTIME		64

class IFileLog;

class CFileLog : public IFileLog
{
private:
	FILE			*m_pFile;

	time_t			m_nTimeNow;			// ��ǰʱ��
	tm				m_tagTimeNow;		// ��ǰʱ��

	tm				m_tagDirTime;		// ��¼�ϴδ�Ŀ¼��ʱ��
	time_t			m_nOpenTime;		// ��¼�ļ�������ʱ��.����Ҫ�´����ļ�ʱ����˱������жԱ�.���һ�����򲻴������ļ���ֱ�ӶԵ�ǰ�ļ�����д�룻�����һ�����򴴽����ļ������Դ˱������¸�ֵ���������ļ���д��
	tm				m_tagOpenTime;		// �ļ����򿪵�ʱ��
	int				m_nWriteSize;		// �ļ�д�����ݵĴ�С

	char			m_strRootDir[LOG_ROOT_DIRECTORY];
	char			m_strLogDir[LOG_DIRECTORY_LEN];
private:
	void			WriteLogInner(const char *pcLogMsg);
	void			OpenDirectory();
	bool			OpenLogFile();
	bool			IsOverHour();		// �Ƿ���˵�ǰ��Сʱ
public:
	CFileLog();
	~CFileLog();

	static CFileLog	&Singleton();

	bool			Initialize(char strDirectoryName[]);
	void			WriteLog(const char *pcLogMsg, ...);
};

class Mutex
{
private:
#ifdef WIN32
	CRITICAL_SECTION	m_mutex;
#else
	pthread_mutex_t		m_mutex;
#endif
	int count;
public:
#ifdef WIN32
	Mutex(int mutex_type);
#else
	Mutex(int mutex_type);
#endif
	~Mutex();

	int		lock();
	int		unlock();
};

#endif
