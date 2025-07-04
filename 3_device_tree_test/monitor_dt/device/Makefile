CAMERA_I2C_DIR:=device/camera_i2c
LCD_SPI_DIR:=device/lcd_spi

export CAMERA_I2C_DIR LCD_SPI_DIR

all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
ECHO:
	@echo $(SUBDIRS)

