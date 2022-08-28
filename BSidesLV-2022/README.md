# BSides Las Vegas 2022

## Overview

C2: [ThunderStorm](https://github.com/iDigitalFlame/ThunderStorm)

Tools/Tech Used

- Userland Hooking
- [UserAdd Service](https://github.com/iDigitalFlame/ThunderStorm/blob/main/jetstream/templates/user_add.go)
  - JetStream "user_add" generator
- [Privexec/Elevate Service](https://github.com/iDigitalFlame/ThunderStorm/blob/main/jetstream/templates/elevate_server.go)
  - JetStream "elevate" generator (server and client)
- Multiple Flurry implants
- Multiple Bolt implants
- PowerShell inline beacons

## IOC List

- [Binaries](files.txt)
- [Cache/Support](cache.txt)
- [Webshell](webshell.asp)

Support Binaries (your favorite Duck service)! are in the [Support/src](Support/src)
folder. You can build these with the [build.sh](Support/build.sh) script
_(Requires MinGW)_
