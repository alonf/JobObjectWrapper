/*******************************************************************************************************  
*   JobObjectWrapper
*
* ProcessActivationService.cs
* 
* http://https://github.com/alonf/JobObjectWrapper
*
* This program is licensed under the MIT License.  
* 
* Alon Fliess
********************************************************************************************************/

using System;
using System.Text;
using System.Diagnostics;
using System.Collections.Specialized;
using System.Security;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting;

namespace InJobProcessCreator
{
    [Serializable]
    public unsafe struct ProcessStartInfoMessage
    {
        [DllImport("kernel32.dll")]
        private static extern uint lstrlenW(void *p);

        public ProcessStartInfoMessage(ProcessStartInfo source)
        {
            _arguments = source.Arguments;
            _createNoWindow = source.CreateNoWindow;
            _domain = source.Domain;
            _environmentVariables = source.EnvironmentVariables;
            _errorDialog = source.ErrorDialog;
            _errorDialogParentHandle = source.ErrorDialogParentHandle;
            _fileName = source.FileName;
            _loadUserProfile = source.LoadUserProfile;

            if (source.Password != null)
            {
                //Copy password
                IntPtr p = Marshal.SecureStringToGlobalAllocUnicode(source.Password);
                byte[] pb = new byte[lstrlenW(p.ToPointer()) + 2];
                Marshal.Copy(p, pb, 0, pb.Length);
                _password = pb;
                Marshal.FreeHGlobal(p);
            }
            else
            {
                _password = new byte[0];
            }


            _redirectStandardError = source.RedirectStandardError;
            _redirectStandardInput = source.RedirectStandardInput;
            _redirectStandardOutput = source.RedirectStandardOutput;
            _standardErrorEncoding = source.StandardErrorEncoding;
            _standardOutputEncoding = source.StandardOutputEncoding;
            _userName = source.UserName;
            _useShellExecute = source.UseShellExecute;
            _verb = source.Verb;
            _windowStyle = source.WindowStyle;
            _workingDirectory = source.WorkingDirectory;
        }
        

        public static implicit operator ProcessStartInfo(ProcessStartInfoMessage msg)
        {
            ProcessStartInfo processStartInfo = new ProcessStartInfo
            {
                Arguments = msg._arguments,
                CreateNoWindow = msg._createNoWindow,
                Domain = msg._domain
            };

            //don't set environment variable if we are going to start the process
            //with ShellExecute
            if (!msg._useShellExecute) 
            {
                processStartInfo.EnvironmentVariables.Clear();

                foreach (System.Collections.DictionaryEntry pair in msg._environmentVariables)
                {
                    processStartInfo.EnvironmentVariables.Add(pair.Key.ToString(), pair.Value.ToString());
                }
            }
             
            processStartInfo.ErrorDialog = msg._errorDialog;
            processStartInfo.ErrorDialogParentHandle = msg._errorDialogParentHandle;
            processStartInfo.FileName = msg._fileName;
            processStartInfo.LoadUserProfile = msg._loadUserProfile;
            if (msg._password.Length != 0)
            {
                fixed (byte* pPassword = &msg._password[0])
                {
                    processStartInfo.Password = new SecureString((char*)pPassword, msg._password.Length);
                }
            }
            else
            {
                processStartInfo.Password = null;
            }

            processStartInfo.RedirectStandardError = msg._redirectStandardError;
            processStartInfo.RedirectStandardInput = msg._redirectStandardInput;
            processStartInfo.RedirectStandardOutput = msg._redirectStandardOutput;
            processStartInfo.StandardErrorEncoding = msg._standardErrorEncoding;
            processStartInfo.StandardOutputEncoding = msg._standardOutputEncoding;
            processStartInfo.UserName = msg._userName;
            processStartInfo.UseShellExecute = msg._useShellExecute;
            processStartInfo.Verb = msg._verb;
            processStartInfo.WindowStyle = msg._windowStyle;
            processStartInfo.WorkingDirectory = msg._workingDirectory;

            return processStartInfo;
        }

        private readonly string _arguments;
        private readonly bool _createNoWindow;
        private readonly string _domain;
        private readonly StringDictionary _environmentVariables;
        private readonly bool _errorDialog;
        private readonly IntPtr _errorDialogParentHandle;
        private readonly string _fileName;
        private readonly bool _loadUserProfile;
        private readonly byte[] _password;
        private readonly bool _redirectStandardError;
        private readonly bool _redirectStandardInput;
        private readonly bool _redirectStandardOutput;
        private readonly Encoding _standardErrorEncoding;
        private readonly Encoding _standardOutputEncoding;
        private readonly string _userName;
        private readonly bool _useShellExecute;
        private readonly string _verb;
        private readonly ProcessWindowStyle _windowStyle;
        private readonly string _workingDirectory;

    }

    public class ProcessActivationService : MarshalByRefObject
    {
        //Make shure that clients can not use 'new'
        internal ProcessActivationService()
        {

        }

        public override object InitializeLifetimeService()
        {
            return null;
        }

        public Process CreateChildProcess(ProcessStartInfoMessage msg)
        {
            return Process.Start(msg);
        }

        public bool IsAlive => true;

        //client side
        public static ProcessActivationService GetProcessActivationService(string portName)
        {
            return (ProcessActivationService)Activator.GetObject(typeof(ProcessActivationService), "ipc://localhost:" + portName + "/ProcessActivationService.rem");
        }

        //Server side
        internal static void RegisterRemotingService(string portName)
        {
            CreateChannel(portName);
            RemotingConfiguration.RegisterWellKnownServiceType(typeof(ProcessActivationService), "ProcessActivationService.rem", WellKnownObjectMode.Singleton);
        }

        private static void CreateChannel(string portName)
        {
            //There should be only one registered channel
            //if (ChannelServices.RegisteredChannels.Length > 0)
            //    return;

            IpcChannel channel = new IpcChannel("localhost:" + portName);
            ChannelServices.RegisterChannel(channel, false);
        }
    }
}
