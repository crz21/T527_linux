# 编译器
CC = gcc

# debug文件夹里的makefile文件需要最后执行，所以这里需要执行的子目录要排除debug文件夹，这里使用awk排除了debug文件夹，读取剩下的文件夹
# 先用ls -l显示所有文件和文件夹，用grep ^d选出文件夹，用awk选上不带"debug"名字的文件夹名，并赋值给SUBDIRS makefile用shell的$命令需要用$$表示
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')

# 目标文件所在的目录
OBJS_DIR = ./debug/bin

#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)

# 最终bin文件的名字，可以更改为自己需要的
BIN = test_spi

#bin文件所在的目录
BIN_DIR = ./debug/bin

#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_SOURCE=${wildcard *.c}

#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中 %.c为makefile中的所有 *.c通配符则对应的是系统中所有c文件
CUR_OBJS=${patsubst %.c, %.o, ${CUR_SOURCE}}

#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR

#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all : $(SUBDIRS) $(CUR_OBJS) DEBUG

#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
    make -C $@
DEBUG:ECHO
    #直接去debug目录下执行makefile文件
    make -C debug
ECHO:
    @echo $(SUBDIRS)
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR 静态模式
$(CUR_OBJS):%.o:%.c
    $(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@

# .PHONY 表明一个目标为伪目标
.PHONY : clean
clean :
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJS_DIR)/*.o