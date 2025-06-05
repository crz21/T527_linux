#include <errno.h>
#include <fcntl.h>
// #include <linux/spi/spidev.h>
#include "/usr/src/linux-headers-5.15.147/include/uapi/linux/spi/spidev.h"
// #include "/usr/include/linux/spi/spidev.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEV_SPI "/dev/spidev1.0"

static uint32_t SPI_MODE = SPI_MODE_0;
static uint8_t BITS_PER_WORD = 8;
static uint32_t SPEED = 100 * 10;

// mosi��misoͬʱ��������tx_buf��ȡ�����ݷ��͵�ͬʱ��Ҳ���ȡ���ݴ���rx_buf
int spi_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, int len)
{
    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(struct spi_ioc_transfer));
    transfer.tx_buf = (unsigned long)tx_buf;
    transfer.rx_buf = (unsigned long)rx_buf;
    transfer.len = len;
    transfer.delay_usecs = 500;  // ������ɺ����ʱ
    transfer.speed_hz = SPEED;
    transfer.bits_per_word = BITS_PER_WORD;
    transfer.tx_nbits = 1;   // ������
    transfer.rx_nbits = 1;   // ������
    transfer.cs_change = 0;  // ������cs���ɿ�
    transfer.word_delay_usecs = 0;
    transfer.pad = 0;
    int res = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);  // ����transfer
    if (res < 0) printf("ioctl : %s\n", strerror(errno));
    return res;
}
int main()
{
    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(struct spi_ioc_transfer));
    int fd0 = open(DEV_SPI, O_RDWR);
    if (fd0 < 0) {
        perror("Fail to Open\n");
        return -1;
    }

    // ����spiģʽ
    if (ioctl(fd0, SPI_IOC_WR_MODE, &SPI_MODE) == -1) printf("err: can't set spi mode");

    uint8_t tx_buf[3] = {0x01, 0x80, 0x00};
    uint8_t rx_buf[3];

    // mcp3004Ҫ����cs0���½�����Ϊspiͨ�ſ�ʼ�ı�־
    // ��Ϊ��ǰspi1��ֻ��һ��cs0�豸�����Գ�ʼʱcs0һֱ�ǵ͵�ƽ
    // �ȷ���һ�����ݣ�����cs0����
    // spi_transfer(fd0, tx_buf, rx_buf, 1);
    
    transfer.tx_buf = (unsigned long)tx_buf;
    transfer.rx_buf = (unsigned long)rx_buf;
    transfer.len = 1;
    transfer.delay_usecs = 500;  // ������ɺ����ʱ
    transfer.speed_hz = SPEED;
    transfer.bits_per_word = BITS_PER_WORD;
    transfer.tx_nbits = 1;   // ������
    transfer.rx_nbits = 1;   // ������
    transfer.cs_change = 0;  // ������cs���ɿ�
    transfer.word_delay_usecs = 0;
    transfer.pad = 0;


    int res = ioctl(fd0, SPI_IOC_MESSAGE(1), &transfer);  // ����transfer
    if (res < 0) printf("ioctl : %d\n", sizeof(struct spi_ioc_transfer));
    if (res < 0) printf("ioctl : %s\n", strerror(errno));
    // return res;

    uint16_t value;
    while (1) {
        // spi_transfer(fd0, tx_buf, rx_buf, 3);

        transfer.tx_buf = (unsigned long)tx_buf;
        transfer.rx_buf = (unsigned long)rx_buf;
        transfer.len = 3;
        transfer.delay_usecs = 500;  // ������ɺ����ʱ
        transfer.speed_hz = SPEED;
        transfer.bits_per_word = BITS_PER_WORD;
        transfer.tx_nbits = 1;   // ������
        transfer.rx_nbits = 1;   // ������
        transfer.cs_change = 0;  // ������cs���ɿ�

        int res = ioctl(fd0, SPI_IOC_MESSAGE(1), &transfer);  // ����transfer
        if (res < 0) printf("ioctl : %s\n", strerror(errno));
        value = (rx_buf[1] << 8) + rx_buf[2];
        printf("value=%d\r\n ", value);
        sleep(1);
    }

    return 0;
}
