
/*******************************************************************************************************  
*
* JobLimits.h
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
#include "SecurityTypes.h"

namespace JobManagement 
{
	using namespace Security;

	ref class JobObject;

	
	/// <summary>
	/// JobLimits is a sealed class that wraps the Win32 Job Object Limits    
	/// which define the limits for the job and its processes. The limits are 
	/// the boundaries for the processes running under the job.
	/// Joblimits expands the Win32 limits with Absolute timer limit.
	/// </summary>
	///<remarks> You can limit processes in the Job in many areas, such as: 
	/// time limit (CPU time and absolute time), Memory limits, force priority &amp; scheduling class, set the process working set,
	/// and force several security related limits. 
	/// Security limits are deprecated on Windows Vista &amp; Server 2008. 
	/// We will try to find a solution on per process basis for those platforms</remarks>

	public ref class JobLimits sealed
	{
	public:
		JobLimits(JobObject ^job) : _job(job) {}

		// The time limit for a process to run under
		property System::Nullable<System::TimeSpan> PerProcessUserTimeLimit
		{
			System::Nullable<System::TimeSpan> get();
			void set(System::Nullable<System::TimeSpan> value);
		}

		// The time limit for a job to run under
		property System::Nullable<System::TimeSpan> PerJobUserTimeLimit
		{
			System::Nullable<System::TimeSpan> get();
			void set(System::Nullable<System::TimeSpan> value);
		}

		// The minimum size of a working-set
		property System::Nullable<System::IntPtr> MinimumWorkingSetSize
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		// The maximum size of a working-set
		property System::Nullable<System::IntPtr> MaximumWorkingSetSize
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		// The maximum number of active processes
		property System::Nullable<unsigned int> ActiveProcessLimit
		{
			System::Nullable<unsigned int> get();
			void set(System::Nullable<unsigned int> value);
		}

		// The processesor affinity
		property System::Nullable<System::IntPtr> Affinity
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		// The process priority class
		property System::Nullable<System::Diagnostics::ProcessPriorityClass> PriorityClass
		{
			System::Nullable<System::Diagnostics::ProcessPriorityClass> get();
			void set(System::Nullable<System::Diagnostics::ProcessPriorityClass> value);
		}

		//The Scheduling Class is a number from 0 to 9  
		property System::Nullable<unsigned char> SchedulingClass
		{
			System::Nullable<unsigned char> get();
			void set(System::Nullable<unsigned char> value);
		}

		//Allow child process to use the CREATE_BREAKAWAY_FROM_JOB 
		property bool CanChildProcessBreakAway
		{
			bool get();
			void set(bool value);
		}

		//Allows any process associated with the job to create child processes that are not associated with the job.
		property bool IsChildProcessBreakAway
		{
			bool get();
			void set(bool value);
		}

		//Forces a call to the SetErrorMode function with the SEM_NOGPFAULTERRORBOX flag for each process associated with the job. 
		property bool IsDieOnUnhandledException
		{
			bool get();
			void set(bool value);
		}

		/********************************************************************************************
		* This limit was exposed in Win2k but was not supported by Win2k. Apparently it is not		*
		* supported also in later version of windows. Latest windows version check is Windows Vista	*
		* ////Causes all processes associated with the job to terminate when the last handle to the *
		* job is closed.																			*
		* //property bool IsKillOnJobClose															*
		* //{																						*
		* //	bool get();																			*
		* //	void set(bool value);																*
		* //}																						*
		*********************************************************************************************/

		//specifies the limit for the virtual memory that can be committed by a process
		property System::Nullable<System::IntPtr> ProcessMemoryLimit
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}	

		//specifies the limit for the virtual memory that can be committed for the job
		property System::Nullable<System::IntPtr> JobMemoryLimit
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}	


		//Prevents processes associated with the job from creating desktops and switching desktops using the CreateDesktop and SwitchDesktop functions
		property bool IsDesktopCreationAndSwitchingEnabled
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from calling the ChangeDisplaySettings function.
		property bool IsLimitDisplaySettings
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from calling the ExitWindows or ExitWindowsEx function.
		property bool CanCallExitWindows
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from accessing global atoms. When this flag is used, each job has its own atom table
		property bool CanAccessAtomTable 
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from using USER handles owned by processes not associated with the same job.
		property bool CanUseOutOfJobUserHandles
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from reading data from the clipboard.
		property bool CanReadClipboard
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from changing system parameters by using the SystemParametersInfo function
		property bool CanChangeUISystemParameters
		{
			bool get();
			void set(bool value);
		}

		//Prevents processes associated with the job from writing data to the clipboard.
		property bool CanWriteClipboard
		{
			bool get();
			void set(bool value);
		}

		//Applies a filter to the token when a process impersonates a client
		void FilterSecurityTokenAndPriviliges(HTokenGroup_t sidsToDisable, HPriviligesGroup_t privilegesToDelete, HTokenGroup_t restrictedSids);

		//Prevents any process in the job from using a token that specifies the local administrators group.
		property bool IsAdminProcessAllow
		{
			bool get();
			void set(bool value);
		}

		//Forces processes in the job to run under a specific token
		void RunJobProcessesAs(System::IntPtr hToken);

		//Prevents any process in the job from using a token that was not created with the CreateRestrictedToken function.
		property bool IsAllowOnlyRestrictedTokenProcesses
		{
			bool get();
			void set(bool value);
		}

		void UserHandleGrantAccess(System::IntPtr userHandle, bool bGrant);

		//sets the job object with an absolute timer
		void SetAbsoluteTimer(System::DateTime absoluteDateTime);

		void SetAbsoluteTimer(System::TimeSpan liveTimeSpan);

		//Clear absolute timer
		void ClearAbsoluteTimer();

	private:

		//called by create absolute timer
		void ChangeAbsoluteTimer(System::TimeSpan timerTimeSpan);

		// Specify what you want to happen when the Elapsed event is 
		// raised.
		void OnTimedEvent( Object^ source, System::Timers::ElapsedEventArgs^ e);

		JOBOBJECT_BASIC_LIMIT_INFORMATION QueryBasicInformation();
		void SetBasicInformation(bool hasValue,  JOBOBJECT_BASIC_LIMIT_INFORMATION &basicLimitInformation, DWORD flag);
		bool CheckBasicFlag(const JOBOBJECT_BASIC_LIMIT_INFORMATION &basicLimitInformation, DWORD flag);
		bool CheckBasicFlag(DWORD flag);
		void SetBasicFlag(DWORD flag, bool isOn);
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION QueryExtendedInformation();
		void SetExtendedInformation(bool hasValue,  JOBOBJECT_EXTENDED_LIMIT_INFORMATION &extendedLimitInformation, DWORD flag);
		bool CheckExtendedFlag(const JOBOBJECT_EXTENDED_LIMIT_INFORMATION &extendedLimitInformation, DWORD flag);
		bool CheckExtendedFlag(DWORD flag);
		void SetExtendedFlag(DWORD flag, bool isOn);
		JOBOBJECT_BASIC_UI_RESTRICTIONS QueryBasicUIRestrictions();
		void SetUIRestrictionsFlag(DWORD flag, bool value);
		bool CheckUIRestrictionsFlag(DWORD flag);
		void SetSecurityLimitInformation(bool hasValue,  JOBOBJECT_SECURITY_LIMIT_INFORMATION &securityLimitInformation, DWORD flag);
		JOBOBJECT_SECURITY_LIMIT_INFORMATION QuerySecurityLimitInformation();
		void SetSecurityLimitFlag(DWORD flag, bool value);
		bool CheckSecurityLimitFlag(DWORD flag);

		ref class ProcessPriorityClassConverter
		{
		public:
			ProcessPriorityClassConverter(System::Diagnostics::ProcessPriorityClass processPriorityClass);
			ProcessPriorityClassConverter(DWORD processPriorityClass);
			operator System::Diagnostics::ProcessPriorityClass();
			operator DWORD();
			operator System::Nullable<System::Diagnostics::ProcessPriorityClass>();
			static ProcessPriorityClassConverter();

		private:
			System::Diagnostics::ProcessPriorityClass _managedProcessPriorityClass;
			DWORD _nativeProcessPriorityClass;
			static void FillTables(DWORD nativeValue, System::Diagnostics::ProcessPriorityClass managedValue);
			static System::Collections::Generic::Dictionary<DWORD, System::Diagnostics::ProcessPriorityClass> ^_native2ManagedTable;
			static System::Collections::Generic::Dictionary<System::Diagnostics::ProcessPriorityClass, DWORD> ^_managed2NativeTable;
		};

		JobObject ^_job;
		System::Timers::Timer ^_liveTimer;
	};
}