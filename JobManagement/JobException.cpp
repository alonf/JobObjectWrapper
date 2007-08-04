#pragma once
#include "stdafx.h"
#include "JobException.h"

namespace JobManagement
{
	System::Nullable<unsigned int> JobException::Win32LastError::get()
	{
		return _win32Error;
	}

	System::String ^JobException::Win32ErrorMessage::get()
	{
		if (!_win32Error.HasValue)
			return System::String::Empty;
		wchar_t errorMessage[1024];
		
		BOOL bResult = ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM |
										FORMAT_MESSAGE_IGNORE_INSERTS,
										NULL, _win32Error.Value,
										MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
										errorMessage, 1024, NULL);
		if (!bResult)
		{
			return nullptr;
		}

		return gcnew System::String(errorMessage);
	}
}