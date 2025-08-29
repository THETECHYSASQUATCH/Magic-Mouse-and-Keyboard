using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace DriverInstaller;

class Program
{
    // P/Invoke to update driver via pnputil (we'll shell out) or use DIFx API (simpler: pnputil.exe)
    static int Main(string[] args)
    {
        if (!RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            Console.Error.WriteLine("Windows only.");
            return 1;
        }
        if (!IsAdmin())
        {
            Console.Error.WriteLine("Must run elevated (administrator).");
            return 1;
        }

        string root = AppContext.BaseDirectory;
        // Expect packaged subfolders or allow explicit path arguments
        string srcRoot = args.FirstOrDefault(a => Directory.Exists(a)) ?? Path.Combine(root, "drivers");
        if (!Directory.Exists(srcRoot))
        {
            Console.Error.WriteLine($"Driver folder not found: {srcRoot}");
            return 1;
        }

        // Find INF files
        var infs = Directory.GetFiles(srcRoot, "*.inf", SearchOption.AllDirectories)
            .Where(f => !Path.GetFileName(f).Equals("layout.inf", StringComparison.OrdinalIgnoreCase))
            .ToList();
        if (infs.Count == 0)
        {
            Console.Error.WriteLine("No INF files found under " + srcRoot);
            return 1;
        }

        Console.WriteLine("Found INFs:");
        foreach (var inf in infs) Console.WriteLine("  " + inf);

        bool anyFail = false;
        foreach (var inf in infs)
        {
            Console.WriteLine($"Installing {Path.GetFileName(inf)} ...");
            // pnputil /add-driver <inf> /install /subdirs
            var psi = new ProcessStartInfo("pnputil.exe", $"/add-driver \"{inf}\" /install")
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true
            };
            using var p = Process.Start(psi)!;
            string stdout = p.StandardOutput.ReadToEnd();
            string stderr = p.StandardError.ReadToEnd();
            p.WaitForExit();
            Console.WriteLine(stdout.Trim());
            if (!string.IsNullOrWhiteSpace(stderr)) Console.Error.WriteLine(stderr.Trim());
            if (p.ExitCode != 0)
            {
                Console.Error.WriteLine($"Failed to install {inf} (exit {p.ExitCode}).");
                anyFail = true;
            }
        }

        if (anyFail)
        {
            Console.Error.WriteLine("One or more driver installs failed.");
            return 2;
        }

        Console.WriteLine("All drivers processed.");
        return 0;
    }

    static bool IsAdmin()
    {
        try
        {
            var wi = System.Security.Principal.WindowsIdentity.GetCurrent();
            var wp = new System.Security.Principal.WindowsPrincipal(wi);
            return wp.IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator);
        }
        catch { return false; }
    }
}
