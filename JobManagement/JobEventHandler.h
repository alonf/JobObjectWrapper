
/*******************************************************************************************************
*   JobObjectWrapper
*
* JobEventHandler.h
*
* http://https://github.com/alonf/JobObjectWrapper
*
* This program is licensed under the MIT License.
*
* Alon Fliess
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

	/// <summary>
	/// The base class for all Job Object related event args
	/// </summary>
	public ref class JobEventArgs abstract : public System::EventArgs
	{
	protected:
		/// <summary>
		/// Initializes a new instance of JobEventArgs
		/// </summary>
		/// <param name="messageSpecificValue">A specific value related to the current event. 
		/// Most of the time it is the related process id</param>
		JobEventArgs(unsigned int messageSpecificValue) : _messageSpecificValue(messageSpecificValue) {}

		/// <summary>
		/// A specific value related to the current event. 
		/// Most of the time it is the related process id
		/// </summary>
		/// <returns>A specific value related to the current event. 
		/// Most of the time it is the related process id</returns>
		property unsigned int MessageSpecificValue { unsigned int get() { return _messageSpecificValue; } }

	private:
		const unsigned int _messageSpecificValue;
	};
	
	/// <summary>
	/// Generic delegate for all Job Events
	/// </summary>
	generic<typename TEventArgs> // where TEventArgs : JobEventArgs ^
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

	/// <summary>
	/// a base class for all Job Object event args that has a process argument
	/// </summary>
	public ref class JobProcessEventArgs abstract : public JobEventArgs
	{
	public:
		/// <summary>
		/// The event related process. 
		/// </summary>
		/// <remarks>
		/// Since a race-condition may occur, this value may be null, if the related process has been exited.
		/// It also may refer to a <see cref="System::Diagnostics::Process"/> that 
		/// is no longer valid.
		/// In rare cases (depend on the operating system) the event may be raised by a process that is already gone,
		/// and a new process with the same process id is currently running which has nothing to do with the event.
		/// In those rare cases the <see cref="TheProcess"/> may hold the wrong instance.
		/// </remarks>
		/// <returns>The process that related to the current event</returns>
		property System::Diagnostics::Process ^TheProcess { System::Diagnostics::Process ^get(); }

		/// <summary>
		/// The event related process id.
		/// </summary>
		/// <remarks>
		/// Since a race-condition may occur, this value may be no longer in use (the process has gone).
		/// In rare cases (depend on the operating system) the event may be raised by a process that is already gone,
		/// and a new process with the same process id is currently running which has nothing to do with the event. 
		/// </remarks>
		/// <returns>The event related process id.</returns>
		property unsigned int TheProcessId { unsigned int get(); }

		/// <summary>
		/// The best Win32 name that can be found. It may be the executable name, or the process id.
		/// </summary>
		property System::String ^Win32Name { System::String ^get(); }

		/// <summary>
		/// The full path of the process executable
		/// </summary>
		property System::String ^Win32Path { System::String ^get(); }

	internal:
		JobProcessEventArgs(unsigned int processId) : JobEventArgs(processId), _process(TryGetProcess(processId)) {} 
		
	private:
		static System::Diagnostics::Process ^TryGetProcess(unsigned int processId);
		System::Diagnostics::Process ^_process;
	};
	
	/// <summary>
	/// Exit codes that indicate an abnormal exit of a process in a job
	/// </summary>
	public enum class ExitReasonIds : long long int
	{ 
		NoError = 0,
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
		StackOverflow = STATUS_STACK_OVERFLOW,
		Timeout = STATUS_TIMEOUT
	};

	/// <summary>
	/// This event argument is used to indicate that a process associated with the job abnormal terminated.
	/// </summary>
	public ref class AbnormalExitProcessEventArgs sealed : public JobProcessEventArgs
	{
	public:
		/// <summary>
		/// One of the <see cref="ExitReasonIds"/> values that points to the exit reason.
		/// </summary>
		/// <returns>The exit reason id</returns>
		property ExitReasonIds ExitReasonId { ExitReasonIds get(); } 

		/// <summary>
		/// One of the <see cref="ExitReasonIds"/> names that points to the exit reason
		/// </summary>
		/// <returns>The exit reason name</returns>
		property System::String ^ExitReasonMessage { System::String ^get(); }

		/// <summary>
		/// Initializes a new instance of AbnormalExitProcessEventArgs
		/// </summary>
		/// <param name="messageSpecificValue">The process that has abnormal terminated</param>
		AbnormalExitProcessEventArgs(unsigned int messageSpecificValue) : JobProcessEventArgs(messageSpecificValue) {}	
	
	internal:
		static const unsigned int NativeEvent = JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS; 
	};
	typedef EventEntry<AbnormalExitProcessEventArgs> AbnormalExitProcessEventEntry;

#define JOB_EVENT_ARG(NAME, EVENT) \
	public ref class NAME##EventArgs sealed : public JobEventArgs \
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
	
	/// <summary>
	/// Event argument for the event that indicates that the active process limit has been exceeded. 
	/// </summary>
	JOB_EVENT_ARG(ActiveProcessLimit, JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT);

	/// <summary>
	/// Event argument for the event that indicates that the active process count has been decremented to 0. For example, if the job currently has two active processes, the system sends this message after they both terminate. 
	/// </summary>
	JOB_EVENT_ARG(ActiveProcessZero, JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO);

	/// <summary>
	/// Event argument for the event that indicates that the JOB_OBJECT_POST_AT_END_OF_JOB option is in effect and the end-of-job time limit has been reached. Upon posting this message, the time limit is canceled and the job's processes can continue to run. 
	/// </summary>
	JOB_EVENT_ARG(EndOfJobTime, JOB_OBJECT_MSG_END_OF_JOB_TIME);
	
	/// <summary>
	/// Event argument for the event that indicates that a process has exceeded a per-process time limit. The system sends this message after the process termination has been requested. 
	/// </summary>
	JOB_PROCESS_EVENT_ARG(EndOfProcessTime, JOB_OBJECT_MSG_END_OF_PROCESS_TIME);
 
	/// <summary>
	/// Event argument for the event that indicates that a process associated with the job has exited. 
	/// </summary>
	JOB_PROCESS_EVENT_ARG(ExitProcess,JOB_OBJECT_MSG_EXIT_PROCESS);
 
	/// <summary>
	/// Event argument for the event that indicates that a process associated with the job caused the job to exceed the job-wide memory limit (if one is in effect). 
	/// </summary>
	JOB_PROCESS_EVENT_ARG(JobMemoryLimit, JOB_OBJECT_MSG_JOB_MEMORY_LIMIT);
 
	/// <summary>
	/// Event argument for the event that indicates that a process has been added to the job. Processes added to a job at the time a completion port is associated are also reported. 
	/// </summary>
	JOB_PROCESS_EVENT_ARG(NewProcess, JOB_OBJECT_MSG_NEW_PROCESS);

	/// <summary>
	/// Event argument for the event that indicates that a process associated with the job has exceeded its memory limit (if one is in effect). 
	/// </summary>
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

		/// <summary>
		/// This class exposes the various Job Object events.
		/// </summary>
		/// <remarks> The C++/CLI implementation uses templates and macros. In C# the code is much cleaner, the event args is based on generic delegate.
		/// The Job Object can raise events on many occasions such as: process related to the job exited or terminated,
		/// a new process is born, a number of process limit has been reached, the number of processes in the job dropped to zero,
		/// Job or process has reached its timeout limit or a memory limit has happened.
		/// The first time a client is registered to an event, a new thread is created. 
		/// All events will be invoked with this thread.
		/// For those events that related to a process, the name and process id can be found in the event argument.
		/// Be aware that in some cases, such as process creation or exit, this value may no longer be valid since a built-in race condition. 
		/// </remarks>
		/// <example>
		/// C#: 
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
	public ref class JobEvents
	{
	public:

		/// <summary>
		/// Initializes a new instance of JobEvents
		/// </summary>
		/// <param name="job">The source (<see cref="JobObject"/>) of the event</param>
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

		/// <summary>
		/// Indicates that a process associated with the job exited with an exit code that indicates an abnormal exit.
		/// </summary>
		EVENT_ENTRY(AbnormalExitProcess)

		/// <summary>
		/// Indicates that the active process limit has been exceeded. 
		/// </summary>
		EVENT_ENTRY(ActiveProcessLimit)

		/// <summary>
		/// Indicates that the active process count has been decremented to 0. For example, if the job currently has two active processes, the system sends this message after they both terminate. 
		/// </summary>
		EVENT_ENTRY(ActiveProcessZero);

		/// <summary>
		/// Indicates that the JOB_OBJECT_POST_AT_END_OF_JOB option is in effect and the end-of-job time limit has been reached. Upon posting this message, the time limit is canceled and the job's processes can continue to run. 
		/// </summary>
		EVENT_ENTRY(EndOfJobTime)
	
		/// <summary>
		/// Indicates that a process has exceeded a per-process time limit. The system sends this message after the process termination has been requested. 
		/// </summary>
		EVENT_ENTRY(EndOfProcessTime)
 
		/// <summary>
		/// Indicates that a process associated with the job has exited. 
		/// </summary>
		EVENT_ENTRY(ExitProcess)
 
		/// <summary>
		/// Indicates that a process associated with the job caused the job to exceed the job-wide memory limit (if one is in effect). 
		/// </summary>
		EVENT_ENTRY(JobMemoryLimit)
 
		/// <summary>
		/// Indicates that a process has been added to the job. Processes added to a job at the time a completion port is associated are also reported. 
		/// </summary>
		EVENT_ENTRY(NewProcess)

		/// <summary>
		/// Indicates that a process associated with the job has exceeded its memory limit (if one is in effect). 
		/// </summary>
		EVENT_ENTRY(ProcessMemoryLimit)
	
	private:
		JobEventHandler _jobEventHandler;
	};
}