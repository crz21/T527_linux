// #include <stdio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init helloword_init(void)
{
    printk(KERN_EMERG "[KERN_ENERG] Hello Module Init\n");
    printk("[default] Hello Module Init\n");

    return 0;
}

static void __exit helloword_exit(void) { printk("[default] Hello Module Exit\n"); }

module_init(helloword_init);
module_exit(helloword_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("crz");
MODULE_DESCRIPTION("hello module");
MODULE_ALIAS("hello_module");
