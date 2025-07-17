CC=gcc
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
OBJS_DIR=debug
DEV_DIR=device
APP_DIR=app
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, ${CUR_SOURCE}}
APP_IN=bmi160_user.c bmi160.c
APP_OUT=main
PWD=$(shell pwd)

-include soc_select.config

if ($(CONFIG_ENABLE_T527),y)
KDIR:=/lib/modules/`uname -r`/build # 全志t527专用
CROSS_COMPILE=
ARCH=
DTBO=sun55i-t527-bmi160.dtbo
DTS=sun55i-t527-bmi160.dts
endif

ifeq ($(CONFIG_ENABLE_RK3576),y)
KDIR:=/home/pi/RK3576/rk3576_linux6.1_release/kernel-6.1/ # rk3576专用
CROSS_COMPILE=aarch64-none-linux-gnu-
ARCH=arm64
DTBO=rk3576-bmi160.dtbo
DTS=rk3576-bmi160.dts
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

		
