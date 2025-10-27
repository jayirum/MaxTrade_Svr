// QueryPerformance.cpp: implementation of the CQueryPerformance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XingAPI_Sample_eSearch.h"
#include "QueryPerformance.h"
#include <Winbase.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LARGE_INTEGER	CQueryPerformance::ms_liFrequency;

CQueryPerformance::CQueryPerformance()
{

}

CQueryPerformance::~CQueryPerformance()
{

}

void CQueryPerformance::Init()
{
	QueryPerformanceFrequency( &ms_liFrequency );
}

void CQueryPerformance::Start()
{
	QueryPerformanceCounter( &m_liStart );
}

double CQueryPerformance::Finish()
{
	LARGE_INTEGER	liFinish;
	QueryPerformanceCounter( &liFinish );

	double fTime = (double)liFinish.QuadPart - (double)m_liStart.QuadPart;
	fTime = fTime * 1000.0 / (double)ms_liFrequency.QuadPart;

	TRACE( "QueryPerformance : %f\n", fTime );
	return fTime;
}