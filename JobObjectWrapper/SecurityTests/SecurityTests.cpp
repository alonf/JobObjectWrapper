// SecurityTests.cpp : main project file.

#include "stdafx.h"
#include <windows.h>

using namespace System;
using namespace JobManagement;

int main(array<System::String ^> ^args)
{
	HANDLE hToken;
	HANDLE hRestrictedToken;

	BOOL hResult = ::LogonUser(L"TestUser", L"", L"12345678", LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken);
	hResult = ::CreateRestrictedToken(hToken, DISABLE_MAX_PRIVILEGE, 0, 0, 0, 0, 0, 0, &hRestrictedToken);

	DWORD error = ::GetLastError();

	JobObject jo(L"SecText");
	jo.Limits->RunJobProcessesAs(IntPtr(hRestrictedToken));
    return 0;
}
