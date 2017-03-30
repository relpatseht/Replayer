Replayer and Low Level Debugging Utililities
Andrew Shurney
andrew.shurney@gmail.com

Usage:
The replayer executable can be used from the command line or as a stand alone application.
Pass the "-h" flag as a command line argument for information on command line usage. As a
stand alone application, simply choose the executable you want to record or replay, the
working directory, and then hit whichever button you want. You will be prompted to with a
file dialog to point to the replay file you will read from or write to.

Current Shortcommings:
1) Does not record network traffic. I made minor efforts at this but ran out of time to fix the
   defects and so opted to comment out the network hooks.
2) Does not control order of thread execution. I think this is basically impossible, but not doing
   it causes the replay to break when the number of calls is determined by thread execution order.
3) Runs slower on XP. XP does not support SRW (read/write) locks, so they were emulated with
   critical sections. This is, obviously, much slower.

Solution Structure:
- DebugUtils:
    Contains all solutions part of the low level debugging utilities library.
  - FuncHooker:
      Function detouring library designed to work on x86 and x64 architectures. It has no
      built in facilities to hook functions of a different process. Efforts were made to
      be platform agnostic, but a few unix implementations of classes are still buggy.
  - OSUtilities:
      Low level utilities library used by FuncHooker and RemoteExecution. Has a number
      of useful utility functions for dealing with the operating system and processor.
      Some of these utilities are platform agnostic, some are windows specific.
  - RemoteExecution:
      Library for executing code on a different process. The functions you execute need
      POD arguments, so usage usually consists of forcing the other process to load a library
      and calling a setup function on that.
- Replayer:
    Contains all solutions used in the program replayer
    - RelRecord:
        This DLL is injected into the process being recorded. It hooks most non-deterministic
        functions and streams the data out over a named pipe.
    - RelReplay:
        This DLL is injected into the process being replayed. It hooks most non-deterministic
        functions and uses a stream from an input file as their output.
    - Replayer:
        This is the actual application replaying frontend. It works from the command line and as
        a windows application.
    - ReplayInjector:
        Since the Replayer is a C# application, this C DLL is used to actually inject DLLs into
        the remote process.
    - ReplayUtils:
        This is a library of shared utilities between RelRecord and RelReplay.
- TestCases:
    Contains all solutions used as simple test cases of FuncHooker and RemoteExecution
  - TestDll:
      A simple dll whose functions will be hooked by a different project.
  - TestExe:
      A simple exe whose functions will be hooked by a different project.
  - TestExeHookDll:
      The DLL which will be injected into TestExe containing hooking functions. This is a
      simple test of FuncHooker. Use it as an example.
  - TestExeHooker:
      The application which actually injects TestExeHookDll into TestExe and installs the hooks.
      This is a simple test of RemoteExecution. Use it as an example.
  - TestInProcessHook:
      Small application using and hooking the functions in TestDll. This is a simple test of
      FuncHooker. Use it as an example.
- StdExt:
    Contains extensions to the standard library, such as a poor hash_map implementation and
    a cstdint implementation
- zlib:
   A small library for gzip compression and decompression. I didn't write this.

