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

#define DEV_I2C "/dev/i2c-1"

// 计算mlx9614实际温度值
uint16_t mlx_data_transform(uint8_t Data[3])
{
    uint16_t temp;
    temp = (Data[1] << 8) + Data[0];  // 高位与低位结合
    temp = temp * 2 - 27315;          // 将数据扩大100倍
    return temp;
}

int main()
{
    int fd = open(DEV_I2C, O_RDWR);
    if (fd < 0) {
        perror("Fail to Open\n");
        return -1;
    }

    uint8_t addr = 0x38;
    uint8_t reg = 0x71;
    uint8_t value[6];
    uint8_t reg_buf[3];
    int res;
    uint32_t humidity;
    uint32_t temperature;
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

    res = ioctl(fd, I2C_RDWR, &data);
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

    res = ioctl(fd, I2C_RDWR, &data);
    if (res < 0) printf("Read failed2\n");
    printf("statue= %d %d %d %d\n", value[0], value[1], value[2], value[3]);
    humidity = (value[1] << 12) | (value[2] << 4) | (value[3] >> 4);
    humidity = (humidity * 100);
    humidity = humidity / 0x100000;
    printf("humidity= %d \n", humidity);
    while (1) {
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

        res = ioctl(fd, I2C_RDWR, &data);
        if (res < 0) printf("Read failed3\n");

        temperature = ((value[3] & 0xF) << 16) | (value[4] << 8) | (value[5]);
        temperature = (temperature * 200);
        temperature = temperature / 0x100000;
        temperature = temperature - 50;
        printf("temp=[%d]\r\n", temperature);
        }
}