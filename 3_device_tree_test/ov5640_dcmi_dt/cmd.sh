#! /bin/bash

make clean
make
sudo cp debug/obj/sun55i-t527-ov5640-dcmi.dtbo /boot/overlays/