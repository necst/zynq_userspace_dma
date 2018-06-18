
#include <stdio.h>
#include <stdint.h>
#include <hls_stream.h>
#include <ap_int.h>

#include "stream_utils.hpp"

#define NUM 25
#define A 1
#define B 2
#define C 3

extern void top_vec_2d_sum(hls::stream<data_axis> &inStream1,
		hls::stream<data_axis> &inStream2,
		hls::stream<data_axis> &outStream,
		uint32_t num,
		data_t a,
		data_t b,
		data_t c);

int main()
{
	int32_t in1[NUM], in2[NUM], out[NUM];
	unsigned i;
	hls::stream<data_axis> inStream1, inStream2, outStream;
	int errval = 0;

	for (i = 0; i < NUM; i++)
	{
		in1[i] = -i;
		in2[i] = NUM - i;
		out[i] = 0;
	}

	// pump data to core
	for (i = 0; i < NUM; i++)
	{
		data_axis da1, da2;
		set_data_axis<uint32_t>(da1, 0, 0);
		set_data_axis<uint32_t>(da2, 0, 0);

		da1.data = ap_uint<32>(in1[i]);
		inStream1.write(da1);
		da2.data = ap_uint<32>(in2[i]);
		inStream2.write(da2);
	}

	top_vec_2d_sum(inStream1, inStream2, outStream, NUM, A, B, C);

	for (i = 0; i < NUM; i++)
	{
		union {uint32_t u; int32_t i;} u2i;
		data_axis out = outStream.read();
		u2i.u = out.data.to_uint();

		int32_t oracle = A * in1[i] + B * in2[i] + C;
		if (u2i.i != oracle)
		{
			printf("index %u: received %d instead of %d\n", i, u2i.i, oracle);
			errval = -1;
		}
		if (i < NUM - 1 && out.last)
		{
			printf("value %u set TLAST!\n", i);
			errval = -1;
		}
		if (i == NUM - 1 && !out.last)
		{
			printf("last value did NOT set TLAST!\n");
			errval = -1;
		}
		if (i == NUM - 1 && out.last)
		{
			printf("last value properly set TLAST!\n");
		}
	}


	if (!errval)
	{
		printf("no errors found\n");
	}

	return errval;
}
