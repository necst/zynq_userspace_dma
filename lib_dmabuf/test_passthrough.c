#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dma_engine_buf.h"
#include "xhw_internals.h"
#include "utils.h"

#define NUM_BUFFERS 2

#define BUFSIZE (1024U * 4U)
#define PLUS 1

int main(int argc, char **argv)
{
    unsigned long sizes[NUM_BUFFERS] = { BUFSIZE, BUFSIZE };
    struct udmabuf buffers[NUM_BUFFERS];
    struct dma_engine engine;
    unsigned int i, err = 0;
    int *b1, *b2;
    enum dma_err_status err_retval;

    load_udma_buffers( NUM_BUFFERS, sizes, buffers);

    printf("DMA buffers created\n");

    print_buffer_status(0, buffers);
    print_buffer_status(1, buffers + 1);

    get_dma_interfaces(1, NULL, NULL, &engine);

    printf("DMA engine created\n");

    /* init buffers */
    b1 = (int*)buffers->vaddr;
    b2 = (int*)buffers[1].vaddr;
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        b1[i] = (int)i + PLUS;
        b2[i] = 0;
    }

    /*
     * initiate DMA transaction from device
     */
    printf("\nstarting transfer from device...\n");
    err_retval = set_simple_transfer_from_device(&engine, buffers + 1, 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_from_device(&engine);
    check_err(err_retval);
    printf("transfer from device started\n");

    /*
     * initiate DMA transaction to device
     */
    print_engine(&engine);
    printf("\nstarting transfer to device...\n");
    err_retval = set_simple_transfer_to_device(&engine, buffers, 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_to_device(&engine);
    check_err(err_retval);
    printf("transfer to device started\n");
    print_engine(&engine);

    /*
     * wait for transactions to and from device
     */
    printf("\nwaiting for transfer to device...\n");
    err_retval = wait_simple_transfer_to_device(&engine, 0);
    check_err(err_retval);

    printf("\nwaiting for transfer from device...\n");
    err_retval = wait_simple_transfer_from_device(&engine, 0);
    check_err(err_retval);
    print_engine(&engine);

    /*
     * check results
     */
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        if (b2[i] != b1[i]) {
            err = 1;
            printf("ERROR in position %u: %i instead of %i\n", i, b2[i], b1[i]);
        }
    }

    if (!err) {
        printf("no errors found\n");
    }

    destroy_dma_interfaces(1, &engine);

    unload_udma_buffers( NUM_BUFFERS, buffers);

    return err;
}
