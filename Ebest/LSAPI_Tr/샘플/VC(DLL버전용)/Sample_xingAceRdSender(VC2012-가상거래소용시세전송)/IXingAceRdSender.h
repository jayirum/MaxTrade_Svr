#pragma once

class IXingAceRdSender
{
public:
	IXingAceRdSender () { ZeroMemory( this, sizeof( IXingAceRdSender ) ); }
	~IXingAceRdSender() { Uninit(); }

	inline	BOOL	Init					( LPCTSTR szPath=NULL );
	inline	void	Uninit					();
	inline	BOOL	IsInit					()											{ return m_hModule != NULL; }

	inline	HMODULE	GetSafeHandle			()											{ return m_hModule; }
	
	inline	BOOL	Open					();
	inline	BOOL	Send					( const char* pszTR, const char* pszInBlock, int nInBlockSize, const char* pszOutBlock, int nOutBlockSize );
	inline	void	Close					();

protected:
	inline	int		LoadLibHelper			( LPCTSTR szPath );

protected:
	HMODULE			m_hModule;

	typedef BOOL (__stdcall *FP_XINGACERDOPEN)();
	typedef BOOL (__stdcall *FP_XINGACERDSEND)( const char*, const char*, int, const char*, int );
	typedef void (__stdcall *FP_XINGACERDCLOSE)();

	FP_XINGACERDOPEN	m_fpOpen;
	FP_XINGACERDSEND	m_fpSend;
	FP_XINGACERDCLOSE	m_fpClose;
};

BOOL IXingAceRdSender::Init( LPCTSTR szPath )
{
	if( IsInit() )
	{
		return TRUE;
	}

	return LoadLibHelper( szPath );
}

BOOL IXingAceRdSender::LoadLibHelper( LPCTSTR szPath )
{
	TCHAR szCommLib[MAX_PATH] = { 0 };

	if( szPath == NULL || strlen( szPath ) == 0 )
	{
		lstrcpy( szCommLib, _T( "xingAceRdSender.dll" ) );
	}
	else
	{
		sprintf_s( szCommLib, _T( "%s\\xingAceRdSender.dll" ), szPath );
	}

	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetCurrentDirectory( MAX_PATH, szCurrentDir );
	SetCurrentDirectory( szPath );
	m_hModule = ::LoadLibrary( szCommLib );
	if( m_hModule == NULL )
	{
		TRACE( "\nxingAceRdSender DLL Load에 실패하였습니다.\n에러코드 : %d\n파일위치 : %s\n", ::GetLastError(), szCommLib );
	}
	SetCurrentDirectory( szCurrentDir );

	return NULL == m_hModule ? FALSE : TRUE;
}

void IXingAceRdSender::Uninit()
{
	if( IsInit() )
	{
		Close();
		::FreeLibrary( m_hModule );
	}
	m_hModule = NULL;
}

BOOL IXingAceRdSender::Open()
{
	ASSERT( m_hModule );
	if( NULL == m_hModule   ) return FALSE;
	if( NULL == m_fpOpen ) m_fpOpen = (FP_XINGACERDOPEN)GetProcAddress(m_hModule, "xingAceRDOpen");
	ASSERT( m_fpOpen );
	if( NULL == m_fpOpen ) return FALSE;
	return m_fpOpen();
}

BOOL IXingAceRdSender::Send( const char* pszTR, const char* pszInBlock, int nInBlockSize, const char* pszOutBlock, int nOutBlockSize )
{
	ASSERT( m_hModule );
	if( NULL == m_hModule		) return FALSE;
	if( NULL == m_fpSend ) m_fpSend = (FP_XINGACERDSEND)GetProcAddress(m_hModule, "xingAceRDSend");
	ASSERT( m_fpSend );
	if( NULL == m_fpSend ) return FALSE;
	return m_fpSend( pszTR, pszInBlock, nInBlockSize, pszOutBlock, nOutBlockSize );
}

void IXingAceRdSender::Close()
{
	ASSERT( m_hModule );
	if( NULL == m_hModule		) return;
	if( NULL == m_fpClose	) m_fpClose = (FP_XINGACERDCLOSE)GetProcAddress(m_hModule, "xingAceRDClose");
	ASSERT( m_fpClose );
	if( NULL == m_fpClose	) return;
	m_fpClose();
	return;
}
