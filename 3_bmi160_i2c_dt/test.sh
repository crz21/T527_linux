#! /bin/bash
## crz

clear
make clean
make
sudo cp sun55i-t527-bmi160.dtbo /boot/overlays/
sudo set-device enable bmi160
# sudo reboot
