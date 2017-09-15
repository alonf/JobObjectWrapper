/*******************************************************************************************************  
*   JobObjectWrapper, ConsoleApplicationTestJob
*
* Program.cs
* 
* http://https://github.com/alonf/JobObjectWrapper
*
* This program is licensed under the MIT License.  
* 
* Alon Fliess
********************************************************************************************************/

using System;

namespace ConsoleApplicationTestJob
{
    class Program
    {
        static void Main()
        {
            JobManagement.JobObject jo = new JobManagement.JobObject("Hello");
           
            jo.Events.OnAbnormalExitProcess += Events_OnAbnormalExitProcess;
            jo.Events.OnActiveProcessLimit += Events_OnActiveProcessLimit;
            jo.Events.OnActiveProcessZero += Events_OnActiveProcessZero;
            jo.Events.OnEndOfJobTime += Events_OnEndOfJobTime;
            jo.Events.OnEndOfProcessTime += Events_OnEndOfProcessTime;
            jo.Events.OnExitProcess += Events_OnExitProcess;
            jo.Events.OnJobMemoryLimit += Events_OnJobMemoryLimit;
            jo.Events.OnNewProcess += Events_OnNewProcess;
            jo.Events.OnProcessMemoryLimit += Events_OnProcessMemoryLimit;
            jo.Limits.ActiveProcessLimit = 6;
            jo.Limits.ProcessMemoryLimit = new IntPtr(40000000);
            Console.WriteLine("{0}", jo.Limits.ProcessMemoryLimit);
            jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMinutes(10); //TimeSpan.FromMilliseconds(100);
            System.Diagnostics.ProcessStartInfo si =
                new System.Diagnostics.ProcessStartInfo("whoami.exe")
                {
                    RedirectStandardOutput = true,
                    UseShellExecute = false
                };
            System.Diagnostics.Process p = jo.CreateProcessSecured(si);
            while (!p.StandardOutput.EndOfStream)
            {
                Console.WriteLine(p.StandardOutput.ReadLine());
            }
            jo.CreateProcessMayBreakAway(new System.Diagnostics.ProcessStartInfo("cmd.exe"));

            si.FileName = "cmd.exe";
            si.RedirectStandardOutput = false;
            si.RedirectStandardInput = true;
            si.CreateNoWindow = false;
            p = jo.CreateProcessSecured(si);
            p.StandardInput.WriteLine("dir");
            System.Threading.Thread.Sleep(100);
            p.StandardInput.WriteLine("whoami /all");

            si.FileName = "mspaint.exe";
            si.RedirectStandardOutput = false;
            si.RedirectStandardInput = false;
            si.CreateNoWindow = true;
            si.UseShellExecute = false;
            jo.CreateProcessSecured(si);
            jo.ShutDownInJobProcessActivationService();

            si.FileName = "notepad.exe";
            si.RedirectStandardOutput = false;
            si.RedirectStandardInput = false;
            si.CreateNoWindow = true;
            si.UseShellExecute = true;
            jo.CreateProcessSecured(si);

            foreach (System.Diagnostics.Process process in jo.ConstructAssignedProcessList())
            {
                Console.WriteLine(process.ProcessName + " " + process.Id);
            }
            Console.ReadKey();
            jo.TerminateAllProcesses(8);
        }

        static void Events_OnProcessMemoryLimit(object sender, JobManagement.ProcessMemoryLimitEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has reached the per process memory limit");
        }

        static void Events_OnNewProcess(object sender, JobManagement.NewProcessEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has been created");
        }

        static void Events_OnJobMemoryLimit(object sender, JobManagement.JobMemoryLimitEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has reached the job memory limit");
        }

        static void Events_OnExitProcess(object sender, JobManagement.ExitProcessEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has exited");
        }

        static void Events_OnEndOfProcessTime(object sender, JobManagement.EndOfProcessTimeEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has been reached process end of time");
        }

        static void Events_OnEndOfJobTime(object sender, JobManagement.EndOfJobTimeEventArgs args)
        {
            Console.WriteLine("End of time job has been reached");
        }

        static void Events_OnActiveProcessZero(object sender, JobManagement.ActiveProcessZeroEventArgs args)
        {
            Console.WriteLine("There are no more active processes in the job");
        }

        static void Events_OnActiveProcessLimit(object sender, JobManagement.ActiveProcessLimitEventArgs args)
        {
            Console.WriteLine("Maximum active process limit has been reached");
        }

        static void Events_OnAbnormalExitProcess(object sender, JobManagement.AbnormalExitProcessEventArgs args)
        {
            Console.WriteLine($"Process {args.TheProcess?.ProcessName ?? args.TheProcessId.ToString()} has abnormaly exited with exit id: {args.ExitReasonId} - Message: {args.ExitReasonMessage}");
        }
    }
}
