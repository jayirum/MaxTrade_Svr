#include "CStkInfo.h"


CStkInfo::CStkInfo()
{

}

CStkInfo::~CStkInfo()
{

}


VOID CStkInfo::AddStkInfo(TStkInfo* p)
{
	string sStk = p->sStkCd;

	m_mapStk[sStk] = p;
}


BOOL CStkInfo::GetArtcInfo(string sStkCd, double* pTickVal, double* pTickSize, int* pDotCnt, LONG* pLCAmt)
{
	auto find = m_mapStk.find(sStkCd);
	if (find == m_mapStk.end())
		return FALSE;

	*pTickVal = (*find).second->dTickValue;
	*pTickSize = (*find).second->dTickSize;
	*pDotCnt = (*find).second->nDotCnt;
	*pLCAmt = (*find).second->lLCAmt;
	return TRUE;
}

BOOL	CStkInfo::GetCurrPrc(string sStkCd, _Out_ double* pCurrPrc, _Out_ char* pzLastTime)
{
	BOOL ret = FALSE;
	*pCurrPrc = 0;
	*pzLastTime = 0;
	auto find = m_mapStk.find(sStkCd);
	if (find != m_mapStk.end())
	{
		*pCurrPrc = (*find).second->dCurrPrc;
		strcpy(pzLastTime, (*find).second->zLastTime);
		ret = TRUE;
	}

	return TRUE;
}