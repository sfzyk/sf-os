#!/bin/sh
set -e
. ./iso.sh

qemu-system-i386 -s -S -m 2G -hda sf-os.iso
