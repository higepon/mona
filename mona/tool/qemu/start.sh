#!/bin/sh
QEMU=qemu
#QEMU=~/Desktop/qemu-0.10.3/i386-softmmu/qemu
$QEMU -serial file:/tmp/hige.log -cdrom ../mkimg/mona.iso -fda ../mkimg/mona.img -boot d  -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80 -redir tcp:8007:10.0.2.15:7 #-net user -soundhw pcspk -k
