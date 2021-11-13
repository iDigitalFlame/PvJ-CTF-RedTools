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
	"os"

	"github.com/iDigitalFlame/xmt/com/pipe"
)

func main() {
	if len(os.Args) < 2 {
		os.Exit(0)
	}

	u, err := pipe.Dial(pipe.Format("WinSockInitW"))
	if err != nil {
		os.Exit(1)
	}
	u.Write([]byte(os.Args[1]))
	u.Close()
}
