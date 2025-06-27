wget  https://mirrors.tuna.tsinghua.edu.cn/armbian-releases/_toolchain/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu.tar.xz

git clone https://github.com/ARM-software/arm-trusted-firmware.git

cd arm-trusted-firmware

make PLAT=sun50i_h616  DEBUG=1 bl31

cd ../uboot/

make walnutpi_1b_defconfig

make BL31=../arm-trusted-firmware/build/sun50i_h616/debug/bl31.bin