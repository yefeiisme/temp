#include "Temp.h"

void main()
{
	DNPokerGen::GetInstance()->Init();

	printf("All Poker Calculate Over\n");

	DNPokerGen::GetInstance()->DumpGroupPokerFile();

	printf("All Poker Dump Over\n");

	getchar();
}

static bool pokerValueIndexPairComparator(const std::pair<int32_t, int32_t> &a, const std::pair<int32_t, int32_t> &b)
{
	return a.first < b.first;
}

int32_t DNOneDeckPoker::changeDNPokerJQK(const int8_t& pokerValue)
{
	if (pokerValue>10)
	{
		return 10;
	}
	return pokerValue;
}

DNOnePoker & DNOnePoker::operator=(const DNOnePoker &d1)
{
	if (d1.poker_color == this->poker_color && d1.poker_value == this->poker_value)
	{
		return *this;
	}
	this->poker_color = d1.poker_color;
	this->poker_value = d1.poker_value;
	return *this;
}

bool DNOnePoker::operator==(const DNOnePoker & other) const
{
	return (isNullPoker() && other.isNullPoker()) || (poker_color == other.poker_color && poker_value == other.poker_value);
}

bool DNOnePoker::operator!=(const DNOnePoker & other) const
{
	return !(*this == other);
}

bool DNOnePoker::isNullPoker() const
{
	return (poker_value == 0 || poker_color == 0);
}

void DNOnePoker::reset()
{
	poker_value = 0;
	poker_color = 0;
}

DNOnePoker * DNOneDeckPoker::findMaxPoker(DNOnePoker poker[5])
{
	DNOnePoker * max_poker = &poker[0];
	for (int i = 1; i < 5; i++)
	{
		DNOnePoker * current_poker = &poker[i];

		if ((max_poker->poker_value < current_poker->poker_value) ||
			(max_poker->poker_value == current_poker->poker_value && max_poker->poker_color < poker[i].poker_color)
			)
		{
			max_poker = &poker[i];
		}
	}

	return max_poker;
}

//计算牛型/牛型牌对应的索引/其他索引
void    DNOneDeckPoker::calcCowData(DNGroupPoker &groupPoker, bool bByPokerType)
{
	if (!groupPoker.bCalc)
	{
		if (bByPokerType)
		{
			//按照牛型来计算牌
			groupPoker.dn_type = findCow(groupPoker.poker, groupPoker.niu_poker_index, groupPoker.other_poker_index);
			groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
			groupPoker.bCalc = true;
			return;
		}
		else
		{
			//按照单手牌来计算牌
			groupPoker.calcByNoNiu();
			groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
			groupPoker.bCalc = true;
			return;
		}
	}
}

int32_t DNOneDeckPoker::findCow(DNOnePoker poker[5], int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
{
    static const int32_t all_niu_poker_index[5] = { 0, 1, 2, 3, 4 };
    static const int32_t index[5]                = { -1, -1, -1, -1, -1 };
    int32_t     Niu_type    = DNCardType::MEI_NIU;

    int wuxiao_num = 0;
    int wuxiao_sum = 0;
    int wuhua_num = 0;

    int all_poker_sum = 0;

    //判断是否为五小牛、五花牛
    for (int i = 0; i < 5; ++i)
    {
        all_poker_sum += changeDNPokerJQK(poker[i].poker_value);

        if (poker[i].poker_value < 5)
        {
            ++wuxiao_num;
            wuxiao_sum += poker[i].poker_value;
        }
        else if (poker[i].poker_value > 10)
        {
            ++wuhua_num;
        }
    }

    if ( (wuxiao_num == 5)&&(wuxiao_sum <= 10))
    {
        // 判断是否为五小牛
        memcpy(niuPokerIndex, all_niu_poker_index, sizeof(int32_t)*5);
        return DNCardType::WU_XIAO_NIU;

    }
    else if (wuhua_num == 5)
    {
        //判断是否为五花牛
        memcpy(niuPokerIndex, all_niu_poker_index, sizeof(int32_t)*5);
        return DNCardType::WU_HUA_NIU;
    }

    //判断是否为四炸
    std::vector<std::pair<int32_t, int32_t> > sorted_pokers;
    for (int i=0;i<5;i++)
    {
        sorted_pokers.push_back(std::pair<int32_t, int32_t>(poker[i].poker_value, i));
    }

    std::sort(sorted_pokers.begin(), sorted_pokers.end(), pokerValueIndexPairComparator);

    int si_zha_count = 0;
    for (int i=1; i<5; i++)
    {
        std::pair<int32_t, int32_t> & cur  = sorted_pokers[i - 1];
        std::pair<int32_t, int32_t> & curPair = sorted_pokers[i];
        if (sorted_pokers[i-1].first == curPair.first)
        {
            niuPokerIndex[si_zha_count] = cur.second;
            si_zha_count++;
            if (si_zha_count == 3)
            {
                niuPokerIndex[si_zha_count] = curPair.second;
                return DNCardType::SI_ZHA;
            }
        }
        else
        {
            if (i >= 2)
            {
                // Fast quit.
                break;
            }
            si_zha_count = 0;
            otherPokerIndex[0] = cur.second;
        }
    }


    int nSum = 0;
    int i = 0, j = 0, k = 0;
    for(i = 0; i < 5; ++ i)
    {
        for(j = i + 1; j < 5; ++ j)
        {
            for(k = j + 1; k < 5; ++ k)
            {
                nSum = changeDNPokerJQK(poker[i].poker_value) + changeDNPokerJQK(poker[j].poker_value) + changeDNPokerJQK(poker[k].poker_value);

                if(nSum % 10 == 0)
                {
                    niuPokerIndex[0] = i;
                    niuPokerIndex[1] = j;
                    niuPokerIndex[2] = k;

                    int n = 0; 
                    int otherPokerIndex_index = 0;
                    for (int m = 0; m < 5; m++)
                    {
                        for (n = 0; n < 3; n++)
                        {
                            if (m == niuPokerIndex[n])
                            {
                                break; 
                            }
                        }
                        if (n >= 3)
                        {
                            otherPokerIndex[otherPokerIndex_index] = m; 
                            otherPokerIndex_index++; 
                            if (otherPokerIndex_index >= 2)
                            {
                                break; 
                            }
                        }
                    }
                    int32_t type = all_poker_sum % 10;
                    if (type == 0)
                    {
                        type = DNCardType::NIU_NIU;
                    }
                    return  type;
                }
            }
        }
    }
    memcpy(niuPokerIndex, index, sizeof(int32_t)*5);
    return Niu_type;
}

void DNOneDeckPoker::findCow(DNGroupPoker &groupPoker, int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
{
	int32_t n_index[5] ={ -1, -1, -1, -1, -1 };
	int32_t o_index[2] ={ -1, -1 };

	if (niuPokerIndex == NULL)
	{
		niuPokerIndex = n_index;
	}
	if (otherPokerIndex == NULL)
	{
		otherPokerIndex = o_index;
	}

	groupPoker.dn_type = findCow(groupPoker.poker, niuPokerIndex, otherPokerIndex);
	groupPoker.setNiuPokerIndex(niuPokerIndex, otherPokerIndex);
	groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
}

CWorkThread::CWorkThread()
{
	m_pfnCalculateFunc	= &CWorkThread::GenerateGroupPoker2;

	m_mapPokerSet.clear();

	m_uKeyType	= 0;
	m_vectResult.clear();

	m_vectGroupPoker1.clear();
	m_vectGroupPoker2.clear();
	m_vectGroupPoker3.clear();
	m_vectGroupPoker4.clear();
	m_vectGroupPoker5.clear();

	m_bInited	= false;
	m_bRunning	= true;
	m_bCalcOver	= true;
}

CWorkThread::~CWorkThread()
{
}

bool CWorkThread::Initialize()
{
	if (m_bInited)
		return false;

	std::thread	newThread(&CWorkThread::ThreadFunc, this);
	newThread.detach();

	return true;
}

void CWorkThread::AddGroupPoker(DNGroupPoker &tagGroupPoker)
{
	uint32_t	uDNType	= tagGroupPoker.dn_type;
	auto		Iter	= m_mapPokerSet.find(uDNType);

	if (Iter != m_mapPokerSet.end())
	{
		DNPokerSet &poker_set = Iter->second;
		poker_set.AddGroupPoker(tagGroupPoker);
	}
	else
	{
		DNPokerSet poker_set(uDNType);
		poker_set.AddGroupPoker(tagGroupPoker);
		m_mapPokerSet[uDNType] = poker_set;
	}
}

void CWorkThread::ThreadFunc()
{
	while (m_bRunning)
	{
		if (!m_bCalcOver)
		{
			(this->*m_pfnCalculateFunc)();
			m_bCalcOver	= true;
		}

		yield(1);
	};
}

void CWorkThread::GenerateGroupPoker()
{
	//int	nCount	= 0;
	//for (int nIndex1 = 0; nIndex1 < m_vectGroupPoker1.size(); ++nIndex1)
	//{
	//	int64_t	nPokerBit1	= m_vectGroupPoker1[nIndex1].nPokerBit;

	//	for (int nIndex2 = 0; nIndex2 < m_vectGroupPoker2.size(); ++nIndex2)
	//	{
	//		if (nPokerBit1 & m_vectGroupPoker2[nIndex2].nPokerBit)
	//		{
	//			++m_uFindCount;
	//			continue;
	//		}

	//		int64_t	nPokerBit2	= nPokerBit1 | m_vectGroupPoker2[nIndex2].nPokerBit;

	//		for (int nIndex3 = 0; nIndex3 < m_vectGroupPoker3.size(); ++nIndex3)
	//		{
	//			if (nPokerBit2 & m_vectGroupPoker3[nIndex3].nPokerBit)
	//			{
	//				++m_uFindCount;
	//				continue;
	//			}

	//			int64_t	nPokerBit3	= nPokerBit2 | m_vectGroupPoker3[nIndex3].nPokerBit;

	//			for (int nIndex4 = 0; nIndex4 < m_vectGroupPoker4.size(); ++nIndex4)
	//			{
	//				if (nPokerBit3 & m_vectGroupPoker4[nIndex4].nPokerBit)
	//				{
	//					++m_uFindCount;
	//					continue;
	//				}

	//				int64_t	nPokerBit4	= nPokerBit3 | m_vectGroupPoker4[nIndex4].nPokerBit;

	//				for (int nIndex5 = 0; nIndex5 < m_vectGroupPoker5.size(); ++nIndex5)
	//				{
	//					if (nPokerBit4 & m_vectGroupPoker5[nIndex5].nPokerBit)
	//					{
	//						++m_uFindCount;
	//						continue;
	//					}

	//					if (AddGroupPoker(m_vectGroupPoker1[nIndex1], m_vectGroupPoker2[nIndex2], m_vectGroupPoker3[nIndex3], m_vectGroupPoker4[nIndex4], m_vectGroupPoker5[nIndex5]))
	//					{
	//						++nIndex1;
	//						if (nIndex1 >= m_vectGroupPoker1.size())
	//							return;

	//						++nIndex2;
	//						if (nIndex2 >= m_vectGroupPoker2.size())
	//							return;

	//						++nIndex3;
	//						if (nIndex3 >= m_vectGroupPoker3.size())
	//							return;

	//						++nIndex4;
	//						if (nIndex4 >= m_vectGroupPoker4.size())
	//							return;

	//						++nCount;
	//						if (nCount == 1)
	//							return;
	//					}
	//					else
	//					{
	//						++m_uFindCount;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	auto	Iter1		= m_mapPokerSet.find(m_nType1);
	auto	dnVector1	= Iter1->second.GetGroupVector();

	auto	Iter2		= m_mapPokerSet.find(m_nType2);
	auto	dnVector2	= Iter2->second.GetGroupVector();

	auto	Iter3		= m_mapPokerSet.find(m_nType3);
	auto	dnVector3	= Iter3->second.GetGroupVector();

	auto	Iter4		= m_mapPokerSet.find(m_nType4);
	auto	dnVector4	= Iter4->second.GetGroupVector();

	auto	Iter5		= m_mapPokerSet.find(m_nType5);
	auto	dnVector5	= Iter5->second.GetGroupVector();

	for (int nIndex1 = 0; nIndex1 < dnVector1.size(); ++nIndex1)
	{
		int64_t	nPokerBit1	= dnVector1[nIndex1].nPokerBit;

		for (int nIndex2 = 0; nIndex2 < dnVector2.size(); ++nIndex2)
		{
			if (nPokerBit1 & dnVector2[nIndex2].nPokerBit)
				continue;

			int64_t	nPokerBit2	= nPokerBit1 | dnVector2[nIndex2].nPokerBit;

			for (int nIndex3 = 0; nIndex3 < dnVector3.size(); ++nIndex3)
			{
				if (nPokerBit2 & dnVector3[nIndex3].nPokerBit)
					continue;

				int64_t	nPokerBit3	= nPokerBit2 | dnVector3[nIndex3].nPokerBit;

				for (int nIndex4 = 0; nIndex4 < dnVector4.size(); ++nIndex4)
				{
					if (nPokerBit3 & dnVector4[nIndex4].nPokerBit)
						continue;

					int64_t	nPokerBit4	= nPokerBit3 | dnVector4[nIndex4].nPokerBit;

					for (int nIndex5 = 0; nIndex5 < dnVector5.size(); ++nIndex5)
					{
						if (nPokerBit4 & dnVector5[nIndex5].nPokerBit)
							continue;

						if (AddGroupPoker(dnVector1[nIndex1], dnVector2[nIndex2], dnVector3[nIndex3], dnVector4[nIndex4], dnVector5[nIndex5]))
							return;
					}
				}
			}
		}
	}
}

void CWorkThread::GenerateGroupPoker2()
{
	//while (1)
	//{
	//	int		nIndex1		= rand() % m_vectGroupPoker1.size();
	//	int64_t	nPokerBit1	= m_vectGroupPoker1[nIndex1].nPokerBit;

	//	int	nIndex2	= rand() % m_vectGroupPoker2.size();
	//	if (nPokerBit1 & m_vectGroupPoker2[nIndex2].nPokerBit)
	//		continue;

	//	int64_t	nPokerBit2	= nPokerBit1 | m_vectGroupPoker2[nIndex2].nPokerBit;

	//	int	nIndex3	= rand() % m_vectGroupPoker3.size();
	//	if (nPokerBit2 & m_vectGroupPoker3[nIndex3].nPokerBit)
	//		continue;

	//	int64_t	nPokerBit3	= nPokerBit2 | m_vectGroupPoker3[nIndex3].nPokerBit;

	//	int	nIndex4	= rand() % m_vectGroupPoker4.size();
	//	if (nPokerBit3 & m_vectGroupPoker4[nIndex4].nPokerBit)
	//		continue;

	//	int64_t	nPokerBit4	= nPokerBit3 | m_vectGroupPoker4[nIndex4].nPokerBit;

	//	int	nIndex5	= rand() % m_vectGroupPoker5.size();
	//	if (nPokerBit4 & m_vectGroupPoker5[nIndex5].nPokerBit)
	//		continue;

	//	if (AddGroupPoker(m_vectGroupPoker1[nIndex1], m_vectGroupPoker2[nIndex2], m_vectGroupPoker3[nIndex3], m_vectGroupPoker4[nIndex4], m_vectGroupPoker5[nIndex5]))
	//		break;
	//}

	auto	Iter1		= m_mapPokerSet.find(m_nType1);
	auto	dnVector1	= Iter1->second.GetGroupVector();

	auto	Iter2		= m_mapPokerSet.find(m_nType2);
	auto	dnVector2	= Iter2->second.GetGroupVector();

	auto	Iter3		= m_mapPokerSet.find(m_nType3);
	auto	dnVector3	= Iter3->second.GetGroupVector();

	auto	Iter4		= m_mapPokerSet.find(m_nType4);
	auto	dnVector4	= Iter4->second.GetGroupVector();

	auto	Iter5		= m_mapPokerSet.find(m_nType5);
	auto	dnVector5	= Iter5->second.GetGroupVector();

	while (1)
	{
		int		nIndex1		= rand() % dnVector1.size();
		int64_t	nPokerBit1	= dnVector1[nIndex1].nPokerBit;

		int	nIndex2	= rand() % dnVector2.size();
		if (nPokerBit1 & dnVector2[nIndex2].nPokerBit)
			continue;

		int64_t	nPokerBit2	= nPokerBit1 | dnVector2[nIndex2].nPokerBit;

		int	nIndex3	= rand() % dnVector3.size();
		if (nPokerBit2 & dnVector3[nIndex3].nPokerBit)
			continue;

		int64_t	nPokerBit3	= nPokerBit2 | dnVector3[nIndex3].nPokerBit;

		int	nIndex4	= rand() % dnVector4.size();
		if (nPokerBit3 & dnVector4[nIndex4].nPokerBit)
			continue;

		int64_t	nPokerBit4	= nPokerBit3 | dnVector4[nIndex4].nPokerBit;

		int	nIndex5	= rand() % dnVector5.size();
		if (nPokerBit4 & dnVector5[nIndex5].nPokerBit)
			continue;

		if (AddGroupPoker(dnVector1[nIndex1], dnVector2[nIndex2], dnVector3[nIndex3], dnVector4[nIndex4], dnVector5[nIndex5]))
			break;
	}
}

bool CWorkThread::AddGroupPoker(DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5)
{
	for (std::vector<DNFiveGroupPoker*>::iterator Iter = m_vectResult.begin(); Iter != m_vectResult.end(); ++Iter)
	{
		if ((*Iter)->HaveSameGroup(GroupPoker1, GroupPoker2, GroupPoker3, GroupPoker4, GroupPoker5))
			return false;
	}

	DNFiveGroupPoker	*pNewFiveGroupPoker	= new DNFiveGroupPoker;
	if (nullptr == pNewFiveGroupPoker)
		return false;

	pNewFiveGroupPoker->AddGroupPoker(GroupPoker1);
	pNewFiveGroupPoker->AddGroupPoker(GroupPoker2);
	pNewFiveGroupPoker->AddGroupPoker(GroupPoker3);
	pNewFiveGroupPoker->AddGroupPoker(GroupPoker4);
	pNewFiveGroupPoker->AddGroupPoker(GroupPoker5);

	m_vectResult.push_back(pNewFiveGroupPoker);

	return true;
}

DNPokerGen* DNPokerGen::GetInstance()
{
	static DNPokerGen obj;
	return &obj;
}

DNPokerGen::DNPokerGen()
{
	m_pWorkThread	= nullptr;
	m_nThreadCount	= 0;
	m_uFindCount	= 0;

	m_mapPokerGroup.clear();
	m_mapTypeRate.clear();
}

DNPokerGen::~DNPokerGen()
{
	delete[] m_pWorkThread;

	m_mapTypeRate.clear();
}

void DNGroupPoker::Confuse()
{

	for (int i = 0; i < 5;)
	{
		poker[i].poker_color = rand() % 4 + 1;
		bool flg_if_same = false;
		for (int j =0; j < i; j++)
		{
			if (poker[i].poker_color == poker[j].poker_color && poker[i].poker_value == poker[j].poker_value)
			{
				flg_if_same= true;
				break;
			}
		}
		if (!flg_if_same)
		{
			i++;
		}

	}
	setMaxPoker(DNOneDeckPoker::findMaxPoker(poker));
}

bool DNPokerGen::Init()
{
	InitWorkThread();

	srand(time(NULL));

	int	nBit	= 0;
	for (int nPokerValue = 0; nPokerValue < MAX_DNPOKER_VALUE; ++nPokerValue)
	{
		for (int nPokerColor = 0; nPokerColor < DNPokerColorType::DN_POKER_COLOR_SPADES; ++nPokerColor)
		{
			m_byPokerBit[nPokerValue][nPokerColor]	= nBit++;
		}
	}

	for (char i = MIN_DNPOKER_VALUE; i <= MAX_DNPOKER_VALUE; i++)
	{
		for (char j = MIN_DNPOKER_VALUE; j <= MAX_DNPOKER_VALUE; j++)
		{
			for (char m = MIN_DNPOKER_VALUE; m <= MAX_DNPOKER_VALUE; m++)
			{
				for (char n = MIN_DNPOKER_VALUE; n <= MAX_DNPOKER_VALUE; n++)
				{
					for (char o = MIN_DNPOKER_VALUE; o <= MAX_DNPOKER_VALUE; o++)
					{
						DNGroupPoker group_poker;

						group_poker.poker[0].poker_value = i;
						group_poker.poker[0].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[1].poker_value = j;
						group_poker.poker[1].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[2].poker_value = m;
						group_poker.poker[2].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[3].poker_value = n;
						group_poker.poker[3].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[4].poker_value = o;
						group_poker.poker[4].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						if (group_poker.poker[0].poker_value == group_poker.poker[1].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[2].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[3].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[4].poker_value)
						{
							continue;
						}

						DNOneDeckPoker::calcCowData(group_poker, true);

						group_poker.Confuse();

						group_poker.poker[0].byPokerValue	= group_poker.poker[0].poker_value << 4 | group_poker.poker[0].poker_color;
						group_poker.poker[1].byPokerValue	= group_poker.poker[1].poker_value << 4 | group_poker.poker[1].poker_color;
						group_poker.poker[2].byPokerValue	= group_poker.poker[2].poker_value << 4 | group_poker.poker[2].poker_color;
						group_poker.poker[3].byPokerValue	= group_poker.poker[3].poker_value << 4 | group_poker.poker[3].poker_color;
						group_poker.poker[4].byPokerValue	= group_poker.poker[4].poker_value << 4 | group_poker.poker[4].poker_color;

						group_poker.nPokerBit	= 1 << m_byPokerBit[group_poker.poker[0].poker_value-1][group_poker.poker[0].poker_color-1]
												| 1 << m_byPokerBit[group_poker.poker[1].poker_value-1][group_poker.poker[1].poker_color-1]
												| 1 << m_byPokerBit[group_poker.poker[2].poker_value-1][group_poker.poker[2].poker_color-1]
												| 1 << m_byPokerBit[group_poker.poker[3].poker_value-1][group_poker.poker[3].poker_color-1]
												| 1 << m_byPokerBit[group_poker.poker[4].poker_value-1][group_poker.poker[4].poker_color-1];

						//AddGroupPoker(group_poker);
						AddThreadGroupPoker(group_poker);
					}
				}
			}
		}
	}

	GenerateNormalPokerGroup();

	WaitAllThread();

	//TurnToSpecial();

	GenerateSpecialPokerGroup(DNCardType::SI_ZHA);
	GenerateSpecialPokerGroup(DNCardType::WU_HUA_NIU);
	GenerateSpecialPokerGroup(DNCardType::WU_XIAO_NIU);

	WaitAllThread();

	return true;
}

bool DNPokerGen::InitWorkThread()
{
	m_nThreadCount	= 8;
	m_pWorkThread	= new CWorkThread[m_nThreadCount];

	if (nullptr == m_pWorkThread)
		return false;

	for (int nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
	{
		if (!m_pWorkThread[nIndex].Initialize())
			return false;
	}

	return true;
}

void DNPokerGen::DumpGroupPokerFile()
{
	FILE	*pFile	= fopen("bombed_bull_poker_group.txt", "w");
	if (nullptr == pFile)
		return;

	for (std::map<uint32_t, std::vector<DNFiveGroupPoker*>>::iterator Iter = m_mapPokerGroup.begin(); Iter != m_mapPokerGroup.end(); ++Iter)
	{
		uint32_t						uKeyType		= Iter->first;
		std::vector<DNFiveGroupPoker*>	&vectFiveGroup	= Iter->second;

		for (std::vector<DNFiveGroupPoker*>::iterator Iter_Five = vectFiveGroup.begin(); Iter_Five != vectFiveGroup.end(); ++Iter_Five)
		{
			fprintf(pFile, "%u", uKeyType);

			std::vector<DNGroupPoker>	&vectGroupPoker	= (*Iter_Five)->GetGroupPokerVector();

			for (std::vector<DNGroupPoker>::iterator Iter_Group = vectGroupPoker.begin(); Iter_Group != vectGroupPoker.end(); ++Iter_Group)
			{
				DNGroupPoker	&pGroupPoker	= (*Iter_Group);

				for (int nIndex = 0; nIndex < BOMBED_BULL_GROUP_POKER_COUNT; ++nIndex)
				{
					fprintf(pFile, " %hhd %hhd", pGroupPoker.poker[nIndex].poker_value, pGroupPoker.poker[nIndex].poker_color);
				}
			}

			fprintf(pFile, "\n");
		}
	}

	fclose(pFile);
}

void DNPokerGen::InitBombedBullPokerGroup()
{
	DNOnePoker	tag52Poker[MAX_DNPOKER_COUNT];
	DNOnePoker	tag25Poker[BOMBED_BULL_POKER_COUNT];
	int			nPokerIndex	= 0;

	for (int nPokerColor = DNPokerColorType::DN_POKER_COLOR_DIAMONDS; nPokerColor <= DNPokerColorType::DN_POKER_COLOR_SPADES; ++nPokerColor)
	{
		for (int nPokerValue = MIN_DNPOKER_VALUE; nPokerValue <= MAX_DNPOKER_VALUE; ++nPokerValue)
		{
			tag52Poker[nPokerIndex].poker_value	= nPokerValue;
			tag52Poker[nPokerIndex].poker_color	= nPokerColor;

			++nPokerIndex;
		}
	}

	Combination25PokerFrom52Poker(tag52Poker, 0, BOMBED_BULL_POKER_COUNT, tag25Poker);
}

void DNPokerGen::Combination25PokerFrom52Poker(DNOnePoker(&tag52Poker)[MAX_DNPOKER_COUNT], const int nStart, int nCount, DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT])
{
	for (int nIndex = nStart; nIndex < MAX_DNPOKER_COUNT - nCount + 1; ++nIndex)
	{
		if (nCount > 1)
		{// 不是第25张牌，只填充nIndex在tagAllPoker的牌到Start所在tagOutPoker的位置
			tag25Poker[nStart].poker_value	= tag52Poker[nIndex].poker_value;
			tag25Poker[nStart].poker_color	= tag52Poker[nIndex].poker_color;

			Combination25PokerFrom52Poker(tag52Poker, nStart + 1, nCount - 1, tag25Poker);
		}
		else
		{// 是第25张牌，遍历当前到第52张牌，并填充到tag25Poker的最后一个元素中，组成完整的25张牌，并丢给CombinationBombedBullGroup进行牌型的组合、计算
			DNOnePoker tagOutPoker[BOMBED_BULL_POKER_COUNT];
			memset(&tagOutPoker, 0, sizeof(tagOutPoker));

			for (int nPokerIndex = nStart; nPokerIndex < MAX_DNPOKER_COUNT; ++nPokerIndex)
			{
				tag25Poker[nStart].poker_value	= tag52Poker[nPokerIndex].poker_value;
				tag25Poker[nStart].poker_color	= tag52Poker[nPokerIndex].poker_color;

				CalcBombedBullGroupCount(tag25Poker);
				//DNOnePoker *pLeftPoker[BOMBED_BULL_POKER_COUNT];
				//memset(pLeftPoker, 0, sizeof(pLeftPoker));

				//for (int nDestPokerIndex = 0; nDestPokerIndex < BOMBED_BULL_POKER_COUNT; ++nDestPokerIndex)
				//{
				//	pLeftPoker[nDestPokerIndex]	= &tag25Poker[nDestPokerIndex];
				//}

				//CombinationPokerFromList(pLeftPoker, BOMBED_BULL_POKER_COUNT, BOMBED_BULL_GROUP_POKER_COUNT, tagOutPoker, 0);
			}
		}
	}
}

void DNPokerGen::CombinationPokerFromList(DNOnePoker *pSrcPoker[BOMBED_BULL_POKER_COUNT], int nSrcPokerCount, int nCount, DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT], int nDestPokerIndex)
{
	if (BOMBED_BULL_GROUP_POKER_COUNT == nSrcPokerCount)
	{
		// 最后5张牌，直接填到tag25Poker中去
		for (int nIndex = 0; nIndex < nSrcPokerCount; ++nIndex)
		{
			tag25Poker[nIndex + nDestPokerIndex].poker_value	= pSrcPoker[nIndex]->poker_value;
			tag25Poker[nIndex + nDestPokerIndex].poker_color	= pSrcPoker[nIndex]->poker_color;
		}

		// 生成各个牌型，insert到指定的map中，Key为每5组牌的牌型组成的Value
		AddBombedbullGroupPoker(tag25Poker);

		return;
	}

	DNOnePoker *pLeftPoker[BOMBED_BULL_POKER_COUNT];
	memset(pLeftPoker, 0, sizeof(pLeftPoker));

	for (int nSrcPokerIndex = 0; nSrcPokerIndex < nSrcPokerCount - nCount + 1; ++nSrcPokerIndex)
	{
		tag25Poker[nDestPokerIndex].poker_value	= pSrcPoker[nSrcPokerIndex]->poker_value;
		tag25Poker[nDestPokerIndex].poker_color	= pSrcPoker[nSrcPokerIndex]->poker_color;

		int	nLeftPokerIndex	= 0;
		for (int nIndex = 0; nIndex < nSrcPokerCount; ++nIndex)
		{
			if (nIndex == nSrcPokerIndex)
				continue;

			pLeftPoker[nLeftPokerIndex]	= pSrcPoker[nIndex];

			++nLeftPokerIndex;
		}

		if (nCount > 1)
		{
			// 在找pPokerList中找5张牌的组合
			CombinationPokerFromList(pLeftPoker, nSrcPokerCount - 1, nCount - 1, tag25Poker, nDestPokerIndex + 1);
		}
		else
		{
			// 找到第5张牌了，组成一副手牌，把剩余的牌放到vectLeft中，继续递归找下一组
			CombinationPokerFromList(pLeftPoker, nSrcPokerCount - 1, BOMBED_BULL_GROUP_POKER_COUNT, tag25Poker, nDestPokerIndex + 1);
		}
	}
}

void DNPokerGen::AddBombedbullGroupPoker(DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT])
{
	int	nPokerCount	= 1;
	int	nGroupIndex	= 0;

	DNGroupPoker	*pGroupPoker	= new DNGroupPoker[BOMBED_BULL_GROUP_COUNT];
	if (nullptr == pGroupPoker)
	{
		return;
	}

	for (int nGroupIndex = 0; nGroupIndex < BOMBED_BULL_GROUP_COUNT; ++nGroupIndex)
	{
		pGroupPoker[nGroupIndex].setPoker(&tag25Poker[nGroupIndex*BOMBED_BULL_GROUP_POKER_COUNT]);

		DNOneDeckPoker::calcCowData(pGroupPoker[nGroupIndex], true);
	}

	uint32_t	uKey	= 0;

	for (int nPokerIndex = 0; nPokerIndex < BOMBED_BULL_GROUP_POKER_COUNT; ++nPokerIndex)
	{
		uKey	= uKey << 4;
		uKey |= (pGroupPoker[nPokerIndex].dn_type & 0x0000000f);
	}

	//std::map<uint32_t, std::vector<DNGroupPoker*>*>::iterator	Iter	= m_mapPokerGroup.find(uKey);

	//if (m_mapPokerGroup.end() == Iter)
	//{
	//	std::vector<DNGroupPoker*>	*pvectPokerGroup	= new std::vector<DNGroupPoker*>;
	//	if (nullptr == pvectPokerGroup)
	//		return;

	//	pvectPokerGroup->push_back(pGroupPoker);

	//	m_mapPokerGroup[uKey]	= pvectPokerGroup;
	//}
	//else
	//{
	//	Iter->second->push_back(pGroupPoker);
	//}
}

void DNPokerGen::CalcBombedBullGroupCount(DNOnePoker(&tag25Poker)[BOMBED_BULL_POKER_COUNT])
{
	int	nPokerCount	= 1;
	int	nGroupIndex	= 0;

	DNGroupPoker	pGroupPoker[BOMBED_BULL_GROUP_COUNT];
	if (nullptr == pGroupPoker)
	{
		return;
	}

	for (int nGroupIndex = 0; nGroupIndex < BOMBED_BULL_GROUP_COUNT; ++nGroupIndex)
	{
		pGroupPoker[nGroupIndex].setPoker(&tag25Poker[nGroupIndex*BOMBED_BULL_GROUP_POKER_COUNT]);

		DNOneDeckPoker::calcCowData(pGroupPoker[nGroupIndex], true);
	}

	uint32_t	uKey	= 0;

	for (int nPokerIndex = 0; nPokerIndex < BOMBED_BULL_GROUP_POKER_COUNT; ++nPokerIndex)
	{
		uKey	= uKey << 4;
		uKey |= (pGroupPoker[nPokerIndex].dn_type & 0x0000000f);
	}

	std::map<uint32_t,uint64_t>::iterator	Iter	= m_mapPokerGroupCount.find(uKey);

	if (m_mapPokerGroupCount.end() == Iter)
	{
		m_mapPokerGroupCount[uKey]	= 1;
	}
	else
	{
		++Iter->second;
	}
}

void DNPokerGen::AddGroupPoker(DNGroupPoker& group_poker)
{
	uint32_t dn_type = group_poker.dn_type;
	std::map<uint32_t, DNPokerSet>::iterator pItr = poker_maps_.find(dn_type);
	if (pItr != poker_maps_.end())
	{
		DNPokerSet& poker_set = pItr->second;
		poker_set.AddGroupPoker(group_poker);
	}
	else
	{
		DNPokerSet poker_set(dn_type);
		poker_set.AddGroupPoker(group_poker);
		poker_maps_[dn_type] = poker_set;
	}
}

void DNPokerGen::AddThreadGroupPoker(DNGroupPoker& group_poker)
{
	for (int nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
	{
		m_pWorkThread[nIndex].AddGroupPoker(group_poker);
	}
}

void DNPokerGen::GenerateNormalPokerGroup()
{
	//for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	for (int nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		for (int nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			for (int nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				for (int nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}
	for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	{
		for (int nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
		{
			for (int nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
			{
				for (int nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
				{
					for (int nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}
}

void DNPokerGen::GenerateSpecialPokerGroup(const int nPokerType)
{
	//int nType1	= 0;
	//int	nType2	= 0;
	//int	nType3	= 0;
	//int	nType4	= 0;
	//int	nType5	= 0;

	//nType1	= nPokerType;
	////for (int nType1 = 0; nType1 < DNCardType::WU_XIAO_NIU; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	nType2	= nPokerType;
	//	//for (nType2 = 0; nType2 < DNCardType::WU_XIAO_NIU; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		nType3	= nPokerType;
	//		//for (nType3 = 0; nType3 < DNCardType::WU_XIAO_NIU; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			nType4	= nPokerType;
	//			//for (nType4 = 0; nType4 < DNCardType::WU_XIAO_NIU; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	//{
	//	std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
	//	std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

	//	for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
	//	{
	//		std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
	//		std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

	//		for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
	//		{
	//			std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
	//			std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

	//			for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
	//			{
	//				std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
	//				std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

	//				nType5	= nPokerType;
	//				//for (nType5 = 0; nType5 < DNCardType::WU_XIAO_NIU; ++nType5)
	//				{
	//					std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
	//					std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

	//					uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

	//					DispatchTask(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
	//				}
	//			}
	//		}
	//	}
	//}
	int nType1	= 0;
	int	nType2	= 0;
	int	nType3	= 0;
	int	nType4	= 0;
	int	nType5	= 0;

	nType1	= nPokerType;
	//for (int nType1 = 0; nType1 < DNCardType::WU_XIAO_NIU; ++nType1)
	{
		for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
		{
			for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
			{
				for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
				{
					for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}

	for (nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	{
		nType2	= nPokerType;
		//for (nType2 = 0; nType2 < DNCardType::WU_XIAO_NIU; ++nType2)
		{
			for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
			{
				for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
				{
					for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}

	for (nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	{
		for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
		{
			nType3	= nPokerType;
			//for (nType3 = 0; nType3 < DNCardType::WU_XIAO_NIU; ++nType3)
			{
				for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
				{
					for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}

	for (nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	{
		for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
		{
			for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
			{
				nType4	= nPokerType;
				//for (nType4 = 0; nType4 < DNCardType::WU_XIAO_NIU; ++nType4)
				{
					for (nType5 = 0; nType5 < DNCardType::SI_ZHA; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}

	for (nType1 = 0; nType1 < DNCardType::SI_ZHA; ++nType1)
	{
		for (nType2 = 0; nType2 < DNCardType::SI_ZHA; ++nType2)
		{
			for (nType3 = 0; nType3 < DNCardType::SI_ZHA; ++nType3)
			{
				for (nType4 = 0; nType4 < DNCardType::SI_ZHA; ++nType4)
				{
					nType5	= nPokerType;
					//for (nType5 = 0; nType5 < DNCardType::WU_XIAO_NIU; ++nType5)
					{
						uint32_t	uKey	= nType1 << 16 | nType2 << 12 | nType3 << 8 | nType4 << 4 | nType5;

						DispatchTask(uKey, nType1, nType2, nType3, nType4, nType5);
					}
				}
			}
		}
	}
}

void DNPokerGen::TurnToSpecial()
{
	for (int nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
	{
		m_pWorkThread[nIndex].TurnToSpecialFunc();
	}
}

void DNPokerGen::WaitAllThread()
{
	while (1)
	{
		int	nThreadOverCount	= 0;

		for (int nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
		{
			if (m_pWorkThread[nIndex].IsCalcOver())
			{
				++nThreadOverCount;

				std::vector<DNFiveGroupPoker*>	&vectResult	= m_pWorkThread[nIndex].GetResult();
				if (!vectResult.empty())
				{
					++m_uFindCount;

					m_mapPokerGroup[m_pWorkThread[nIndex].GetKey()]	= vectResult;

					m_pWorkThread[nIndex].ClearResult();
				}
			}
		}

		if (nThreadOverCount == m_nThreadCount)
		{
			printf("All FindCount[%u]\n", m_uFindCount);
			return;
		}

		yield(1);
	};
}

void DNPokerGen::DispatchTask(uint32_t uKey, const int nType1, const int nType2, const int nType3, const int nType4, const int nType5)
{
	while (1)
	{
		for (int nIndex = 0; nIndex < m_nThreadCount; ++nIndex)
		{
			if (m_pWorkThread[nIndex].IsCalcOver())
			{
				std::vector<DNFiveGroupPoker*>	&vectResult	= m_pWorkThread[nIndex].GetResult();
				if (!vectResult.empty())
				{
					++m_uFindCount;

					m_mapPokerGroup[m_pWorkThread[nIndex].GetKey()]	= vectResult;

					//printf("Key[%u] FindCount[%u]\n", m_pWorkThread[nIndex].GetKey(), m_uFindCount);
				}

				m_pWorkThread[nIndex].SetCalcData(uKey, nType1, nType2, nType3, nType4, nType5);
				return;
			}
		}

		yield(1);
	};
}

void DNPokerGen::GenerateAllType()
{
	for (int nType1 = 0; nType1 < DNCardType::DN_TYPE_COUNT; ++nType1)
	{
		std::map<uint32_t, DNPokerSet>::iterator	Iter1	= poker_maps_.find(nType1);
		std::vector<DNGroupPoker>	dnVector1	= Iter1->second.GetGroupVector();

		for (int nType2 = 0; nType2 < DNCardType::DN_TYPE_COUNT; ++nType2)
		{
			std::map<uint32_t, DNPokerSet>::iterator	Iter2	= poker_maps_.find(nType2);
			std::vector<DNGroupPoker>	dnVector2	= Iter2->second.GetGroupVector();

			for (int nType3 = 0; nType3 < DNCardType::DN_TYPE_COUNT; ++nType3)
			{
				std::map<uint32_t, DNPokerSet>::iterator	Iter3	= poker_maps_.find(nType3);
				std::vector<DNGroupPoker>	dnVector3	= Iter3->second.GetGroupVector();

				for (int nType4 = 0; nType4 < DNCardType::DN_TYPE_COUNT; ++nType4)
				{
					std::map<uint32_t, DNPokerSet>::iterator	Iter4	= poker_maps_.find(nType4);
					std::vector<DNGroupPoker>	dnVector4	= Iter4->second.GetGroupVector();

					for (int nType5 = 0; nType5 < DNCardType::DN_TYPE_COUNT; ++nType5)
					{
						std::map<uint32_t, DNPokerSet>::iterator	Iter5	= poker_maps_.find(nType5);
						std::vector<DNGroupPoker>	dnVector5	= Iter5->second.GetGroupVector();

						uint32_t	uKey	= nType1 << 4 | nType2 << 8 | nType3 << 12 | nType4 << 16 | nType5 << 20;

						GenerateOneGroupType(uKey, dnVector1, dnVector2, dnVector3, dnVector4, dnVector5);
					}
				}
			}
		}
	}
}

void DNPokerGen::GenerateOneGroupType(uint32_t uKey, std::vector<DNGroupPoker> &vect1, std::vector<DNGroupPoker> &vect2, std::vector<DNGroupPoker> &vect3, std::vector<DNGroupPoker> &vect4, std::vector<DNGroupPoker> &vect5)
{
	int	nCount	= 0;
	for (std::vector<DNGroupPoker>::iterator Iter1 = vect1.begin(); Iter1 != vect1.end(); ++Iter1)
	{
		int64_t	nPokerBit1	= (*Iter1).nPokerBit;

		for (std::vector<DNGroupPoker>::iterator Iter2 = vect2.begin(); Iter2 != vect2.end(); ++Iter2)
		{
			if (nPokerBit1 & (*Iter2).nPokerBit)
				continue;

			int64_t	nPokerBit2	= nPokerBit1 | (*Iter2).nPokerBit;

			for (std::vector<DNGroupPoker>::iterator Iter3 = vect3.begin(); Iter3 != vect3.end(); ++Iter3)
			{
				if (nPokerBit2 & (*Iter3).nPokerBit)
					continue;

				int64_t	nPokerBit3	= nPokerBit2 | (*Iter3).nPokerBit;

				for (std::vector<DNGroupPoker>::iterator Iter4 = vect4.begin(); Iter4 != vect4.end(); ++Iter4)
				{
					if (nPokerBit3 & (*Iter4).nPokerBit)
						continue;

					int64_t	nPokerBit4	= nPokerBit3 | (*Iter4).nPokerBit;

					for (std::vector<DNGroupPoker>::iterator Iter5 = vect5.begin(); Iter5 != vect5.end(); ++Iter5)
					{
						if (nPokerBit4 & (*Iter5).nPokerBit)
							continue;

						//int64_t	nPokerBit5	= nPokerBit4 | (*Iter5).nPokerBit;

						if (AddGroupType(uKey, *Iter1, *Iter2, *Iter3, *Iter4, *Iter5))
						{
							++Iter1;
							if (Iter1 == vect1.end())
								return;

							++Iter2;
							if (Iter2 == vect2.end())
								return;

							++Iter3;
							if (Iter3 == vect3.end())
								return;

							++Iter4;
							if (Iter4 == vect4.end())
								return;

							++nCount;
							if (nCount == 10)
								return;
						}
					}
				}
			}
		}
	}
}

bool DNPokerGen::AddGroupType(uint32_t uKey, DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5)
{
	std::map<uint32_t,std::vector<DNFiveGroupPoker*>>::iterator	Iter_All	= m_mapPokerGroup.find(uKey);
	if (m_mapPokerGroup.end() == Iter_All)
	{
		DNFiveGroupPoker	*pNewFiveGroupPoker	= new DNFiveGroupPoker;
		if (nullptr == pNewFiveGroupPoker)
			return false;

		pNewFiveGroupPoker->AddGroupPoker(GroupPoker1);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker2);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker3);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker4);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker5);

		std::vector<DNFiveGroupPoker*>	vectNew;
		vectNew.push_back(pNewFiveGroupPoker);

		m_mapPokerGroup[uKey]	= vectNew;
	}
	else
	{
		for (std::vector<DNFiveGroupPoker*>::iterator Iter_FiveGroup = Iter_All->second.begin(); Iter_FiveGroup != Iter_All->second.end(); ++Iter_FiveGroup)
		{
			if ((*Iter_FiveGroup)->HaveSameGroup(GroupPoker1, GroupPoker2, GroupPoker3, GroupPoker4, GroupPoker5))
				return false;
		}

		DNFiveGroupPoker	*pNewFiveGroupPoker	= new DNFiveGroupPoker;
		if (nullptr == pNewFiveGroupPoker)
			return false;

		pNewFiveGroupPoker->AddGroupPoker(GroupPoker1);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker2);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker3);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker4);
		pNewFiveGroupPoker->AddGroupPoker(GroupPoker5);

		Iter_All->second.push_back(pNewFiveGroupPoker);
	}

	return true;
}

DNFiveGroupPoker::DNFiveGroupPoker()
{
	m_vectGroupPoker.clear();
}

DNFiveGroupPoker::~DNFiveGroupPoker()
{
	m_vectGroupPoker.clear();
}

bool DNFiveGroupPoker::HaveSameGroup(DNGroupPoker &GroupPoker1, DNGroupPoker &GroupPoker2, DNGroupPoker &GroupPoker3, DNGroupPoker &GroupPoker4, DNGroupPoker &GroupPoker5)
{
	if (m_vectGroupPoker[0].IsSameGroupPoker(GroupPoker1) &&
		m_vectGroupPoker[1].IsSameGroupPoker(GroupPoker2) &&
		m_vectGroupPoker[2].IsSameGroupPoker(GroupPoker3) &&
		m_vectGroupPoker[3].IsSameGroupPoker(GroupPoker4) &&
		m_vectGroupPoker[4].IsSameGroupPoker(GroupPoker5)
		)
		return true;

	return false;
}

DNPokerSet::DNPokerSet(uint32_t card_type)
{
	card_type_ = card_type;
	m_setGroupBit.clear();
	GroupPokers_.clear();
}

DNPokerSet::DNPokerSet()
{
	card_type_ = 0;
	m_setGroupBit.clear();
	GroupPokers_.clear();
}

DNPokerSet::~DNPokerSet()
{
	card_type_ = 0;
	m_setGroupBit.clear();
	GroupPokers_.clear();
}

void DNPokerSet::AddGroupPoker(DNGroupPoker& Poker)
{
	if (m_setGroupBit.end() != m_setGroupBit.find(Poker.nPokerBit))
		return;

	GroupPokers_.push_back(Poker);
	m_setGroupBit.insert(Poker.nPokerBit);
}


uint32_t DNPokerSet::GetCardType()
{
	return card_type_;
}

uint32_t DNPokerSet::GetPokerGroupCount()
{
	return GroupPokers_.size();
}

const DNGroupPoker* DNPokerSet::GetRandomGroupPoker()
{
	uint32_t num = GroupPokers_.size();
	if (num == 0)
	{
		return nullptr;
	}

	uint32_t index = num - 1 - rand() % num;
	DNGroupPoker* group_poker = &GroupPokers_[index];
	group_poker->Confuse();
	return group_poker;
}
