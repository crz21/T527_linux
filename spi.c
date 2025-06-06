#include <errno.h>
#include <fcntl.h>
// #include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "/usr/include/linux/spi/spidev.h"
#define DEV_SPI "/dev/spidev1.0"
#define	SPI_MODE_0		(0|0)		/* (original MicroWire) */
#define	SPI_MODE_1		(0|SPI_CPHA)
#define	SPI_MODE_2		(SPI_CPOL|0)
#define	SPI_MODE_3		(SPI_CPOL|SPI_CPHA)

static uint32_t spi_mode = SPI_MODE_0;// | SPI_3WIRE;
static uint8_t bits_word = 8;
static uint32_t speed = 500000;
static void pabort(const char *s)
{
    if (errno != 0)
        perror(s);
    else
        printf("%s\n", s);

    abort();
}
// mosi跟miso同时工作，从tx_buf中取出数据发送的同时，也会读取数据存入rx_buf
int spi_transfer(int fd, uint8_t *tx, uint8_t *rx, uint32_t len)
{
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = 500,  // 发送完成后的延时
        .speed_hz = speed,
        .bits_per_word = bits_word,
        .tx_nbits = 1,   // 单线制
        .rx_nbits = 1,   // 单线制
        .cs_change = 0,  // 传输后把cs线松开
    };

    int res = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);  // 触发transfer
    if (res < 1) pabort("can't send spi message");

    return res;
}
int main(int argc, char *argv[])
{
    uint16_t value;
    int res;

    if (argc > 1) {
        printf("input command :%s\n", argv[1]);
        if (strcasecmp(argv[1], "help") == 0)
            printf("input help \n");
        else if (strcasecmp(argv[1], "cmd") == 0)
            printf("input cmd\n");
    }

    int fd0 = open(DEV_SPI, O_RDWR);
    if (fd0 < 0) {
        perror("Fail to Open\n");
        return -1;
    }

    /** 配置spi模式 */
    if (ioctl(fd0, SPI_IOC_WR_MODE32, &spi_mode) == -1) printf("err: can't set spi mode");
    if (ioctl(fd0, SPI_IOC_RD_MODE32, &spi_mode) == -1) printf("can't get spi mode");
    if (ioctl(fd0, SPI_IOC_WR_BITS_PER_WORD, &bits_word) == -1) printf("can't set bits per word");
    if (ioctl(fd0, SPI_IOC_RD_BITS_PER_WORD, &bits_word) == -1) printf("can't get bits per word");
    if (ioctl(fd0, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) printf("can't set max speed hz");
    if (ioctl(fd0, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) printf("can't get max speed hz");

    uint8_t tx_buf[3] = {0x01, 0x80, 0x00};
    uint8_t rx_buf[3];

    /** mcp3004要求以cs0的下降沿作为spi通信开始的标志
     * 因为当前spi1下只有一个cs0设备，所以初始时cs0一直是低电平
     * 先发送一个数据，触发cs0拉高
     */
    spi_transfer(fd0, tx_buf, rx_buf, 1);

    while (1) {
        res = spi_transfer(fd0, tx_buf, rx_buf, 3);
        if (res >= 1) {
            value = (rx_buf[1] << 8) + rx_buf[2];
            printf("\r\nvalue=%d\r\n", value);
        }
        sleep(1);
    }

    return 0;
}
