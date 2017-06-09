#include "stdafx.h"
#include "ITabFile.h"
#include "KFilePath.h"
#include "KFile.h"

//---------------------------------------------------------------------------
// ����:	KFile
// ����:	���캯��
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
KFile::KFile()
{
	m_hFile	= NULL;
	m_dwLen	= 0;
	m_dwPos	= 0;
}
//---------------------------------------------------------------------------
// ����:	~KFile
// ����:	���캯��
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
KFile::~KFile()
{
	Close();
}
//---------------------------------------------------------------------------
// ����:	Open
// ����:	��һ���ļ���׼����ȡ
// ����:	FileName	�ļ���
// ����:	�ɹ�����TRUE��ʧ�ܷ���FALSE��
//---------------------------------------------------------------------------
BOOL KFile::Open(LPSTR FileName)
{
      char PathName[MAXPATH];

        // close prior file handle
        //if (m_hFile != INVALID_HANDLE_VALUE)
        //      Close();

        if (m_hFile != NULL)
                Close();

        // get full path name
        g_GetFullPath(PathName, FileName);
/*#ifndef WIN32
        char *name_ptr = PathName;
        while(*name_ptr) {
                if(*name_ptr == '\\') *name_ptr = '/';
                name_ptr++;
        }
#endif
        // Open the file for read
        m_hFile = CreateFile(
                PathName,               // pointer to name of the file with path
                GENERIC_READ,   // access (read-write) mode
                FILE_SHARE_READ,// share mode
                NULL,                   // pointer to security attributes
                OPEN_EXISTING,  // how to create
                FILE_ATTRIBUTE_NORMAL,// file attributes
                NULL);                  // template file*/
#ifndef WIN32
        char *ptr = PathName;
        while(*ptr) {
          if(*ptr == '\\') *ptr = '/';
          ptr++;
        }

                //open lower case file for linux
                char lcasePathName[MAXPATH];
                char szRootPath[MAXPATH];
                g_GetRootPath(szRootPath);
                strcpy(lcasePathName, PathName);

                if (memcmp(lcasePathName, szRootPath, strlen(szRootPath)) == 0)
                        strlwr(lcasePathName + strlen(szRootPath));
                else
                        strlwr(lcasePathName);
                if (NULL == (m_hFile = fopen(lcasePathName, "rb")))
#endif
        m_hFile = fopen(PathName, "rb");

        // check file handle
        //if (m_hFile == INVALID_HANDLE_VALUE)
        //      return FALSE;

        if (m_hFile == NULL)
        {
#ifdef __linux
			
			perror(PathName);
#endif
                return FALSE;
        }

        return TRUE;
}
//---------------------------------------------------------------------------
// ����:	Close
// ����:	�رմ򿪵��ļ�
// ����:	void
// ����:	void
//---------------------------------------------------------------------------
void KFile::Close()
{
	//if (m_hFile != INVALID_HANDLE_VALUE)
		//CloseHandle(m_hFile);

	if (m_hFile)
		fclose((FILE*)m_hFile);
	
	//m_hFile	= (FILE *)INVALID_HANDLE_VALUE;
	m_hFile	= NULL;
	m_dwLen	= 0;
	m_dwPos	= 0;
}
//---------------------------------------------------------------------------
// ����:	Read
// ����:	��ȡ�ļ�����
// ����:	lpBuffer	��ȡ���ݴ�ŵ��ڴ�����
//			dwReadBytes	��ȡ���ݵ��ֽ���
// ����:	�ɹ����ض�ȡ���ֽ�����ʧ�ܷ���0��
//---------------------------------------------------------------------------
DWORD KFile::Read(LPVOID lpBuffer, DWORD dwReadBytes)
{
	DWORD dwBytesRead;
	
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return 0;
	if (m_hFile == NULL)
		return 0;
	
//	ReadFile(m_hFile, lpBuffer, dwReadBytes, &dwBytesRead, NULL);
	dwBytesRead = fread(lpBuffer, 1, dwReadBytes, m_hFile);	
	m_dwPos += dwBytesRead;
	
	return dwBytesRead;
}
//---------------------------------------------------------------------------
// ����:	Seek
// ����:	�ƶ��ļ�ָ��λ��
// ����:	lDistance		�ƶ�����
//			dwMoveMethod	�ƶ�������FILE_BEGIN��FILE_CURRENT��FILE_END
// ����:	�ɹ�����ָ��λ�ã�ʧ�ܷ���SEEK_ERROR��
//---------------------------------------------------------------------------
DWORD KFile::Seek(LONG lDistance, DWORD dwMoveMethod)
{
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return SEEK_ERROR;

	if (m_hFile == NULL)
		return SEEK_ERROR;

//	m_dwPos = SetFilePointer(m_hFile, lDistance, NULL, dwMoveMethod);
	fseek(m_hFile, lDistance, dwMoveMethod);
	m_dwPos = ftell(m_hFile);
	return m_dwPos;
}
//---------------------------------------------------------------------------
// ����:	Size
// ����:	ȡ���ļ�����
// ����:	void
// ����:	�ɹ������ļ����ȣ�ʧ�ܷ���0��
//---------------------------------------------------------------------------
DWORD KFile::Size()
{
	//if (m_hFile == INVALID_HANDLE_VALUE)
	//	return 0;

	if (m_hFile == NULL)
		return 0;

	if (m_dwLen == 0) {
		DWORD temp = m_dwPos;
		m_dwLen = Seek(0, FILE_END);
		Seek(temp, FILE_BEGIN);
		//m_dwLen = GetFileSize(m_hFile, NULL);
	}

	return m_dwLen;
}

#ifndef WIN32

char* strlwr(char* string)
{
	char* cp;

	for(cp = string; *cp; ++cp)
	{
		if('A' <= *cp && *cp <= 'Z')
		{
			*cp += 'a' - 'A';
		}
	}

	return string;
}

char* strupr(char* string)
{
	char * cp;

	for(cp = string; *cp; ++cp)
	{
		if('a' <= *cp && *cp <= 'z')
		{
			*cp += 'A' - 'a';
		}
	}

	return string;
}

void xtoa(unsigned long val, char *buf, 
		  unsigned radix, int is_neg)
{
	char *p;                /* pointer to traverse string */
	char *firstdig;         /* pointer to first digit */
	char temp;              /* temp char */
	unsigned digval;        /* value of digit */

	p = buf;

	if (is_neg)
	{
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	firstdig = p;           /* save pointer to first digit */

	do
	{
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */

		/* convert to ascii and store */
		if (digval > 9)
		{
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		}
		else
		{
			*p++ = (char) (digval + '0');       /* a digit */
		}
	}while (val > 0);

	/* We now have the digit of the number in the buffer, but in reverse
	   order.  Thus we reverse them now. */

	*p-- = '\0';            /* terminate string; p points to last digit */

	do
	{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	}while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char* itoa(int val, char *buf, int radix)
{
	if(radix == 10 && val < 0)
	{
		xtoa((unsigned long)val, buf, radix, 1);
	}
	else
	{
		xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
	}

	return buf;
}

#endif
