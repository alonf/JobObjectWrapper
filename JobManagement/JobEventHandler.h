
/*******************************************************************************************************  
*
* JobEventHandler.h
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

namespace JobManagement 
{
	ref class JobObject;

	interface class IEventEntry
	{
		void Invoke(JobObject ^job, unsigned int messageSpecificValue);
		void AddDelegate(System::MulticastDelegate ^del);
		void RemoveDelegate(System::MulticastDelegate ^del);
	};

	public ref class JobEventArgs abstract : public System::EventArgs
	{
	public:
		JobEventArgs(unsigned int messageSpecificValue) : _messageSpecificValue(messageSpecificValue) {}

	protected:
		property unsigned int MessageSpecificValue { unsigned int get() { return _messageSpecificValue; } }

	private:
		const unsigned int _messageSpecificValue;
	};
	
	//Generic delegate for all Job Events
	generic<typename TEventArgs> //where TEventArgs : JobEventArgs
	public delegate void jobEventHandler(System::Object ^sender, TEventArgs args);
	

	template<typename TEventArgs>
	ref class EventEntry sealed : IEventEntry
	{
	public:
		event jobEventHandler<TEventArgs ^> ^OnJobEvent;
		virtual void Invoke(JobObject ^job, unsigned int messageSpecificValue);
		virtual void AddDelegate(System::MulticastDelegate ^del);
		virtual void RemoveDelegate(System::MulticastDelegate ^del);
	};

	public ref class JobProcessEventArgs abstract : public JobEventArgs
	{
	public:
		property System::Diagnostics::Process ^TheProcess { System::Diagnostics::Process ^get(); }
		property unsigned int TheProcessId { unsigned int get(); }

	internal:
		JobProcessEventArgs(unsigned int processId) : JobEventArgs(processId), _process(TryGetProcess(processId)) {} 
		
	private:
		static System::Diagnostics::Process ^TryGetProcess(unsigned int processId);
		System::Diagnostics::Process ^_process;
	};
	
	public enum class ExitReasonIds 
	{ 
		Unknown = -1,
		AccessViolation = STATUS_ACCESS_VIOLATION,
		ArrayBoundsExeeded = STATUS_ARRAY_BOUNDS_EXCEEDED,
		BreakPoint = STATUS_BREAKPOINT,
		ControlCExit = STATUS_CONTROL_C_EXIT,
		DataTypeMisalignmnt = STATUS_DATATYPE_MISALIGNMENT,
		FloatDenormalOperand = STATUS_FLOAT_DENORMAL_OPERAND,
		FloatDivideByZero = STATUS_FLOAT_DIVIDE_BY_ZERO,
		FloatInexactResult = STATUS_FLOAT_INEXACT_RESULT,
		FloatInvalidOperation = STATUS_FLOAT_INVALID_OPERATION,
		FloatMultipleFaults = STATUS_FLOAT_MULTIPLE_FAULTS,
		FloatMultipleTraps = STATUS_FLOAT_MULTIPLE_TRAPS,
		FloatOverflow = STATUS_FLOAT_OVERFLOW,
		FloatStackCheck = STATUS_FLOAT_STACK_CHECK,
		FloatUnderflow = STATUS_FLOAT_UNDERFLOW,
		GuardPageViolation = STATUS_GUARD_PAGE_VIOLATION,
		IllegalInstruction = STATUS_ILLEGAL_INSTRUCTION,
		InPageError = STATUS_IN_PAGE_ERROR,
		InvalidDisposition = STATUS_INVALID_DISPOSITION,
		IntegerDivideByZero = STATUS_INTEGER_DIVIDE_BY_ZERO,
		IntegerOverflow = STATUS_INTEGER_OVERFLOW,
		NoncontinuableException = STATUS_NONCONTINUABLE_EXCEPTION,
		PrivilegedInstruction = STATUS_PRIVILEGED_INSTRUCTION,
		RegNatConsumption = STATUS_REG_NAT_CONSUMPTION,
		SingleStep = STATUS_SINGLE_STEP,
		StackOverflow = STATUS_STACK_OVERFLOW
	};

	//Indicates that a process associated with the job exited with an exit code that indicates an abnormal exit.
	public ref class AbnormalExitProcessEventArgs sealed : public JobProcessEventArgs
	{
	public:
		property ExitReasonIds ExitReasonId { ExitReasonIds get(); } 
		property System::String ^ExitReasonMessage { System::String ^get(); }
		AbnormalExitProcessEventArgs(unsigned int messageSpecificValue) : JobProcessEventArgs(messageSpecificValue) {}	
	
	internal:
		static const unsigned int NativeEvent = JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS; 
	};
	typedef EventEntry<AbnormalExitProcessEventArgs> AbnormalExitProcessEventEntry;

#define JOB_EVENT_ARG(NAME, EVENT) public ref class NAME##EventArgs sealed : public JobEventArgs \
	{ \
	public: \
		NAME##EventArgs(unsigned int messageSpecificValue) : JobEventArgs(messageSpecificValue) {} \
	internal: \
		static const unsigned int NativeEvent = EVENT; 		\
	}; \
	typedef EventEntry<NAME##EventArgs> NAME##EventEntry
	
#define JOB_PROCESS_EVENT_ARG(NAME, EVENT) public ref class NAME##EventArgs sealed : public JobProcessEventArgs \
	{						\
	internal:				\
		static const unsigned int NativeEvent = EVENT; 		\
	public:				\
		NAME##EventArgs(unsigned int messageSpecificValue) : JobProcessEventArgs(messageSpecificValue) {} \
	}; \
	typedef EventEntry<NAME##EventArgs> NAME##EventEntry
	
	//Indicates that the active process limit has been exceeded. 
	JOB_EVENT_ARG(ActiveProcessLimit, JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT);

	//Indicates that the active process count has been decremented to 0. For example, if the job currently has two active processes, the system sends this message after they both terminate. 
	JOB_EVENT_ARG(ActiveProcessZero, JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO);

	//Indicates that the JOB_OBJECT_POST_AT_END_OF_JOB option is in effect and the end-of-job time limit has been reached. Upon posting this message, the time limit is canceled and the job's processes can continue to run. 
	JOB_EVENT_ARG(EndOfJobTime, JOB_OBJECT_MSG_END_OF_JOB_TIME);
	
	//Indicates that a process has exceeded a per-process time limit. The system sends this message after the process termination has been requested. 
	JOB_PROCESS_EVENT_ARG(EndOfProcessTime, JOB_OBJECT_MSG_END_OF_PROCESS_TIME);
 
	//Indicates that a process associated with the job has exited. 
	JOB_PROCESS_EVENT_ARG(ExitProcess,JOB_OBJECT_MSG_EXIT_PROCESS);
 
	//Indicates that a process associated with the job caused the job to exceed the job-wide memory limit (if one is in effect). 
	JOB_PROCESS_EVENT_ARG(JobMemoryLimit, JOB_OBJECT_MSG_JOB_MEMORY_LIMIT);
 
	//Indicates that a process has been added to the job. Processes added to a job at the time a completion port is associated are also reported. 
	JOB_PROCESS_EVENT_ARG(NewProcess, JOB_OBJECT_MSG_NEW_PROCESS);

	//Indicates that a process associated with the job has exceeded its memory limit (if one is in effect). 
	JOB_PROCESS_EVENT_ARG(ProcessMemoryLimit, JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT);
	
	
	ref class JobEventHandler sealed
	{
	public:
		JobEventHandler(JobObject ^job);
		
		template<typename TEventArgs>
		void AddDelegate(jobEventHandler<TEventArgs ^> ^del);
		template<typename TEventArgs>
		void RemoveDelegate(jobEventHandler<TEventArgs ^> ^del);
		
		virtual ~JobEventHandler();
	private:

		void Invoke(unsigned int nativeEvent, unsigned int messageSpecificValue);
		void CreateCompletionPort();
		void CreateListeningThread();
		void ExitListeningThread();
		void ListeningThreadRoutine();

		JobObject ^_job;
		HANDLE _hCompletionPort;
		System::Threading::Thread ^_listeningThread;
		System::Collections::Generic::Dictionary<unsigned int, IEventEntry ^> _eventMap;
	};

	public ref class JobEvents
	{
	public:
		JobEvents(JobObject ^job) : _jobEventHandler(job) {}
		
#define EVENT_ENTRY(EVENT_ARG) \
	event jobEventHandler<EVENT_ARG##EventArgs ^> ^On##EVENT_ARG \
		{ \
			void add(jobEventHandler<EVENT_ARG##EventArgs ^> ^value) \
			{ \
				_jobEventHandler.AddDelegate(value); \
			} \
\
			void remove(jobEventHandler<EVENT_ARG##EventArgs ^> ^value) \
			{ \
				_jobEventHandler.RemoveDelegate(value); \
			} \
		} \

		//Indicates that a process associated with the job exited with an exit code that indicates an abnormal exit.
		EVENT_ENTRY(AbnormalExitProcess)

		//Indicates that the active process limit has been exceeded. 
		EVENT_ENTRY(ActiveProcessLimit)

		//Indicates that the active process count has been decremented to 0. For example, if the job currently has two active processes, the system sends this message after they both terminate. 
		EVENT_ENTRY(ActiveProcessZero);

		//Indicates that the JOB_OBJECT_POST_AT_END_OF_JOB option is in effect and the end-of-job time limit has been reached. Upon posting this message, the time limit is canceled and the job's processes can continue to run. 
		EVENT_ENTRY(EndOfJobTime)
	
		//Indicates that a process has exceeded a per-process time limit. The system sends this message after the process termination has been requested. 
		EVENT_ENTRY(EndOfProcessTime)
 
		//Indicates that a process associated with the job has exited. 
		EVENT_ENTRY(ExitProcess)
 
		//Indicates that a process associated with the job caused the job to exceed the job-wide memory limit (if one is in effect). 
		EVENT_ENTRY(JobMemoryLimit)
 
		//Indicates that a process has been added to the job. Processes added to a job at the time a completion port is associated are also reported. 
		EVENT_ENTRY(NewProcess)

		//Indicates that a process associated with the job has exceeded its memory limit (if one is in effect). 
		EVENT_ENTRY(ProcessMemoryLimit)
	
	private:
		JobEventHandler _jobEventHandler;
	};
}