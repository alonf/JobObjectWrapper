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
	hResult = ::CreateRestrictedToken(hToken, 0, 0, 0, 0, 0, 0, 0, &hRestrictedToken);

	DWORD error = ::GetLastError();

	//JobObject jo(L"SecText");


	/*jo.Limits->RunJobProcessesAs(IntPtr(hRestrictedToken));*/
	JobObject jo(L"Hello");

	JobObject joByHandle(jo.Handle);

	//System::Diagnostics::ProcessStartInfo ^si = gcnew ::Diagnostics::ProcessStartInfo(L"notepad.exe");
 //   si->RedirectStandardOutput = true;
 //   si->UseShellExecute = false;
 //   
	//jo.CreateProcessSecured(si);

	////jo.TerminateAllProcesses(8);

	jo.SetAbsoluteTimer(System::DateTime(2007, 8, 10, 17, 30, 0));

	jo.ClearAbsoluteTimer();

	joByHandle.SetAbsoluteTimer(System::TimeSpan(0, 0, 10));

	System::Console::ReadKey();

    return 0;
}
