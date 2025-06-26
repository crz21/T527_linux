#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include "ov5640_dcmi.h"

#define DEV_NAME "I2C1_OV5640"
#define DEV_CNT (1)

/*定义 led 资源结构体，保存获取得到的节点信息以及转换后的虚拟寄存器地址*/
static dev_t ov5640_devno;               // 定义字符设备的设备号
static struct cdev ov5640_chr_dev;       // 定义字符设备结构体chr_dev
struct class *class_ov5640;              // 保存创建的类
struct device *device_ov5640;            // 保存创建的设备
struct device_node *ov5640_device_node;  // 设备树节点结构体

/*------------------IIC设备内容----------------------*/
struct i2c_client *ov5640_client = NULL;  // 保存ov5640设备对应的i2c_client结构体，匹配成功后由.prob函数带回。

static int i2c_read_ov5640(struct i2c_client *ov5640_client, char *data, uint32_t length)
{
    int error = 0;
    uint8_t address_data = data[0];
    struct i2c_msg ov5640_msg[2];
    /*设置读取位置msg*/
    ov5640_msg[0].addr = ov5640_client->addr;  // ov5640在 iic 总线上的地址
    ov5640_msg[0].flags = 0;                   // 标记为发送数据
    ov5640_msg[0].buf = &address_data;         // 写入的首地址
    ov5640_msg[0].len = 1;                     // 写入长度

    /*设置读取位置msg*/
    ov5640_msg[1].addr = ov5640_client->addr;  // ov5640在 iic 总线上的地址
    ov5640_msg[1].flags = I2C_M_RD;            // 标记为读取数据
    ov5640_msg[1].buf = data + 1;              // 读取得到的数据保存位置
    ov5640_msg[1].len = length;                // 读取长度

    error = i2c_transfer(ov5640_client->adapter, ov5640_msg, 2);

    if (error != 2) {
        printk(KERN_EMERG "i2c_read_ov5640 error\n");
        return -1;
    }
    return 0;
}

static int i2c_write_ov5640(struct i2c_client *ov5640_client, char *data, uint32_t length)
{
    int error = 0;
    struct i2c_msg send_msg;  // 要发送的数据结构体

    /*发送 iic要写入的地址 reg*/
    send_msg.addr = ov5640_client->addr;  // ov5640在 iic 总线上的地址
    send_msg.flags = 0;                   // 标记为发送数据
    send_msg.buf = data;                  // 写入的首地址
    send_msg.len = length;                // reg长度

    /*执行发送*/
    error = i2c_transfer(ov5640_client->adapter, &send_msg, 1);

    if (error != 1) {
        printk(KERN_EMERG "i2c_write_ov5640 error,error=%d \n", error);
        return -1;
    }
    return 0;
}

/** 字符设备操作函数集，read */
ssize_t ov5640_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    char rx_buf[200] = {0};
    int error;

    /** 先将用户空间的地址读到内核空间 */
    error = copy_from_user(rx_buf, buf, 1);

    /** 从设备读数据到内核空间 */
    i2c_read_ov5640(ov5640_client, rx_buf, len);
    
    /** 将内核空间得到的数据拷贝到用户空间 */
    error = copy_to_user(buf, rx_buf + 1, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    return 0;
}

ssize_t ov5640_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    char tx_buf[200] = {0};
    int error;

    /*将用户空间的数据复制到内核空间*/
    error = copy_from_user(tx_buf, buf, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    i2c_write_ov5640(ov5640_client, tx_buf, len);
    return 0;
}

/** 字符设备操作函数集 start */
static int ov5640_init(void)
{
    int error = 0;

    if (error < 0) {
        /*初始化错误*/
        printk(KERN_DEBUG "\n ov5640_init error \n");
        return -1;
    } else
        printk(KERN_DEBUG "\n ov5640_init ok ! \n");
    return 0;
}

/** 字符设备操作函数集，open */
static int ov5640_open(struct inode *inode, struct file *filp)
{
    /*向 ov5640 发送配置数据，让ov5640处于正常工作状态*/
    ov5640_init();
    return 0;
}

/** 字符设备操作函数集，release */
static int ov5640_release(struct inode *inode, struct file *filp) { return 0; }

/** 字符设备操作函数集，结构体 */
static struct file_operations ov5640_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = ov5640_open,
    .release = ov5640_release,
    .read = ov5640_read,
    .write = ov5640_write,
};
/** 字符设备操作函数集 end */

/** 平台驱动函数集 start */
static int ov5640_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret = -1;

    printk(KERN_EMERG "match successed  \n");

    /** 采用动态分配的方式，获取设备编号，次设备号为0，设备名称为rgb-leds，可通过命令cat
     * /proc/devices查看DEV_CNT为1，当前只申请一个设备编号 */
    ret = alloc_chrdev_region(&ov5640_devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        printk("fail to alloc ov5640_devno\n");
        goto alloc_err;
    }

    /** 关联字符设备结构体cdev与文件操作结构体file_operations */
    ov5640_chr_dev.owner = THIS_MODULE;
    cdev_init(&ov5640_chr_dev, &ov5640_chr_dev_fops);

    /** 添加设备至cdev_map散列表中 */
    ret = cdev_add(&ov5640_chr_dev, ov5640_devno, DEV_CNT);
    if (ret < 0) {
        printk("fail to add cdev\n");
        goto add_err;
    } else
        printk(KERN_EMERG "ov5640_devno=%d\n", ov5640_devno);

    /** 创建类 */
    class_ov5640 = class_create(THIS_MODULE, DEV_NAME);

    /** 创建设备 DEV_NAME 指定设备名 */
    device_ov5640 = device_create(class_ov5640, NULL, ov5640_devno, NULL, DEV_NAME);
    ov5640_client = client;
    printk(KERN_EMERG "create successed \n");
    return 0;

add_err:
    /** 添加设备失败时，需要注销设备号 */
    unregister_chrdev_region(ov5640_devno, DEV_CNT);
    printk(KERN_EMERG "\n add_err error! \n");
alloc_err:

    return -1;
}

static int ov5640_remove(struct i2c_client *client)
{
    /** 删除设备 */
    printk(KERN_EMERG "\t  remove device!  \n");
    device_destroy(class_ov5640, ov5640_devno);       // 清除设备
    class_destroy(class_ov5640);                      // 清除类
    cdev_del(&ov5640_chr_dev);                        // 清除设备号
    unregister_chrdev_region(ov5640_devno, DEV_CNT);  // 取消注册字符设备
    return 0;
}

/*定义ID匹配表*/
static const struct i2c_device_id gtp_device_id[] = {{"crz,ov5640", 0}, {}};

/*定义设备树匹配表*/
static const struct of_device_id ov5640_of_match_table[] = {{.compatible = "crz,ov5640"}, {/* sentinel */}};

/*定义i2c总线设备结构体*/
struct i2c_driver ov5640_driver = {
    .probe = ov5640_probe,
    .remove = ov5640_remove,
    .id_table = gtp_device_id,
    .driver =
        {
            .name = "crz,ov5640",
            .owner = THIS_MODULE,
            .of_match_table = ov5640_of_match_table,
        },
};
/** 平台驱动函数集 end */

/** 驱动初始化 */
static int __init ov5640_driver_init(void)
{
    int ret;

    printk(KERN_EMERG "ov5640_driver_init\n");
    ret = i2c_add_driver(&ov5640_driver);
    return ret;
}

/** 驱动注销 */
static void __exit ov5640_driver_exit(void)
{
    printk(KERN_EMERG "ov5640_driver_exit\n");
    i2c_del_driver(&ov5640_driver);
}

module_init(ov5640_driver_init);
module_exit(ov5640_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("crz");
MODULE_DESCRIPTION("hello module");
MODULE_ALIAS("test_module");
