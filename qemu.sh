#!/bin/sh
set -e
. ./iso.sh
. ./clean-folder.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -k en-us -m 2G -hda sf-os.iso
