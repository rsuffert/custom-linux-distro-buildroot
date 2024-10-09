#!/bin/bash

qemu-system-i386 --device e1000,netdev=eth0,mac=aa:bb:cc:dd:ee:ff \
	--netdev tap,id=eth0,script=custom-scripts/qemu-ifup \
	--kernel output/images/bzImage \
	--hda output/images/rootfs.ext2 --nographic \
	--hdb sdb.bin \
	--nographic \
	--append "console=ttyS0 root=/dev/sda"

# gerar arquivo sbd.bin com dd if=/dev/zero of=sdb.bin bs=512 count=2097152 (1GB)