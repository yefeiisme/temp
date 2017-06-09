#ifndef KTabFile_H
#define KTabFile_H

#include "KList.h"
#include "ITabFile.h"
#include "MemClass.h"

typedef struct tagTabOffset
{
	unsigned long		dwOffset;
	unsigned long		dwLength;
} TABOFFSET;

class KTabFile : public ITabFile
{
private:
	int			m_Width;
	int			m_Height;
	CMemClass	m_Memory;
	CMemClass	m_OffsetTable;
private:
	void		CreateTabOffset();
	bool		GetValue(const int nRow, const int nColumn, char *lpRString, const unsigned long dwSize);
public:
	KTabFile();
	~KTabFile();
	bool		Load(const char *FileName);
	bool		LoadPack(char *FileName);
	int			FindRow(char *szRow);//������1Ϊ����ֵ
	int			FindColumn(char *szColumn);//������1Ϊ����ֵ
	inline int	GetHeight() const
	{
		return m_Height;
	};
	bool		GetString(const int nRow, char *pstrColumn, char *pstrDefault, char *pstrRString, const unsigned long dwSize);
	bool		GetString(const int nRow, const int nColumn, char *pstrDefault, char *pstrRString, const unsigned long dwSize);
	bool		GetInteger(const int nRow, char *pstrColumn, const int nDefault, int *pnValue);
	bool		GetInteger(const int nRow, const int nColumn, const int nDefault, int *pnValue);
	bool		GetFloat(const int nRow, char *pstrColumn, const float fDefault, float *pfValue);
	bool		GetFloat(const int nRow, const int nColumn, const float fDefault, float *pfValue);
	void		Clear();
	void		Release();
};

#endif
