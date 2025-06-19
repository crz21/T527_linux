KDIR:=/lib/modules/`uname -r`/build 
OBJS_DIR=debug/obj
obj-m:=i2c_bmi160.o
PWD=$(shell pwd)

# source file.
app_in=main.c
# executable file.
app_out=main

all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=''
	$(CROSS_COMPILE)gcc -o $(app_out) $(app_in)
	dtc -@ -I dts -O dtb -o sun55i-t527-bmi160.dtbo sun55i-t527-bmi160.dts 

.PHONE:clean
clean:
	rm -rf *.o *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers .i2c_bmi160.* *.mod .Module.* .modules.* main *.dtbo 
		