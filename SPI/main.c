#include <stdint.h>

#include "spi.h"

int main(int argc, char *argv[])
{
    int fd0;
    uint8_t tx_buf[3] = {0x01, 0x80, 0x00};
    uint8_t rx_buf[3];
    uint16_t value;
    int res;

    if (argc > 1) {
        printf("input command :%s\n", argv[1]);
        if (strcasecmp(argv[1], "help") == 0)
            printf("input help \n");
        else if (strcasecmp(argv[1], "cmd") == 0)
            printf("input cmd\n");
    }

    spi_init(&fd0);
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