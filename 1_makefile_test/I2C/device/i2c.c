#include "i2c.h"

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
#include <stdint.h>

#define DEV_I2C "/dev/i2c-1"

int i2c_read_val(int fd, uint8_t* buf)
{
    uint8_t addr = 0x38;
    uint8_t reg = 0x71;
    uint8_t reg_buf[3];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data data;
    int res;

    reg_buf[0] = 0xAC;
    reg_buf[1] = 0x33;
    reg_buf[2] = 0;
    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].len = 3;
    msgs[0].buf = reg_buf;

    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 6;
    msgs[1].buf = buf;

    data.msgs = msgs;
    data.nmsgs = 2;

    res = ioctl(fd, I2C_RDWR, &data);

    return res;
}

void i2c_init(int* fd)
{
    uint8_t addr = 0x38;
    uint8_t reg = 0x71;
    uint8_t value[6];
    uint8_t reg_buf[3];
    int res;
    uint32_t humidity;
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data data;

    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 3;
    msgs[1].buf = value;

    data.msgs = msgs;
    data.nmsgs = 2;

    res = ioctl(*fd, I2C_RDWR, &data);
    if (res < 0) printf("Read failed1\n");

    if (value[0] != 0x18)
        printf("error num=%d\n", value[0]);
    else
        printf("num=%d\n", value[0]);

    sleep(1);
    reg_buf[0] = 0xAC;
    reg_buf[1] = 0x33;
    reg_buf[2] = 0;
    msgs[0].addr = addr;
    msgs[0].flags = 0;
    msgs[0].len = 3;
    msgs[0].buf = reg_buf;

    msgs[1].addr = addr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 6;
    msgs[1].buf = value;

    data.msgs = msgs;
    data.nmsgs = 2;

    res = ioctl(*fd, I2C_RDWR, &data);
    if (res < 0) printf("Read failed2\n");

    humidity = (value[1] << 12) | (value[2] << 4) | (value[3] >> 4);
    humidity = (humidity * 100);
    humidity = humidity / 0x100000;
    printf("humidity= %d \n", humidity);
}