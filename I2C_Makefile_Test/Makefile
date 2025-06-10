CC=gcc
INCLUDE_DIR=-I./device
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
OBJS_DIR=debug/obj
ROOT_DIR=$(shell pwd)
BIN=test_spi
BIN_DIR=debug/bin
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, ${CUR_SOURCE}}

export CC BIN OBJS_DIR BIN_DIR ROOT_DIR

all:$(SUBDIRS) $(CUR_OBJS) DEBUG
.PHONY:all
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C debug
ECHO:
	@echo $(SUBDIRS)
$(CUR_OBJS):%.o:%.c
	$(CC) $(INCLUDE_DIR) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@

.PHONY:clean
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJS_DIR)/*.o
