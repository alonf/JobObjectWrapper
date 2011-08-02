using System;
using System.Collections.Generic;
using System.Text;
using JobManagement;
using System.Reflection;
using System.Threading;
using System.Diagnostics;

namespace DieOnUnhandledException
{
    class Program
    {
        static int zero = 0;
        static volatile ManualResetEvent finished = new ManualResetEvent(false);
   
        static void Main(string[] args)
        {
            if (args.Length > 0 && args[0] == "die")
            {
                zero /= zero;
            }

            using (JobObject jo = new JobObject("DieOnUnhandledExceptionJobObject"))
            {
                jo.Limits.IsKillOnJobHandleClose = true;
                jo.Limits.IsTerminateJobProcessesOnDispose = true;

                jo.Events.OnNewProcess += new jobEventHandler<NewProcessEventArgs>(Events_OnNewProcess);
                jo.Events.OnAbnormalExitProcess += new jobEventHandler<AbnormalExitProcessEventArgs>(Events_OnAbnormalExitProcess);

                jo.Limits.IsDieOnUnhandledException = true;

                ProcessStartInfo psi = new ProcessStartInfo(Assembly.GetExecutingAssembly().Location, "die");
                jo.CreateProcessSecured(psi);

                finished.WaitOne();
            }

        }

        static void Events_OnNewProcess(object sender, NewProcessEventArgs args)
        {
            try
            {
                Console.WriteLine("Process {0} has been created.", args.Win32Name);
            }
            catch
            {
            }
        }

        static void Events_OnAbnormalExitProcess(object sender, AbnormalExitProcessEventArgs args)
        {
            try
            {
                Console.WriteLine(args.ExitReasonMessage);
            }
            catch
            {
            }
            finished.Set();
        }
    }
}
