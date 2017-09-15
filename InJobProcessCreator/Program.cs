/*******************************************************************************************************  
*   Program.cs
*
* IOCounters.h
* 
* http://https://github.com/alonf/JobObjectWrapper
*
* This program is licensed under the MIT License.  
* 
* Alon Fliess
********************************************************************************************************/

using System;
using System.Threading;

namespace InJobProcessCreator
{
    class Program
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="args">args[0] is the name of the ready event handle, arg[1] is the IPC address (port name), arg[2] is the finish event</param>
        /// <returns></returns>
        static int Main(string[] args)
        {
            //System.Diagnostics.Debugger.Break();
            try
            {
                if (args.Length < 3)
                    return -1;


                ProcessActivationService.RegisterRemotingService(args[1]);
                SetEvent(args[0]);
                WaitForExitEvent(args[2]);
                return 0;

            }
            catch (Exception)
            {
                return -2;
            }
        }

        private static void WaitForExitEvent(string finishEventName)
        {
            EventWaitHandle.OpenExisting(finishEventName).WaitOne();
        }

        private static void SetEvent(string readyEventName)
        {
            bool bResult = EventWaitHandle.OpenExisting(readyEventName).Set();
            if (bResult == false)
                throw new Exception("Can't set the ready event"); 
        }
    }
}
