INCLUDE_DIR=-I../
C_FLAGS=
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) $(INCLUDE_DIR) $(C_FLAGS) -c $^ -o $(PWD)/$(OBJS_DIR)/$@
ECHO:
	@echo $(SUBDIRS)
