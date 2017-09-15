
/*******************************************************************************************************
*   JobObjectWrapper
*
* JobException.h
*
* http://https://github.com/alonf/JobObjectWrapper
*
* This program is licensed under the MIT License.
*
* Alon Fliess
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
		JobLimits(JobObject ^job) : _job(job) 
		{
			/// <summary>
			/// Shutdown all process when the Job Management object gets disposed
			/// </summary>
			IsTerminateJobProcessesOnDispose = true;
		}

		/// <summary>
		/// The time limit for a process to run under
		/// </summary>
		property System::Nullable<System::TimeSpan> PerProcessUserTimeLimit
		{
			System::Nullable<System::TimeSpan> get();
			void set(System::Nullable<System::TimeSpan> value);
		}

		/// <summary>
		/// The time limit for a job to run under
		/// </summary>
		property System::Nullable<System::TimeSpan> PerJobUserTimeLimit
		{
			System::Nullable<System::TimeSpan> get();
			void set(System::Nullable<System::TimeSpan> value);
		}

		/// <summary>
		/// The minimum size of a working-set
		/// </summary>
		property System::Nullable<System::IntPtr> MinimumWorkingSetSize
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		/// <summary>
		/// The maximum size of a working-set
		/// </summary>
		property System::Nullable<System::IntPtr> MaximumWorkingSetSize
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		/// <summary>
		/// The maximum number of active processes
		/// </summary>
		property System::Nullable<unsigned int> ActiveProcessLimit
		{
			System::Nullable<unsigned int> get();
			void set(System::Nullable<unsigned int> value);
		}

		/// <summary>
		/// The processesor affinity
		/// </summary>
		property System::Nullable<System::IntPtr> Affinity
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}

		/// <summary>
		/// The process priority class
		/// </summary>
		property System::Nullable<System::Diagnostics::ProcessPriorityClass> PriorityClass
		{
			System::Nullable<System::Diagnostics::ProcessPriorityClass> get();
			void set(System::Nullable<System::Diagnostics::ProcessPriorityClass> value);
		}

		/// <summary>
		///The Scheduling Class is a number from 0 to 9  
		/// </summary>
		property System::Nullable<unsigned char> SchedulingClass
		{
			System::Nullable<unsigned char> get();
			void set(System::Nullable<unsigned char> value);
		}

		/// <summary>
		///Allow child process to use the CREATE_BREAKAWAY_FROM_JOB 
		/// </summary>
		property bool CanChildProcessBreakAway
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Allows any process associated with the job to create child processes that are not associated with the job.
		/// </summary>
		property bool IsChildProcessBreakAway
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Forces a call to the SetErrorMode function with the SEM_NOGPFAULTERRORBOX flag for each process associated with the job. 
		/// </summary>
		property bool IsDieOnUnhandledException
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		/// Causes all processes associated with the job to terminate when the last handle to the job is closed
		/// </summary>
		/// <remarks>
		/// Use the <see cref="IsKillOnJobHandleClose"/> in cases when you share the job object handle 
		/// with other processes. this will terminate all the processes assigned to the job only when the last handle is closed.
		/// Using the <see cref="IsKillOnJobHandleClose"/> will shutdown all processes even on abnormal termination of
		/// the process that hold the job handle.
		/// You can also use the <see cref="IsTerminateJobProcessesOnDispose"/> for those cases that you want to be sure that all processes are closed 
		/// when the controling Job Object is disposed, no matter if other processes hold a handle to the same job.
		/// </remarks>
		property bool IsKillOnJobHandleClose	
		{								
			bool get();					
			void set(bool value);		
		}								
		

		/// <summary>
		///specifies the limit for the virtual memory that can be committed by a process
		/// </summary>
		property System::Nullable<System::IntPtr> ProcessMemoryLimit
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}	

		/// <summary>
		///specifies the limit for the virtual memory that can be committed for the job
		/// </summary>
		property System::Nullable<System::IntPtr> JobMemoryLimit
		{
			System::Nullable<System::IntPtr> get();
			void set(System::Nullable<System::IntPtr> value);
		}	


		/// <summary>
		///Prevents processes associated with the job from creating desktops and switching desktops using the CreateDesktop and SwitchDesktop functions
		/// </summary>
		property bool IsDesktopCreationAndSwitchingEnabled
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from calling the ChangeDisplaySettings function.
		/// </summary>
		property bool IsLimitDisplaySettings
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from calling the ExitWindows or ExitWindowsEx function.
		/// </summary>
		property bool CanCallExitWindows
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from accessing global atoms. When this flag is used, each job has its own atom table
		/// </summary>
		property bool CanAccessAtomTable 
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from using USER handles owned by processes not associated with the same job.
		/// </summary>
		property bool CanUseOutOfJobUserHandles
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from reading data from the clipboard.
		/// </summary>
		property bool CanReadClipboard
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from changing system parameters by using the SystemParametersInfo function
		/// </summary>
		property bool CanChangeUISystemParameters
		{
			bool get();
			void set(bool value);
		}

		/// <summary>
		///Prevents processes associated with the job from writing data to the clipboard.
		/// </summary>
		property bool CanWriteClipboard
		{
			bool get();
			void set(bool value);
		}


		/// <summary>
		/// Grants or denies access to a handle to a User object to a job that has a user-interface restriction. 
		/// When access is granted, all processes associated with the job can subsequently recognize and use the handle.
		/// When access is denied, the processes can no longer use the handle
		/// </summary>
		/// <param name="userHandle"> A handle to a User object</param>
		/// <param name="bGrant">If this parameter is true, all processes associated with the job can recognize and use the handle.
		/// If the parameter is false, the processes cannot use the handle. </param>
		void UserHandleGrantAccess(System::IntPtr userHandle, bool bGrant);

		/// <summary>
		/// When disposing the Job Object, this value determined whether to terminated all processes in the job
		/// </summary>
		/// <remarks>
		/// You can also use the <see cref="IsKillOnJobHandleClose"/>. In cases when you share the job object handle 
		/// with other processes, you may want to terminate all the processes only when the last handle is closed.
		/// Using the <see cref="IsKillOnJobHandleClose"/> will shutdown all processes even on abnormal termination of
		/// the process that hold the job handle.
		/// Use <see cref="IsTerminateJobProcessesOnDispose"/> in those cases that you want to be sure the all processes are closed 
		/// when the controling Job Object is disposed, no matter if other processes hold a handle to the same job.
		/// </remarks>
		property bool IsTerminateJobProcessesOnDispose;

		/// <summary>
		/// Set a system timer, that when it goes off, all the processes in the job are terminated
		/// </summary>
		/// <param name="absoluteDateTime">The specific time to kill all processes</param>
		void SetAbsoluteTimer(System::DateTime absoluteDateTime);

		/// <summary>
		/// Set a system timer, that when it goes off, all the processes in the job are terminated
		/// </summary>
		/// <param name="liveTimeSpan">The relative time to kill all processes</param>
		void SetAbsoluteTimer(System::TimeSpan liveTimeSpan);

		/// <summary>
		/// Stop the system timer and clear its values
		/// </summary>
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