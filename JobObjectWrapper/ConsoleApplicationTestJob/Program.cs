
/*******************************************************************************************************  
*
* ConsoleApplicationTestJob
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

using System;
using System.Collections.Generic;
using System.Text;

namespace ConsoleApplicationTestJob
{
    class Program
    {
        static void Main(string[] args)
        {
            JobManagement.JobObject jo = new JobManagement.JobObject("Hello");
            jo.Events.OnAbnormalExitProcess += new JobManagement.jobEventHandler<JobManagement.AbnormalExitProcessEventArgs>(Events_OnAbnormalExitProcess);
            jo.Events.OnActiveProcessLimit += new JobManagement.jobEventHandler<JobManagement.ActiveProcessLimitEventArgs>(Events_OnActiveProcessLimit);
            jo.Events.OnActiveProcessZero += new JobManagement.jobEventHandler<JobManagement.ActiveProcessZeroEventArgs>(Events_OnActiveProcessZero);
            jo.Events.OnEndOfJobTime += new JobManagement.jobEventHandler<JobManagement.EndOfJobTimeEventArgs>(Events_OnEndOfJobTime);
            jo.Events.OnEndOfProcessTime += new JobManagement.jobEventHandler<JobManagement.EndOfProcessTimeEventArgs>(Events_OnEndOfProcessTime);
            jo.Events.OnExitProcess += new JobManagement.jobEventHandler<JobManagement.ExitProcessEventArgs>(Events_OnExitProcess);
            jo.Events.OnJobMemoryLimit += new JobManagement.jobEventHandler<JobManagement.JobMemoryLimitEventArgs>(Events_OnJobMemoryLimit);
            jo.Events.OnNewProcess += new JobManagement.jobEventHandler<JobManagement.NewProcessEventArgs>(Events_OnNewProcess);
            jo.Events.OnProcessMemoryLimit += new JobManagement.jobEventHandler<JobManagement.ProcessMemoryLimitEventArgs>(Events_OnProcessMemoryLimit);
            jo.Limits.ActiveProcessLimit = 6;
            jo.Limits.ProcessMemoryLimit = new IntPtr(40000000);
            Console.WriteLine("{0}", jo.Limits.ProcessMemoryLimit);
            jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMinutes(10); //TimeSpan.FromMilliseconds(100);
            System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo("whoami.exe");
            si.RedirectStandardOutput = true;
            si.UseShellExecute = false;
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

            si.FileName = "calc.exe";
            si.RedirectStandardOutput = false;
            si.RedirectStandardInput = false;
            si.CreateNoWindow = true;
            si.UseShellExecute = true;
            p = jo.CreateProcessSecured(si);

            foreach (System.Diagnostics.Process process in jo.ConstructAssignedProcessList())
            {
                Console.WriteLine(process.ProcessName + " " + process.Id);
            }
            Console.ReadKey();
            jo.TerminateAllProcesses(8);
        }

        static void Events_OnProcessMemoryLimit(object sender, JobManagement.ProcessMemoryLimitEventArgs args)
        {
            Console.WriteLine("Process {0} has reached the per process memory limit", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName);
        }

        static void Events_OnNewProcess(object sender, JobManagement.NewProcessEventArgs args)
        {
            Console.WriteLine("Process {0} has been created", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName);
        }

        static void Events_OnJobMemoryLimit(object sender, JobManagement.JobMemoryLimitEventArgs args)
        {
            Console.WriteLine("Process {0} has reached the job memory limit", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName);
        }

        static void Events_OnExitProcess(object sender, JobManagement.ExitProcessEventArgs args)
        {
            Console.WriteLine("Process {0} has exited", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName);
        }

        static void Events_OnEndOfProcessTime(object sender, JobManagement.EndOfProcessTimeEventArgs args)
        {
            Console.WriteLine("Process {0} has been reached process end of time", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName);
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
            Console.WriteLine("Process {0} has abnormaly exited with exit id: {1} - Message: {2}", args.TheProcess == null ? args.TheProcessId.ToString() : args.TheProcess.ProcessName, args.ExitReasonId, args.ExitReasonMessage);
        }
    }
}
