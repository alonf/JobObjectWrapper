using System;
using System.Collections.Generic;
using System.Text;
using JobManagement;
using System.Threading;
using System.Diagnostics;

namespace EndOfJobMemory
{
    class Program
    {
        static bool _isStop = false;

        static void Main(string[] args)
        {
            try
            {
                using (JobObject jo = new JobObject("JobMemoryLimitExample"))
                {
                    jo.Limits.JobMemoryLimit = new IntPtr(30000000);
                    jo.Events.OnJobMemoryLimit += new jobEventHandler<JobMemoryLimitEventArgs>(Events_OnJobMemoryLimit);

                    while (!_isStop)
                    {
                        ProcessStartInfo psi = new ProcessStartInfo("calc.exe");

                        Process proc = jo.CreateProcessMayBreakAway(psi);

                        Thread.Sleep(100);
                    }
                }
            }
            catch (Exception)
            { }
        }

        static void Events_OnJobMemoryLimit(object sender, JobMemoryLimitEventArgs args)
        {
            _isStop = true;
            (sender as JobObject).TerminateAllProcesses(8);
            Console.WriteLine("Job has reacehed its memory limit");
        }
    }
}
