#!/usr/bin/bash
# Copyright (C) 2020 - 2022 iDigitalFlame
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

mkdir bin 2> /dev/null

x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/duck.rc -o /tmp/duck-64.o
x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/goose.rc -o /tmp/goose-64.o
x86_64-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/manifest.rc -o /tmp/manifest-64.o

x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
-Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/duck-64.exe /tmp/duck-64.o /tmp/manifest-64.o src/duck.c

x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
-Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/goose-64.exe /tmp/goose-64.o /tmp/manifest-64.o src/goose.c

x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
-Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/nop-64.exe /tmp/manifest-64.o src/nop.c

x86_64-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
-Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/nop-svc-64.exe /tmp/manifest-64.o src/nop_svc.c

# i686-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/duck.rc -o /tmp/duck-32.o
# i686-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/goose.rc -o /tmp/goose-32.o
# i686-w64-mingw32-windres --codepage=0xFDE9 --language=0x409 --output-format=coff -i src/manifest.rc -o /tmp/manifest-32.o

# i686-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
# -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/duck-32.exe /tmp/duck-32.o /tmp/manifest-32.o src/duck.c

# i686-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
# -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/goose-32.exe /tmp/goose-32.o /tmp/manifest-32.o src/goose.c

# i686-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
# -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/goose-32.exe /tmp/goose-32.o /tmp/manifest-32.o src/goose.c

# i686-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
# -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/nop-32.exe /tmp/manifest-32.o src/nop.c

# i686-w64-mingw32-gcc -mwindows -fPIC -lkernel32 -lntdll -Wa,--strip-local-absolute \
# -Wp,-femit-struct-debug-reduced,-O2 -Wl,-x,-s,-nostdlib,--no-insert-timestamp -o bin/nop-svc-32.exe /tmp/manifest-32.o src/nop_svc.c

# rm /tmp/duck-32.o
# rm /tmp/duck-64.o
# rm /tmp/goose-32.o
# rm /tmp/goose-64.o
# rm /tmp/manifest-64.o
