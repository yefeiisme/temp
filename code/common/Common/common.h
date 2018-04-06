#ifndef __COMMON_H_
#define __COMMON_H_

#include "commondefine.h"
#include "ICommon.h"

class CRandom : public IRandom
{
public:
	CRandom();
	~CRandom();

	inline void			SetRandomSeed(const uint32_t uRandomSeed)
	{
		m_uRandomSeed	= uRandomSeed;
	}

	uint32_t			Random(const uint32_t uMax)
	{
		int64_t nTemp	= m_uRandomSeed * 0x08088405 + 1;
		m_uRandomSeed	= nTemp;
		int64_t nResult	= nTemp * uMax;
		nResult = nResult >> 32;
		return (uint32_t)nResult;
	}

	inline void			Release()
	{
		delete this;
	}

private:
	uint32_t			m_uRandomSeed;
};

#endif

