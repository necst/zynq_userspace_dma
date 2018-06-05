#include <stdio.h>
#include <stdlib.h>
#include "dmabuf.h"

#define NUM_BUFFERS 2

static int flash_bitstream(const char *path)
{
    int retval;
    char *command = malloc( 2048 * sizeof(char) );
    sprintf(command, "cat %s > /dev/xdevcfg\n", path);
    retval = system(command);

    free(command);
    return retval;
}

#define BUFSIZE 1024U
#define PLUS 1

int main(int argc, char **argv)
{
	unsigned long sizes[NUM_BUFFERS] = { BUFSIZE, BUFSIZE };
	struct udmabuf buffers[NUM_BUFFERS];
    struct dma_engine engine;
    unsigned int i, err;
    int *b1, *b2;

	load_udma_buffers( NUM_BUFFERS, sizes, buffers);

    get_dma_interfaces(1, &engine, NULL);

    printf("ubuffer 0:\n\tphys addr %lx\n\tvirt mapping %p\n\tlength %lu\n",
        (unsigned long)buffers->paddr, buffers->vaddr, buffers->size);

    printf("ubuffer 1:\n\tphys addr %lx\n\tvirt mapping %p\n\tlength %lu\n",
        (unsigned long)buffers[1].paddr, buffers[1].vaddr, buffers[1].size);

    printf("created\n");

    /* init buffers */
    b1 = (int*)buffers->vaddr;
    b2 = (int*)buffers[1].vaddr;
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        b1[i] = (int)i + PLUS;
        b2[i] = 0;
    }

    /* send data */
    set_simple_transfer_to_device(&engine, buffers, 0, BUFSIZE);

    start_simple_transfer_to_device(&engine);

    wait_simple_transfer_to_device(&engine, 0);

    /* collect results */
    set_simple_transfer_from_device(&engine, buffers + 1, 0, BUFSIZE);

    start_simple_transfer_from_device(&engine);

    wait_simple_transfer_from_device(&engine, 0);

    /* check results */
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        if (b2[i] != b1[i] + PLUS) {
            err = 1;
            printf("ERROR in position %u: %i instead of %i\n", i, b2[i], b1[i]);
        }
    }

    if (!err) {
        printf("no erros found!\n");
    }
    
    destroy_dma_interfaces(1, &engine);

    printf("destroyed\n");

	unload_udma_buffers( NUM_BUFFERS, buffers);
	return 0;
}
