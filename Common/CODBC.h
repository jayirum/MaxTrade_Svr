#pragma once

#include "MaxTradeInc.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "CPingTimer.h"
#pragma warning(disable:4996)
using namespace std;

#define SQLBUF_SIZE 1024



//#define SQL_C_CHAR    SQL_CHAR             /* CHAR, VARCHAR, DECIMAL, NUMERIC */
//#define SQL_C_LONG    SQL_INTEGER          /* INTEGER                      */
//#define SQL_C_SHORT   SQL_SMALLINT         /* SMALLINT                     */
//#define SQL_C_FLOAT   SQL_REAL             /* REAL                         */
//#define SQL_C_DOUBLE  SQL_DOUBLE           /* FLOAT, DOUBLE                */


enum class DBMS_TYPE{ MSSQL, MYSQL};

struct TParamInfo
{
    SQLSMALLINT     valueType;
    SQLSMALLINT     paramType;
    string  sParam;
    int     nParam;
    double  dParam;

    TParamInfo()
    {
        valueType = SQL_UNKNOWN_TYPE;
        paramType = SQL_UNKNOWN_TYPE;
    }
};
class CODBC {

public:
    CODBC(DBMS_TYPE  dbms);
    ~CODBC();

    bool Initialize(int ping_timeout_sec=30);
    void DeInitialize();
    bool Connect(const char* connectionString);
    void Disconnect();
    bool IsConnected() { return (m_hDbc != nullptr); }

    
    /// ////////////////////////////////////////////
    VOID Init_ExecQry(char* pzQry);
    bool Exec_Qry(bool& bNeedReconn);
    void DeInit_ExecQry();

    bool GetNextData();
    bool GetDataLong(int colIdx, long* out);
    bool GetDataStr(int colIdx, int nBufLen, char* out);
    bool GetDataStr(int colIdx, string* out);
    bool GetDataDbl(int colIdx, double* out);
    /// ////////////////////////////////////////////

    char* getMsg() { return m_zMsg; }
    void PingConnection();
private:
    bool HandleError(const char* fn, SQLHANDLE handle, SQLSMALLINT type);
    void FreeStmts();
    bool Is_WithinIdx(int idx)
    {
        return (m_vecParam.size() > (UINT)idx);
    }
    
    
private:
    vector< TParamInfo*>    m_vecParam;
    string                  m_sSPName;
private:
    SQLHENV         m_hEnv;
    SQLHDBC         m_hDbc;
    SQLHSTMT        m_hstmt;
    SQLSMALLINT     m_nNumCol;
    char            m_zSQL[SQLBUF_SIZE];
    char            m_zMsg[4096];
    DBMS_TYPE       m_dbms;

    int                 m_ping_timeout_sec;
    CPingTimer*         m_pingTimer;
};

//
//int main() {
//    SqlConnection sqlConnection;
//
//    // Replace the connection string with your actual SQL Server details
//    const char* connectionString = "DRIVER={SQL Server};SERVER=your_server;DATABASE=your_database;UID=your_username;PWD=your_password";
//
//    if (sqlConnection.Connect(connectionString)) {
//        std::cout << "Connected to the database." << std::endl;
//
//        // Example: Calling a stored procedure
//        const char* storedProcedureName = "your_stored_procedure";
//        const char* parameter1 = "value1";
//        const char* parameter2 = "value2";
//
//        if (sqlConnection.ExecuteStoredProcedure(storedProcedureName, parameter1, parameter2)) {
//            std::cout << "Stored procedure executed successfully." << std::endl;
//        }
//
//        // Disconnect from the database
//        sqlConnection.Disconnect();
//    }
//    else {
//        std::cerr << "Failed to connect to the database." << std::endl;
//    }
//
//    return 0;
//}
