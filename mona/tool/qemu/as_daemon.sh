#!/bin/sh
QEMU=qemu
#QEMU=~/Desktop/qemu-0.10.3/i386-softmmu/qemu
$QEMU -serial file:/tmp/mona_serial.log -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d  -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80 --daemonize -nographic -pidfile /tmp/mona.pid -no-kqemu #-net user -soundhw pcspk -k
