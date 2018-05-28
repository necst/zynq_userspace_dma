#include <stdio.h>
#include "dmabuf.h"

#define NUM_BUFFERS 3

int main(int argc, char **argv)
{
	unsigned long sizes[NUM_BUFFERS] = { 1, 2, 3};
	struct udmabuf buffers[NUM_BUFFERS];
	load_udma_buffers( NUM_BUFFERS, sizes, buffers);
	unload_udma_buffers( NUM_BUFFERS, buffers);
	return 0;
}
