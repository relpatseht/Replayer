/************************************************************************************\
 * Replayer - An Andrew Shurney Production                                          *
\************************************************************************************/

/*! \file		ReplayerBackend.cs
 *  \author		Andrew Shurney
 *  \brief		Interfaces with the ReplayInjector (C dll)
 */

using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Replayer
{
    class ReplayerBackend
    {

        private IntPtr data = IntPtr.Zero;
        private IntPtr replayInjector = IntPtr.Zero;

        [StructLayout(LayoutKind.Sequential)]
        internal struct PROCESS_INFORMATION
        {
           public IntPtr hProcess;
           public IntPtr hThread;
           public int dwProcessId;
           public int dwThreadId;
        }
        
        [StructLayout(LayoutKind.Sequential)]
        internal struct STARTUPINFO
        {
            public uint cb;
            public string lpReserved;
            public string lpDesktop;
            public string lpTitle;
            public uint dwX;
            public uint dwY;
            public uint dwXSize;
            public uint dwYSize;
            public uint dwXCountChars;
            public uint dwYCountChars;
            public uint dwFillAttribute;
            public uint dwFlags;
            public short wShowWindow;
            public short cbReserved2;
            public IntPtr lpReserved2;
            public IntPtr hStdInput;
            public IntPtr hStdOutput;
            public IntPtr hStdError;
        }

        internal struct SECURITY_ATTRIBUTES
        {
            public int length;
            public IntPtr lpSecurityDescriptor;
            public bool bInheritHandle;
        }

        [DllImport("ReplayInjector.dll", EntryPoint="CreateReplayInjector", CallingConvention=CallingConvention.Cdecl, CharSet=CharSet.Ansi)]
        static extern IntPtr CreateReplayInjector(ref PROCESS_INFORMATION procInfo);

        [DllImport("ReplayInjector.dll", EntryPoint = "DestroyReplayInjector", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern void DestroyReplayInjector(IntPtr injector);

        [DllImport("ReplayInjector.dll", EntryPoint = "StartProcWithDLL", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern bool StartProcWithDLL(IntPtr injector, 
                                            [MarshalAs(UnmanagedType.LPStr)] String dllDir,
                                            [MarshalAs(UnmanagedType.LPStr)] String dllName,
                                            [MarshalAs(UnmanagedType.LPStr)] String functionName,
                                            [MarshalAs(UnmanagedType.LPStr)] String argument);

        [DllImport("ReplayInjector.dll", EntryPoint = "GetProcId", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        static extern Int32 GetProcId(IntPtr injector);

        [DllImport("kernel32.dll", EntryPoint = "LoadLibraryEx", CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        static extern IntPtr LoadLibraryEx(string dllFilePath, IntPtr hFile, uint dwFlags);

        [DllImport("kernel32.dll", EntryPoint = "FreeLibrary", CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        static extern bool FreeLibrary(IntPtr dllPointer);

        [DllImport("kernel32.dll", EntryPoint="SetDllDirectory", CallingConvention = CallingConvention.Winapi, CharSet = CharSet.Ansi, SetLastError = true)]
        static extern bool SetDllDirectory(string lpPathName);

        const UInt32 LOAD_WITH_ALTERED_SEARCH_PATH = 0x00000008;

        [DllImport("Kernel32.dll", EntryPoint="CreateProcess", CallingConvention=CallingConvention.Winapi, CharSet=CharSet.Ansi)]
        static extern bool CreateProcess([MarshalAs(UnmanagedType.LPStr)] String lpApplicationName,
                                         [MarshalAs(UnmanagedType.LPStr)] String lpCommandLine,
                                         ref SECURITY_ATTRIBUTES lpProcessAttributes, 
                                         ref SECURITY_ATTRIBUTES lpThreadAttributes,
                                         Boolean bInheritHandles, UInt32 dwCreationFlags, IntPtr lpEnvironment,
                                         [MarshalAs(UnmanagedType.LPStr)] String lpCurrentDirectory, 
                                         ref STARTUPINFO lpStartupInfo, 
                                         out PROCESS_INFORMATION lpProcessInformation);

        const UInt32 CREATE_NEW_CONSOLE = 0x00000010;
        const UInt32 CREATE_SUSPENDED = 0x00000004;
        const UInt32 DEBUG_PROCESS = 0x00000001;

        public ReplayerBackend(String exePath, String workingDirectory, String arguments)
        {
            SECURITY_ATTRIBUTES procSecurity = new SECURITY_ATTRIBUTES();
            procSecurity.length = 12;
            procSecurity.bInheritHandle = true;

            STARTUPINFO procStartInfo = new STARTUPINFO();
            PROCESS_INFORMATION procInfo = new PROCESS_INFORMATION();
            procStartInfo.cb = 68;

            if (!CreateProcess(exePath + " " + arguments, null, ref procSecurity, ref procSecurity, true,
                              CREATE_SUSPENDED, IntPtr.Zero, workingDirectory, 
                              ref procStartInfo, out procInfo))
                throw new Exception(exePath + " could not be found or is not a valid executable.");

            Directory.SetCurrentDirectory(Application.StartupPath);
            data = CreateReplayInjector(ref procInfo);
        }

        ~ReplayerBackend()
        {
            Destroy();
            FreeLibrary(replayInjector);
        }

        public void Destroy()
        {
            if(data != IntPtr.Zero)
            {
                DestroyReplayInjector(data);
                data = IntPtr.Zero;
            }
        }

        public bool StartRecording(String pipeName)
        {
            return StartProcWithDLL(data, Application.StartupPath, "Record.dll", "StartRecording", pipeName);
        }

        public bool StartReplaying(String filename)
        {
            return StartProcWithDLL(data, Application.StartupPath, "Replay.dll", "StartReplaying", filename);
        }

        public Process GetProcess()
        {
            return Process.GetProcessById(GetProcId(data));
        }
    }
}
