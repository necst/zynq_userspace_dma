
#pragma once

#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <stdint.h>

#include "io_utils.hpp"

#define VAL2(x) x
#define VAL(x) VAL2(x)


template<typename DestT> void pump_data_in(hls::stream<data_axis> &inStream,
		hls::stream<DestT>  &outStream,
		const uint32_t length)
{

#ifndef MORE_THAN_32B
	typedef uint32_t bridge_t;
#else
	typedef uint64_t bridge_t;
#endif

	for(uint32_t i = 0; i < length; i++)
	{
#pragma HLS PIPELINE II=1
		union {bridge_t in; DestT out;} tmp;

		tmp.in = (bridge_t)(inStream.read().data.to_uint());
		outStream.write(tmp.out);
	}
}

template<typename T> void set_data_axis(data_axis &d, T data, int last)
{
#pragma HLS INLINE
	d.data = ap_uint<DATA_WIDTH>(data);
	d.dest = 0;
	d.id = 0;
	d.keep = (1 << (DATA_WIDTH / 8)) - 1;
	d.strb = (1 << (DATA_WIDTH / 8)) - 1;
	d.user = 0;
	d.last = (last != 0)? ap_uint<1>(1) : ap_uint<1>(0);
}

template<typename InT> void pump_data_out(hls::stream<InT> &inStream,
		hls::stream<data_axis>  &outStream,
		const uint32_t length)
{

#ifndef MORE_THAN_32B
	typedef uint32_t bridge_t;
#else
	typedef uint64_t bridge_t;
#endif

	for(uint32_t i = 0; i < length; i++)
	{
#pragma HLS PIPELINE II=1
		union {InT in; bridge_t out;} tmp;

		tmp.in = inStream.read();

		data_axis outData;
		set_data_axis<bridge_t>(outData, tmp.out, i == length - 1);

		outStream.write(outData);
	}
}
