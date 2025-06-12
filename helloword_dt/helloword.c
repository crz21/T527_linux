//#include <stdio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

//int main(int argc, char*argv[])
//{
    

//    return 0;
//}

static int __init helloword_init(void)
{
 //   printk()

    return 0;
}
module_init(helloword_init);

static void __exit helloword_exit(void)
{
    
}
module_exit(helloword_exit);

MODULE_LICENSE("GPL2");
MODULE_AUTHOR("crz");
MODULE_DESCRIPTION("hello module");
MODULE_ALIAS("test_module");

