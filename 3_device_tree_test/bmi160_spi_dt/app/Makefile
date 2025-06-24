INCLUDE_DIR=-I../
# SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
# CUR_SOURCE=${wildcard *.c}
# CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
PWD=$(shell pwd)

all:
# $(SUBDIRS):ECHO
# 	make -C $@
# $(CUR_OBJS):%.o:%.c
	$(CROSS_COMPILE)$(CC) $(INCLUDE_DIR) $(CFLAGS) -g -o $(PWD)/../$(OBJS_DIR)/$(APP_OUT) $(PWD)/$(APP_IN) -lm
# ECHO:
# 	@echo $(SUBDIRS)



