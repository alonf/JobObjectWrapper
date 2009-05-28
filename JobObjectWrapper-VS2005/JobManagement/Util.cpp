/*******************************************************************************************************  
*
* Util.cpp
*
* Copyright 2007 The JobObjectWrapper Team  
* http://www.codeplex.com/JobObjectWrapper/Wiki/View.aspx?title=Team
*
* This program is licensed under the Microsoft Permissive License (Ms-PL).  You should 
* have received a copy of the license along with the source code.  If not, an online copy
* of the license can be found at http://www.codeplex.com/JobObjectWrapper/Project/License.aspx.
*   
*  Notes :
*      - First release by Alon Fliess
********************************************************************************************************/

#include "stdafx.h"
#include "util.h"
#include <assert.h>
#include <algorithm>

namespace JobManagement 
{
	DWORD MarshalingContext::_tlsEntry = ~0;

	MarshalingContext::MarshalingContext()
	{
		if (_tlsEntry == ~0)
		{
			_tlsEntry = ::TlsAlloc();
		}
		_previous = ::TlsGetValue(_tlsEntry);
		::TlsSetValue(_tlsEntry, this);
		_hHeap = ::HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0, 0);
	}

	MarshalingContext::~MarshalingContext()
	{
		::TlsSetValue(_tlsEntry, _previous);
		::HeapDestroy(_hHeap);
		std::for_each(_allocatedStrings.begin(), _allocatedStrings.end(), FreeHGlobal);
	}

	MarshalingContext *MarshalingContext::Current()
	{
		MarshalingContext *This = static_cast<MarshalingContext *>(::TlsGetValue(_tlsEntry));
		System::Diagnostics::Debug::Assert(This != NULL, L"Could not find MarshalingContext");  
		return This;
	}
		
	void *MarshalingContext::Alloc(SIZE_T size)
	{
		return ::HeapAlloc(_hHeap, HEAP_GENERATE_EXCEPTIONS, size);
	}

	LPCWSTR MarshalingContext::ConvertString(System::String ^text)
	{
		LPCWSTR pStr = static_cast<LPCWSTR>(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(text).ToPointer());
		_allocatedStrings.push_back(pStr);
		return pStr;
	}

	LPCWSTR MarshalingContext::Managed2NativeString(System::String ^text)
	{
		return MarshalingContext::Current()->ConvertString(text);
	}

	void MarshalingContext::FreeHGlobal(LPCWSTR pStr)
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void *)pStr));
	}

	LARGE_INTEGER Convert(__int64 value)
	{
		LARGE_INTEGER li;
		li.QuadPart = value;
		return li;
	}

	__int64 Convert(LARGE_INTEGER value)
	{
		return value.QuadPart;	
	}
}
