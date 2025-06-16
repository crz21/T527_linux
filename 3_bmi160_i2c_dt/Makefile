KDIR := /lib/modules/`uname -r`/build 
CC=gcc
# INCLUDE_DIR=-I./device
# SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") printf $$9}')
OBJS_DIR=debug/obj
# BIN_DIR=debug/bin
obj-m:=i2c_bmi160.o
PWD=$(shell pwd)
# BIN=tesp_i2c
# CUR_SOURCE=${wildcard *.c}
# CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
export CC OBJS_DIR PWD

# all:$(SUBDIRS) $(CUR_OBJS) DEBUG
all:
	make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=""
	gcc -o main main.c

# $(SUBDIRS):ECHO
# 	make -C $@

# DEBUG:ECHO
# 	make -C debug

# ECHO:
# 	@echo $(SUBDIRS)

# $(CUR_OBJS):%.o:%.c
# 	$(CC) $(INCLUDE_DIR) -c $^ -o $(PWD)/$(OBJS_DIR)/$@

.PHONE:clean
clean:
	rm -rf *.o *.cmd *.ko *.mod.c *.tmp_versions *.order *.symvers .i2c_bmi160.* *.mod .Module.* .modules.* main *.dtbo 
		