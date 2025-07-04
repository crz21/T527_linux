DTBO:=sun55i-t527-imx219_i2c.dtbo
DTS:=sun55i-t527-imx219_i2c.dts

all:
	make -C $(KDIR) M=$(PWD)/$(CAMERA_I2C_PATH)/ modules CROSS_COMPILE=''
	cp *.ko ../$(OBJS_DIR)/
	dtc -@ -I dts -O dtb -o $(PWD)/$(OBJS_DIR)/$(DTBO) $(PWD)/$(CAMERA_I2C_DIR)/$(DTS)
	rm -rf *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers .ov5640_dcmi.* *.mod .Module.* .modules.* main *.dtbo *.o *.mod.o 

