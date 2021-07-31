package main

import (
	"net"
	"os"
	"time"
)

// beacon.go
// Creates a beacon, duh.

func main() {
	for {
		n, err := net.DialTimeout("tcp", "<BEACON SERVER IP>:<PORT>", time.Second*5)
		if err != nil {
			println("err", err.Error())
			return
		}
		n.Write([]byte("<BEACON CODDE>"))
		if n.Close(); len(os.Args) == 1 {
			break
		}
		time.Sleep(time.Minute * 2)
	}
}
