
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
#include "JobException.h"

//Remove the unicode macro
#undef CreateProcess

namespace JobManagement 
{
	//Foreword declarations
	ref class JobLimits;
	value class JobIOCounters;


	/// <summary>
    /// This is the .Net wrapper class for the Win32 Job Object mechanism
	/// </summary>
    /// Usage Example:
    /// <example>
    ///  using (JobObject jo = new JobObject("JobMemoryLimitExample"))
    ///  {
    ///     jo.Limits.JobMemoryLimit = new IntPtr(30000000);
    ///     jo.Events.OnJobMemoryLimit += new jobEventHandler&lt;JobMemoryLimitEventArgs&gt;(Events_OnJobMemoryLimit);
    ///
    ///     while (!_isStop)
    ///     {
    ///        ProcessStartInfo psi = new ProcessStartInfo("calc.exe");
    ///        Process proc = jo.CreateProcessMayBreakAway(psi);
    ///
    ///        Thread.Sleep(100);
    ///     }
    ///  }
    /// </example>
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
        /// Use this constructor for Interop scenarios.
        /// The Job Object wrapper duplicates the handle, and closes it 
        /// on its dispose method.
        /// The method tries to find the original job object name.
        /// </summary>
        /// <param name="hJob">The Win32 job object handle</param>
		JobObject(System::IntPtr hJob);

		/// <summary>
		/// Dispose the job, this also closes the native job handle
		/// </summary>
		~JobObject();

		/// <summary>
		/// Create a process in a Job Object sandbox.
		/// </summary>
		/// <remarks> If the newly created process creates a child process before entering the job, 
		/// the child process will not be part of the job.</remarks>
		/// <param name="startupInfo">The process startup information</param>
		/// <returns>The newly create Process object</returns>
		System::Diagnostics::Process ^CreateProcessMayBreakAway(System::Diagnostics::ProcessStartInfo ^startupInfo);

		
		/// <summary>
		/// Create a process in a Job Object sandbox; the method uses other process to create the child process.
		/// </summary>
		/// <remarks>Usually creating a process in a job object is a three steps process. 
		/// First we create the new process in a suspend state (using the CREATE_SUSPENDED flag), 
		/// then we assign the process to the Job, and just after that we allow the process to run using ResumeThread.
		/// The problem is that .Net currently does not enable creating processes in a suspend state. 
		/// The chosen solution is to use a child activation service process running in the job. 
		/// Any time we need to create a process in the Job we ask this child process to run the desired process, 
		/// hence implying an execution in a Job. 
		/// </remarks>
		/// <param name="startupInfo">The process startup information</param>
		/// <returns>The return value is a remoting Process proxy. 
		/// If you close the activation service process, you lose the returned process. 
		/// Usually you need this object if you redirect standard input, output or error.</returns>
		System::Diagnostics::Process ^CreateProcessSecured(System::Diagnostics::ProcessStartInfo ^startupInfo);

		/// <summary>
		/// Create a process in a Job Object sandbox.
		/// </summary>
		/// <remarks> If the newly created process creates a child process before entering the job, 
		/// the child process will not be part of the job.</remarks>
		/// <param name="fileName">The file name to run, Starting a process by specifying its file name is similar to typing 
		/// the information in the Run dialog box of the Windows Start menu</param>
		/// <returns>The newly create Process object</returns>
		System::Diagnostics::Process ^CreateProcessMayBreakAway(System::String ^fileName);

		
		/// <summary>
		/// Create a process in a Job Object sandbox; the method uses other process to create the child process.
		/// </summary>
		/// <remarks>Usually creating a process in a job object is a three steps process. 
		/// First we create the new process in a suspend state (using the CREATE_SUSPENDED flag), 
		/// then we assign the process to the Job, and just after that we allow the process to run using ResumeThread.
		/// The problem is that .Net currently does not enable creating processes in a suspend state. 
		/// The chosen solution is to use a child activation service process running in the job. 
		/// Any time we need to create a process in the Job we ask this child process to run the desired process, 
		/// hence implying an execution in a Job. 
		/// </remarks>
		/// <param name="fileName">The file name to run, Starting a process by specifying its file name is similar to typing 
		/// the information in the Run dialog box of the Windows Start menu</param>
		/// <returns>The return value is a remoting Process proxy. 
		/// If you close the activation service process, you lose the returned process. 
		/// Usually you need this object if you redirect standard input, output or error.</returns>
		System::Diagnostics::Process ^CreateProcessSecured(System::String ^fileName);

		/// <summary>
		/// Shut down the process activation service. 
		/// If you call again to any of the CreateProcessSecured methods you get a new process activation service. 
		/// </summary>
		/// <remarks>By calling this method, you invalidate any Process remoting proxy that has returned 
		/// from CreateProcessSecured.</remarks> 
		void ShutDownInJobProcessActivationService();

		/// <summary>
		/// Terminates all processes currently associated with the job.
		/// </summary>
		/// <remarks>
		/// It is not possible for any of the processes associated with the job to postpone or handle the termination. 
		/// It is as if TerminateProcess were called for each process associated with the job
		/// </remarks>
		/// <param name="exitCode">The exit code to be used by all processes and threads in the job object.</param>
		/// <exception cref="JobException"/>
		void TerminateAllProcesses(unsigned int exitCode);

		
		/// <summary>
		/// Use Limits to access the Job Object limit facility.
		/// </summary>
		///<remarks> You can limit processes in the Job in many areas, such as: 
		/// time limit (CPU time and absolute time), Memory limits, force priority and scheduling class, set the process working set,
		/// and force several security related limits. 
		/// Security limits are deprecated on Windows Vista and Server 2008. 
		/// We will try to find a solution on per process basis for those platforms</remarks>
		property JobLimits ^Limits
		{
			JobLimits ^get();
		}

		/// <summary>
		/// This member exposes the various Job Object events.
		/// </summary>
		/// <remarks> The Job Object can raise events on many occasions such as: process related to the job exited or terminated,
		/// a new process is born, a number of process limit has been reached, the number of processes in the job dropped to zero,
		/// Job or process has reached its timeout limit or a memory limit has happened.
		/// The first time a client is registered to an event, a new thread is created. 
		/// All events will be invoked with this thread.
		/// For those events that related to a process, the name and process id can be found in the event argument.
		/// Be aware that in some cases, such as process creation or exit, this value may no longer be valid since a built-in race condition. 
		/// </remarks>
		/// <example>
		/// C#: 
		///
		/// class Program
		///	{
        ///		static System.Threading.ManualResetEvent finishEvent = new System.Threading.ManualResetEvent(false);
		///
		/// 	static void Main(string[] args)
        ///		{
        ///			try
        ///		    {
        ///		        using (JobObject jo = new JobObject("EndOfProcessTimeExample"))
        ///		        {
        ///		            jo.Events.OnEndOfProcessTime += new jobEventHandler&lt;EndOfProcessTimeEventArgs&gt;(Events_OnEndOfProcessTime);
        ///		            jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMilliseconds(100);
		/// 
        ///		            System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo("cmd.exe");
        ///		            si.RedirectStandardInput = true;
        ///		            si.UseShellExecute = false;
        ///		            System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);
        ///             
        ///		            p.StandardInput.WriteLine("@for /L %n in (1,1,1000000) do @echo %n");
		/// 
        ///		            finishEvent.WaitOne();
        ///		        }
        ///		    }
        ///		    catch (Exception e)
        ///		    {
        ///		        Console.WriteLine(e.Message);
        ///		    }
        ///		}
		/// 
        ///		static void Events_OnEndOfProcessTime(object sender, EndOfProcessTimeEventArgs args)
        ///		{
        ///		    Console.WriteLine("Process {0} has reached its time limit", args.Win32Name);
        ///		    finishEvent.Set();
        ///		}
		///	}
		/// </example>
		property JobEvents ^Events
		{
			JobEvents ^get();
		}

		/// <summary>
		/// The handle of the native Win32 Job Object. Do not close this handle. The handle will be closed on dispose.
		/// </summary>
		property System::IntPtr Handle
		{
			System::IntPtr get();
		}

		/// <summary>
		///	True if the processes in the job reached the job timeout limit
		///	</summary>
		///	<remarks> 
		///	This refers only to the CPU based time limit and not to the absolute timer. The value will be reset when a new timeout limit will be set.
		///	</remarks>
		property bool IsJobTimeout
		{
			bool get();
		}


		/// <summary>
		///	The thread will continue when the processes in the job reached the job timeout limit
		///	</summary>
		///	<remarks> 
		///	This refers only to the CPU based time limit and not to the absolute timer. Thread will wait again when a new timeout limit will be set.
		///	</remarks>
		/// <exception cref="JobManagement::JobException"/>
		void Join();		
		
		/// <summary>
		///	The thread will continue when the processes in the job reached the job timeout limit, or when the supplied timeout parameter is reached
		///	</summary>
		///	<remarks> 
		///	This refers only to the CPU based time limit and not to the absolute timer. Thread will wait again when a new timeout limit will be set.
		///	</remarks>
		/// <param name="timeout">The time-out interval. The function returns if the interval elapses, 
		/// even if the processes in the job have not reached the timeout limit.</param>
		/// <returns>
		/// true when the processes in the job have reached the timeout limit.
		/// false when a timeout interval has passed.
		/// </returns>
		/// <exception cref="JobManagement::JobException"/>
		bool Join(System::TimeSpan timeout);

		/// <summary>
		/// Assigns a process to an existing job object.
		/// </summary>
		/// <param name="process">
		/// the process to associate with the job object. The process must not already be assigned to a job. 
		/// </param>
		/// <exception cref="JobManagement::JobException"/>
		void AssignProcessToJob(System::Diagnostics::Process ^process);

		/// <summary>
		/// The total amount of user-mode execution time for all active processes associated with the job, 
		/// as well as all terminated processes no longer associated with the job
		/// </summary>
		property System::TimeSpan TotalUserTime
		{
			System::TimeSpan get();
		}

		/// <summary>
		/// The total amount of kernel-mode execution time for all active processes associated with the job, as well as all terminated processes no longer associated with the job, in 100-nanosecond ticks. 
		/// </summary>
		property System::TimeSpan TotalKernelTime
		{
			System::TimeSpan get();
		}

		/// <summary>
		/// The total amount of user-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job user-mode time limit
		/// </summary>
		property System::TimeSpan ThisPeriodTotalUserTime
		{
			System::TimeSpan get();
		}

		/// <summary>
		/// The total amount of kernel-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job kernel-mode time limit
		/// </summary>
		/// <remarks>
		/// This member is set to zero on creation of the job, and each time a per-job kernel-mode time limit is established.
		/// </remarks>
		property System::TimeSpan ThisPeriodTotalKernelTime
		{
			System::TimeSpan get();
		}

		/// <summary>
		/// The total number of page faults encountered by all active processes associated with the job, as well as all terminated processes no longer associated with the job. 
		/// </summary>
		property unsigned int TotalPageFaultCount
		{
			unsigned int get();
		}

		/// <summary>
		/// The total number of processes associated with the job during its lifetime, including those that have terminated. 
		/// </summary>
		/// <remarks>
		/// when a process is associated with a job, but the association fails because of a limit violation, this value is incremented. 
		/// </remarks>
		property unsigned int TotalProcesses
		{
			unsigned int get();
		}

		/// <summary>
		/// The total number of processes currently associated with the job. 
		/// </summary>
		/// <remarks>
		/// When a process is associated with a job, but the association fails because of a limit violation, this value is temporarily incremented. When the terminated process exits and all references to the process are released, this value is decremented. 
		/// </remarks>
		property unsigned int ActiveProcesses
		{
			unsigned int get();
		}

		/// <summary>
		/// The total number of processes terminated because of a limit violation.
		/// </summary>
		property unsigned int TotalTerminatedProcesses
		{
			unsigned int get();
		}

		/// <summary>
		///  A high water mark, the peak memory used by any process ever associated with the job. 
		/// </summary>
		property System::IntPtr PeakProcessMemoryUsed 
		{
			System::IntPtr get();
		}

		/// <summary>
		/// The peak memory usage of all processes currently associated with the job.
		/// </summary>
		property System::IntPtr PeakJobMemoryUsed 
		{
			System::IntPtr get();
		}

		/// <summary>
		/// Contains basic accounting and I/O accounting information for a job object.
		/// </summary>
		/// <remarks>
		/// Includes information for all processes that have ever been associated with the job, 
		/// in addition to the information for all processes currently associated with the job. 
		/// </remarks>
		property JobIOCounters IOCounters
		{
			JobIOCounters get();
		}

		/// <summary>
		/// Return a list of processes in the job.
		/// </summary>
		/// <remarks>
		/// This method returns only the processes that calling to <see cref="System::Diagnostics::Process::GetProcessById(int)"/> had not failed
		/// </remarks>
		/// <returns>List of processes in the job</returns>
		array<System::Diagnostics::Process ^>^ ConstructAssignedProcessList();

		/// <summary>
		/// return a list of process ids in the job
		/// </summary>
		/// <remarks>
		/// This information may not be valid, since a race-condition may occur. A returned process id may no longer be active.
		/// </remarks>
		/// <returns>List of process ids in the job</returns>
		array<unsigned int>^ GetAssignedProcessList();

		/// <summary>
		/// Determines whether the current process is running in the job.
		/// </summary>
		property bool IsCurrentProcessInJob
		{
			bool get();
		}

		/// <summary>
		/// Determines whether the calling process is running in the job.
		/// </summary>
		bool IsProcessInJob(System::Diagnostics::Process ^process);

	
		/// <summary>
		/// Determines whether the Job was opened and not created
		/// </summary>
		/// <remarks> 
		/// There are two cases that this value is true. 
		///		a)	The object was created with the constructor that gets a Win32 handle to an existing Job Object.
		///		b)	The object was created with a constructor that takes a Job Object name, and the name was already in use.
		/// </remarks>
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

		[System::Runtime::InteropServices::DllImportAttribute(L"ntdll.dll", CharSet=System::Runtime::InteropServices::CharSet::Unicode)]
		static  unsigned int NtQueryObject(HANDLE hObj, int objInfoClass, void* pObjInfo, unsigned int objInfoLength, System::IntPtr dummy);

		HANDLE _hJob;
		System::String ^_name;
		JobLimits ^_limits;
		JobEvents ^_events;
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
