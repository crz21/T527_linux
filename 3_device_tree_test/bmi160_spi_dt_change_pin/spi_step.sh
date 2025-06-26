#! /bin/bash

make clean
make
sudo cp debug/obj/sun55i-t527-bmi160-spi-change-pin.dtbo /boot/overlays/