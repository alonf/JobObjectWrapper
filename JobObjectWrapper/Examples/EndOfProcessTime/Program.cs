using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using JobManagement;

namespace EndOfProcessTime
{
    // Demonstrating the process time limit usage
    class Program
    {
        static System.Threading.ManualResetEvent finishEvent = new System.Threading.ManualResetEvent(false);

        static void Main(string[] args)
        {
            try
            {
                using (JobObject jo = new JobObject("EndOfProcessTimeExample"))
                {
                    jo.Events.OnEndOfProcessTime += new jobEventHandler<EndOfProcessTimeEventArgs>(Events_OnEndOfProcessTime);
                    jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMilliseconds(100);

                    System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo("cmd.exe");
                    si.RedirectStandardInput = true;
                    si.UseShellExecute = false;
                    System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);
                    
                    p.StandardInput.WriteLine("@for /L %n in (1,1,1000000) do @echo %n");

                    finishEvent.WaitOne();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// The event handler for the OnEndOfProcessTime event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        static void Events_OnEndOfProcessTime(object sender, EndOfProcessTimeEventArgs args)
        {
            Console.WriteLine("Process {0} has reached its time limit", args.Win32Name);
            finishEvent.Set();
        }
    }
}
