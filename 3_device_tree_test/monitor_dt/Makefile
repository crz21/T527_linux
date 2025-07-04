CC:=gcc
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
OBJS_DIR:=debug/obj
PWD:=$(shell pwd)
KDIR:=/lib/modules/`uname -r`/build 
CROSS_COMPILE=
CFLAGS= 

export CC OBJS_DIR KDIR CROSS_COMPILE CFLAGS PWD SUBDIRS

all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)

.PHONE:clean
clean:
	rm -rf $(OBJS_DIR)/* 
		