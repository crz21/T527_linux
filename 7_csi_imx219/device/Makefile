obj-m:=i2c_imx219.o
PWD=$(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	cp *.ko ../$(OBJS_DIR)/
	dtc -@ -I dts -O dtb -o ../$(OBJS_DIR)/$(DTBO) $(PWD)/$(DTS)
	rm -rf *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers .i2c_imx219.* *.mod .Module.* .modules.* main *.dtbo *.o *.mod.o 
