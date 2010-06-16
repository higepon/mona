#!/bin/sh
QEMU=qemu
#QEMU=~/Desktop/qemu-0.10.3/i386-softmmu/qemu
$QEMU -k ja -serial file:$HOME/mona-qemu.log -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -no-kvm -boot d -soundhw es1370 --daemonize -nographic -pidfile /tmp/mona.pid -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80
