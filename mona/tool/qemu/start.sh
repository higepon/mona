#!/bin/sh
QEMU=qemu
$QEMU -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d -soundhw pcspk -k ja -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80
