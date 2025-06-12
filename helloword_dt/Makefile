# Point to Linux Kernel Headers
KDIR := /lib/modules/`uname -r`/build 
PWD := $(shell pwd)

obj-m := helloword.o

default:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=""
.PHONE:clean
clean:
	rm -rf *.o *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers
