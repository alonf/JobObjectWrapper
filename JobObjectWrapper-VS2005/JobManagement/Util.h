
/*******************************************************************************************************  
*
* Util.h
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

#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <list>
#include "JobException.h"

namespace JobManagement 
{
	class MarshalingContext sealed
	{
	public:
		MarshalingContext();
		~MarshalingContext();
		static MarshalingContext *Current();
		void *Alloc(SIZE_T size);
		LPCWSTR ConvertString(System::String ^text);
		template<typename T> static T *Alloc(SIZE_T size)
		{
			return static_cast<T *>(MarshalingContext::Current()->Alloc(size));
		}
		
		void *operator new(size_t size)
		{
			throw gcnew System::Exception("Local scope only");
		}

		static LPCWSTR Managed2NativeString(System::String ^text);

	private:
		static void FreeHGlobal(LPCWSTR pStr);

		static DWORD _tlsEntry;
		LPVOID _previous;
		HANDLE _hHeap;
		std::list<LPCWSTR> _allocatedStrings;
	};

	template <typename TNative, typename TManaged, class TConverterHelper>
	class Converter
	{
	public:
		Converter(const TNative *pNative)
		{
			Assign(pNative);
		}
		
		Converter(const TManaged managed)
		{
			const TNative *pNative = TConverterHelper::Convert2Native(managed);
			Assign(pNative);
		}

		operator TNative()
		{
			return *_pNative;
		}

		operator TNative*()
		{
			return _pNative;
		}

		operator TManaged()
		{
			return TConverterHelper::Convert2Managed(_pNative);
		}

	private:
		TNative *_pNative;
		//Called by the ctor
		void Assign(const TNative *pNative)
		{
			_pNative = reinterpret_cast<TNative *>(MarshalingContext::Current()->Alloc(sizeof(TNative)));
			::memcpy_s(_pNative, sizeof(TNative), pNative, sizeof(TNative));
		}	
	};

	template<typename TInformationStruct, JOBOBJECTINFOCLASS InformationEnum>
	TInformationStruct QueryJobInformation(HANDLE hJob)
	{
		TInformationStruct informationStruct ;
		DWORD returnLength;

		DWORD bResult = ::QueryInformationJobObject(hJob, InformationEnum,
							&informationStruct, sizeof(informationStruct), 
							&returnLength);
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}
		return informationStruct;
	}

	LARGE_INTEGER Convert(__int64 value);
	__int64 Convert(LARGE_INTEGER value);
}