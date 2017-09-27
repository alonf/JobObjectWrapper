using JobManagement;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace NestedJobs
{
    class Program
    {
        static readonly System.Threading.ManualResetEvent FinishEvent = new System.Threading.ManualResetEvent(false);
        static bool nested;

        static void Main(string[] args)
        {
            nested = args.Length > 0;
            try
            {
                using (JobObject jo = new JobObject(nested ? "NestedJobExampleChild" : "NestedJobExampleParent"))
                {
                    jo.Events.OnEndOfProcessTime += Events_OnEndOfProcessTime;
                    Process childProcess;

                    if (nested)
                    {

                        Console.WriteLine("Nested Job");
                        jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMilliseconds(1000);
                        jo.Limits.IsKillOnJobHandleClose = true;

                        System.Diagnostics.ProcessStartInfo si =
                            new System.Diagnostics.ProcessStartInfo("cmd.exe")
                            {
                                RedirectStandardInput = true,
                                UseShellExecute = false
                            };
                        childProcess = jo.CreateProcessMayBreakAway(si);

                        childProcess.StandardInput.WriteLine("@for /L %n in (1,1,1000000) do @echo %n");

                        FinishEvent.WaitOne();
                    }
                    else
                    {
                        Console.WriteLine("Parent Job");
                        jo.Limits.PerProcessUserTimeLimit = TimeSpan.FromMilliseconds(50);
                        jo.Limits.IsKillOnJobHandleClose = true;

                        System.Diagnostics.ProcessStartInfo si =
                            new System.Diagnostics.ProcessStartInfo("NestedJobs.exe")
                            {
                                Arguments = "nested",
                                UseShellExecute = false,
                            };
                        childProcess = jo.CreateProcessMayBreakAway(si);
                        FinishEvent.WaitOne();
                        Console.ReadKey();
                    }
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
            Console.WriteLine("Process {0} has reached its time limit (in {1} job)", args.Win32Name,nested?"child":"parent");
            FinishEvent.Set();
        }
    }
}
