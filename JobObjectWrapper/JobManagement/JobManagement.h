// JobManagement.h

#pragma once
#include <windows.h>
#include "JobEventHandler.h"

//Remove the unicode macro
#undef CreateProcess

namespace JobManagement 
{
	ref class JobLimits;
	value class JobIOCounters;

	public ref class JobObject
	{
	public:
		//Create new Job object. The ctor creates the Win32 Jobobject with empty name
		JobObject() : _name(System::String::Empty) {CreateJob();}

		//Create new Job object. The ctor creates the Win32 Jobobject with a name
		JobObject(System::String ^name) : _name(name) {CreateJob();}

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
		HANDLE _hJob;
		System::String ^_name;
		JobLimits ^_limits;
		JobEvents ^_events;

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
