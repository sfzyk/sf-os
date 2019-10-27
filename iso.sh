#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/sf-os.kernel isodir/boot/sf-os.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "sf-os" {
	multiboot /boot/sf-os.kernel
}
EOF
grub-mkrescue -o sf-os.iso isodir
