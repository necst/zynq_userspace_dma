# cd /usr/src/linux-headers-4.9.0-xilinx/
# sudo make modules_prepare
# cd ..
# ls
# cd linux-headers-4.9.0-custom1-xilinx-dirty/
# ls
# sudo make modules_prepare
# sudo make prepare scripts
# sudo make scripts

cd udmabuf
export KERNEL_SRC_DIR=/lib/modules/4.9.0-custom1-xilinx-dirty/build
make
cd ..
