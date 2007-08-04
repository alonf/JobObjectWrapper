
/*******************************************************************************************************  
*
* JobEventHandler.cpp
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
#include <windows.h>
#include "JobEventHandler.h"
#include "JobManagement.h"
#include "JobException.h"

namespace JobManagement 
{
	//EventEntry

	template <typename TEventArgs>
	void EventEntry<TEventArgs>::Invoke(JobObject ^job, unsigned int messageSpecificValue)
	{
		try
		{
			TEventArgs eventArgs(messageSpecificValue);
			OnJobEvent(job, %eventArgs);
		}
		catch (System::Exception ^ex)
		{
			System::Diagnostics::Debug::Assert(false, System::String::Format(L"Unhandled exception: {0}", ex->Message));
		}
	}

	template <typename TEventArgs>
	void EventEntry<TEventArgs>::AddDelegate(System::MulticastDelegate ^del)
	{
		System::Diagnostics::Debug::Assert(del->GetType() == jobEventHandler<TEventArgs ^>::typeid);
		OnJobEvent += dynamic_cast<jobEventHandler<TEventArgs ^>^>(del);
	}

	template <typename TEventArgs>
	void EventEntry<TEventArgs>::RemoveDelegate(System::MulticastDelegate ^del)
	{
		System::Diagnostics::Debug::Assert(del->GetType() == jobEventHandler<TEventArgs ^>::typeid);
		OnJobEvent -= dynamic_cast<jobEventHandler<TEventArgs ^>^>(del);
	}

	
	//JobProcessEventArgs

	System::Diagnostics::Process ^JobProcessEventArgs::TheProcess::get()
	{
		if (_process != nullptr)
			return _process;

		_process = TryGetProcess(MessageSpecificValue);
		
		return _process;
	}

	System::Diagnostics::Process ^JobProcessEventArgs::TryGetProcess(unsigned int processId)
	{
		System::Diagnostics::Process ^process = nullptr;
		try
		{
			process = System::Diagnostics::Process::GetProcessById(processId);
		}
		catch (...)
		{
		}
		return process;
	}

	unsigned int JobProcessEventArgs::TheProcessId::get()
	{
		return MessageSpecificValue;
	}
	
	//AbnormalExitProcessEventArgs 

	ExitReasonIds AbnormalExitProcessEventArgs::ExitReasonId::get()
	{
		if (TheProcess == nullptr)
			return ExitReasonIds::Unknown;

		System::Diagnostics::Debug::Assert(System::Enum::IsDefined(ExitReasonIds::typeid, TheProcess->ExitCode), L"the exit code should be one of the enum values");
		
		return ExitReasonIds(TheProcess->ExitCode);

	}

	System::String ^AbnormalExitProcessEventArgs::ExitReasonMessage::get()
	{
		if (TheProcess == nullptr)
		{
			return System::String::Format(L"Process id {0} has abnormal terminated", MessageSpecificValue);
		}

		return System::String::Format(L"Process {0} (id: {1}) has abnormal terminated. The exit reason is: {2}",
			TheProcess->ProcessName, TheProcess->Id, System::Enum::GetName(ExitReasonIds::typeid, ExitReasonId));
	}


	//JobEventHandler

	JobEventHandler::JobEventHandler(JobObject ^job) : _job(job) 
	{
		CreateCompletionPort();
		CreateListeningThread();
	}

	
	template<typename TEventArgs>
	void JobEventHandler::AddDelegate(jobEventHandler<TEventArgs ^> ^del)
	{
		IEventEntry ^entry;

		if (_eventMap.ContainsKey(TEventArgs::NativeEvent))
		{
			entry = _eventMap[TEventArgs::NativeEvent];
		}
		else
		{
			entry = gcnew EventEntry<TEventArgs>();
			_eventMap[TEventArgs::NativeEvent] = entry;
		}

		entry->AddDelegate(del);
	}
		
	template<typename TEventArgs>
	void JobEventHandler::RemoveDelegate(jobEventHandler<TEventArgs ^> ^del)
	{
		if (!_eventMap.ContainsKey(TEventArgs::NativeEvent))
			return;

		//else
		_eventMap[TEventArgs::NativeEvent]->RemoveDelegate(del);
	}
		
	void JobEventHandler::Invoke(unsigned int nativeEvent, unsigned int messageSpecificValue)
	{
		if (!_eventMap.ContainsKey(nativeEvent))
			return;

		//else
		_eventMap[nativeEvent]->Invoke(_job, messageSpecificValue);
	}

	void JobEventHandler::CreateCompletionPort()
	{
		_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		if (_hCompletionPort == NULL)
		{
			throw gcnew JobException(true);
		}
		
		JOBOBJECT_ASSOCIATE_COMPLETION_PORT associateCompletionPort;
		associateCompletionPort.CompletionKey = 0;
		associateCompletionPort.CompletionPort = _hCompletionPort;

		DWORD retVal = ::SetInformationJobObject(_job->NativeHandle, JobObjectAssociateCompletionPortInformation,
									&associateCompletionPort, sizeof(associateCompletionPort));

		if (!retVal)
		{
			throw gcnew JobException(true);
		}
	}

	void JobEventHandler::CreateListeningThread()
	{
		_listeningThread = gcnew System::Threading::Thread(
			gcnew System::Threading::ThreadStart(this, &JobEventHandler::ListeningThreadRoutine));
		_listeningThread->Name = L"Job Event Thread";
		_listeningThread->IsBackground = true;
		_listeningThread->Start();
	}


	void JobEventHandler::ExitListeningThread()
	{
		::PostQueuedCompletionStatus(_hCompletionPort, 0, 1, 0);
		bool bTerminated = _listeningThread->Join(System::TimeSpan::FromSeconds(2));
		if (!bTerminated)
			_listeningThread->Abort();
	}


	void JobEventHandler::ListeningThreadRoutine()
	{
		unsigned int messageSpecificValue;
		unsigned int nativeEvent;
		ULONG_PTR completionKey;

		while (true)
		{
			BOOL bResult = ::GetQueuedCompletionStatus(_hCompletionPort, (LPDWORD)&nativeEvent, &completionKey, (LPOVERLAPPED *)&messageSpecificValue, INFINITE);
			if (!bResult)
			{
				//Check if exception is apropriate here (it is a worker thread)
				throw gcnew JobException(true);
			}

			//end the current thread if exiting
			if (completionKey)
				return;

			this->Invoke(nativeEvent, messageSpecificValue);
		}
	}

	JobEventHandler::~JobEventHandler()
	{
		ExitListeningThread();
		
		::CloseHandle(_hCompletionPort);
	}
}