using System;
using JobManagement;

namespace EndOfProcessTime
{
    // Demonstrating the process time limit usage
    class Program
    {
        static readonly System.Threading.ManualResetEvent FinishEvent = new System.Threading.ManualResetEvent(false);

        static void Main()
        {
            try
            {
                using (JobObject jo = new JobObject("EndOfProcessTimeExample"))
                {
                    jo.Events.OnEndOfProcessTime += Events_OnEndOfProcessTime;
                    jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMilliseconds(100);

                    System.Diagnostics.ProcessStartInfo si =
                        new System.Diagnostics.ProcessStartInfo("cmd.exe")
                        {
                            RedirectStandardInput = true,
                            UseShellExecute = false
                        };
                    System.Diagnostics.Process p = jo.CreateProcessMayBreakAway(si);
                    
                    p.StandardInput.WriteLine("@for /L %n in (1,1,1000000) do @echo %n");

                    FinishEvent.WaitOne();
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
            FinishEvent.Set();
        }
    }
}
