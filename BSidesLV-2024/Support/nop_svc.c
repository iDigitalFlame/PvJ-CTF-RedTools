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

HANDLE state;
SERVICE_STATUS_HANDLE run;

void WINAPI UpdateState(DWORD);
void WINAPI NopService(DWORD, LPWSTR*);

int main(int argc, char *argv[]) {
    SERVICE_TABLE_ENTRYW t[] = {{L"", &NopService}, {NULL, NULL}};
    StartServiceCtrlDispatcherW(t);
    return 0;
}

DWORD WINAPI ServiceManager(DWORD c, DWORD v, void *d, void *x) {
    switch(c) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        UpdateState(0x3);
        SetEvent(state);
        break;
    default:
        UpdateState(0x4);
        break;
    }
    return 0;
}

void WINAPI UpdateState(DWORD s) {
    SERVICE_STATUS v = {0x10, s, 0x5, 0, 0, 0, 0};
    SetServiceStatus(run, &v);
}
void WINAPI NopService(DWORD argc, LPWSTR *argv) {
    run = RegisterServiceCtrlHandlerExW(L"", &ServiceManager, NULL);
    UpdateState(0x2);
    state = CreateEventW(NULL, TRUE, FALSE, NULL);
    UpdateState(0x4);
    WaitForSingleObject(state, INFINITE);
    UpdateState(0x3);
    CloseHandle(state);
    UpdateState(0x1);
}
