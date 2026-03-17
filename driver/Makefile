obj-m := imu_lsm6dso.o
imu_lsm6dso-y := imu_lsm6dso_main.o imu_lsm6dso_cfg.o
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR = ~/workspace/ldd/source/linux/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
