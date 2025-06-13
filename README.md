Linux驱动的一些学习记录

/**************** CMAKE *****************/
1、cmake -S {源文件目录} -B {过程文件所需保存的目录}
2、cmake .&&make

一步生成方式
cmake ..        &&                                make
      当前文件夹  当前面条件为是的情况下后面条件生成
/****************************************/


/*************** MAKEFILE ***************/
/****************************************/


/*************** DATATREE ***************/
1、实现入口函数xxx_init()和卸载函数xxx_exit()

2、申请设备号 register_chrdev_region()

3、初始化字符设备，cdev_init函数、cdev_add函数

4、硬件初始化，如时钟寄存器配置使能，GPIO设置为输入输出模式等。

5、构建file_operation结构体内容，实现硬件各个相关的操作

6、在终端上使用mknod根据设备号来进行创建设备文件(节点)
(也可以在驱动使用class_create创建设备类、在类的下面device_create创建设备节点)

 
dtc -@ -I dts -O dtb -o sun55i-t527-test.dtbo sun55i-t527-test.dts

dts：Device TreeSource
dtc：DeviceTree Compiler
dtb：DeviceTree Blob
dtbo：Device Tree Blob Overlay
    
     dtc         -o
dts -----> dtb -----> dtbo


 -@, --symbols
        Enable generation of symbols

-I, --in-format <arg>
        Input formats are:
                dts - device tree source text
                dtb - device tree blob
                fs  - /proc/device-tree style directory

 -O, --out-format <arg>
        Output formats are:
                dts - device tree source text
                dtb - device tree blob
                yaml - device tree encoded as YAML
                asm - assembler source

 -o, --out <arg>
        Output file
/****************************************/