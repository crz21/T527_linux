# Point to Linux Kernel Headers
KDIR=/lib/modules/`uname -r`/build 
PWD=$(shell pwd)
# ARCH=arm
# CROSS_COMPILE=arm-linux-gnueabihf-
# export ARCH CROSS_COMPILE

obj-m:=helloword.o

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules CROSS_COMPILE=''
.PHONE:clean
clean:
	rm -rf *.o *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers
