#pragma once

#include "Comm.h"
#include <map>
using namespace std;

enum class EN_MKT_TP { MKT_NOREADY = 0, MKT_OPEN = 1, MKT_END = 3, MKT_SUSPEND = 7 };

#define TIME_LEN	17// yyyymmddhhmmssmmm
struct TStkInfo
{
	char	acntTp;
	string	sArtcCd;
	string	sStkCd;
	double 	dTickValue;
	double 	dTickSize;
	int		nDotCnt;
	LONG	lLCAmt;
	EN_MKT_TP mktTp;
	double	dCurrPrc;
	char	zCurrPrc[32];
	char	zLastTime[TIME_LEN + 1];

	TStkInfo()
	{
		dTickValue = 0; dTickSize = 0; nDotCnt = 0; dCurrPrc = 0; 
		ZeroMemory(zCurrPrc, sizeof(zCurrPrc));
		ZeroMemory(zLastTime, sizeof(zLastTime));
	}
};



class CStkInfo
{
public:
	CStkInfo();
	~CStkInfo();

	VOID	AddStkInfo(TStkInfo* p);
	BOOL	GetArtcInfo(string sStkCd, double* pTickVal, double* pTickSize, int* pDotCnt, LONG* pLCAmt);
	BOOL	GetCurrPrc(string sStkCd, _Out_ double* pCurrPrc, _Out_ char* pzLastTime);
private:
	map<string, TStkInfo*>	m_mapStk;
};

