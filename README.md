Linux驱动的一些学习记录

一、CMAKE

1、cmake -S {源文件目录} -B {过程文件所需保存的目录}

2、cmake .&&make

3、一步生成方式
cmake ..        &&                                make
      当前文件夹  当前面条件为是的情况下后面条件生成



二、MAKEFILE



三、DEVICETREE

1、实现入口函数xxx_init()和卸载函数xxx_exit()

2、申请设备号 register_chrdev_region()

3、初始化字符设备，cdev_init函数、cdev_add函数

4、硬件初始化，如时钟寄存器配置使能，GPIO设置为输入输出模式等。

5、构建file_operation结构体内容，实现硬件各个相关的操作

6、在终端上使用mknod根据设备号来进行创建设备文件(节点)
(也可以在驱动使用class_create创建设备类、在类的下面device_create创建设备节点)

7、dts中如果是扩展则需要__overlay__ {}

8、<name>[@<unit_address>] 则后需要跟 reg = <unit_address>

9、设备树中属性的意义

        a、compatible：设备节点中对应的节点信息已经被内核构造成struct platform_device。
        驱动可以通过相应的函数从中提取信息。compatible属性是用来查找节点的方法之一，另外还可以通过节点名或节点路径查找指定节点。

        b、address：(几乎)所有的设备都需要与CPU的IO口相连，所以其IO端口信息就需要在设备节点节点中说明。常用的属性有
        #address-cells，用来描述子节点"reg"属性的地址表中用来描述首地址的cell的数量，
        #size-cells，用来描述子节点"reg"属性的地址表中用来描述地址长度的cell的数量。

        c、interrupts：一个计算机系统中大量设备都是通过中断请求CPU服务的，所以设备节点中就需要在指定中断号。常用的属性有
        interrupt-controller 一个空属性用来声明这个node接收中断信号，即这个node是一个中断控制器。
        #interrupt-cells，是中断控制器节点的属性，用来标识这个控制器需要几个单位做中断描述符,用来描述子节点中"interrupts"属性使用了父节点中的interrupts属性的具体的哪个值。一般，如果父节点的该属性的值是3，则子节点的interrupts一个cell的三个32bits整数值分别为:<中断域 中断 触发方式>,如果父节点的该属性是2，则是<中断 触发方式>
        interrupt-parent,标识此设备节点属于哪一个中断控制器，如果没有设置这个属性，会自动依附父节点的
        interrupts,一个中断标识符列表，表示每一个中断输出信号

        d、gpio：gpio也是最常见的IO口，常用的属性有
        "gpio-controller"，用来说明该节点描述的是一个gpio控制器
        "#gpio-cells"，用来描述gpio使用节点的属性一个cell的内容，即 `属性 = <&引用GPIO节点别名 GPIO标号 工作模式>

        e、status：状态属性用于指示设备的“操作状态”，通过status可以去禁止设备或者启用设备，可用的操作状态如下表。默认情况下不设置status属性设备是使能的。

        f、ranges：属性值类型：任意数量的 <子地址、父地址、地址长度>编码。

10、节点aliases：别名
 
11、设备树格式
        /dts-v1/;
        /plugin/;

        / {
                fragment@0 {
                target-path = "/";
                __overlay__ {
                        /*在此添加要插入的节点*/
                        .......
                };
                };

                fragment@1 {
                target = <&XXXXX>;
                __overlay__ {
                        /*在此添加要插入的节点*/
                        .......
                };
                };
        .......
        };
        第1行： 用于指定dts的版本。

        第2行： 表示允许使用未定义的引用并记录它们，设备树插件中可以引用主设备树中的节点，而这些“引用的节点”对于设备树插件来说就是未定义的，所以设备树插件应该加上“/plugin/”。

        第6行： 指定设备树插件的加载位置，默认我们加载到根节点下，既“target-path =“/”,或者使用target = <&XXXXX>，增加节点或者属性到某个节点下。

        第7-8行： 我们要插入的设备及节点或者要引用(追加)的设备树节点放在__overlay__ {…}内，你可以增加、修改或者覆盖主设备树的节点。

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



