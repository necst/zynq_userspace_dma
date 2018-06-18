############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################

set curdir [ file dirname [ file normalize [ info script ] ] ]

open_project -reset pynq_vec_2d_sum
set_top top_vec_2d_sum
add_files $curdir/src/vec_2d_sum.cpp
add_files $curdir/src/io_utils.hpp
add_files $curdir/src/stream_utils.hpp
add_files -tb $curdir/src/vec_2d_sum_tb.cpp
open_solution -reset "solution1"
set_part {xc7z020clg400-3} -tool vivado
create_clock -period 10 -name default
#source "./pynq_vec_2d_sum/solution1/directives.tcl"
csim_design -compiler gcc
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog -description "IP computing the linear sum of two input vectors with offset" -display_name "2D vector sum"

exit 0
