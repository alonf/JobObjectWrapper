
/*******************************************************************************************************  
*
* SecurityTypeConvertors.cpp
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
#include "SecurityTypes.h"

namespace JobManagement 
{
	namespace Security
	{
		bool Util::SetPrivilege(System::IntPtr hToken,          // access token handle
							  System::String ^privilege,	  // name of privilege to enable/disable
							  bool bEnablePrivilege)   // to enable or disable privilege
		{
			MarshalingContext _mc;

			TOKEN_PRIVILEGES tp;
			LUID luid;
			LPCWSTR lpszPrivilege = MarshalingContext::Managed2NativeString(privilege);

			if ( !::LookupPrivilegeValue( 
					NULL,            // lookup privilege on local system
					lpszPrivilege,   // privilege to lookup 
					&luid ) )        // receives LUID of privilege
						throw gcnew JobException(true);

			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;

			// Enable the privilege or disable all privileges.
			tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;


			if ( !::AdjustTokenPrivileges(
				hToken.ToPointer(), 
				FALSE, 
				&tp, 
				sizeof(TOKEN_PRIVILEGES), 
				(PTOKEN_PRIVILEGES) NULL, 
				(PDWORD) NULL) )
					throw gcnew JobException(true);

			if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
				  throw gcnew JobException(true, gcnew JobException("The token does not have the specified privilege"));

			return TRUE;
		}
					

		PTOKEN_GROUPS SidAndAttributesGroupConverterHelper::Convert2Native(HTokenGroup_t managedTokenGroup)
		{
			PTOKEN_GROUPS pTokenGroup = MarshalingContext::Alloc<TOKEN_GROUPS>(sizeof(DWORD /*TOKEN_GROUPS::GroupCount*/) + sizeof(SID_AND_ATTRIBUTES  /*TOKEN_GROUPS::Groups*/) * managedTokenGroup->Count) ;

			pTokenGroup->GroupCount = managedTokenGroup->Count;

			for (int i = 0; i < managedTokenGroup->Count; ++i)
			{
				pTokenGroup->Groups[i] = managedTokenGroup[i];
			}

			return pTokenGroup;
		}

		HTokenGroup_t SidAndAttributesGroupConverterHelper::Convert2Managed(PTOKEN_GROUPS nativeTokenGroup)
		{
			HTokenGroup_t managedTokenGroup = gcnew TokenGroup_t(nativeTokenGroup->GroupCount);

			for (unsigned int i = 0; i < nativeTokenGroup->GroupCount; ++i)
			{
				managedTokenGroup->Add(SidAndAttributes(nativeTokenGroup->Groups[i]));
			}

			return managedTokenGroup;
		}
		

		PTOKEN_PRIVILEGES PriviligesGroupConverterHelper::Convert2Native(HPriviligesGroup_t managedPriviligesGroup)
		{
			PTOKEN_PRIVILEGES pTokenPrivileges = MarshalingContext::Alloc<TOKEN_PRIVILEGES>(sizeof(DWORD /*TOKEN_PRIVILEGES::PrivilegeCount*/) + sizeof(LUID_AND_ATTRIBUTES  /*TOKEN_PRIVILEGES::Privileges*/) * managedPriviligesGroup->Count) ;

			pTokenPrivileges->PrivilegeCount = managedPriviligesGroup->Count;

			for (int i = 0; i < managedPriviligesGroup->Count; ++i)
			{
				pTokenPrivileges->Privileges[i] = managedPriviligesGroup[i];
			}

			return pTokenPrivileges;
		}
		
		HPriviligesGroup_t PriviligesGroupConverterHelper::Convert2Managed(PTOKEN_PRIVILEGES nativePriviligesGroup)
		{
			HPriviligesGroup_t managedPriviligesGroup = gcnew PriviligesGroup_t(nativePriviligesGroup->PrivilegeCount);

			for (unsigned int i = 0; i < nativePriviligesGroup->PrivilegeCount; ++i)
			{
				managedPriviligesGroup->Add(LuidAndAttributes(nativePriviligesGroup->Privileges[i]));
			}

			return managedPriviligesGroup;
		}

		SidAndAttributes::SidAndAttributes(const SID_AND_ATTRIBUTES& nativeSidAndAttribute)
		{
			Sid = System::IntPtr(nativeSidAndAttribute.Sid);
			Attributes = static_cast<SidGroupAttributes>(nativeSidAndAttribute.Attributes);
			
		}

		SidAndAttributes::operator SID_AND_ATTRIBUTES()
		{
			SID_AND_ATTRIBUTES sidAndAttribute;
			sidAndAttribute.Attributes = static_cast<DWORD>(Attributes);
			sidAndAttribute.Sid = Sid.ToPointer();

			return sidAndAttribute;
		}

		
		LuidAndAttributes::LuidAndAttributes(const LUID_AND_ATTRIBUTES &luidAndAttributes)
		{
			Luid = *((__int64 *)(&luidAndAttributes.Luid));
			LuidPriviligesAttributes = static_cast<PriviligesAttributes>(luidAndAttributes.Attributes);
		}

		LuidAndAttributes::operator LUID_AND_ATTRIBUTES()
		{
			LUID_AND_ATTRIBUTES luidAndAttributes;
			*((__int64 *)&(luidAndAttributes.Luid)) = Luid;
			luidAndAttributes.Attributes = static_cast<DWORD>(LuidPriviligesAttributes);

			return luidAndAttributes;
		}
	}
}