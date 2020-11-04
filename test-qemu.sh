#!/bin/sh
set -e
. ./iso.sh
. ./clean-folder.sh

qemu-system-i386 -s -S  -k en-us  -m 2G -hda sf-os.iso
