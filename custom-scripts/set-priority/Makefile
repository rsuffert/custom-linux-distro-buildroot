BUILDROOT_DIR := ../..
KDIR := $(BUILDROOT_DIR)/output/build/linux-custom
COMPILER := $(BUILDROOT_DIR)/output/host/bin/i686-buildroot-linux-gnu-gcc

all:
	$(COMPILER) -o setpriority setpriority.c
	cp setpriority $(BUILDROOT_DIR)/output/target/bin
clean:
	rm setpriority