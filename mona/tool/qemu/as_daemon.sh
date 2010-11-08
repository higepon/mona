#!/bin/sh
QEMU=qemu
#QEMU=~/Desktop/qemu-0.10.3/i386-softmmu/qemu
#$QEMU -k ja -serial file:$HOME/mona-qemu.log -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d -soundhw es1370 --daemonize -nographic -pidfile /tmp/mona.pid  -drive file=/home/taro/mona/tool/mkimg/mona.iso,media=cdrom,if=virtio -net user -net nic,model=virtio -drive file=/home/taro/mona/tool/mkimg/mona.img,media=disk,if=virtio -drive file=/home/taro/mona/tool/mkimg/test.img,media=disk,if=virtio -drive file=/home/taro/mona/tool/mkimg/hdd.img,media=disk,if=virtio -redir tcp:8080:10.0.2.15:80


$QEMU -m 512 -enable-kvm -k ja -serial file:$HOME/mona-qemu.log -cdrom ~/mona/tool/mkimg/mona.iso -fda ~/mona/tool/mkimg/mona.img -boot d  -soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80 -redir tcp:8007:10.0.2.15:7 -redir tcp:8181:10.0.2.15:8181 \
-drive file=$HOME/mona/tool/mkimg/mona.iso,media=cdrom,if=virtio \
-drive file=$HOME/mona/tool/mkimg/mona.img,media=disk,if=virtio \
-drive file=$HOME/mona/tool/mkimg/test.img,media=disk,if=virtio \
-drive file=$HOME/mona/tool/mkimg/fat32_test.img,media=disk,if=virtio \
-drive file=$HOME/mona/tool/mkimg/fat32_user.img,media=disk,if=virtio \
 --daemonize -nographic -pidfile /tmp/mona.pid
