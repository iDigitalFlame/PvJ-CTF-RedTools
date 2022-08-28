// Copyright (C) 2020 - 2022 iDigitalFlame
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

HANDLE stopDuck;
SERVICE_STATUS_HANDLE duck;

void WINAPI DuckQuack(DWORD);
void WINAPI DuckService(DWORD, LPWSTR*);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        SERVICE_TABLE_ENTRYW t[] = {{L"", &DuckService}, {NULL, NULL}};
        StartServiceCtrlDispatcherW(t);
        return 0;
    }
    STARTUPINFOW s;
    PROCESS_INFORMATION i;
    memset(&i, 0, sizeof(i));
    memset(&s, 0, sizeof(s));
    s.cb = sizeof(STARTUPINFOW);
    if (!CreateProcessW(L"goose.exe", L"C:\\Windows\\System32\\goose.exe", NULL, NULL, TRUE, 0, NULL, NULL, &s, &i)) {
        return 1;
    }
    WaitForSingleObject(i.hProcess, INFINITE);
    CloseHandle(i.hThread);
    CloseHandle(i.hProcess);
    return 0;
}

DWORD WINAPI DuckQuacker(DWORD c, DWORD v, void *d, void *x) {
    switch(c) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        DuckQuack(0x3);
        SetEvent(stopDuck);
        break;
    default:
        DuckQuack(0x4);
        break;
    }
    return 0;
}

void WINAPI DuckQuack(DWORD s) {
    SERVICE_STATUS v = {0x10, s, 0x5, 0, 0, 0, 0};
    SetServiceStatus(duck, &v);
}
void WINAPI DuckService(DWORD argc, LPWSTR *argv) {
    duck = RegisterServiceCtrlHandlerExW(L"DuckService", &DuckQuacker, NULL);
    DuckQuack(0x2);
    stopDuck = CreateEventW(NULL, TRUE, FALSE, NULL);
    DuckQuack(0x4);
    WCHAR v[MAX_PATH];
    int n = GetModuleFileNameW(NULL, (LPWSTR)v, MAX_PATH);
    if (n == 0) {
        CloseHandle(stopDuck);
        DuckQuack(0x1);
        return;
    }
    STARTUPINFOW s;
    PROCESS_INFORMATION i;
    memset(&i, 0, sizeof(i));
    memset(&s, 0, sizeof(s));
    s.cb = sizeof(STARTUPINFOW);
    if (!CreateProcessW(v, v, NULL, NULL, TRUE, 0, NULL, NULL, &s, &i)) {
        CloseHandle(stopDuck);
        DuckQuack(0x1);
        return;
    }
    WaitForSingleObject(stopDuck, INFINITE);
    TerminateProcess(i.hProcess, 0);
    CloseHandle(i.hThread);
    CloseHandle(i.hProcess);
    DuckQuack(0x3);
    CloseHandle(stopDuck);
    DuckQuack(0x1);
}
