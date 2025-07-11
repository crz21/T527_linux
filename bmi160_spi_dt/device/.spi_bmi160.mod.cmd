cmd_/home/pi/bmi160_spi_dt/device/spi_bmi160.mod := printf '%s\n'   spi_bmi160.o | awk '!x[$$0]++ { print("/home/pi/bmi160_spi_dt/device/"$$0) }' > /home/pi/bmi160_spi_dt/device/spi_bmi160.mod
