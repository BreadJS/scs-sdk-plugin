using System;
using System.Diagnostics;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;

namespace SCSSdkClient
{
    /// <summary>
    /// Helper class to handle sandboxed shared memory access
    /// </summary>
    public static class SharedMemoryHelper
    {
        /// <summary>
        /// Find the PID of the SCS game process
        /// </summary>
        public static int FindGamePid()
        {
            foreach (var process in Process.GetProcessesByName("eurotrucks2"))
            {
                return process.Id;
            }
            foreach (var process in Process.GetProcessesByName("amtrucks"))
            {
                return process.Id;
            }
            return -1;
        }

        /// <summary>
        /// Get the actual shared memory path (handles sandbox case)
        /// </summary>
        public static string GetSharedMemoryPath(string defaultName)
        {
            // On Windows, use default
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                return defaultName;
            }

            // On Linux, first check if normal shared memory exists
            string normalPath = "/dev/shm/" + defaultName.TrimStart('/');
            if (File.Exists(normalPath))
            {
                return normalPath;
            }

            // Check if game is running in a sandbox
            int pid = FindGamePid();
            if (pid > 0)
            {
                string sandboxPath = $"/proc/{pid}/root/dev/shm/{defaultName.TrimStart('/')}";
                if (File.Exists(sandboxPath))
                {
                    return sandboxPath;
                }
            }

            // Fall back to default
            return normalPath;
        }

        /// <summary>
        /// Open shared memory as a file stream (works with sandbox)
        /// </summary>
        public static FileStream OpenSharedMemory(string path, uint size)
        {
            return new FileStream(path, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite, (int)size, FileOptions.SequentialScan);
        }
    }
}
