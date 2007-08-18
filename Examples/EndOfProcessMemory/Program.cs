using System;
using System.Collections.Generic;
using System.Text;
using JobManagement;
using System.Threading;

namespace EndOfProcessMemory
{
    class Program
    {
        static bool _isStop = false;
        static void Main(string[] args)
        {
            using (JobObject jo = new JobObject("ProcessMemoryLimitExample"))
            {
                jo.Limits.ProcessMemoryLimit = new IntPtr(2150000);
                jo.Events.OnProcessMemoryLimit += new jobEventHandler<ProcessMemoryLimitEventArgs>(Events_OnProcessMemoryLimit);

                try 
                {
                    System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo("cmd.exe");
                    si.RedirectStandardInput = true;
                    si.UseShellExecute = false;
                    System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);
                    p.StandardInput.WriteLine(@"cd\");
                    while (!_isStop)
                    {
                        p.StandardInput.WriteLine("dir /s");
                        Thread.Sleep(1500);
                    }
                }
                catch(Exception)
                {
                }
            }
        }

        static void Events_OnProcessMemoryLimit(object sender, ProcessMemoryLimitEventArgs args)
        {
            _isStop = true;
            Thread.Sleep(500);
            Console.WriteLine("Process has reached its memory limit");
        }
    }
}
