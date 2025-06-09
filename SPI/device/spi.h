#ifndef __SPI_H__
#define __SPI_H__

int spi_transfer(int fd, uint8_t *tx, uint8_t *rx, uint32_t len);
int spi_init(int *fd);
#endif
