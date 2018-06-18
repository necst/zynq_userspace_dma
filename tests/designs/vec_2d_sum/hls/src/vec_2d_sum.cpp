
#include <hls_stream.h>
#include <stdint.h>

#include "io_utils.hpp"
#include "stream_utils.hpp"


void compute(hls::stream<data_t> &inStream1,
		hls::stream<data_t> &inStream2,
		hls::stream<data_t> &outStream,
		uint32_t num,
		data_t a,
		data_t b,
		data_t c)
{
	for(uint32_t i = 0; i < num; i++)
	{
#pragma HLS PIPELINE II=1
#pragma HLS latency min=2

		data_t in1 = inStream1.read();
		data_t in2 = inStream2.read();

		data_t out = a * in1 + b * in2 + c;

		outStream.write(out);
	}
}


void top_vec_2d_sum(hls::stream<data_axis> &inStream1,
		hls::stream<data_axis> &inStream2,
		hls::stream<data_axis> &outStream,
		const uint32_t num,
		data_t a,
		data_t b,
		data_t c)
{
#pragma HLS INTERFACE axis register port=inStream1
#pragma HLS INTERFACE axis register port=inStream2
#pragma HLS INTERFACE axis register port=outStream
#pragma HLS INTERFACE s_axilite register port=return bundle=control
#pragma HLS INTERFACE s_axilite register port=num bundle=control
#pragma HLS INTERFACE s_axilite register port=a bundle=control
#pragma HLS INTERFACE s_axilite register port=b bundle=control
#pragma HLS INTERFACE s_axilite register port=c bundle=control

#pragma HLS DATAFLOW

	hls::stream<data_t> inData1("x_stream"), inData2("y_stream"), outData("z_stream");

#pragma HLS STREAM variable=inData1 depth=1 dim=1
#pragma HLS STREAM variable=inData2 depth=1 dim=1
#pragma HLS STREAM variable=outData depth=1 dim=1

	pump_data_in<data_t>(inStream1, inData1, num);
	pump_data_in<data_t>(inStream2, inData2, num);

	compute(inData1, inData2, outData, num, a, b, c);

	pump_data_out<data_t>(outData, outStream, num);

}
