# BSides Las Vegas 2024

## Overview

C2: [ThunderStorm](https://github.com/iDigitalFlame/ThunderStorm)

Tools/Tech Used

- Userland Hooking
- [UserAdd Service](https://github.com/iDigitalFlame/ThunderStorm/blob/main/jetstream/templates/user_add.go)
  - JetStream "user_add" generator
- Multiple Flurry implants
- Multiple Bolt implants
- PowerShell inline beacons

### Access Methods

- Misconfigured IIS
- AD CS Exploitation
- Weak/Unchanged Passwords
- Tasks/Cron

## IOC List

- [Implants](implants.md)
- [Cache/Support](files.md)
- [Webshell](webshell.asp)

Support Binaries (your favorite Duck service)! are in the [Support/src](Support/src)
folder. You can build these with the [build.sh](Support/build.sh) script
_(Requires MinGW)_
