KDIR:=/lib/modules/`uname -r`/build # Point to Linux Kernel Headers
PWD:=$(shell pwd)

obj-m:=helloword.o

default:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=""
	dtc -@ -I dts -O dtb -o sun55i-t527-helloword.dtbo sun55i-t527-helloword.dts

clean:
	rm -rf *.o *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers *.dtbo
