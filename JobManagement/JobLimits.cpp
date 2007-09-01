
/*******************************************************************************************************  
*
* JobLimits.cpp
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

#include "StdAfx.h"
#include "JobLimits.h"
#include "JobManagement.h"
#include "JobException.h"
#include <msclr\lock.h>

namespace JobManagement 
{
	JobLimits::ProcessPriorityClassConverter::ProcessPriorityClassConverter(System::Diagnostics::ProcessPriorityClass processPriorityClass)
		: _managedProcessPriorityClass(processPriorityClass)
	{
		_nativeProcessPriorityClass = _managed2NativeTable[_managedProcessPriorityClass];
	}

	JobLimits::ProcessPriorityClassConverter::ProcessPriorityClassConverter(DWORD processPriorityClass)
		: _nativeProcessPriorityClass(processPriorityClass)
	{
		_managedProcessPriorityClass = _native2ManagedTable[_nativeProcessPriorityClass];
	}

	JobLimits::ProcessPriorityClassConverter::operator System::Diagnostics::ProcessPriorityClass ()
	{
		return _managedProcessPriorityClass;
	}

	JobLimits::ProcessPriorityClassConverter::operator DWORD()
	{
		return _nativeProcessPriorityClass;
	}

	JobLimits::ProcessPriorityClassConverter::operator System::Nullable<System::Diagnostics::ProcessPriorityClass>()
	{
		return System::Nullable<System::Diagnostics::ProcessPriorityClass>(*this);
	}
	
	void JobLimits::ProcessPriorityClassConverter::FillTables(DWORD nativeValue, System::Diagnostics::ProcessPriorityClass managedValue)
	{
		_native2ManagedTable[nativeValue] = managedValue;
		_managed2NativeTable[managedValue] = nativeValue;
	}


	// Converting the enummed priority class to the system priority class
	static JobLimits::ProcessPriorityClassConverter::ProcessPriorityClassConverter()
	{
		_native2ManagedTable = gcnew System::Collections::Generic::Dictionary<DWORD, System::Diagnostics::ProcessPriorityClass>();
		_managed2NativeTable = gcnew System::Collections::Generic::Dictionary<System::Diagnostics::ProcessPriorityClass, DWORD>();

		FillTables(ABOVE_NORMAL_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::AboveNormal);
		FillTables(BELOW_NORMAL_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::BelowNormal);
		FillTables(HIGH_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::High);
		FillTables(IDLE_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::Idle);
		FillTables(NORMAL_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::Normal);
		FillTables(REALTIME_PRIORITY_CLASS, System::Diagnostics::ProcessPriorityClass::RealTime);
	}

	System::Nullable<System::TimeSpan> JobLimits::PerProcessUserTimeLimit::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_PROCESS_TIME))
			return System::Nullable<System::TimeSpan>();


		return System::TimeSpan::FromTicks(Convert(basicLimitInformation.PerProcessUserTimeLimit));
	}

	void JobLimits::PerProcessUserTimeLimit::set(System::Nullable<System::TimeSpan> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.PerProcessUserTimeLimit = Convert(value.Value.Ticks);
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_PROCESS_TIME);
	}
	
	
	System::Nullable<System::TimeSpan> JobLimits::PerJobUserTimeLimit::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_JOB_TIME))
			return System::Nullable<System::TimeSpan>();

		return System::TimeSpan::FromTicks(Convert(basicLimitInformation.PerJobUserTimeLimit));
	}

	void JobLimits::PerJobUserTimeLimit::set(System::Nullable<System::TimeSpan> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.PerJobUserTimeLimit = Convert(value.Value.Ticks);
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_JOB_TIME);
	}
	
	System::Nullable<System::IntPtr> JobLimits::MinimumWorkingSetSize::get()
	{		
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_WORKINGSET))
			return System::Nullable<System::IntPtr>();

		return System::IntPtr(reinterpret_cast<void *>(basicLimitInformation.MinimumWorkingSetSize));
	}

	void JobLimits::MinimumWorkingSetSize::set(System::Nullable<System::IntPtr> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.MinimumWorkingSetSize = reinterpret_cast<SIZE_T>(value.Value.ToPointer());
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_WORKINGSET | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);
	}

	System::Nullable<System::IntPtr> JobLimits::MaximumWorkingSetSize::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_WORKINGSET))
			return System::Nullable<System::IntPtr>();

		return System::IntPtr(reinterpret_cast<void *>(basicLimitInformation.MaximumWorkingSetSize));
	}

	void JobLimits::MaximumWorkingSetSize::set(System::Nullable<System::IntPtr> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.MaximumWorkingSetSize = reinterpret_cast<SIZE_T>(value.Value.ToPointer());
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_WORKINGSET | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);
	}

	
	System::Nullable<unsigned int> JobLimits::ActiveProcessLimit::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_ACTIVE_PROCESS))
			return System::Nullable<unsigned int>();

		return basicLimitInformation.ActiveProcessLimit;

	}

	void JobLimits::ActiveProcessLimit::set(System::Nullable<unsigned int> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.ActiveProcessLimit = value.Value;
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_ACTIVE_PROCESS | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);

	}

	System::Nullable<System::IntPtr> JobLimits::Affinity::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_AFFINITY))
			return System::Nullable<System::IntPtr>();

		return System::IntPtr(reinterpret_cast<void *>(basicLimitInformation.Affinity));
	}

	void JobLimits::Affinity::set(System::Nullable<System::IntPtr> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.Affinity = reinterpret_cast<SIZE_T>(value.Value.ToPointer());
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_AFFINITY | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);
	}

	System::Nullable<System::Diagnostics::ProcessPriorityClass> JobLimits::PriorityClass::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_PRIORITY_CLASS))
			return System::Nullable<System::Diagnostics::ProcessPriorityClass>();

		return ProcessPriorityClassConverter(basicLimitInformation.PriorityClass);
	}

	void JobLimits::PriorityClass::set(System::Nullable<System::Diagnostics::ProcessPriorityClass> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.PriorityClass = ProcessPriorityClassConverter(value.Value);
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_PRIORITY_CLASS | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);
	}

	System::Nullable<unsigned char> JobLimits::SchedulingClass::get()
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (!CheckBasicFlag(basicLimitInformation, JOB_OBJECT_LIMIT_SCHEDULING_CLASS))
			return System::Nullable<unsigned char>();

		return static_cast<unsigned char>(basicLimitInformation.SchedulingClass);

	}

	void JobLimits::SchedulingClass::set(System::Nullable<unsigned char> value)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		if (value.HasValue)
		{
			basicLimitInformation.SchedulingClass = value.Value;
		}

		SetBasicInformation(value.HasValue, basicLimitInformation, JOB_OBJECT_LIMIT_SCHEDULING_CLASS | JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME);
	}

	//extended information


	bool JobLimits::CanChildProcessBreakAway::get()
	{
		return CheckExtendedFlag(JOB_OBJECT_LIMIT_BREAKAWAY_OK);
	}

	void JobLimits::CanChildProcessBreakAway::set(bool value)
	{
		SetExtendedFlag(JOB_OBJECT_LIMIT_BREAKAWAY_OK, value);
	}

	bool JobLimits::IsChildProcessBreakAway::get()
	{
		return CheckExtendedFlag(JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK);
	}
	
	void JobLimits::IsChildProcessBreakAway::set(bool value)
	{
		SetExtendedFlag(JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK, value);
	}
	
	bool JobLimits::IsDieOnUnhandledException::get()
	{
		return CheckExtendedFlag(JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION);
	}

	void JobLimits::IsDieOnUnhandledException::set(bool value)
	{
		SetExtendedFlag(JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION, value);
	}

	bool JobLimits::IsKillOnJobHandleClose::get()
	{
		return CheckExtendedFlag(JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE);
	}
		
	void JobLimits::IsKillOnJobHandleClose::set(bool value)
	{
		SetExtendedFlag(JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE, value);
	}

	//specifies the limit for the virtual memory that can be committed by a process
	System::Nullable<System::IntPtr> JobLimits::ProcessMemoryLimit::get()
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = QueryExtendedInformation();

		if (!CheckExtendedFlag(extendedLimitInformation, JOB_OBJECT_LIMIT_PROCESS_MEMORY))
			return System::Nullable<System::IntPtr>();

		return System::IntPtr((void *)extendedLimitInformation.ProcessMemoryLimit);
	}

	//specifies the limit for the virtual memory that can be committed by a process
	void JobLimits::ProcessMemoryLimit::set(System::Nullable<System::IntPtr> value)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = QueryExtendedInformation();

		if (value.HasValue)
		{
			extendedLimitInformation.ProcessMemoryLimit = (SIZE_T)value.Value.ToPointer();
		}

		SetExtendedInformation(value.HasValue, extendedLimitInformation, JOB_OBJECT_LIMIT_PROCESS_MEMORY);
	}	

	//specifies the limit for the virtual memory that can be committed by the Job
	System::Nullable<System::IntPtr> JobLimits::JobMemoryLimit::get()
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = QueryExtendedInformation();

		if (!CheckExtendedFlag(extendedLimitInformation, JOB_OBJECT_LIMIT_JOB_MEMORY))
			return System::Nullable<System::IntPtr>();

		return System::IntPtr((void *)extendedLimitInformation.JobMemoryLimit);

	}

	//specifies the limit for the virtual memory that can be committed by the job
	void JobLimits::JobMemoryLimit::set(System::Nullable<System::IntPtr> value)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = QueryExtendedInformation();

		if (value.HasValue)
		{
			extendedLimitInformation.JobMemoryLimit = (SIZE_T)value.Value.ToPointer();
		}

		SetExtendedInformation(value.HasValue, extendedLimitInformation, JOB_OBJECT_LIMIT_JOB_MEMORY);
	}	


	//UI Restriction
	
	bool JobLimits::IsDesktopCreationAndSwitchingEnabled::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_DESKTOP);
	}

	void JobLimits::IsDesktopCreationAndSwitchingEnabled::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_DESKTOP, value);
	}
	
	bool JobLimits::IsLimitDisplaySettings::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_DISPLAYSETTINGS);
	}

	void JobLimits::IsLimitDisplaySettings::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_DISPLAYSETTINGS,value);
	}

	bool JobLimits::CanCallExitWindows::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_EXITWINDOWS);
	}

	void JobLimits::CanCallExitWindows::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_EXITWINDOWS, value);
	}

	bool JobLimits::CanAccessAtomTable ::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_GLOBALATOMS);
	}

	void JobLimits::CanAccessAtomTable ::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_GLOBALATOMS, value);
	}

	bool JobLimits::CanUseOutOfJobUserHandles::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_HANDLES);
	}

	void JobLimits::CanUseOutOfJobUserHandles::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_HANDLES, value);
	}

	bool JobLimits::CanReadClipboard::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_READCLIPBOARD);
	}

	void JobLimits::CanReadClipboard::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_READCLIPBOARD, value);
	}

	bool JobLimits::CanChangeUISystemParameters::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS);
	}

	void JobLimits::CanChangeUISystemParameters::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS, value);
	}

	bool JobLimits::CanWriteClipboard::get()
	{
		return CheckUIRestrictionsFlag(JOB_OBJECT_UILIMIT_WRITECLIPBOARD);
	}

	void JobLimits::CanWriteClipboard::set(bool value)
	{
		SetUIRestrictionsFlag(JOB_OBJECT_UILIMIT_WRITECLIPBOARD, value);
	}

	void JobLimits::FilterSecurityTokenAndPriviliges(HTokenGroup_t sidsToDisable, HPriviligesGroup_t privilegesToDelete, HTokenGroup_t restrictedSids)
	{
		MarshalingContext x; //Do not delete this line

		JOBOBJECT_SECURITY_LIMIT_INFORMATION securityLimitInformation;
		securityLimitInformation.PrivilegesToDelete = PriviligesGroupConverter(privilegesToDelete);
		securityLimitInformation.RestrictedSids = SidAndAttributesGroupConverter(restrictedSids);
		securityLimitInformation.SidsToDisable = SidAndAttributesGroupConverter(sidsToDisable);
		SetSecurityLimitInformation(true, securityLimitInformation, JOB_OBJECT_SECURITY_FILTER_TOKENS);
	}

	//Prevents any process in the job from using a token that specifies the local administrators group.
	bool JobLimits::IsAdminProcessAllow::get()
	{
		return !CheckSecurityLimitFlag(JOB_OBJECT_SECURITY_NO_ADMIN);
	}

	void JobLimits::IsAdminProcessAllow::set(bool value)
	{
		SetSecurityLimitFlag(JOB_OBJECT_SECURITY_NO_ADMIN, !value);
	}

	void JobLimits::RunJobProcessesAs(System::IntPtr hToken)
	{
		HANDLE hProcessToken;

		if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hProcessToken))
			throw gcnew JobException(true);

		try
		{
			//Util::SetPrivilege(System::IntPtr(hProcessToken), L"SeCreateTokenPrivilege", true);
			Util::SetPrivilege(System::IntPtr(hProcessToken), L"SeAssignPrimaryTokenPrivilege", true);


			JOBOBJECT_SECURITY_LIMIT_INFORMATION securityLimitInformation;
			::ZeroMemory(&securityLimitInformation, sizeof(securityLimitInformation));
			securityLimitInformation.JobToken = hToken.ToPointer();
			SetSecurityLimitInformation(true, securityLimitInformation, JOB_OBJECT_SECURITY_ONLY_TOKEN);
		}
		finally
		{
			try
			{
				//Util::SetPrivilege(System::IntPtr(hProcessToken), L"SeCreateTokenPrivilege", false);
				Util::SetPrivilege(System::IntPtr(hProcessToken), L"SeAssignPrimaryTokenPrivilege", false);
			}
			finally
			{
				::CloseHandle(hProcessToken);
			}
		}
	}

	//Prevents any process in the job from using a token that was not created with the CreateRestrictedToken function.
	bool JobLimits::IsAllowOnlyRestrictedTokenProcesses::get()
	{
		return CheckSecurityLimitFlag(JOB_OBJECT_SECURITY_RESTRICTED_TOKEN);
	}
	
	void JobLimits::IsAllowOnlyRestrictedTokenProcesses::set(bool value)
	{
		SetSecurityLimitFlag(JOB_OBJECT_SECURITY_RESTRICTED_TOKEN, value);
	}


	//Helper methods for set/get limit information

	void JobLimits::SetBasicInformation(bool hasValue,  JOBOBJECT_BASIC_LIMIT_INFORMATION &basicLimitInformation, DWORD flag)
	{
		if (hasValue)
		{
			basicLimitInformation.LimitFlags |= flag;
		}
		else
		{
			basicLimitInformation.LimitFlags &= ~flag;
		}

		basicLimitInformation.LimitFlags &= 0xFF;

		DWORD bResult = ::SetInformationJobObject(_job->NativeHandle, JobObjectBasicLimitInformation, 
							&basicLimitInformation, sizeof(basicLimitInformation));
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}
	}

	void JobLimits::SetBasicFlag(DWORD flag, bool isOn)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = QueryBasicInformation();
		SetBasicInformation(isOn, basicLimitInformation, flag);
	}

	void JobLimits::SetExtendedInformation(bool hasValue,  JOBOBJECT_EXTENDED_LIMIT_INFORMATION &extendedLimitInformation, DWORD flag)
	{

		if (hasValue)
		{
			extendedLimitInformation.BasicLimitInformation.LimitFlags |= flag;
		}
		else
		{
			extendedLimitInformation.BasicLimitInformation.LimitFlags &= ~flag;
		}

		DWORD bResult = ::SetInformationJobObject(_job->NativeHandle, JobObjectExtendedLimitInformation,
							&extendedLimitInformation, sizeof(extendedLimitInformation));
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}
	}


	void JobLimits::SetExtendedFlag(DWORD flag, bool isOn)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = QueryExtendedInformation();
		SetExtendedInformation(isOn, extendedLimitInformation, flag);
	}

	JOBOBJECT_BASIC_LIMIT_INFORMATION JobLimits::QueryBasicInformation()
	{
		return QueryJobInformation<JOBOBJECT_BASIC_LIMIT_INFORMATION, JobObjectBasicLimitInformation>(_job->NativeHandle);
	}

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION JobLimits::QueryExtendedInformation()
	{
		return QueryJobInformation<JOBOBJECT_EXTENDED_LIMIT_INFORMATION, JobObjectExtendedLimitInformation>(_job->NativeHandle);
	}

	bool JobLimits::CheckBasicFlag(const JOBOBJECT_BASIC_LIMIT_INFORMATION &basicLimitInformation, DWORD flag)
	{
		return (basicLimitInformation.LimitFlags & flag) != 0;
	}

	bool JobLimits::CheckBasicFlag(DWORD flag)
	{
		JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimitInformation = 
			QueryBasicInformation();

		return CheckBasicFlag(basicLimitInformation, flag);
	}

	bool JobLimits::CheckExtendedFlag(const JOBOBJECT_EXTENDED_LIMIT_INFORMATION &extendedLimitInformation, DWORD flag)
	{
		return (extendedLimitInformation.BasicLimitInformation.LimitFlags & flag) != 0;
	}

	bool JobLimits::CheckExtendedFlag(DWORD flag)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = 
			QueryExtendedInformation();

		return CheckExtendedFlag(extendedLimitInformation, flag);
	}
	
	JOBOBJECT_BASIC_UI_RESTRICTIONS JobLimits::QueryBasicUIRestrictions()
	{
		return QueryJobInformation<JOBOBJECT_BASIC_UI_RESTRICTIONS, JobObjectBasicUIRestrictions>(_job->NativeHandle);
	}

	void JobLimits::SetUIRestrictionsFlag(DWORD flag, bool value)
	{
		JOBOBJECT_BASIC_UI_RESTRICTIONS basicUIRestrictions = QueryBasicUIRestrictions();

		if (value)
		{
			basicUIRestrictions.UIRestrictionsClass |= flag;
		}
		else
		{
			basicUIRestrictions.UIRestrictionsClass &= ~flag;
		}

		DWORD bResult = ::SetInformationJobObject(_job->NativeHandle, JobObjectBasicUIRestrictions,
							&basicUIRestrictions, sizeof(basicUIRestrictions));
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}		
	}

	bool JobLimits::CheckUIRestrictionsFlag(DWORD flag)
	{
		return (QueryBasicUIRestrictions().UIRestrictionsClass & flag) != 0;
	}

	JOBOBJECT_SECURITY_LIMIT_INFORMATION JobLimits::QuerySecurityLimitInformation()
	{
		JOBOBJECT_SECURITY_LIMIT_INFORMATION securityLimitInformation;
		DWORD returnLength;

		DWORD bResult = ::QueryInformationJobObject(_job->NativeHandle, JobObjectSecurityLimitInformation,
							&securityLimitInformation, sizeof(securityLimitInformation), 
							&returnLength);
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}

		return securityLimitInformation;
	}

	void JobLimits::SetSecurityLimitInformation(bool hasValue,  JOBOBJECT_SECURITY_LIMIT_INFORMATION &securityLimitInformation, DWORD flag)
	{
		if (hasValue)
		{
			securityLimitInformation.SecurityLimitFlags |= flag;
		}
		else
		{
			securityLimitInformation.SecurityLimitFlags &= ~flag;
		}

		DWORD bResult = ::SetInformationJobObject(_job->NativeHandle, JobObjectSecurityLimitInformation,
							&securityLimitInformation, sizeof(securityLimitInformation));
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}
	}

	void JobLimits::SetSecurityLimitFlag(DWORD flag, bool value)
	{
		JOBOBJECT_SECURITY_LIMIT_INFORMATION securityLimitInformation = QuerySecurityLimitInformation();

		if (value)
		{
			securityLimitInformation.SecurityLimitFlags |= flag;
		}
		else
		{
			securityLimitInformation.SecurityLimitFlags &= ~flag;
		}

		DWORD bResult = ::SetInformationJobObject(_job->NativeHandle, JobObjectSecurityLimitInformation,
							&securityLimitInformation, sizeof(securityLimitInformation));
		if (bResult == 0)
		{
			throw gcnew JobException(true);
		}		
	}

	bool JobLimits::CheckSecurityLimitFlag(DWORD flag)
	{
		return (QuerySecurityLimitInformation().SecurityLimitFlags & flag) != 0;
	}


	void JobLimits::UserHandleGrantAccess(System::IntPtr userHandle, bool bGrant)
	{
		::UserHandleGrantAccess(userHandle.ToPointer(), _job->NativeHandle, bGrant);
	}

	void JobLimits::OnTimedEvent( System::Object^ source, System::Timers::ElapsedEventArgs^ e)
	{
		_job->TerminateAllProcesses((unsigned int)ExitReasonIds::Timeout);
	}

	void JobLimits::SetAbsoluteTimer(System::DateTime toLiveDateTime)
	{
		msclr::lock l(this);

		System::TimeSpan subDate = toLiveDateTime.Subtract(System::DateTime::Now);
		if (subDate.Milliseconds < 0.0)
			throw gcnew System::TimeoutException("Invocation time occured in the past");

		if (_liveTimer != nullptr)
		{
			ChangeAbsoluteTimer(subDate);
			return;
		}

		_liveTimer = gcnew System::Timers::Timer(subDate.TotalMilliseconds);
		_liveTimer->AutoReset = FALSE;
		_liveTimer->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &JobLimits::OnTimedEvent);
		_liveTimer->Start();
	}

	void JobLimits::SetAbsoluteTimer(System::TimeSpan liveTimeSpan)
	{
		SetAbsoluteTimer(System::DateTime::Now.Add(liveTimeSpan));
	}

	void JobLimits::ChangeAbsoluteTimer(System::TimeSpan timerTimeSpan)
	{
		msclr::lock l(this);

		_liveTimer->Stop();
		_liveTimer->Interval = timerTimeSpan.TotalMilliseconds;
		_liveTimer->Start();
	}

	void JobLimits::ClearAbsoluteTimer()
	{
		msclr::lock l(this);

		if (_liveTimer != nullptr)
		{
			_liveTimer->Stop();
			_liveTimer->Close();
			delete _liveTimer;
			_liveTimer = nullptr;
		}
	}
}