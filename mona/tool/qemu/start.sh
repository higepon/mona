#!/bin/sh
QEMU=qemu
#QEMU=~/Desktop/qemu-0.12.5/i386-softmmu/qemu
# -k ja option is required by "Caps Lock and Ctrl swap".
# the last virtio disk is used for tests.
$QEMU -m 512 -enable-kvm -k ja -serial file:$HOME/mona-qemu.log -cdrom ../mkimg/mona.iso -fda ../mkimg/mona.img -boot d  -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80 -redir tcp:8007:10.0.2.15:7 \
-drive file=../mkimg/mona.iso,media=cdrom,if=virtio \
-drive file=../mkimg/mona.img,media=disk,if=virtio \
-drive file=../mkimg/test.img,media=disk,if=virtio \
-drive file=../mkimg/fat32_test.img,media=disk,if=virtio \
-drive file=../mkimg/fat32_user.img,media=disk,if=virtio

\
