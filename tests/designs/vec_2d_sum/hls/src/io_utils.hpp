
#pragma once

#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <stdint.h>

typedef int32_t data_t;

#define DATA_WIDTH ( ((int)sizeof(data_t)) * 8 )
//#define MORE_THAN_32B

typedef struct ap_axiu<DATA_WIDTH, 1, 1, 1> data_axis;
