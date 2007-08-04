#pragma once
#include "stdafx.h"
#include <Windows.h>
namespace JobManagement 
{
	public ref class JobException : public System::Exception
	{
	internal:
		JobException() {}
		JobException(bool useCurrentWin32Error) : _win32Error(::GetLastError()), System::Exception(Win32ErrorMessage) {}
		JobException(bool useCurrentWin32Error, System::Exception ^innerException) : _win32Error(::GetLastError()), System::Exception(Win32ErrorMessage, innerException) {}
		JobException(unsigned int win32Error) : _win32Error(win32Error), System::Exception(Win32ErrorMessage) {}
		JobException(unsigned int win32Error, System::Exception ^innerException) : _win32Error(win32Error), System::Exception(Win32ErrorMessage, innerException) {}
		JobException(System::String ^message) : System::Exception(message) {}
		JobException(System::String ^message, System::Exception ^innerException)
			: System::Exception(message, innerException) {}
		JobException(System::Runtime::Serialization::SerializationInfo ^info,
					 System::Runtime::Serialization::StreamingContext context)
					 : System::Exception(info, context) {}
	public:
		property System::Nullable<unsigned int> Win32LastError {System::Nullable<unsigned int> get();}
		property System::String ^Win32ErrorMessage { System::String ^get(); }

	private:
		System::Nullable<unsigned int> _win32Error;
	};
}