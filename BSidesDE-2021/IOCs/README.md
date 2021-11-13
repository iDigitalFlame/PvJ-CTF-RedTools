# IOC files

This is a somewhat complete list of the IOCs and files used during the CTF.

## agents.txt

Process/files that would be connecting to the C2

## launchers.txt

Files that would be used for persistance.

## text.txt

Files that were accessed or read by the agents. Can be used to track execution.

## linked.txt

Files used as cradles for support, or to launch another process. These are basically
DLL files that re-pointed to another DLL.

## support.txt

Files used for support structures that may be called by Windows specific APIs.
