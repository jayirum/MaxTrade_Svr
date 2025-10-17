#include "CODBC.h"
#include <stdio.h>
#include <stdarg.h>

#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000
char sqlstate[SQL_RETURN_CODE_LEN];
SQLINTEGER native_error;
SQLCHAR error_message[SQL_RESULT_LEN];

CODBC::CODBC(DBMS_TYPE  dbms) : m_hEnv(NULL), m_hDbc(NULL), m_dbms(dbms)
{
    m_hstmt = NULL;
    m_hEnv = NULL;
    m_hDbc = NULL;
}

CODBC::~CODBC()
{
    DeInitialize();
}

void CODBC::DeInitialize()
{
    delete m_pingTimer;

    FreeStmts();
    Disconnect();
    SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
    m_hstmt = NULL;
    m_hEnv = NULL;
    m_hDbc = NULL;
}

bool CODBC::Initialize(int ping_timeout_sec)
{
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);
    if (!SQL_SUCCEEDED(ret)) {
        sprintf(m_zMsg, "Error in allocating environment handle[SQLAllocHandle]");
        return false;
    }
    ret = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (!SQL_SUCCEEDED(ret)) {
        sprintf(m_zMsg, "Error in setting ODBC version attribute[SQLSetEnvAttr]");
        return false;
    }

    m_pingTimer = new CPingTimer(ping_timeout_sec);
    return true; 
}


bool CODBC::Connect(const char* connectionString) 
{
    // allocates an environment, connection, statement, or descriptor handle.
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
    if (!SQL_SUCCEEDED(ret))
    {
        HandleError("SQLAllocHandle", m_hDbc, SQL_HANDLE_DBC);
        return false;
    }
     
    ret = SQLDriverConnect(m_hDbc, NULL, (SQLCHAR*)connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
    if(!SQL_SUCCEEDED(ret))
    {
        HandleError("SQLDriverConnect", m_hDbc, SQL_HANDLE_DBC);
        return false;
    }

    // AutoCommit 활성화 추가
    SQLSetConnectAttr(m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_UINTEGER);


    return true;
}

void CODBC::Disconnect()
{
    if (m_hDbc != NULL) {
        SQLDisconnect(m_hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
        m_hDbc = NULL;
    }
}


void CODBC::FreeStmts()
{
    if (m_hstmt != NULL) {
        SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
        m_hstmt = NULL;
    }
}

//void CODBC::ClearParams()
//{
//    for (UINT i = 0; i < m_vecParam.size(); i++)
//    {
//        delete m_vecParam[i];
//    }
//    m_vecParam.clear();
//    FreeStmts();
//}


VOID CODBC::Init_ExecQry(char* pzQry)
{
    strcpy(m_zSQL, pzQry);
}



bool CODBC::Exec_Qry(bool& bNeedReconn)
{
    bNeedReconn = false;

    if (!IsConnected()) {
        sprintf(m_zMsg, "Not connected the database");
        bNeedReconn = true;
        return false;
    }


    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hstmt);
    if (!SQL_SUCCEEDED(ret))
    {
        bNeedReconn = HandleError("SQLAllocHandle", m_hDbc, SQL_HANDLE_DBC);
        return false;
    }



    // Execute the stored procedure
    ret = SQLExecDirectA(m_hstmt, (SQLCHAR*)m_zSQL, SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        bNeedReconn= HandleError("SQLExecute", m_hstmt, SQL_HANDLE_STMT);
        return false;
    }

    //// 명시적 Commit 추가
    //SQLRETURN commitRet = SQLEndTran(SQL_HANDLE_DBC, m_hDbc, SQL_COMMIT);
    //if (!SQL_SUCCEEDED(commitRet)) {
    //    HandleError("SQLEndTran", m_hDbc, SQL_HANDLE_DBC);
    //    return false;
    //}

    m_nNumCol = 0;
    ret = SQLNumResultCols(m_hstmt, &m_nNumCol);
    if (!SQL_SUCCEEDED(ret))
    {
        bNeedReconn = HandleError("SQLNumResultCols", m_hstmt, SQL_HANDLE_STMT);
        return false;
    }

    return true;
}

bool CODBC::GetDataLong(int colIdx, long* out)
{
    *out = 0;
    if (colIdx > m_nNumCol)
    {
        sprintf(m_zMsg, "col count:%d < colIdx:%d", m_nNumCol, colIdx);
        return false;
    }
    SQLRETURN ret = SQLGetData(m_hstmt, colIdx, SQL_C_LONG, out, sizeof(long), NULL);
    if (!SQL_SUCCEEDED(ret))
        HandleError("SQLGetData", m_hstmt, SQL_HANDLE_STMT);

    return SQL_SUCCEEDED(ret);
}

bool CODBC::GetDataStr(int colIdx, int nBufLen, char* out)
{
    *out = 0x00;
    if (colIdx > m_nNumCol)
    {
        sprintf(m_zMsg, "col count:%d < colIdx:%d", m_nNumCol, colIdx);
        return false;
    }
    SQLRETURN ret = SQLGetData(m_hstmt, colIdx, SQL_C_CHAR, out, nBufLen, NULL);
    if (!SQL_SUCCEEDED(ret))
        HandleError("SQLGetData", m_hstmt, SQL_HANDLE_STMT);

    return SQL_SUCCEEDED(ret);
}


bool CODBC::GetDataStr(int colIdx, string* out)
{
    char zData[1024] = { 0 };
    if (colIdx > m_nNumCol)
    {
        sprintf(m_zMsg, "col count:%d < colIdx:%d", m_nNumCol, colIdx);
        return false;
    }
    SQLRETURN ret = SQLGetData(m_hstmt, colIdx, SQL_C_CHAR, zData, sizeof(zData), NULL);
    if (!SQL_SUCCEEDED(ret))
        HandleError("SQLGetData", m_hstmt, SQL_HANDLE_STMT);

    *out = zData;
    return SQL_SUCCEEDED(ret);
}

bool CODBC::GetDataDbl(int colIdx, double* out)
{
    *out = 0;
    if (colIdx > m_nNumCol)
    {
        sprintf(m_zMsg, "col count:%d < colIdx:%d", m_nNumCol, colIdx);
        return false;
    }
    SQLRETURN ret = SQLGetData(m_hstmt, colIdx, SQL_C_DOUBLE, out, sizeof(double), NULL);
    if (!SQL_SUCCEEDED(ret))
        HandleError("SQLGetData", m_hstmt, SQL_HANDLE_STMT);

    return SQL_SUCCEEDED(ret);
}


bool CODBC::GetNextData()
{
    if (m_hstmt == NULL)
        return false;

    return (SQLFetch(m_hstmt) == SQL_SUCCESS);
}

void CODBC::DeInit_ExecQry()
{
    FreeStmts();
}

bool CODBC::HandleError(const char* fn, SQLHANDLE handle, SQLSMALLINT type) 
{
    SQLSMALLINT   i = 0;
    SQLINTEGER   nativeErr;
    SQLCHAR      sqlstate[7];
    SQLCHAR      text[256];
    SQLSMALLINT  len;
    SQLRETURN    ret;
    bool        bNeedReconn = false;


    sprintf(m_zMsg, "[%s]", fn);

    do {
        ret = SQLGetDiagRec(type, handle, ++i, sqlstate, &nativeErr, text, sizeof(text), &len);
        if (SQL_SUCCEEDED(ret)) {
            char    zMsg[1024];
            sprintf(zMsg, "(SQLSTATE:%s)(recNum:%d)(Err Code:%d)(Msg:%s)", sqlstate, i, nativeErr, text);
            strcat(m_zMsg, zMsg);
        }
    } while (ret == SQL_SUCCESS);

    if (strcmp((char*)sqlstate, "08S01") == 0)
    {
        if (m_dbms == DBMS_TYPE::MYSQL)
        {
            if (nativeErr == 2013 || nativeErr == 2006) {
                bNeedReconn = true;
            }
        }
        else if (m_dbms == DBMS_TYPE::MSSQL) {
            if (nativeErr == 10054 || nativeErr == 233 || nativeErr == 64 || nativeErr == 10053) {
                bNeedReconn = true;
            }
        }
    }


    return bNeedReconn;
}


void CODBC::PingConnection()
{
    if (!m_pingTimer->tick())
        return;

    Init_ExecQry((char*)"SELECT 1");

    bool bNeedReconn;
    Exec_Qry(bNeedReconn);
    DeInit_ExecQry();

    //printf("ping...\n");
}


////////////////////////////////////////////////////////////////////////
// example
// ODBCTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#if 0
#include <iostream>
#include "../../Common/CODBC.h"

int main()
{
    CODBC odbc;
    bool ret = odbc.Initialize();

    char connStr[512];
    //strcpy(connStr, "DSN=DSN_MAXTRADE; UID = maxsa; PWD = opsapwd20231205!!!; ");
    strcpy(connStr, "DSN=DSN_MAXTRADE;UID=maxsa;PWD=opsapwd20231205!!!;");
    if (!odbc.Connect(connStr))
    {
        printf(odbc.getMsg());
        getchar();
        return 0;
    }

    for (int loop = 0; loop < 100; loop++)
    {
        int rnd = rand();
        char zQ[1024];
        sprintf(zQ, "SP_DEBUG %d, '%s', '%s', %d, %f", 2, "COLUMN-1", "COLUMN-2", 100000 / rnd, 10000.12 / (double)rnd);
        odbc.Init_ExecQry(zQ);
        if (!odbc.Exec_Qry()) {
            printf(odbc.getMsg());
            getchar();
            return 0;
        }

        char szRs[1024] = { 0, };
        int row = 0;
        while (odbc.GetNextData())
        {
            char z1[1024] = { 0, }, z2[1024] = { 0, };
            long seq = 0;
            long n = 0;
            double d = 0;

            odbc.GetDataLong(1, &seq);
            odbc.GetDataStr(2, sizeof(z1), z1);
            odbc.GetDataStr(3, sizeof(z2), z2);
            odbc.GetDataLong(4, &n);
            odbc.GetDataDbl(5, &d);

            printf("ROW:%d [SEQ:%d][%s][%s][%d][%f]\n", row++, seq, z1, z2, n, d);
        }
        odbc.DeInit_ExecQry();

        Sleep(0);
    }

    getchar();
}
#endif