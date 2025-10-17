#pragma once


#pragma warning( disable : 26495)
#include <afx.h>

#define EXENAME				TEXT("AutoBackup.exe")
#define __APP_VERSION		TEXT("v1.0")

BOOL LoadDBName();
BOOL IsSubject(char* pzFileName);