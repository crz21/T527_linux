#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEV_SPI "/dev/spidev1.0"
#define SPI_MODE_0 (0 | 0) /* (original MicroWire) */
#define SPI_MODE_1 (0 | SPI_CPHA)
#define SPI_MODE_2 (SPI_CPOL | 0)
#define SPI_MODE_3 (SPI_CPOL | SPI_CPHA)

static uint32_t spi_mode = SPI_MODE_0;  // | SPI_3WIRE;
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

int spi_init(int *fd)
{
    *fd = open(DEV_SPI, O_RDWR);
    if (*fd < 0) {
        perror("Fail to Open\n");
        return -1;
    }

    /** 配置spi模式 */
    if (ioctl(*fd, SPI_IOC_WR_MODE32, &spi_mode) == -1) printf("err: can't set spi mode");
    if (ioctl(*fd, SPI_IOC_RD_MODE32, &spi_mode) == -1) printf("can't get spi mode");
    if (ioctl(*fd, SPI_IOC_WR_BITS_PER_WORD, &bits_word) == -1) printf("can't set bits per word");
    if (ioctl(*fd, SPI_IOC_RD_BITS_PER_WORD, &bits_word) == -1) printf("can't get bits per word");
    if (ioctl(*fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) printf("can't set max speed hz");
    if (ioctl(*fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) printf("can't get max speed hz");

    return 0;
}
