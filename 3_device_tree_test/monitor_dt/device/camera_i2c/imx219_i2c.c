#include "imx219_i2c.h"

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

#define DEV_NAME "I2C1_IMX219"
#define DEV_CNT (1)

/*定义 led 资源结构体，保存获取得到的节点信息以及转换后的虚拟寄存器地址*/
static dev_t imx219_devno;               // 定义字符设备的设备号
static struct cdev imx219_chr_dev;       // 定义字符设备结构体chr_dev
struct class *class_imx219;              // 保存创建的类
struct device *device_imx219;            // 保存创建的设备
struct device_node *imx219_device_node;  // 设备树节点结构体

/*------------------IIC设备内容----------------------*/
struct i2c_client *imx219_client = NULL;  // 保存imx219设备对应的i2c_client结构体，匹配成功后由.prob函数带回。

static int i2c_read_imx219(struct i2c_client *imx219_client, char *data, uint32_t length)
{
    int error = 0;
    uint8_t address_data = data[0];
    struct i2c_msg imx219_msg[2];
    /*设置读取位置msg*/
    imx219_msg[0].addr = imx219_client->addr;  // imx219在 iic 总线上的地址
    imx219_msg[0].flags = 0;                   // 标记为发送数据
    imx219_msg[0].buf = &address_data;         // 写入的首地址
    imx219_msg[0].len = 1;                     // 写入长度

    /*设置读取位置msg*/
    imx219_msg[1].addr = imx219_client->addr;  // imx219在 iic 总线上的地址
    imx219_msg[1].flags = I2C_M_RD;            // 标记为读取数据
    imx219_msg[1].buf = data + 1;              // 读取得到的数据保存位置
    imx219_msg[1].len = length;                // 读取长度

    error = i2c_transfer(imx219_client->adapter, imx219_msg, 2);

    if (error != 2) {
        printk(KERN_EMERG "i2c_read_imx219 error,error=%d \n", error);
        return -1;
    }
    return 0;
}

static int i2c_write_imx219(struct i2c_client *imx219_client, char *data, uint32_t length)
{
    int error = 0;
    struct i2c_msg send_msg;  // 要发送的数据结构体

    /*发送 iic要写入的地址 reg*/
    send_msg.addr = imx219_client->addr;  // imx219在 iic 总线上的地址
    send_msg.flags = 0;                   // 标记为发送数据
    send_msg.buf = data;                  // 写入的首地址
    send_msg.len = length;                // reg长度

    /*执行发送*/
    error = i2c_transfer(imx219_client->adapter, &send_msg, 1);

    if (error != 1) {
        printk(KERN_EMERG "i2c_write_imx219 error,error=%d \n", error);
        return -1;
    }
    return 0;
}

/** 字符设备操作函数集，read */
ssize_t imx219_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    char rx_buf[200] = {0};
    int error;

    /** 先将用户空间的地址读到内核空间 */
    error = copy_from_user(rx_buf, buf, 1);

    /** 从设备读数据到内核空间 */
    i2c_read_imx219(imx219_client, rx_buf, len);

    /** 将内核空间得到的数据拷贝到用户空间 */
    error = copy_to_user(buf, rx_buf + 1, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    return 0;
}

ssize_t imx219_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    char tx_buf[200] = {0};
    int error;

    /*将用户空间的数据复制到内核空间*/
    error = copy_from_user(tx_buf, buf, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    i2c_write_imx219(imx219_client, tx_buf, len);
    return 0;
}

/** 字符设备操作函数集 start */
static int imx219_init(void)
{
    int error = 0;

    if (error < 0) {
        /*初始化错误*/
        printk(KERN_DEBUG "\n imx219_init error \n");
        return -1;
    } else
        printk(KERN_DEBUG "\n imx219_init ok ! \n");
    return 0;
}

/** 字符设备操作函数集，open */
static int imx219_open(struct inode *inode, struct file *filp)
{
    /*向 imx219 发送配置数据，让imx219处于正常工作状态*/
    imx219_init();
    return 0;
}

/** 字符设备操作函数集，release */
static int imx219_release(struct inode *inode, struct file *filp) { return 0; }

/** 字符设备操作函数集，结构体 */
static struct file_operations imx219_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = imx219_open,
    .release = imx219_release,
    .read = imx219_read,
    .write = imx219_write,
};
/** 字符设备操作函数集 end */

/** 平台驱动函数集 start */
static int imx219_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret = -1;

    printk(KERN_EMERG "match successed  \n");

    /** 采用动态分配的方式，获取设备编号，次设备号为0，设备名称为rgb-leds，可通过命令cat
     * /proc/devices查看DEV_CNT为1，当前只申请一个设备编号 */
    ret = alloc_chrdev_region(&imx219_devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        printk("fail to alloc imx219_devno\n");
        goto alloc_err;
    }

    /** 关联字符设备结构体cdev与文件操作结构体file_operations */
    imx219_chr_dev.owner = THIS_MODULE;
    cdev_init(&imx219_chr_dev, &imx219_chr_dev_fops);

    /** 添加设备至cdev_map散列表中 */
    ret = cdev_add(&imx219_chr_dev, imx219_devno, DEV_CNT);
    if (ret < 0) {
        printk("fail to add cdev\n");
        goto add_err;
    } else
        printk(KERN_EMERG "imx219_devno=%d\n", imx219_devno);

    /** 创建类 */
    class_imx219 = class_create(THIS_MODULE, DEV_NAME);

    /** 创建设备 DEV_NAME 指定设备名 */
    device_imx219 = device_create(class_imx219, NULL, imx219_devno, NULL, DEV_NAME);
    imx219_client = client;
    printk(KERN_EMERG "create successed \n");
    return 0;

add_err:
    /** 添加设备失败时，需要注销设备号 */
    unregister_chrdev_region(imx219_devno, DEV_CNT);
    printk(KERN_EMERG "\n add_err error! \n");
alloc_err:

    return -1;
}

static int imx219_remove(struct i2c_client *client)
{
    /** 删除设备 */
    printk(KERN_EMERG "\t  remove device!  \n");
    device_destroy(class_imx219, imx219_devno);       // 清除设备
    class_destroy(class_imx219);                      // 清除类
    cdev_del(&imx219_chr_dev);                        // 清除设备号
    unregister_chrdev_region(imx219_devno, DEV_CNT);  // 取消注册字符设备
    return 0;
}

/*定义ID匹配表*/
static const struct i2c_device_id gtp_device_id[] = {{"crz,imx219", 0}, {}};

/*定义设备树匹配表*/
static const struct of_device_id imx219_of_match_table[] = {{.compatible = "crz,imx219"}, {/* sentinel */}};

/*定义i2c总线设备结构体*/
struct i2c_driver imx219_driver = {
    .probe = imx219_probe,
    .remove = imx219_remove,
    .id_table = gtp_device_id,
    .driver =
        {
            .name = "crz,imx219",
            .owner = THIS_MODULE,
            .of_match_table = imx219_of_match_table,
        },
};
/** 平台驱动函数集 end */

/** 驱动初始化 */
static int __init imx219_driver_init(void)
{
    int ret;

    printk(KERN_EMERG "imx219_driver_init\n");
    ret = i2c_add_driver(&imx219_driver);
    return ret;
}

/** 驱动注销 */
static void __exit imx219_driver_exit(void)
{
    printk(KERN_EMERG "imx219_driver_exit\n");
    i2c_del_driver(&imx219_driver);
}

module_init(imx219_driver_init);
module_exit(imx219_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("crz");
MODULE_DESCRIPTION("hello module");
MODULE_ALIAS("test_module");
