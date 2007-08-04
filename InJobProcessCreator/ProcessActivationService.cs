using System;
using System.Collections.Generic;
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
        private unsafe extern static uint lstrlenW(void *p);

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
            ProcessStartInfo processStartInfo = new ProcessStartInfo();
            processStartInfo.Arguments = msg._arguments;
            processStartInfo.CreateNoWindow = msg._createNoWindow;
            processStartInfo.Domain = msg._domain;

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

        private string _arguments;
        private bool _createNoWindow;
        private string _domain;
        private StringDictionary _environmentVariables;
        private bool _errorDialog;
        private IntPtr _errorDialogParentHandle;
        private string _fileName;
        private bool _loadUserProfile;
        private byte[] _password;
        private bool _redirectStandardError;
        private bool _redirectStandardInput;
        private bool _redirectStandardOutput;
        private Encoding _standardErrorEncoding;
        private Encoding _standardOutputEncoding;
        private string _userName;
        private bool _useShellExecute;
        private string _verb;
        private ProcessWindowStyle _windowStyle;
        private string _workingDirectory;

    }

    public unsafe class ProcessActivationService : MarshalByRefObject
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
        static extern bool AssignProcessToJobObject(IntPtr hJob, IntPtr hProcess);

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

        public bool IsAlive
        {
            get
            {
                return true;
            }
        }

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
