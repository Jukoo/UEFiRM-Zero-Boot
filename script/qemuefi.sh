#!/bin/bash


qemu-system-x86_64 -cpu qemu64 -bios /usr/share/edk2/x64/OVMF.fd \
  -drive file=img/baseufi.disk,if=ide

