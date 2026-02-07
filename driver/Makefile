obj-m := imu_lsm6dso.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR = ~/workspace/ldd/source/linux/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
