package main

import (
	"flag"
	"fmt"
	"strconv"
	"strings"

	"github.com/iDigitalFlame/xmt/util"
)

func main() {
	var u, f, n, x string

	flag.StringVar(&n, "n", "WindowSecProvAPI", "Pipe Name")
	flag.StringVar(&f, "f", "C:\\Windows\\System32\\shellUI32.dll", "Binary List Path")
	flag.StringVar(&x, "x", "RedTeamRules", "Binary List XOR Key")
	flag.StringVar(&u, "u", "http://192.168.0.1:8080/guard", "Last Resort Download URL")
	flag.Parse()

	var (
		r              = util.Rand.Intn(100)
		eU, eF, eN, eX = []byte(u), []byte(f), []byte(n), []byte(x)
	)
	shuffle(r, eU)
	shuffle(r, eF)
	shuffle(r, eN)
	shuffle(r, eX)

	fmt.Printf(
		"\nvar (\n\tv0 = crypto.XOR(x(%d, []byte{%s}))\n\tv1 = x(%d, []byte{%s})\n\tv2 = x(%d, []byte{%s})\n\tv3 = x(%d, []byte{%s})\n)\n\n",
		r, output(eX),
		r, output(eF),
		r, output(eN),
		r, output(eU),
	)
}
func output(b []byte) string {
	var s strings.Builder
	for i := range b {
		if i > 0 {
			s.WriteRune(',')
		}
		s.WriteString(strconv.Itoa(int(b[i])))
	}
	return s.String()
}
func shuffle(r int, b []byte) {
	for i := range b {
		b[i] -= byte(i + r)
	}
}
