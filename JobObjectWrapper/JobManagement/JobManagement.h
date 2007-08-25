
/*******************************************************************************************************  
*
* JobManagement.h
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
*      - This is the main header file of the job object wrapper. 
*        It includes the JobObject class declaration. 
*        Three other classes that the JobObject expose are JobLimits, JobEvents, and JobIOCounters 
*        that are declared in other files.  
*
********************************************************************************************************/

#pragma once
#include <windows.h>
#include "JobEventHandler.h"

//Remove the unicode macro
#undef CreateProcess

namespace JobManagement 
{
	//Foreword declarations
	ref class JobLimits;
	value class JobIOCounters;


	/// <summary>
    /// This is the .Net wrapper class for the Win32 class
    /// Usage Example:
    /// <example><![CDATA[
    ///  using (JobObject jo = new JobObject("JobMemoryLimitExample"))
    ///  {
    ///     jo.Limits.JobMemoryLimit = new IntPtr(30000000);
    ///     jo.Events.OnJobMemoryLimit += new jobEventHandler<JobMemoryLimitEventArgs>(Events_OnJobMemoryLimit);
    ///
    ///     while (!_isStop)
    ///     {
    ///        ProcessStartInfo psi = new ProcessStartInfo("calc.exe");
    ///        Process proc = jo.CreateProcessMayBreakAway(psi);
    ///
    ///        Thread.Sleep(100);
    ///     }
    ///  }
    /// ]]></example>
    /// </summary>
	public ref class JobObject
	{
	public:
		/// <summary>
        /// Create a Job Object wrapper with no Win32 object name
        /// </summary>
		JobObject() : _name(System::String::Empty) {CreateJob();}

		 /// <summary>
        /// Create a Job Object wrapper with Win32 object name
        /// </summary>
        /// <param name="name">the name of the Job Object. This name can be use to share the same job object.</param>
        JobObject(System::String ^name) : _name(name) {CreateJob();}

		/// <summary>
        /// Create a Job Object wrapper from WIn32 Job handle. 
        /// Use this constractor for Interop scenario.
        /// The Job Object wrapper duplicates the handle, and closes it 
        /// on its dispose method.
        /// The method tries to find the original job object name.
        /// </summary>
        /// <param name="hJob">The Win32 job object handle</param>
		JobObject(System::IntPtr hJob);

		//Dispose the job
		~JobObject();

		//Create a process in a Job Object sandbox, If the newly created process creates a child process before entering the job, the child process will not be part of the job.
		System::Diagnostics::Process ^CreateProcessMayBreakAway(System::Diagnostics::ProcessStartInfo ^startupInfo);

		//Create a process in a Job Object sandbox, the method uses other process to create the child process
		System::Diagnostics::Process ^CreateProcessSecured(System::Diagnostics::ProcessStartInfo ^startupInfo);

		//Close the process creator helper process
		void ShutDownInJobProcessActivationService();

		//Terminate all process in the job. You may create new process
		void TerminateAllProcesses(unsigned int exitCode);

		//sets the job object with an absolute timer
		void SetAbsoluteTimer(System::DateTime absoluteDateTime);
		void SetAbsoluteTimer(System::TimeSpan liveTimeSpan);

		//Clear absolute timer
		void ClearAbsoluteTimer();

		//Return a limit object that can query & set limits on the job
		property JobLimits ^Limits
		{
			JobLimits ^get();
		}

		//Return an event object that can be use to connect to job related events
		property JobEvents ^Events
		{
			JobEvents ^get();
		}

		//The underlying Win32 HANDLE
		property System::IntPtr Handle
		{
			System::IntPtr get();
		}

		//True if the processes in the job reached the job timeout limit
		property bool IsJobTimeout
		{
			bool get();
		}

		//Wait for all processes to exit on Job timeout limit
		void Join();

		//Wait for all processes to exit on Job timeout limit
		void Join(System::TimeSpan timeout);

		void AssignProcessToJob(System::Diagnostics::Process ^process);

		//The total amount of user-mode execution time for all active processes associated with the job, as well as all terminated processes no longer associated with the job, in 100-nanosecond ticks. 
		property System::TimeSpan TotalUserTime
		{
			System::TimeSpan get();
		}
		
		//The total amount of kernel-mode execution time for all active processes associated with the job, as well as all terminated processes no longer associated with the job, in 100-nanosecond ticks. 
		property System::TimeSpan TotalKernelTime
		{
			System::TimeSpan get();
		}
		
		//The total amount of user-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job user-mode time limit, in 100-nanosecond ticks. 
		property System::TimeSpan ThisPeriodTotalUserTime
		{
			System::TimeSpan get();
		}
		
		//The total amount of kernel-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job kernel-mode time limit, in 100-nanosecond ticks. 
		//This member is set to zero on creation of the job, and each time a per-job kernel-mode time limit is established.
		property System::TimeSpan ThisPeriodTotalKernelTime
		{
			System::TimeSpan get();
		}

		//The total number of page faults encountered by all active processes associated with the job, as well as all terminated processes no longer associated with the job. 
		property unsigned int TotalPageFaultCount
		{
			unsigned int get();
		}

		//The total number of processes associated with the job during its lifetime, including those that have terminated. For example, when a process is associated with a job, but the association fails because of a limit violation, this value is incremented. 
		property unsigned int TotalProcesses
		{
			unsigned int get();
		}

		//The total number of processes currently associated with the job. When a process is associated with a job, but the association fails because of a limit violation, this value is temporarily incremented. When the terminated process exits and all references to the process are released, this value is decremented. 
		property unsigned int ActiveProcesses
		{
			unsigned int get();
		}

		//The total number of processes terminated because of a limit violation.
		property unsigned int TotalTerminatedProcesses
		{
			unsigned int get();
		}

		//The peak memory used by any process ever associated with the job. 
		property System::IntPtr PeakProcessMemoryUsed 
		{
			System::IntPtr get();
		}

		//The peak memory usage of all processes currently associated with the job.
		property System::IntPtr PeakJobMemoryUsed 
		{
			System::IntPtr get();
		}
		
		//Contains basic accounting and I/O accounting information for a job object.
		property JobIOCounters IOCounters
		{
			JobIOCounters get();
		}

		//return a list of processes in the job, this method returns only the processes that 
		//calling to System::Diagnostics::Process::GetProcessById had not failed
		array<System::Diagnostics::Process ^>^ ConstructAssignedProcessList();

		//return a list of processe ids in the job
		array<unsigned int>^ GetAssignedProcessList();
		
		//Determines whether the current process is running in the job.
		property bool IsCurrentProcessInJob
		{
			bool get();
		}

		//Determines whether the process is running in the job.
		bool IsProcessInJob(System::Diagnostics::Process ^process);

		property bool IsTerminateJobProcessesOnDispose;

		//Determines whether the Job was opened and not created 
		property bool IsOpenedAsWin32SharedObject
		{
			bool get();
		}

	internal:
		property HANDLE NativeHandle
		{
			HANDLE get() { return _hJob; }
		}

	private:
		
		//Throw exception if the host process is in a Job itself
		void ProbeForRunningInJob();

		JOBOBJECT_BASIC_PROCESS_ID_LIST *QueryJobObjectBasicProcessIdList(unsigned int numberOfEntries);
		JOBOBJECT_BASIC_PROCESS_ID_LIST *QueryJobObjectBasicProcessIdList();
		array<System::Diagnostics::Process ^>^ ConvertBasicProcessIdListToProcessArray(JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList);
		array<unsigned int>^ ConvertBasicProcessIdListToIdArray(JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList);

		//called by the ctor to create the underlying Win32 job object
		void CreateJob();

		//called by the ctor to wrap the Win32 handle
		void InitializeJobObjectWrapper();

		//Set the process name by its handle. Called by ctor
		void SetProcessNameByHandle();

		//called by create absolute timer
		void ChangeAbsoluteTimer(System::TimeSpan timerTimeSpan);

		// Specify what you want to happen when the Elapsed event is 
		// raised.
		void OnTimedEvent( Object^ /*source*/, System::Timers::ElapsedEventArgs^ /*e*/ );

		[System::Runtime::InteropServices::DllImportAttribute(L"ntdll.dll", CharSet=System::Runtime::InteropServices::CharSet::Unicode)]
		static  unsigned int NtQueryObject(HANDLE hObj, int objInfoClass, void* pObjInfo, unsigned int objInfoLength, System::IntPtr dummy);

		HANDLE _hJob;
		System::String ^_name;
		JobLimits ^_limits;
		JobEvents ^_events;
		System::Timers::Timer ^_liveTimer;
		bool _isOpenedAsWin32SharedObject;

		ref class InJobProcessActivationServiceClient
		{
		public:
			InJobProcessActivationServiceClient(JobObject ^job) : _job(job) {CreateActivationService();}
		
			void CreateActivationService();
			void CloseActivationService();
			System::Diagnostics::Process ^CreateChildProcess(System::Diagnostics::ProcessStartInfo ^startupInfo);
			property bool IsActivationServiceAlive {bool get();}
			~InJobProcessActivationServiceClient();
			
		private:
			System::Threading::EventWaitHandle ^_finishEvent; 
			System::Diagnostics::Process ^_serviceProcess;
			InJobProcessCreator::ProcessActivationService ^_processActivationService;
			JobObject ^_job;
		};

		InJobProcessActivationServiceClient ^_inJobProcessActivationServiceClient;
	};
}
