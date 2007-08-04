
/*******************************************************************************************************  
*
* JobManagement.cpp
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
#pragma warning(disable : 4091) 

#include "JobManagement.h"
#include <vcclr.h>
#include <msclr\lock.h>
#include "JobLimits.h"
#include "IOCounters.h"
#include "util.h"
#include "JobException.h"

namespace JobManagement 
{
	//Throw exception if the host process is in a Job itself
	void JobObject::ProbeForRunningInJob()
	{
		BOOL bInAnyJob;

		::IsProcessInJob(::GetCurrentProcess(), NULL, &bInAnyJob);

		if (bInAnyJob)
			throw gcnew JobException(L"The management process is running inside a Job, it will not be able to assign child processes to a new Job.");
	}

	void JobObject::CreateJob()
	{
		ProbeForRunningInJob();

		MarshalingContext x;

		LPCWSTR pName = MarshalingContext::Managed2NativeString(_name);
		_hJob = ::CreateJobObjectW(NULL, pName);
		if (_hJob == NULL)
		{
			throw gcnew JobException(true);
		}

		_limits = gcnew JobLimits(this);

		//Dont shutdown all process when the Job Management object gets disposed
		IsTerminateJobProcessesOnDispose = false;
	}

	JobObject::~JobObject()
	{
		delete _events;

		if (IsTerminateJobProcessesOnDispose)
			TerminateAllProcesses(0);
	}

	//Create a process in a Job Object sandbox
	System::Diagnostics::Process ^JobObject::CreateProcessMayBreakAway(System::Diagnostics::ProcessStartInfo ^startupInfo)
	{
		System::Diagnostics::Process ^process =
			System::Diagnostics::Process::Start(startupInfo);
	
		//if the newly created process creates a child process before this line. 
		//The child process will not be part of the job.
		//To prevent this call the CreateProcessSecured, notice: CreateProcessSecured is havier
		AssignProcessToJob(process);

		return process;
	}
	
	System::Diagnostics::Process ^JobObject::CreateProcessSecured(System::Diagnostics::ProcessStartInfo ^startupInfo)
	{
		msclr::lock l(this);
		
		if (_inJobProcessActivationServiceClient == nullptr)
			_inJobProcessActivationServiceClient = gcnew InJobProcessActivationServiceClient(this);

		return _inJobProcessActivationServiceClient->CreateChildProcess(startupInfo);
	}

	void JobObject::ShutDownInJobProcessActivationService()
	{
		msclr::lock l(this);

		if (_inJobProcessActivationServiceClient != nullptr)
		{
			delete _inJobProcessActivationServiceClient;
			_inJobProcessActivationServiceClient = nullptr;
		}
	}

	//Terminate all process in the job. You may create new process
	void JobObject::TerminateAllProcesses(unsigned int exitCode)
	{
		BOOL bResult = ::TerminateJobObject(_hJob, exitCode);
		if (bResult != TRUE)
		{
			throw gcnew JobException(true);
		}
	}

	//Return a limit object that can query & set limits on the job
	JobLimits ^JobObject::Limits::get()
	{	
		return _limits;
	}

	//Return an event object that can be use to connect to job related events
	JobEvents ^JobObject::Events::get()
	{	
		if (_events != nullptr)
			return _events;

		msclr::lock l(this);

		if (_events == nullptr)
			_events = gcnew JobEvents(this);

		return _events;
	}

	//The underlying Win32 HANDLE
	System::IntPtr JobObject::Handle::get()
	{
		System::Diagnostics::Debug::Assert(_hJob != NULL, L"The native job object does not exist.");
		return System::IntPtr(_hJob);
	}

	//True if the processes in the job reached the job timeout limit
	bool JobObject::IsJobTimeout::get()
	{
		//Check if the Job Object is in the signal state
		return ::WaitForSingleObject(_hJob, 0) != WAIT_TIMEOUT;
	}

	//Wait for all processes to exit on Job timeout limit
	void JobObject::Join()
	{
		DWORD result = ::WaitForSingleObject(_hJob, INFINITE);
		if (result == WAIT_FAILED)
		{
			throw gcnew JobException(true);
		}
	}

	//Wait for all processes to exit on Job timeout limit
	void JobObject::Join(System::TimeSpan timeout)
	{
		DWORD result = ::WaitForSingleObject(_hJob, (DWORD)(timeout.TotalMilliseconds));
		if (result == WAIT_FAILED)
		{
			throw gcnew JobException(true);
		}
	}

	void JobObject::AssignProcessToJob(System::Diagnostics::Process ^process)
	{
		BOOL bResult = ::AssignProcessToJobObject(_hJob, process->Handle.ToPointer());
			
		if (bResult == FALSE)
		{
			throw gcnew JobException(true);
		}
	}


	//The total amount of user-mode execution time for all active processes associated with the job, as well as all terminated processes no longer associated with the job, in 100-nanosecond ticks. 
	System::TimeSpan JobObject::TotalUserTime::get()
	{	
		msclr::lock l(this);

		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return System::TimeSpan::FromTicks(Convert(basicAccountInformation.TotalUserTime));
	}
		
	//The total amount of kernel-mode execution time for all active processes associated with the job, as well as all terminated processes no longer associated with the job, in 100-nanosecond ticks. 
	System::TimeSpan JobObject::TotalKernelTime::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return System::TimeSpan::FromTicks(Convert(basicAccountInformation.TotalKernelTime));
	}
		
	//The total amount of user-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job user-mode time limit, in 100-nanosecond ticks. 
	System::TimeSpan JobObject::ThisPeriodTotalUserTime::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return System::TimeSpan::FromTicks(Convert(basicAccountInformation.ThisPeriodTotalUserTime));
	}
		
	//The total amount of kernel-mode execution time for all active processes associated with the job (as well as all terminated processes no longer associated with the job) since the last call that set a per-job kernel-mode time limit, in 100-nanosecond ticks. 
	//This member is set to zero on creation of the job, and each time a per-job kernel-mode time limit is established.
	System::TimeSpan JobObject::ThisPeriodTotalKernelTime::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return System::TimeSpan::FromTicks(Convert(basicAccountInformation.ThisPeriodTotalKernelTime));
	}

	//The total number of page faults encountered by all active processes associated with the job, as well as all terminated processes no longer associated with the job. 
	unsigned int JobObject::TotalPageFaultCount::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return basicAccountInformation.TotalPageFaultCount;
	}

	//The total number of processes associated with the job during its lifetime, including those that have terminated. For example, when a process is associated with a job, but the association fails because of a limit violation, this value is incremented. 
	unsigned int JobObject::TotalProcesses::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return basicAccountInformation.TotalProcesses;
	}

	//The total number of processes currently associated with the job. When a process is associated with a job, but the association fails because of a limit violation, this value is temporarily incremented. When the terminated process exits and all references to the process are released, this value is decremented. 
	unsigned int JobObject::ActiveProcesses::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return basicAccountInformation.ActiveProcesses;
	}

	//The total number of processes terminated because of a limit violation.
	unsigned int JobObject::TotalTerminatedProcesses::get()
	{
		JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicAccountInformation = QueryJobInformation<JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, JobObjectBasicAccountingInformation>(_hJob);
		return basicAccountInformation.TotalTerminatedProcesses;
	}

	//The peak memory used by any process ever associated with the job. 
	System::IntPtr JobObject::PeakProcessMemoryUsed::get()
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = 
			QueryJobInformation<JOBOBJECT_EXTENDED_LIMIT_INFORMATION, JobObjectExtendedLimitInformation>(_hJob);
		return System::IntPtr((void *)extendedLimitInformation.PeakProcessMemoryUsed);
	}

	//The peak memory usage of all processes currently associated with the job.
	System::IntPtr JobObject::PeakJobMemoryUsed::get()
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimitInformation = 
			QueryJobInformation<JOBOBJECT_EXTENDED_LIMIT_INFORMATION, JobObjectExtendedLimitInformation>(_hJob);
		return System::IntPtr((void *)extendedLimitInformation.PeakJobMemoryUsed);
	}
		

	//Contains basic accounting and I/O accounting information for a job object.
	JobIOCounters JobObject::IOCounters::get()
	{
		JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION basicAndIoAccounting;
		basicAndIoAccounting = QueryJobInformation<JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION, JobObjectBasicAndIoAccountingInformation>(_hJob);
		return JobIOCounters(basicAndIoAccounting.IoInfo);
	}


	JOBOBJECT_BASIC_PROCESS_ID_LIST *JobObject::QueryJobObjectBasicProcessIdList(unsigned int numberOfEntries)
	{
		size_t dataSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + sizeof(ULONG_PTR) * numberOfEntries;

		JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList = reinterpret_cast<JOBOBJECT_BASIC_PROCESS_ID_LIST *>(::GlobalAlloc(GMEM_FIXED, dataSize)); 
		
		if (pBasicProcessIdList == NULL)
			throw gcnew System::OutOfMemoryException();

		DWORD retLength;
		
		pBasicProcessIdList->NumberOfAssignedProcesses = numberOfEntries;
		BOOL retVal = ::QueryInformationJobObject(_hJob, JobObjectBasicProcessIdList, pBasicProcessIdList, dataSize, &retLength);
		if (retVal == FALSE)
		{
			unsigned int win32Error = ::GetLastError();
			::GlobalFree(reinterpret_cast<HGLOBAL>(pBasicProcessIdList));
			throw gcnew JobException(win32Error);
		}
		return pBasicProcessIdList;
	}

	
	array<System::Diagnostics::Process ^>^ JobObject::ConvertBasicProcessIdListToProcessArray(JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList)
	{
		System::Collections::Generic::List<System::Diagnostics::Process ^> processList;
		for (unsigned int i = 0; i < pBasicProcessIdList->NumberOfProcessIdsInList; ++i)
		{
			try
			{
				System::Diagnostics::Process ^process = System::Diagnostics::Process::GetProcessById(pBasicProcessIdList->ProcessIdList[i]);
				processList.Add(process);
			}
			catch (...)
			{
				System::Diagnostics::Debug::WriteLine(System::String::Format("Fail to create a process object to process id {0}",pBasicProcessIdList->ProcessIdList[i])); 
			}
		}
		return processList.ToArray();
	}
	
	
	JOBOBJECT_BASIC_PROCESS_ID_LIST *JobObject::QueryJobObjectBasicProcessIdList()
	{
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList = QueryJobObjectBasicProcessIdList(100);
	
		if (pBasicProcessIdList->NumberOfProcessIdsInList > pBasicProcessIdList->NumberOfAssignedProcesses)
		{
			unsigned int numberOfEntries = pBasicProcessIdList->NumberOfAssignedProcesses;
			::GlobalFree(reinterpret_cast<HGLOBAL>(pBasicProcessIdList));
			pBasicProcessIdList = NULL;
			pBasicProcessIdList = QueryJobObjectBasicProcessIdList(numberOfEntries);
		}
			return pBasicProcessIdList;
	}

	
	array<System::Diagnostics::Process ^>^ JobObject::ConstructAssignedProcessList()
	{
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList;
		try
		{
			pBasicProcessIdList = QueryJobObjectBasicProcessIdList();
			return ConvertBasicProcessIdListToProcessArray(pBasicProcessIdList);
		}
		finally
		{
			::GlobalFree(reinterpret_cast<HGLOBAL>(pBasicProcessIdList));
		}
	}

	
	array<unsigned int>^ JobObject::ConvertBasicProcessIdListToIdArray(JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList)
	{
		System::Collections::Generic::List<unsigned int> processList;
		for (unsigned int i = 0; i < pBasicProcessIdList->NumberOfProcessIdsInList; ++i)
		{
			processList.Add(pBasicProcessIdList->ProcessIdList[i]);
		}
		return processList.ToArray();
	}

	
	//return a list of processe ids in the job
	array<unsigned int>^ JobObject::GetAssignedProcessList()
	{
		JOBOBJECT_BASIC_PROCESS_ID_LIST *pBasicProcessIdList;
		try
		{
			pBasicProcessIdList = QueryJobObjectBasicProcessIdList();
			return ConvertBasicProcessIdListToIdArray(pBasicProcessIdList);
		}
		finally
		{
			::GlobalFree(reinterpret_cast<HGLOBAL>(pBasicProcessIdList));
		}
	}


	//Determines whether the process is running in the job.
	bool JobObject::IsCurrentProcessInJob::get()
	{
		return IsProcessInJob(System::Diagnostics::Process::GetCurrentProcess());
	}


	//Determines whether the process is running in the job.
	bool JobObject::IsProcessInJob(System::Diagnostics::Process ^process)
	{
		BOOL bResult;

		BOOL bSucceeds = ::IsProcessInJob(process->Handle.ToPointer(), _hJob, &bResult);

		if (!bSucceeds)
			throw gcnew JobException(true);

		return bResult == TRUE;
	}

	/*
		InJobProcessActivationServiceClient
	*/

	JobObject::InJobProcessActivationServiceClient::~InJobProcessActivationServiceClient()
	{
		msclr::lock l(this);

		CloseActivationService();
	}

	//Called also by the ctor
	void JobObject::InJobProcessActivationServiceClient::CreateActivationService()
	{
		msclr::lock l(this);

		System::String ^unique = System::Guid::NewGuid().ToString();
		System::String ^startedEventName = "ProcessActivationServiceStarted" + unique;
		System::String ^finishEventName = "ProcessActivationServiceFinished" + unique;
		System::String ^portName = "Port" + unique;
		_finishEvent = gcnew System::Threading::EventWaitHandle(false, System::Threading::EventResetMode::ManualReset, finishEventName);
		System::Threading::EventWaitHandle ^startedEvent = gcnew System::Threading::EventWaitHandle(false, System::Threading::EventResetMode::ManualReset, startedEventName);
		System::Diagnostics::ProcessStartInfo processStartupInfo(L"InJobProcessCreator.exe", startedEventName + " " + portName + " " + finishEventName);
		processStartupInfo.CreateNoWindow = true;
		processStartupInfo.UseShellExecute = false;
		_serviceProcess = System::Diagnostics::Process::Start(%processStartupInfo);

		if (_serviceProcess == nullptr || _serviceProcess->HasExited)
		{
			throw gcnew JobException("Error creating the Process Activation Service, which responsible to start child process in a job");
		}
		startedEvent->WaitOne();	
		//Assign the newly created process to the job
		_job->AssignProcessToJob(_serviceProcess);
			
		_processActivationService = InJobProcessCreator::ProcessActivationService::GetProcessActivationService(portName);
	
	}

	void JobObject::InJobProcessActivationServiceClient::CloseActivationService()
	{
		msclr::lock l(this);
		try
		{
			if (_finishEvent != nullptr)
			{
				_finishEvent->Set();
				delete _finishEvent;
			}

			if (_serviceProcess != nullptr)
			{
				_serviceProcess->WaitForExit(5000); //wait 5 seconds
				if (!_serviceProcess->HasExited)
					_serviceProcess->Kill();
				delete _serviceProcess;
			}
		}
		catch(System::Exception ^exp)
		{
			throw gcnew JobException("Error when closing activation service", exp);
		}
		finally
		{
			_finishEvent = nullptr;
			_serviceProcess = nullptr;
		}
	}

	System::Diagnostics::Process ^JobObject::InJobProcessActivationServiceClient::CreateChildProcess(System::Diagnostics::ProcessStartInfo ^startupInfo)
	{
		msclr::lock l(this);

		if (!IsActivationServiceAlive)
			CreateActivationService();
		return _processActivationService->CreateChildProcess(InJobProcessCreator::ProcessStartInfoMessage(startupInfo));
	}

	bool JobObject::InJobProcessActivationServiceClient::IsActivationServiceAlive::get()
	{
		msclr::lock l(this);

		try
		{
			return _processActivationService->IsAlive;
		}
		catch (...)
		{
			CloseActivationService();
			return false;
		}
	}
}