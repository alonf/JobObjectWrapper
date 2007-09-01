using System;
using System.Collections.Generic;
using System.Text;
using JobManagement;

namespace KillAllProcessesOnJobClose
{
    class Program
    {
        static void Main(string[] args)
        {
            using (JobObject jo = new JobObject())
            {
                jo.Limits.IsKillOnJobHandleClose = true;

                for (int i = 0; i < 5; ++i)
                {
                    jo.CreateProcessMayBreakAway("mspaint.exe");
                    jo.CreateProcessMayBreakAway("calc.exe");
                    jo.CreateProcessMayBreakAway("notepad.exe"); ;
                }

                Console.WriteLine("Press any key to continue...");
                Console.ReadKey();

            }
        }
    }
}
