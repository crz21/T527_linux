CC=gcc
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
OBJS_DIR=debug/obj
BIN_DIR=debug/bin
DEV_DIR=device
APP_DIR=app
DTBO=sun55i-t527-ov5640-dcmi.dtbo
DTS=sun55i-t527-ov5640-dcmi.dts
APP_IN=main.c 
APP_OUT=main
PWD=$(shell pwd)
KDIR:=/lib/modules/`uname -r`/build 
CROSS_COMPILE=
CFLAGS = 


export CC OBJS_DIR BIN_DIR DTBO DTS DEV_DIR KDIR APP_OUT APP_IN CROSS_COMPILE CFLAGS

all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)

.PHONE:clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJS_DIR)/* 
		