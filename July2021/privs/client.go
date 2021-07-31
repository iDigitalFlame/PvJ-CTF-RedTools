// +build windows

package main

import (
	"os"

	"github.com/iDigitalFlame/xmt/com/pipe"
)

// client.go
// Simple command line binary that connects to a named pipe and sends the args added.

func main() {
	if len(os.Args) < 2 {
		os.Exit(0)
	}

	u, err := pipe.Dial(pipe.Format("<PIPENAME>"))
	if err != nil {
		os.Exit(1)
	}
	u.Write([]byte(os.Args[1]))
	u.Close()
}
