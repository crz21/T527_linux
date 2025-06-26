CC=gcc
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
OBJS_DIR=debug/obj
BIN_DIR=debug/bin
DEV_DIR=device
APP_DIR=app
# CUR_SOURCE=${wildcard *.c}
# CUR_OBJS=${patsubst %.c, %.o, ${CUR_SOURCE}}
DTBO=sun55i-t527-bmi160-spi-change-pin.dtbo
DTS=sun55i-t527-bmi160-spi-change-pin.dts
APP_IN=bmi160_user.c bmi160.c
APP_OUT=main
PWD=$(shell pwd)
KDIR:=/lib/modules/`uname -r`/build 
BIN=main
CROSS_COMPILE=
# obj-$(CONFIG_BMI160) += bmi160/
CFLAGS = 


export CC OBJS_DIR BIN_DIR DTBO DTS DEV_DIR KDIR APP_OUT APP_IN BIN CROSS_COMPILE CFLAGS

all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)

.PHONE:clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJS_DIR)/* 
		