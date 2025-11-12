// QueryPerformance.h: interface for the CQueryPerformance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYPERFORMANCE_H__628E8744_FAED_411D_992B_B41D12A32944__INCLUDED_)
#define AFX_QUERYPERFORMANCE_H__628E8744_FAED_411D_992B_B41D12A32944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueryPerformance  
{
private:
	static	LARGE_INTEGER	ms_liFrequency;
	LARGE_INTEGER			m_liStart;

public:
	static	void	Init();

public:
	CQueryPerformance();
	virtual ~CQueryPerformance();

	void	Start	();
	double	Finish	();
};

#endif // !defined(AFX_QUERYPERFORMANCE_H__628E8744_FAED_411D_992B_B41D12A32944__INCLUDED_)
