#!/bin/bash
set -e

trap 'echo "Code retour = $?"' EXIT

nasm -f elf64 org.asm
ld -o org org.o
./org
