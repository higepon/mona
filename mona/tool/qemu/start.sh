#!/bin/sh
if [ "x$MONA_DEVELOPER_HIGEPON" = x ]; then
  QEMU=qemu
else
  QEMU=/home/taro/src/qemu-0.9.0/i386-softmmu/qemu
fi

$QEMU -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d -soundhw pcspk -k ja -serial file:/tmp/qemu_mona.log -soundhw es1370 -net nic -net user -redir tcp:5555:10.0.2.15:5555 -redir udp:5555:10.0.2.15:5555
