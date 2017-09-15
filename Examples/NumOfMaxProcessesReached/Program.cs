using System;
using JobManagement;
using System.Threading;

namespace NumOfMaxProcessesReached
{
    /// <summary>
    /// Demonstrate the number of active processes limit usage
    /// </summary>
    class Program
    {
        static void Main()
        {
            try
            {
                using (JobObject jo = new JobObject("ProcessLimitExample"))
                {
                    jo.Limits.ActiveProcessLimit = 4;
                    jo.Events.OnActiveProcessLimit += Events_OnActiveProcessLimit;

                    while (true)
                    {
                        System.Diagnostics.ProcessStartInfo si =
                            new System.Diagnostics.ProcessStartInfo("cmd.exe")
                            {
                                RedirectStandardInput = true,
                                UseShellExecute = false
                            };
                        System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);

                        Console.WriteLine($"\nCreated process with id:{p.Id} out of maximum {jo.Limits.ActiveProcessLimit} processes");
                        Thread.Sleep(1500);
                    }

                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }

        /// <summary>
        /// The event handler for the OnActiveProcessLimit event
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        static void Events_OnActiveProcessLimit(object sender, ActiveProcessLimitEventArgs args)
        {
            Console.WriteLine("\nLimit of active processes reached");
        }
    }
}
