
/*******************************************************************************************************  
*
* Program.cs
*
* Copyright 2007 The JobObjectWrapper Team  
* http://www.codeplex.com/JobObjectWrapper/Wiki/View.aspx?title=Team
*
* This program is licensed under the Microsoft Permissive License (Ms-PL).  You should 
* have received a copy of the license along with the source code.  If not, an online copy
* of the license can be found at http://www.codeplex.com/JobObjectWrapper/Project/License.aspx.
*   
*  Notes :
*      - First release by Alon Fliess
********************************************************************************************************/

using System;
using System.Collections.Generic;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting.Channels;
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
            ManualResetEvent.OpenExisting(finishEventName).WaitOne();
        }

        private static void SetEvent(string readyEventName)
        {
            bool bResult = ManualResetEvent.OpenExisting(readyEventName).Set();
            if (bResult == false)
                throw new Exception("Can't set the ready event"); 
        }
    }
}
