#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

int i2c_read_val(int fd, uint8_t* buf);
void i2c_init(int* fd);

#endif
