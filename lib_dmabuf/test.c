#include <stdio.h>
#include "dmabuf.h"

#define NUM_BUFFERS 2

int main(int argc, char **argv)
{
	unsigned long sizes[NUM_BUFFERS] = { 1024, 1024};
	struct udmabuf buffers[NUM_BUFFERS];
    struct dma_engine engine;

	load_udma_buffers( NUM_BUFFERS, sizes, buffers);

    get_dma_interfaces(1, &engine, NULL);

    printf("created\n");

    destroy_dma_interfaces(1, &engine);

    printf("destroyed\n");

	unload_udma_buffers( NUM_BUFFERS, buffers);
	return 0;
}
