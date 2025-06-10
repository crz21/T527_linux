#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "i2c.h"

int main(int argc, char *argv[])
{
    int fd;
    int res;
    uint8_t value[6];
    uint32_t temperature;

    i2c_init(&fd);

    while (1) {
        sleep(1);
        res = i2c_read_val(fd, value);
        if (res < 0) printf("Read failed3\n");

        temperature = ((value[3] & 0xF) << 16) | (value[4] << 8) | (value[5]);
        temperature = (temperature * 200);
        temperature = temperature / 0x100000;
        temperature = temperature - 50;
        printf("temp=[%d]\r\n", temperature);
    }
    return 0;
}