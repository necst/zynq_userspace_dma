
/**
 * @file utils.c
 * @author Alberto Scolari
 * @brief Implementation of debugging utilities.
 */

#include <stdio.h>
#include <stdlib.h>

#include "xhw_internals.h"

int flash_bitstream(const char *path)
{
    int retval;
    char *command = malloc( 2048 * sizeof(char) );
    sprintf(command, "cat %s > /dev/xdevcfg\n", path);
    retval = system(command);

    free(command);
    return retval;
}

void check_err(enum dma_err_status err)
{
    if ( err != NO_ERROR )
    {
        printf("******************************\n"
               "ERROR: error code is %d\n"
               "******************************\n", (int)err);
    }
}

void print_engine( struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;

    printf(
        "regs dump:\n"
        "\tmm2s_control %x\n"
        "\tmm2s_status %x\n"
        "\tmm2s_source_addr_low %x\n"
        "\tmm2s_source_addr_high %x\n"
        "\tmm2s_length %x\n"

        "\ts2mm_control %x\n"
        "\ts2mm_status %x\n"
        "\ts2mm_dest_addr_low %x\n"
        "\ts2mm_dest_addr_high %x\n"
        "\ts2mm_length %x\n",
        regs->mm2s_control,
        regs->mm2s_status,
        regs->mm2s_source_addr_low,
        regs->mm2s_source_addr_high,
        regs->mm2s_length,

        regs->s2mm_control,
        regs->s2mm_status,
        regs->s2mm_dest_addr_low,
        regs->s2mm_dest_addr_high,
        regs->s2mm_length);
}

void print_err_mask(unsigned mask)
{
    if ( BIT(mask, 4) ) {
        printf("DMAIntErr\n");
    }
    if ( BIT(mask, 5) ) {
        printf("DMASlvErr\n");
    }
    if ( BIT(mask, 6) ) {
        printf("DMADecErr\n");
    }
    if ( BIT(mask, 8) ) {
        printf("SGIntErr\n");
    }
    if ( BIT(mask, 9) ) {
        printf("SGSlvErr\n");
    }
    if ( BIT(mask, 10) ) {
        printf("SGDecErr\n");
    }
}

void print_buffer_status(int buf_id, struct udmabuf *buf)
{
    printf("ubuffer %d:\n\tphys addr %lx\n\tvirt mapping %p\n\tlength %lu\n",
        buf_id, (unsigned long)buf->paddr, buf->vaddr, buf->size);
}

void print_kernel_status(struct control_interface *ctrl_intf)
{
    volatile struct axi_control_base_regs *regs = 
        ( volatile struct axi_control_base_regs *)ctrl_intf->control_regs_vaddr;
    printf("\nKernel status:\n\tcontrol %x\n\tglobal interrupt %x\n\t"
        "IP interrupt %x\n\tIP interrupt status %x\n",
        regs->control, regs->global_int, regs->ip_int, regs->ip_int_status);
}
