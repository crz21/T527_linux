
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i2c_bmi160.h"

#define DEV_OPERATION "/dev/I2C1_BMI160"

int main(int argc, char *argv[])
{
    short resive_data[6];
    int id_buf = 0;
    int error;
    int fd = open(DEV_OPERATION, O_RDWR);
    if (fd < 0) {
        printf("open file : %s failed, fd = %d !\n", argv[0], fd);
        return -1;
    }
    error = read(fd, &id_buf, 1);
    if (error < 0) {
        printf("write file error! \n");
        close(fd);
    } else
        printf("id = %d! \n", id_buf);

    while (1) {
        error = read(fd, resive_data, 12);
        if (error < 0) {
            printf("write file error! \n");
            close(fd);
        }

        printf("AX=%d, AY=%d, AZ=%d ", (int)resive_data[0], (int)resive_data[1], (int)resive_data[2]);
        printf("    GX=%d, GY=%d, GZ=%d \n \n", (int)resive_data[3], (int)resive_data[4], (int)resive_data[5]);
        sleep(1);
    }
    return 0;
}
