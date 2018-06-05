#!/bin/bash

# cd /usr/src/linux-headers-4.9.0-custom1-xilinx-dirty
# sudo make scripts

cd udmabuf
export KERNEL_SRC_DIR=/lib/modules/4.9.0-custom1-xilinx-dirty/build
make
cd ..
