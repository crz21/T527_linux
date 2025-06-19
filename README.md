Linux驱动的一些学习记录（linux中一切皆文件！！！）

一、CMAKE
- cmake -S {源文件目录} -B {过程文件所需保存的目录}
- cmake .&&make
- 一步生成方式
        cmake ..          &&                                make
        当前文件夹  当前面条件为是的情况下后面条件生成


二、MAKEFILE
- $(CURDIR)CURDIR是make的内嵌变量，为当前目录；$(PWD)表示当前路径；
- obj-m表示把文件test.o作为"模块"进行编译，不会编译到内核，但是会生成一个独立的 "test.ko" 文件；obj-y表示把test.o文件编译进内核;
- uname -r  表示当前linux版本号
- .PHONE  伪指令
- CROSS_COMPILE=''  交叉编译链按本地来


三、DEVICETREE
- 文件名

        dts：Device TreeSource
        dtc：DeviceTree Compiler
        dtb：DeviceTree Blob
        dtbo：Device Tree Blob Overlay

- 实现入口函数xxx_init()和卸载函数xxx_exit()
- 构建file_operation结构体内容，实现硬件各个相关的操作
- struct xxx_driver  定义xxx总线设备结构体，如果配对上，则会调用xxx_driver.probe函数
- struct of_device_id  该结构体用于与设备树匹配
- struct i2c_device_id  将device和driver的id关联时用
- THIS_MODULE  #define THIS_MODULE (&__this_module)  为下面的结构体

        struct module {
                enum module_state state;

                /* Member of list of modules */
                struct list_head list;

                /* Unique handle for this module */
                char name[MODULE_NAME_LEN];

        #ifdef CONFIG_STACKTRACE_BUILD_ID
                /* Module build ID */
                unsigned char build_id[BUILD_ID_SIZE_MAX];
        #endif

                /* Sysfs stuff. */
                struct module_kobject mkobj;
                struct module_attribute *modinfo_attrs;
                const char *version;
                const char *srcversion;
                struct kobject *holders_dir;

                /* Exported symbols */
                const struct kernel_symbol *syms;
                const s32 *crcs;
                unsigned int num_syms;

        #ifdef CONFIG_ARCH_USES_CFI_TRAPS
                s32 *kcfi_traps;
                s32 *kcfi_traps_end;
        #endif

                /* Kernel parameters. */
        #ifdef CONFIG_SYSFS
                struct mutex param_lock;
        #endif
                struct kernel_param *kp;
                unsigned int num_kp;

                /* GPL-only exported symbols. */
                unsigned int num_gpl_syms;
                const struct kernel_symbol *gpl_syms;
                const s32 *gpl_crcs;
                bool using_gplonly_symbols;

        #ifdef CONFIG_MODULE_SIG
                /* Signature was verified. */
                bool sig_ok;
        #endif

                bool async_probe_requested;

                /* Exception table */
                unsigned int num_exentries;
                struct exception_table_entry *extable;

                /* Startup function. */
                int (*init)(void);

                struct module_memory mem[MOD_MEM_NUM_TYPES] __module_memory_align;

                /* Arch-specific module values */
                struct mod_arch_specific arch;

                unsigned long taints;	/* same bits as kernel:taint_flags */

        #ifdef CONFIG_GENERIC_BUG
                /* Support for BUG */
                unsigned num_bugs;
                struct list_head bug_list;
                struct bug_entry *bug_table;
        #endif

        #ifdef CONFIG_KALLSYMS
                /* Protected by RCU and/or module_mutex: use rcu_dereference() */
                struct mod_kallsyms __rcu *kallsyms;
                struct mod_kallsyms core_kallsyms;

                /* Section attributes */
                struct module_sect_attrs *sect_attrs;

                /* Notes attributes */
                struct module_notes_attrs *notes_attrs;
        #endif

                /* The command line arguments (may be mangled).  People like
                keeping pointers to this stuff */
                char *args;

        #ifdef CONFIG_SMP
                /* Per-cpu data. */
                void __percpu *percpu;
                unsigned int percpu_size;
        #endif
                void *noinstr_text_start;
                unsigned int noinstr_text_size;

        #ifdef CONFIG_TRACEPOINTS
                unsigned int num_tracepoints;
                tracepoint_ptr_t *tracepoints_ptrs;
        #endif
        #ifdef CONFIG_TREE_SRCU
                unsigned int num_srcu_structs;
                struct srcu_struct **srcu_struct_ptrs;
        #endif
        #ifdef CONFIG_BPF_EVENTS
                unsigned int num_bpf_raw_events;
                struct bpf_raw_event_map *bpf_raw_events;
        #endif
        #ifdef CONFIG_DEBUG_INFO_BTF_MODULES
                unsigned int btf_data_size;
                void *btf_data;
        #endif
        #ifdef CONFIG_JUMP_LABEL
                struct jump_entry *jump_entries;
                unsigned int num_jump_entries;
        #endif
        #ifdef CONFIG_TRACING
                unsigned int num_trace_bprintk_fmt;
                const char **trace_bprintk_fmt_start;
        #endif
        #ifdef CONFIG_EVENT_TRACING
                struct trace_event_call **trace_events;
                unsigned int num_trace_events;
                struct trace_eval_map **trace_evals;
                unsigned int num_trace_evals;
        #endif
        #ifdef CONFIG_FTRACE_MCOUNT_RECORD
                unsigned int num_ftrace_callsites;
                unsigned long *ftrace_callsites;
        #endif
        #ifdef CONFIG_KPROBES
                void *kprobes_text_start;
                unsigned int kprobes_text_size;
                unsigned long *kprobe_blacklist;
                unsigned int num_kprobe_blacklist;
        #endif
        #ifdef CONFIG_HAVE_STATIC_CALL_INLINE
                int num_static_call_sites;
                struct static_call_site *static_call_sites;
        #endif
        #if IS_ENABLED(CONFIG_KUNIT)
                int num_kunit_suites;
                struct kunit_suite **kunit_suites;
        #endif


        #ifdef CONFIG_LIVEPATCH
                bool klp; /* Is this a livepatch module? */
                bool klp_alive;

                /* ELF information */
                struct klp_modinfo *klp_info;
        #endif

        #ifdef CONFIG_PRINTK_INDEX
                unsigned int printk_index_size;
                struct pi_entry **printk_index_start;
        #endif

        #ifdef CONFIG_MODULE_UNLOAD
                /* What modules depend on me? */
                struct list_head source_list;
                /* What modules do I depend on? */
                struct list_head target_list;

                /* Destruction function. */
                void (*exit)(void);

                atomic_t refcnt;
        #endif

        #ifdef CONFIG_CONSTRUCTORS
                /* Constructor functions. */
                ctor_fn_t *ctors;
                unsigned int num_ctors;
        #endif

        #ifdef CONFIG_FUNCTION_ERROR_INJECTION
                struct error_injection_entry *ei_funcs;
                unsigned int num_ei_funcs;
        #endif
        #ifdef CONFIG_DYNAMIC_DEBUG_CORE
                struct _ddebug_info dyndbg_info;
        #endif
        } ____cacheline_aligned __randomize_layout;

- 申请设备号 register_chrdev_region()
- 动态方式分配设备号 alloc_chrdev_region()  注销设备号 unregister_chrdev_region()
- 初始化字符设备，cdev_init函数、cdev_add函数  cdev_del()  清除设备号
- 创建设备类 class_create()  清除设备类class_destroy()
- 将设备类和设备号和设备名关联 device_create()  清除设备信息  device_destroy()

- 在终端上使用mknod根据设备号来进行创建设备文件(节点)
(也可以在驱动使用class_create创建设备类、在类的下面device_create创建设备节点)

- 设备树中属性的意义

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

        g、节点aliases：别名
- 设备树插件格式

        格式一

                /dts-v1/;
                /plugin/;

                / {
                        compatible = "allwinner,sun55i-t527";/** 设备主控信息，用于匹配 */
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
                                compatible = "barco,bmi160";/** 插件信息，用于匹配 */
                        };
                        };
                .......
                };
                第1行： 用于指定dts的版本。

                第2行： 表示允许使用未定义的引用并记录它们，设备树插件中可以引用主设备树中的节点，而这些“引用的节点”对于设备树插件来说就是未定义的，所以设备树插件应该加上“/plugin/”。

                第6行： 指定设备树插件的加载位置，默认我们加载到根节点下，既“target-path =“/”,或者使用target = <&XXXXX>，增加节点或者属性到某个节点下。

                第7-8行： 我们要插入的设备及节点或者要引用(追加)的设备树节点放在__overlay__ {…}内，你可以增加、修改或者覆盖主设备树的节点。

        格式二

                /dts-v1/;
                /plugin/;

                &{/} {
                /*此处在根节点"/"下,添加要插入的节点或者属性*/
                };

                &XXXXX {
                /*此处在节点"XXXXX"下,添加要插入的节点或者属性*/
                };

- 命令行

        dtc -@ -I dts -O dtb -o sun55i-t527-test.dtbo sun55i-t527-test.dts


    
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

- 加载/卸载命令

        modprobe在加载时会把依赖模块也一并加载，而insmod不会加载依赖模块

        insmod工具实际上调用了系统调用init_module

        rmmod工具卸载模块时，会调用系统调用delete_module

- 在内核源码中编译设备树

        make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- npi_v7_defconfig

        make ARCH=arm -j4 CROSS_COMPILE=arm-linux-gnueabihf- dtbs

四、shell脚本
