#!/bin/bash
set -e

trap 'echo "Code retour = $?"' EXIT

nasm -f elf64 ../build_asm/asm/org.asm -o ../build_asm/obj/org.o
ld -o ../bin/org ../build_asm/obj/org.o
../bin/org