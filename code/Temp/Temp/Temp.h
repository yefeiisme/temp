#ifndef __TEMP_H_
#define __TEMP_H_

#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <thread>
#include<iostream>
#include<algorithm>
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
#endif

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#define MAX_DNPOKER_COUNT	52			// ȥ����С�� Joker
#define MIN_DNPOKER_VALUE	1			//	A
#define MAX_DNPOKER_VALUE	13			//	K
#define DNPOKER_COLOR_COUNT 4
#define BOMBED_BULL_GROUP_POKER_COUNT	5	// ը��ţһ�������
#define BOMBED_BULL_GROUP_COUNT			5	// ը��ţһ�ֵ�������ÿ��ӵ��һGroup���ƣ�
#define BOMBED_BULL_POKER_COUNT	(BOMBED_BULL_GROUP_COUNT*BOMBED_BULL_GROUP_POKER_COUNT)		// ը��ţһ�����������

const int32_t kPokerNumOfGroup = 5;

//�˿��Ƶ����ֻ�ɫ
struct  DNPokerColorType
{
	enum
	{
		DN_POKER_COLOR_DIAMONDS		 = 1,		//����
		DN_POKER_COLOR_CLUBS		 = 2,		//ӣ��
		DN_POKER_COLOR_HEARTS		 = 3,		//����
		DN_POKER_COLOR_SPADES		 = 4,		//����
	};
};

//һ���˿��Ƶ�����
struct DNOnePoker
{
	int8_t	poker_value;
	int8_t	poker_color;
	int8_t	byPokerValue;

	DNOnePoker()
	{
		poker_color = 0;
		poker_value = 0;
	}

	DNOnePoker& operator=(const DNOnePoker& poker);
	bool operator==(const DNOnePoker& otherPoker) const;
	bool operator!=(const DNOnePoker& poker) const;

	void reset();
	bool isNullPoker() const;
	std::string friendlyName() const;
};

//�����˿˵�ֵ���бȽ�
bool DNPokerValueFirstCmp(const DNOnePoker& one, const DNOnePoker& other);
//���ջ�ɫ���бȽ�
bool DNPokerColorFirstCmp(const DNOnePoker& one, const DNOnePoker& other);


//��ţ�˿�������
struct DNCardType
{
	enum DN_TYPE
	{
		DN_TYPE_START = 0,
		MEI_NIU		  = 0,	//ûţ
		NIU_DING	  = 1,	//ţ��
		NIU_ER		  = 2,	//ţ��
		NIU_SAN		  = 3,	//ţ��
		NIU_SI		  = 4,	//ţ��
		NIU_WU		  = 5,	//ţ��
		NIU_LIU		  = 6,	//ţ��
		NIU_QI		  = 7,	//ţ��
		NIU_BA		  = 8,	//ţ��
		NIU_JIU		  = 9,	//ţ��
		NIU_NIU		  = 10,	//ţţ

		SI_ZHA		  = 11,	//��ը
		WU_HUA_NIU	  = 12,	//�廨ţ
		WU_XIAO_NIU	  = 13,	//��Сţ

		DN_TYPE_COUNT = 14,	// place holder.
	};
};


//һ�鶷ţ��(5�������)
struct DNGroupPoker
{
	DNOnePoker		poker[5];				//5����
	int64_t			nPokerBit;				// 5�������ڵ�����λ
	int32_t			dn_type;				//5���Ƶ�����
	DNOnePoker		*max_poker;				//5���Ƶ������

	bool			bCalc;					//�����Ƿ�����

	int32_t			niu_poker_index[5];		//��������͵�����/����������ţ/��ը/�廨ţ/С��ţ
	int32_t			other_poker_index[2];	//ʣ�������Ƶ�����


	void Confuse();

	DNGroupPoker()
	{
		reset();
	}

	void reset()
	{
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			poker[i].reset();
			niu_poker_index[i] = -1;
		}

		for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
		{
			other_poker_index[i] = -1;
		}
		bCalc						= false;
		max_poker					= nullptr;
		dn_type						= 0;
	}

	void calcByNoNiu()
	{
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			niu_poker_index[i] = i;
		}
		for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
		{
			other_poker_index[i] = -1;
		}
		dn_type = DNCardType::MEI_NIU;
	}

	void setPoker(DNOnePoker newPokers[5])
	{
		memcpy(poker, newPokers, sizeof(DNOnePoker)* 5);
	}

	void setNiuPokerIndex(int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
	{
		memcpy(niu_poker_index, niuPokerIndex, sizeof(int32_t)* 5);
		memcpy(other_poker_index, otherPokerIndex, sizeof(int32_t)* 2);
	}

	void setMaxPoker(DNOnePoker * p)
	{
		max_poker = p;
	}

	inline bool	IsSameGroupPoker(DNGroupPoker &pGroupPoker)
	{
		return (0 == (nPokerBit ^ pGroupPoker.nPokerBit));
	}
};

//һ���˿���
class DNOneDeckPoker
{

public:
	DNOneDeckPoker();
	~DNOneDeckPoker();

	// ������
	static const int  DN_NOBRAND =  -1;

	//һ���˿��Ƶ�����
	inline int32_t getPokerNum()
	{
		return poker_num_;
	}

	//һ���˿��Ƶ���Сֵ
	inline int8_t getMinValue()
	{
		return poker_min_value_;
	}

	//һ���˿��Ƶ����ֵ
	inline int8_t getMaxValue()
	{
		return poker_max_value_;
	}

	//��ʼ��һ���˿�:����һ����˳��
	void initPoker(int8_t minPoker, int8_t maxPoker);
	void	InitPoker();
	void initPokerExceptGroup(int8_t minPoker, int8_t maxPoker, const DNGroupPoker& group_poker);
	//ϴ��
	void shuffle(int16_t iTimes	=	1);

	void GenerateOrderedCardGroup();
	bool DealPokerByOrderedCardGroup(DNOnePoker* poker, uint32_t card_type_index);
	const DNGroupPoker* GetOrderCardGroupByIndex(uint32_t card_type_index);
	bool DealPoker(DNOnePoker* poker, bool is_kill_all, uint32_t card_type_index);
	uint32_t GetRepeatTime(int32_t dn_type);

	//����
	bool dealPoker(DNOnePoker* poker);

	void	RandomDealPoker(DNGroupPoker &tagPoker);

	//��ָ���������ҵ�������
	static DNOnePoker *findMaxPoker(DNOnePoker poker[5]);
	//�Ƚ����ŵ���:a > b : 1; a  == b :0 ;  a < b  : -1
	static bool			compareSinglePoker(DNOnePoker* poker_a, DNOnePoker* poker_b);
	//�Ƚ����鶷ţ��a > b : 1; a  == b :0 ;  a < b  : -1
	static bool			compareGroupPoker(DNGroupPoker & group_poker_a, bool byNiuType_a, DNGroupPoker & group_poker_b, bool byNiuType_b);
	//�Ƚ�����ţ��
	static int32_t		compareNiutype(int32_t a_niu_type, int32_t b_niu_type);

	//����ţ�͵õ��ַ���
	//static std::string	getDNTypeStringByType(const int32_t& tType);

	//����ţ��, int32_t pokerIndex[5],ţ�Ͷ�Ӧ����
	static int32_t		hasCow(DNOnePoker poker[5], int32_t pokerIndex[5]);
	static int32_t		findCow(DNOnePoker poker[5], int32_t niuPokerIndex[5], int32_t otherPokerIndex[2]);
	static int32_t		changeDNPokerJQK(const int8_t& pokerValue);
	static void			hasCow(DNGroupPoker &groupPoker, int32_t pokerIndex[5], int32_t otherPokerIndex[2]);
	static void			findCow(DNGroupPoker &groupPoker, int32_t niuPokerIndex[5], int32_t otherPokerIndex[2]);

	//�������Ƶ���Ϣ����֤ţ���Ƿ�����ȷ��
	static bool         CheckCowWithPokerInfo(int32_t niu_type, int32_t poker_index[kPokerNumOfGroup], DNGroupPoker& group_poker);

	//����ţ�� & ţ���ƶ�Ӧ������ & �����Ƶ�����  & �����
	static void			calcCowData(DNGroupPoker &groupPoker, bool bByPokerType = true);

	bool				dealPokers(DNOnePoker * poker[], int32_t playerNumber);

private:
	int32_t			    DNPokerCompare(DNOnePoker& pokerA, DNOnePoker& pokerB);

public:
	int32_t			poker_num_;
	int32_t			deal_position_;

	DNOnePoker		dn_Poker_[MAX_DNPOKER_COUNT];

	int8_t			poker_min_value_;
	int8_t			poker_max_value_;

	std::map<uint32_t, DNGroupPoker>  group_poker_with_ordered_;
};

class DNFiveGroupPoker
{
public:
	DNFiveGroupPoker();
	~DNFiveGroupPoker();

	inline void			AddGroupPoker(DNGroupPoker &pGroupPoker)
	{
		m_vectGroupPoker.push_back(pGroupPoker);
	}

	inline std::vector<DNGroupPoker>	&GetGroupPokerVector()
	{
		return m_vectGroupPoker;
	}

	bool				HaveSameGroup(DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5);

	inline bool			IsFull()
	{
		return m_vectGroupPoker.size() >= BOMBED_BULL_GROUP_COUNT;
	}
private:
	std::vector<DNGroupPoker>	m_vectGroupPoker;
};

class DNPokerSet
{
public:
	DNPokerSet(uint32_t card_type);
	DNPokerSet();
	~DNPokerSet();
public:
	void AddGroupPoker(DNGroupPoker& Poker);
	uint32_t  GetCardType();
	uint32_t  GetPokerGroupCount();
public:
	const DNGroupPoker* GetRandomGroupPoker();
	inline std::vector<DNGroupPoker>	&GetGroupVector()
	{
		return GroupPokers_;
	}
private:
	uint32_t card_type_;
	std::set<int64_t>			m_setGroupBit;
	std::vector<DNGroupPoker> GroupPokers_;
};

class CWorkThread
{
public:
	CWorkThread();
	~CWorkThread();

	bool							Initialize();
	void							AddGroupPoker(DNGroupPoker &group_poker);

	inline bool						IsCalcOver() const
	{
		return m_bCalcOver;
	}

	inline void						Stop()
	{
		m_bRunning	= false;
	}

	inline uint32_t					GetKey() const
	{
		return m_uKeyType;
	}

	inline std::vector<DNFiveGroupPoker*>	&GetResult()
	{
		return m_vectResult;
	}

	inline void						ClearResult()
	{
		m_vectResult.clear();
	}

	inline void						SetCalcData(const uint32_t uKeyType, const int nType1, const int nType2, const int nType3, const int nType4, const int nType5)
	{
		m_vectResult.clear();
		m_uKeyType	= uKeyType;
		m_nType1	= nType1;
		m_nType2	= nType2;
		m_nType3	= nType3;
		m_nType4	= nType4;
		m_nType5	= nType5;

		m_bCalcOver	= false;
	}

	inline void						TurnToSpecialFunc()
	{
		m_pfnCalculateFunc	= &CWorkThread::GenerateGroupPoker2;
	}
private:
	void							ThreadFunc();
	void							GenerateGroupPoker();
	void							GenerateGroupPoker2();
	bool							AddGroupPoker(DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5);

	inline void						yield(const unsigned int uTime)
	{
#ifdef WIN32
		Sleep(uTime);
#else
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= uTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
#endif
	}
private:
	typedef void (CWorkThread::*pfnFunc)();
	pfnFunc							m_pfnCalculateFunc;

	std::map<uint32_t, DNPokerSet>	m_mapPokerSet;

	uint32_t						m_uKeyType;
	int								m_nType1;
	int								m_nType2;
	int								m_nType3;
	int								m_nType4;
	int								m_nType5;

	std::vector<DNFiveGroupPoker*>	m_vectResult;

	std::vector<DNGroupPoker>		m_vectGroupPoker1;
	std::vector<DNGroupPoker>		m_vectGroupPoker2;
	std::vector<DNGroupPoker>		m_vectGroupPoker3;
	std::vector<DNGroupPoker>		m_vectGroupPoker4;
	std::vector<DNGroupPoker>		m_vectGroupPoker5;

	bool							m_bInited;
	bool							m_bRunning;
	bool							m_bCalcOver;
};

class DNPokerGen
{
private:
	DNPokerGen();
	~DNPokerGen();
public:
	static DNPokerGen* GetInstance();
public:
	bool Init();
	bool InitWorkThread();
	void DumpGroupPokerFile();
	void InitBombedBullPokerGroup();
	void Combination25PokerFrom52Poker(DNOnePoker(&tag52Poker)[MAX_DNPOKER_COUNT], const int nStart, int nCount, DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT]);
	void CombinationPokerFromList(DNOnePoker *pSrcPoker[BOMBED_BULL_POKER_COUNT], int nSrcPokerCount, int nCount, DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT], int nPokerIndex);
public:
	const DNGroupPoker* GetRandomGroupPoker(bool big_or_little);//true -> big rate ; fals -> little rate
	const DNGroupPoker* GetAssignGroupPoker(uint32_t poker_type);
	const DNGroupPoker* GetRandomGroupPoker(uint32_t poker_type_begin, uint32_t poker_type_end);
	const DNGroupPoker *GetRandomGroupBombedBullPoker();
	const DNGroupPoker *GetRandomBombedBullGroupPoker();

	const DNGroupPoker* GetRandomGroupPokerByRate(uint32_t rand_num, bool big_or_little=true);//true -> big rate ; fals -> little rate

private:
	void AddGroupPoker(DNGroupPoker& group_poker);
	void AddThreadGroupPoker(DNGroupPoker& group_poker);
	void GenerateNormalPokerGroup();
	void GenerateSpecialPokerGroup(const int nPokerType);
	void TurnToSpecial();
	void WaitAllThread();
	void DispatchTask(uint32_t uKey, const int nType1, const int nType2, const int nType3, const int nType4, const int nType5);
	void GenerateAllType();
	void GenerateOneGroupType(uint32_t uKey, std::vector<DNGroupPoker> &vect1, std::vector<DNGroupPoker> &vect2, std::vector<DNGroupPoker> &vect3, std::vector<DNGroupPoker> &vect4, std::vector<DNGroupPoker> &vect5);
	bool AddGroupType(uint32_t uKey, DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5);
	void AddBombedbullGroupPoker(DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT]);
	void CalcBombedBullGroupCount(DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT]);

	void yield(const unsigned int ulTime)
	{
#ifdef WIN32
		Sleep(ulTime);
#else
		struct timeval sleeptime;
		sleeptime.tv_sec	= 0;
		sleeptime.tv_usec	= ulTime * 1000;
		select(0, 0, 0, 0, &sleeptime);
#endif
	}
private:
	CWorkThread						*m_pWorkThread;
	int								m_nThreadCount;
	uint32_t						m_uFindCount;

	std::map<uint32_t, DNPokerSet>	poker_maps_;
	std::map<uint32_t,uint64_t>		m_mapPokerGroupCount;
	std::map<uint32_t,std::vector<DNFiveGroupPoker*>>	m_mapPokerGroup;
	std::map<uint32_t, uint32_t>	m_mapTypeRate;

	uint32_t						m_uBombedBullRate;
	uint8_t							m_byPokerBit[MAX_DNPOKER_VALUE][DNPOKER_COLOR_COUNT];

};

#endif
