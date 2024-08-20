// Copyright (C) 2020 - 2024 iDigitalFlame
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501

#define NOWH
#define NOMB
#define NOMSG
#define NONLS
#define NOMCX
#define NOIME
#define NOHELP
#define NOCOMM
#define NOICONS
#define NOCRYPT
#define NOKANJI
#define NOSOUND
#define NOCOLOR
#define NOMENUS
#define NOCTLMGR
#define NOMINMAX
#define NOSCROLL
#define NODRAWTEXT
#define NOMETAFILE
#define NOPROFILER
#define NOKEYSTATES
#define NORASTEROPS
#define NOCLIPBOARD
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOWINOFFSETS
#define NOSHOWWINDOW
#define NOTEXTMETRIC
#define NOSYSCOMMANDS
#define NOGDICAPMASKS
#define NOWINMESSAGES
#define NODEFERWINDOWPOS
#define NOVIRTUALKEYCODES
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winternl.h>

typedef NTSTATUS(WINAPI *funcNtAdjustTokenPrivileges)(
    HANDLE TokenHandle,
    BOOLEAN DisableAllPrivileges,
    PTOKEN_PRIVILEGES NewState,
    ULONG BufferLength,
    PTOKEN_PRIVILEGES PreviousState,
    PULONG ReturnLength);
typedef NTSTATUS(WINAPI *funcNtSetInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength);
typedef NTSTATUS(WINAPI *funcNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength);

DWORD last;
HANDLE stopper;
SERVICE_STATUS_HANDLE duck;

VOID DuckSetup();
VOID DuckDisable();
BOOL DuckDebug(HANDLE, DWORD);

VOID WINAPI DuckStatus(DWORD);
VOID WINAPI DuckService(DWORD, LPWSTR *);

HMODULE ntdll;
funcNtAdjustTokenPrivileges fNtAdjustTokenPrivileges;
funcNtSetInformationProcess fNtSetInformationProcess;

int main(int argc, char *argv[]) {
    SERVICE_TABLE_ENTRYW t[] = {{L"", &DuckService}, {NULL, NULL}};
    StartServiceCtrlDispatcherW(t);
    return 0;
}

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

VOID DuckSetup() {
    ntdll = LoadLibraryW(L"ntdll.dll");
    if (ntdll == NULL) {
        return;
    }
    fNtAdjustTokenPrivileges = (funcNtAdjustTokenPrivileges)GetProcAddress(ntdll, "NtAdjustPrivilegesToken");
    fNtSetInformationProcess = (funcNtSetInformationProcess)GetProcAddress(ntdll, "NtSetInformationProcess");
    funcNtQueryInformationProcess fNtQuery = (funcNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (fNtQuery == NULL || fNtAdjustTokenPrivileges == NULL || fNtSetInformationProcess == NULL) {
        return;
    }
    HANDLE t;
    // 0x200E8 - TOKEN_READ (STANDARD_RIGHTS_READ | TOKEN_QUERY) | TOKEN_WRITE
    //            (TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_GROUPS |
    //              TOKEN_ADJUST_DEFAULT)
    if (OpenProcessToken(GetCurrentProcess(), 0x200E8, &t) == 0) {
        return;
    }
    // 0x2 - SE_PRIVILEGE_ENABLED
    if (DuckDebug(t, 0x2)) {
        // 0x1D - ProcessBreakOnTermination
        DWORD r = 0;
        if (fNtQuery(GetCurrentProcess(), 0x1D, &last, 0x4, &r) > 0) {
            return;
        }
        DWORD v = 1;
        fNtSetInformationProcess(GetCurrentProcess(), 0x1D, &v, 0x4);
        // 0x0 - SE_PRIVILEGE_DISABLED
        DuckDebug(t, 0);
    }
    return;
}
VOID DuckDisable() {
    if (last > 0) {
        return;
    }
    HANDLE t;
    // 0x200E8 - TOKEN_READ (STANDARD_RIGHTS_READ | TOKEN_QUERY) | TOKEN_WRITE
    //            (TOKEN_ADJUST_PRIVILEGES | TOKEN_ADJUST_GROUPS |
    //              TOKEN_ADJUST_DEFAULT)
    if (OpenProcessToken(GetCurrentProcess(), 0x200E8, &t) == 0) {
        return;
    }
    // 0x2 - SE_PRIVILEGE_ENABLED
    if (DuckDebug(t, 0x2)) {
        // 0x1D - ProcessBreakOnTermination
        fNtSetInformationProcess(GetCurrentProcess(), 0x1D, &last, 0x4);
        // 0x0 - SE_PRIVILEGE_DISABLED
        DuckDebug(t, 0);
    }
    CloseHandle(t);
    return;
}

DWORD WINAPI DuckUpdate(DWORD c, DWORD v, void *d, void *x) {
    switch (c) {
    case SERVICE_CONTROL_STOP:
        // No stopping
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        DuckStatus(0x3);
        SetEvent(stopper);
        break;
    default:
        DuckStatus(0x4);
        break;
    }
    return 0;
}

VOID WINAPI DuckStatus(DWORD s) {
    // Only set that we reply to SERVICE_CONTROL_SHUTDOWN
    SERVICE_STATUS v = {0x10, s, 0x4, 0, 0, 0, 0};
    SetServiceStatus(duck, &v);
}
VOID WINAPI DuckService(DWORD argc, LPWSTR *argv) {
    duck = RegisterServiceCtrlHandlerExW(L"DuckService", &DuckUpdate, NULL);
    DuckStatus(0x2); // SERVICE_START_PENDING
    DuckSetup();
    stopper = CreateEventW(NULL, TRUE, FALSE, NULL);
    DuckStatus(0x4); // SERVICE_RUNNING
    WaitForSingleObject(stopper, INFINITE);
    DuckStatus(0x3); // SERVICE_STOP_PENDING
    DuckDisable();
    if (ntdll != NULL) {
        FreeLibrary(ntdll);
    }
    CloseHandle(stopper);
    DuckStatus(0x1); // SERVICE_STOPPED
}
