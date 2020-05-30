// +build windows

package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/iDigitalFlame/xmt/npipe"
)

const pipeName = "cryptoproviderapi"

func main() {
	if len(os.Args) == 1 {
		return
	}
	send(strings.Join(os.Args[1:], " "))
}
func send(s string) {
	u, err := npipe.Dial(fmt.Sprintf(`\\.\pipe\%s`, pipeName))
	if err != nil {
		panic(err)
	}
	if _, err := u.Write([]byte(s)); err != nil {
		panic(err)
	}
	u.Close()
}
