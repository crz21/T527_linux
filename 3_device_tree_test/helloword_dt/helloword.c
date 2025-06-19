#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

// 在设备树匹配上的时候调用
static int test_probe(struct platform_device *pdev)
{
    printk(KERN_EMERG  "test_probe\n");
    return 0; 
}

// 匹配表
static const struct of_device_id test_of_match[] = {
    { .compatible = "walnutpi-test" },//用于匹配设备树compatible
    { }
};

static struct platform_driver pla_drv = {
    .driver = {
        .name = "walnutpi-test",
        .of_match_table = test_of_match,
    },
    .probe = test_probe, 
};

static int test_init(void)
{
    printk(KERN_EMERG   "test_init\n");
    platform_driver_register(&pla_drv);
    return 0;
}
static void test_exit(void)
{
    printk(KERN_EMERG  "test_exit\n");
    platform_driver_unregister(&pla_drv);
}
module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");