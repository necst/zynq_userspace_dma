#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dma_engine_buf.h"
#include "xhw_internals.h"
#include "utils.h"

#define NUM_BUFFERS 3

#define BUFSIZE (1024U * 4U)
#define A 1
#define B 52
#define C 4

int main(int argc, char **argv)
{
	unsigned long sizes[NUM_BUFFERS] = { BUFSIZE, BUFSIZE, BUFSIZE };
	struct udmabuf buffers[NUM_BUFFERS];

    phys_addr_t dmas[] = {0x40400000, 0x40410000};
    unsigned dma_lengths[] = {AXI_CONTROL_REGS_LEN_DEF, AXI_CONTROL_REGS_LEN_DEF};
    struct dma_engine engine[2];

    struct control_interface vec_sum;

    unsigned int i, err = 0;
    int *in1, *in2, *out;
    enum dma_err_status err_retval;

	load_udma_buffers( NUM_BUFFERS, sizes, buffers);

    printf("DMA buffers created\n");

    print_buffer_status(0, buffers);
    print_buffer_status(1, buffers + 1);

    get_dma_interfaces(2, dmas, dma_lengths, engine);

    printf("DMA engine created\n");

    get_control_interface( 0x43C00000, AXI_CONTROL_REGS_LEN_DEF, &vec_sum);

    printf("2D VecSum kernel interface created\n");

    /* init buffers */
    in1 = (int*)buffers[0].vaddr;
    in2 = (int*)buffers[1].vaddr;
    out = (int*)buffers[2].vaddr;
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        in1[i] = (int)i;
        in2[i] = BUFSIZE - (int)i;
        out[i] = 0;
    }

    /*
     * set kernel arguments
     */
    set_kernel_argument(&vec_sum, 0, BUFSIZE / sizeof(int));
    set_kernel_argument(&vec_sum, 1, A);
    set_kernel_argument(&vec_sum, 2, B);
    set_kernel_argument(&vec_sum, 3, C);

    /*
     * initiate DMA transaction from device
     */
    printf("\nstarting transfer from device 0...\n");
    err_retval = set_simple_transfer_from_device(engine, buffers + 2, 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_from_device(engine);
    check_err(err_retval);
    printf("transfer from device 0 started\n");

    /*
     * initiate DMA transaction to devices
     */
    printf("\nstarting transfer to device 0...\n");
    err_retval = set_simple_transfer_to_device(engine, buffers, 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_to_device(engine);
    check_err(err_retval);
    printf("transfer to device 0 started\n");

    printf("\nstarting transfer to device 1...\n");
    err_retval = set_simple_transfer_to_device(engine + 1, buffers, 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_to_device(engine + 1);
    check_err(err_retval);
    printf("transfer to device 1 started\n");

    start_kernel(&vec_sum);

    /*
     * wait for kernel
     */
    printf("\nwaiting for kernel...\n");
    wait_kernel(&vec_sum, 0);

    /*
     * wait for transactions to and from device
     */
    printf("\nwaiting for transfer to device 0...\n");
    err_retval = wait_simple_transfer_to_device(engine, 0);
    check_err(err_retval);

    printf("\nwaiting for transfer to device 1...\n");
    err_retval = wait_simple_transfer_to_device(engine + 1, 0);
    check_err(err_retval);

    printf("\nwaiting for transfer from device 0...\n");
    err_retval = wait_simple_transfer_from_device(engine, 0);
    check_err(err_retval);

    /*
     * check results
     */
    for(i = 0; i < BUFSIZE / sizeof(int); i++) {
        int oracle =  in1[i] * A + in2[i] * B + C;
        if (out[i] != oracle) {
            err = 1;
            printf("ERROR in position %u: %i instead of %i\n", i, out[i], oracle);
        }
    }

    if (!err) {
        printf("no errors found\n");
    }

    destroy_control_interface(&vec_sum);

    destroy_dma_interfaces(2, engine);

	unload_udma_buffers( NUM_BUFFERS, buffers);

	return err;
}
