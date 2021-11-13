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
	"net"
	"os"
	"time"
)

func main() {
	for {
		n, err := net.DialTimeout("tcp", "<beacon_server>", time.Second*5)
		if err != nil {
			println("err", err.Error())
			return
		}
		n.Write([]byte("<beacon_token>"))
		if n.Close(); len(os.Args) == 1 {
			break
		}
		time.Sleep(time.Minute * 2)
	}
}
