CC=gcc
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "output") print $$9}')
OBJS_DIR=output
DEV_DIR=device
APP_DIR=app
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, ${CUR_SOURCE}}
APP_IN=imx219_user.c
APP_OUT=imx219
PWD=$(shell pwd)

-include .config

ifeq ($(CONFIG_ENABLE_RK3576),y)
KDIR:=/home/tp_21/src/rk3576/rk3576_linux6.1_release/kernel-6.1/ # rk3576专用
CROSS_COMPILE=aarch64-none-linux-gnu-
ARCH=arm64
DTBO=rk3576-imx219.dtbo
DTS=rk3576-imx219.dts
endif

CFLAGS += -Wall 

export CC OBJS_DIR DTBO DTS DEV_DIR KDIR APP_OUT APP_IN CROSS_COMPILE CFLAGS ARCH 

all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)

menuconfig:
	kconfig-mconf Kconfig

.PHONE:clean
clean:
	rm -rf $(OBJS_DIR)/* 
	rm -f $(CONFIG_FILE)

		
