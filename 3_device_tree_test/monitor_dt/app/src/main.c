#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static void pabort(const char *s)
{
    if (errno != 0)
        perror(s);
    else
        printf("%s\n", s);

    abort();
}

#define DEV_OPERATION "/dev/I2C1_IMX219"

int8_t imx219_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int res, i = 0;
    uint8_t tx_buf[200] = {0};

    tx_buf[0] = reg_addr;
    for (i = 0; i < len; i++) tx_buf[i + 1] = data[i];
    res = read(fd, tx_buf, len);
    for (i = 0; i < len; i++) data[i] = tx_buf[i];
    if (res < 0) pabort("can't send message");
    return 0;
}

int8_t imx219_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    int res, i = 0;
    uint8_t tx_buf[200] = {0};

    tx_buf[0] = reg_addr;
    for (i = 0; i < len; i++) tx_buf[i + 1] = data[i];
    res = write(fd, tx_buf, len + 1);
    if (res < 0) pabort("can't send message");
    return 0;
}