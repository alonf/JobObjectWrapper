using System;
using JobManagement;
using System.Reflection;
using System.Threading;
using System.Diagnostics;

namespace DieOnUnhandledException
{
    class Program
    {
        private static int _zero;
        private static volatile ManualResetEvent _finished = new ManualResetEvent(false);
   
        static void Main(string[] args)
        {
            if (args.Length > 0 && args[0] == "die")
            {
                _zero /= _zero;
            }

            using (JobObject jo = new JobObject("DieOnUnhandledExceptionJobObject"))
            {
                jo.Limits.IsKillOnJobHandleClose = true;
                jo.Limits.IsTerminateJobProcessesOnDispose = true;

                jo.Events.OnNewProcess += Events_OnNewProcess;
                jo.Events.OnAbnormalExitProcess += Events_OnAbnormalExitProcess;

                jo.Limits.IsDieOnUnhandledException = true;

                ProcessStartInfo psi = new ProcessStartInfo(Assembly.GetExecutingAssembly().Location, "die");
                jo.CreateProcessSecured(psi);

                _finished.WaitOne();
            }

        }

        static void Events_OnNewProcess(object sender, NewProcessEventArgs args)
        {
            try
            {
                Console.WriteLine($"Process {args.Win32Name} has been created.");
            }
            // ReSharper disable once EmptyGeneralCatchClause
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
            // ReSharper disable once EmptyGeneralCatchClause
            catch
            {
            }
            _finished.Set();
        }
    }
}
