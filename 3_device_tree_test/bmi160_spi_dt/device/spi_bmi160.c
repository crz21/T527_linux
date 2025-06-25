#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#define DEV_NAME "SPI_BMI160"
#define DEV_CNT (1)

/*定义 led 资源结构体，保存获取得到的节点信息以及转换后的虚拟寄存器地址*/
static dev_t bmi160_devno;               // 定义字符设备的设备号
static struct cdev bmi160_chr_dev;       // 定义字符设备结构体chr_dev
struct class *class_bmi160;              // 保存创建的类
struct device *device_bmi160;            // 保存创建的设备
struct device_node *bmi160_device_node;  // 设备树节点结构体

/*------------------SPI设备内容----------------------*/
struct spi_device *bmi160_device = NULL;
#define NOP (0xFF)

int spi_transfer(struct spi_device *device, uint8_t *tx_buf, uint8_t *rx_buf, int len)
{
    int res;
    struct spi_message *message;    // 定义发送的消息
    struct spi_transfer *transfer;  // 定义传输结构体
    message = kzalloc(sizeof(struct spi_message), GFP_KERNEL);
    transfer = kzalloc(sizeof(struct spi_transfer), GFP_KERNEL);
    transfer->tx_buf = tx_buf;
    transfer->rx_buf = rx_buf;
    transfer->len = len;
    transfer->delay.value = 300;                  // 发送完成后的延时
    transfer->delay.unit = SPI_DELAY_UNIT_USECS;  // 发送完成后的延时
    transfer->tx_nbits = 1;                       // 单线制
    transfer->rx_nbits = 1;                       // 单线制
    transfer->cs_change = 0;                      // 传输后把cs线松开
    spi_message_init(message);
    spi_message_add_tail(transfer, message);
    res = spi_sync(device, message);  // 触发transfer
    kfree(message);
    kfree(transfer);

    return res;
}

static int spi_read_bmi160(struct spi_device *device, char *data, uint32_t len)
{
    int ret = 0;
    uint8_t tx_data[200] = {0};
    uint8_t reg_addr = 0;
    int i;

    tx_data[0] = data[0];
    for (i = 0; i < len; i++) tx_data[i + 1] = NOP;
    ret = spi_transfer(device, tx_data, data, len + 1);
    if (ret != 0) {
        printk(KERN_EMERG "i2c_write_bmi160 error\n");
        return -1;
    }
    return 0;
}

static int spi_write_bmi160(struct spi_device *device, char *data, uint32_t len)
{
    int ret = 0;
    uint8_t tx_data[200] = {0};
    uint8_t reg_addr = 0;
    int i;

    tx_data[0] = data[0];
    for (i = 0; i < len; i++) tx_data[i + 1] = data[i + 1];
    ret = spi_transfer(device, tx_data, NULL, len);
    if (ret != 0) {
        printk(KERN_EMERG "i2c_write_bmi160 error\n");
        return -1;
    }
    return 0;
}

/** 字符设备操作函数集，read */
ssize_t bmi160_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    char rx_buf[200] = {0};
    int error;

    /** 先将用户空间的地址读到内核空间 */
    error = copy_from_user(rx_buf, buf, 1);
    /** 从设备读数据到内核空间 */
    spi_read_bmi160(bmi160_device, rx_buf, len);
    /** 将内核空间得到的数据拷贝到用户空间 */
    error = copy_to_user(buf, rx_buf + 1, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    return 0;
}

ssize_t bmi160_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    char tx_buf[200] = {0};
    int error;

    /*将用户空间的数据复制到内核空间*/
    error = copy_from_user(tx_buf, buf, len);

    if (error != 0) {
        printk(KERN_EMERG "copy_to_user error!");
        return -1;
    }
    spi_write_bmi160(bmi160_device, tx_buf, len);
    return 0;
}

/** 字符设备操作函数集 start */
static int bmi160_init(void)
{
    int error = 0;

    if (error < 0) {
        /*初始化错误*/
        printk(KERN_DEBUG "\n bmi160_init error \n");
        return -1;
    } else
        printk(KERN_DEBUG "\n bmi160_init ok ! \n");
    return 0;
}

/** 字符设备操作函数集，open */
static int bmi160_open(struct inode *inode, struct file *filp)
{
    /*向 bmi160 发送配置数据，让bmi160处于正常工作状态*/
    bmi160_init();
    return 0;
}

/** 字符设备操作函数集，release */
static int bmi160_release(struct inode *inode, struct file *filp) { return 0; }

/** 字符设备操作函数集，结构体 */
static struct file_operations bmi160_chr_dev_fops = {
    .owner = THIS_MODULE,
    .open = bmi160_open,
    .release = bmi160_release,
    .read = bmi160_read,
    .write = bmi160_write,
};
/** 字符设备操作函数集 end */

/** 平台驱动函数集 start */
static int bmi160_probe(struct spi_device *device)
{
    int ret = -1;

    printk(KERN_EMERG "match successed  \n");

    /** 采用动态分配的方式，获取设备编号，次设备号为0，设备名称为rgb-leds，可通过命令cat
     * /proc/devices查看DEV_CNT为1，当前只申请一个设备编号 */
    ret = alloc_chrdev_region(&bmi160_devno, 0, DEV_CNT, DEV_NAME);
    if (ret < 0) {
        printk("fail to alloc bmi160_devno\n");
        goto alloc_err;
    }

    /** 关联字符设备结构体cdev与文件操作结构体file_operations */
    bmi160_chr_dev.owner = THIS_MODULE;
    cdev_init(&bmi160_chr_dev, &bmi160_chr_dev_fops);

    /** 添加设备至cdev_map散列表中 */
    ret = cdev_add(&bmi160_chr_dev, bmi160_devno, DEV_CNT);
    if (ret < 0) {
        printk("fail to add cdev\n");
        goto add_err;
    } else
        printk(KERN_EMERG "bmi160_devno=%d\n", bmi160_devno);

    /** 创建类 */
    class_bmi160 = class_create(THIS_MODULE, DEV_NAME);

    /** 创建设备 DEV_NAME 指定设备名 */
    device_bmi160 = device_create(class_bmi160, NULL, bmi160_devno, NULL, DEV_NAME);
    bmi160_device = device;
    printk(KERN_EMERG "create successed \n");
    return 0;

add_err:
    /** 添加设备失败时，需要注销设备号 */
    unregister_chrdev_region(bmi160_devno, DEV_CNT);
    printk(KERN_EMERG "\n add_err error! \n");
alloc_err:

    return -1;
}

static int bmi160_remove(struct spi_device *device)
{
    /** 删除设备 */
    printk(KERN_EMERG "\t  remove device!  \n");
    device_destroy(class_bmi160, bmi160_devno);       // 清除设备
    class_destroy(class_bmi160);                      // 清除类
    cdev_del(&bmi160_chr_dev);                        // 清除设备号
    unregister_chrdev_region(bmi160_devno, DEV_CNT);  // 取消注册字符设备
    return 0;
}

/*定义ID匹配表*/
static const struct spi_device_id gtp_device_id[] = {{"barco,bmi160", 0}, {}};

/*定义设备树匹配表*/
static const struct of_device_id bmi160_of_match_table[] = {{.compatible = "barco,bmi160"}, {/* sentinel */}};

/*定义i2c总线设备结构体*/
struct spi_driver bmi160_driver = {
    .probe = bmi160_probe,
    .remove = bmi160_remove,
    .id_table = gtp_device_id,
    .driver =
        {
            .name = "barco,bmi160",
            .owner = THIS_MODULE,
            .of_match_table = bmi160_of_match_table,
        },
};
/** 平台驱动函数集 end */

/** 驱动初始化 */
static int __init bmi160_driver_init(void)
{
    int ret;

    printk(KERN_EMERG "bmi160_driver_init\n");
    ret = spi_register_driver(&bmi160_driver);
    return ret;
}

/** 驱动注销 */
static void __exit bmi160_driver_exit(void)
{
    printk(KERN_EMERG "bmi160_driver_exit\n");
    spi_unregister_driver(&bmi160_driver);
}

module_init(bmi160_driver_init);
module_exit(bmi160_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("crz");
MODULE_DESCRIPTION("spi module");
MODULE_ALIAS("spi_module");
