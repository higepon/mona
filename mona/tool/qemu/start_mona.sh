#!/bin/sh

## For release

qemu -m 512 -enable-kvm -k ja -serial ./mona-qemu.log -cdrom ./mona.iso -fda ./mona.img -boot d  \
-soundhw es1370 -net user -net nic,model=virtio -redir tcp:8080:10.0.2.15:80 -redir tcp:8007:10.0.2.15:7 -redir tcp:8181:10.0.2.15:8181 \
-drive file=./mona.iso,media=cdrom,if=virtio \
-drive file=./mona.img,media=disk,if=virtio \
-drive file=./test.img,media=disk,if=virtio \
-drive file=./fat32_test.img,media=disk,if=virtio \
-drive file=./fat32_user.img,media=disk,if=virtio
