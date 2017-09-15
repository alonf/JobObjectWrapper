using System;
using JobManagement;
using System.Threading;

namespace EndOfProcessMemory
{
    /// <summary>
    /// Demonstrating the Process Memory Limit usage
    /// </summary>
    class Program
    {
        private static bool _isStop;
        static void Main()
        {
            using (JobObject jo = new JobObject("ProcessMemoryLimitExample"))
            {
                jo.Limits.ProcessMemoryLimit = new IntPtr(1<<22);
                jo.Events.OnProcessMemoryLimit += Events_OnProcessMemoryLimit;

                try 
                {
                    System.Diagnostics.ProcessStartInfo si =
                        new System.Diagnostics.ProcessStartInfo("cmd.exe")
                        {
                            RedirectStandardInput = true,
                            UseShellExecute = false
                        };
                    System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);
                    p.StandardInput.WriteLine(@"cd\");
                    while (!_isStop)
                    {
                        p.StandardInput.WriteLine("dir /s");
                        Thread.Sleep(1500);
                    }
                }
                // ReSharper disable once EmptyGeneralCatchClause
                catch
                {
                }
            }
        }

        /// <summary>
        /// The event handler for the OnProcessMemoryLimit event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        private static void Events_OnProcessMemoryLimit(object sender, ProcessMemoryLimitEventArgs args)
        {
            _isStop = true;
            Thread.Sleep(500);
            Console.WriteLine("Process has reached its memory limit");
        }
    }
}
