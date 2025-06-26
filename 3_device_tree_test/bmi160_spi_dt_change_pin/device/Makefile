obj-m:=spi_bmi160.o
PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=''
	cp *.ko ../$(OBJS_DIR)/
	dtc -@ -I dts -O dtb -o ../$(OBJS_DIR)/$(DTBO) $(PWD)/$(DTS)
	rm -rf *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers .i2c_bmi160.* *.mod .Module.* .modules.* main *.dtbo *.o *.mod.o 
