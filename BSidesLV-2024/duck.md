# The Duck Service

The Duck service was a fun one this year.

When started, it registered itself as a Critical Process, which would crash Windows
if it died.

The service was written to prevent being stopped and only respond to shutdown requests.

Source in [`Support/src/duck.c`](Support/src/duck.c)

## Startup

Get `SeDebugPrivilege` (required for the registration)

```C
// Enable SeDebugPrivilege on the supplied Token. The DWORD arg can be used to
// disable or enable the privilege.
BOOL DuckDebug(HANDLE t, DWORD s) {
    if (fNtAdjustTokenPrivileges == NULL) {
        return FALSE;
    }
    TOKEN_PRIVILEGES p;
    p.PrivilegeCount = 1;
    p.Privileges[0].Attributes = s;
    p.Privileges[0].Luid.HighPart = 0;
    p.Privileges[0].Luid.LowPart = 0x14; // SE_DEBUG_PRIVILEGE
    DWORD u = 0;
    NTSTATUS r = fNtAdjustTokenPrivileges(t, FALSE, &p, 0x7C, NULL, &u);
    CloseHandle(t);
    if (r == 0) {
        return TRUE;
    }
    return FALSE;
}
```

Set `ProcessBreakOnTermination`

```C
// 0x2 - SE_PRIVILEGE_ENABLED
if (DuckDebug(t, 0x2)) {
    // Only run if the privilege was got.
    // 0x1D - ProcessBreakOnTermination
    DWORD r = 0;
    // See if we have the flag already, if we can't get it, break.
    if (fNtQuery(GetCurrentProcess(), 0x1D, &last, 0x4, &r) > 0) {
        return;
    }
    DWORD v = 1;
    // 'v' value of 'ProcessBreakOnTermination' to be set. 1 == enabled.
    // 0x4 is the buffer size (its a DWORD, so 4 bytes).
    fNtSetInformationProcess(GetCurrentProcess(), 0x1D, &v, 0x4);
    // Release the privilege.
    // 0x0 - SE_PRIVILEGE_DISABLED
    DuckDebug(t, 0);
}
```

Tell SCM that we'll only respond to shutdown requests.

```C
VOID WINAPI DuckStatus(DWORD s) {
    // Only set that we reply to SERVICE_CONTROL_SHUTDOWN
    SERVICE_STATUS v = {0x10, s, 0x4, 0, 0, 0, 0};
    SetServiceStatus(duck, &v);
}
```

Boom!
