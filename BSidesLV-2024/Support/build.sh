#!/usr/bin/bash
# Copyright (C) 2020 - 2024 iDigitalFlame
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i manifest.rc -o /tmp/manifest-64.o

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i duck.rc -o /tmp/duck-64.o
x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o ../assets/duck.exe /tmp/duck-64.o /tmp/manifest-64.o duck.c

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i malware.rc -o /tmp/malware-64.o
x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o ../assets/malware.exe /tmp/malware-64.o /tmp/manifest-64.o nop.c

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i bitcoin.rc -o /tmp/bitcoin-64.o
x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o ../assets/bitcoin.exe /tmp/bitcoin-64.o /tmp/manifest-64.o nop.c

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i dolphin.rc -o /tmp/dolphin-64.o
x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o ../assets/dolphin.exe /tmp/dolphin-64.o /tmp/manifest-64.o nop_svc.c

rm /tmp/manifest-64.o
rm /tmp/malware-64.o
rm /tmp/bitcoin-64.o
rm /tmp/dolphin-64.o
