# upswitchd
## Bridge two network interfaces entirely in user-mode.  
## Building
- Install `make` using your distro's package manager (`apt install make`, `pacman -S make`, etc.)
- Clone this repository and run `make` to build the project

## Usage
- Run `./upswitchd <interface1> <interface2>` to create a bridge between the two interfaces
- Press CTRL+C to exit

## Supported operating systems
- Currently only Linux.
- Windows doesn't support raw sockets in user-mode, but there are kernel modules, such as
[WinDivert](https://github.com/basil00/Divert) or [Npcap](https://github.com/nmap/npcap),
that could solve this problem, though.
- Not technically "user-mode", but UEFI support would be easy to implement.
