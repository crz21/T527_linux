#! /bin/bash

clear
make clean
make
sudo cp sun55i-t527-bmi160.dtbo /boot/overlays/
