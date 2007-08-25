
/*******************************************************************************************************  
*
* SecurityTypes
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

#include <windows.h>
#include "util.h"

namespace JobManagement 
{
	namespace Security
	{
		public ref class Util abstract sealed 
		{
		public:
			static bool SetPrivilege(System::IntPtr hToken,          // access token handle
							  System::String ^Privilege,	  // name of privilege to enable/disable
							  bool bEnablePrivilege);   // to enable or disable privilege 
		};

		public enum class SidGroupAttributes
		{
			None = 0,
			Enabled = SE_GROUP_ENABLED,
			EnabledByDefault = SE_GROUP_ENABLED_BY_DEFAULT,
			LogonId = SE_GROUP_LOGON_ID,
			Mandatory = SE_GROUP_MANDATORY,
			Owner = SE_GROUP_OWNER,
			Resource = SE_GROUP_RESOURCE,
			UseForDenyOnly = SE_GROUP_USE_FOR_DENY_ONLY
		};

		public value class SidAndAttributes
		{
		public:
			property System::IntPtr Sid;
			property SidGroupAttributes Attributes;
			SidAndAttributes(const SID_AND_ATTRIBUTES& nativeSidAndAttribute);
			operator SID_AND_ATTRIBUTES();
		};

		typedef System::Collections::Generic::List<SidAndAttributes> TokenGroup_t;	
		typedef TokenGroup_t ^HTokenGroup_t;	

		class SidAndAttributesGroupConverterHelper abstract sealed
		{
		public:
			static PTOKEN_GROUPS Convert2Native(HTokenGroup_t managedTokenGroup);
			static HTokenGroup_t Convert2Managed(PTOKEN_GROUPS nativeTokenGroup);
		};

		typedef Converter<TOKEN_GROUPS, HTokenGroup_t, SidAndAttributesGroupConverterHelper> SidAndAttributesGroupConverter;

		[System::Flags]
		public enum class PriviligesAttributes
		{
			Enabled = SE_PRIVILEGE_ENABLED,
			EnabledByDefault = SE_PRIVILEGE_ENABLED_BY_DEFAULT,
			Removed = SE_PRIVILEGE_REMOVED,
			UsedForAccess = SE_PRIVILEGE_USED_FOR_ACCESS
		};

		public value class LuidAndAttributes
		{
		public:
			property __int64 Luid;
			property PriviligesAttributes LuidPriviligesAttributes;
			LuidAndAttributes(const LUID_AND_ATTRIBUTES &luidAndAttributes);
			operator LUID_AND_ATTRIBUTES();
		};

		typedef System::Collections::Generic::List<LuidAndAttributes> PriviligesGroup_t;	
		typedef PriviligesGroup_t^ HPriviligesGroup_t;	

		class PriviligesGroupConverterHelper abstract sealed
		{
		public:
			static PTOKEN_PRIVILEGES Convert2Native(HPriviligesGroup_t managedPriviligesGroup);
			static HPriviligesGroup_t Convert2Managed(PTOKEN_PRIVILEGES nativePriviligesGroup);
		};

		typedef Converter<TOKEN_PRIVILEGES, HPriviligesGroup_t, PriviligesGroupConverterHelper> PriviligesGroupConverter;
	}
}