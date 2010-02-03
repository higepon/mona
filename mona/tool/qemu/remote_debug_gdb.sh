#!/bin/sh
QEMU=qemu
$QEMU -serial file:/tmp/hige.log -serial tcp::43770,server -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d  -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80  -redir tcp:8007:10.0.2.15:7
