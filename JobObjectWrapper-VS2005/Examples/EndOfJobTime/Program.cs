using System;
using System.Collections.Generic;
using System.Text;
using JobManagement;
using System.Threading;

namespace EndOfJobTime
{
    /// <summary>
    /// This project demonstrates the usage of the job time limit
    /// </summary>
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                using (JobObject jo = new JobObject("EndOfJobTimeExample"))
                {
                    jo.Events.OnEndOfJobTime += new jobEventHandler<EndOfJobTimeEventArgs>(Events_OnEndOfJobTime);
                    jo.Limits.PerJobUserTimeLimit = TimeSpan.FromMilliseconds(100);
                
                    while (!jo.IsJobTimeout)
                    {
                        System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo("cmd.exe");
                        si.RedirectStandardInput = true;
                        si.UseShellExecute = false;
                        System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);

                        p.StandardInput.WriteLine("@for /L %n in (1,1,1000000) do @echo %n");
                        Thread.Sleep(500);
                    }
                    
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// The event handler for the OnEndOfJobTime event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        static void Events_OnEndOfJobTime(object sender, EndOfJobTimeEventArgs args)
        {
            Console.WriteLine("Job has reached it time limit");
        }
    }
}
