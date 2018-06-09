#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "dmabuf.h"
#include "xdma_internals.h"

#define AXI_DMA_REGISTER_LOCATION 0x40400000
#define DESCRIPTOR_REGISTERS_SIZE 0x10000

static const char sg_err_msg[] = "ERROR: DMA engine is set in Scatter/Gather mode; can handle only Direct Register Mode";

void xdma_engine_init(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs = (volatile struct axi_direct_dma_regs *)engine->vaddr;

    /* reset everything, no interrupt mode */
    regs->mm2s_control = 4;
    while(regs->mm2s_control & 4);

    if ( BIT(regs->mm2s_status, 3) )
    {
        printf("%s\n", sg_err_msg);
        exit(-1);
    }
    SET_BITFIELD(regs->mm2s_status, 12, 14, 0);
#ifndef __64BITS__
    regs->mm2s_source_addr_high = 0;
#endif

    regs->s2mm_control = 4;
    while(regs->s2mm_control & 4);

    if ( BIT(regs->s2mm_status, 3) )
    {
        printf("%s\n", sg_err_msg);
        exit(-1);
    }
    SET_BITFIELD(regs->s2mm_status, 12, 14, 0);
#ifndef __64BITS__
    regs->s2mm_dest_addr_high = 0;
#endif
}

int get_dma_interfaces(unsigned num_dma, unsigned *offsets, struct dma_engine *engines)
{
	char *result;
	int fd;
    unsigned i;

	fd = open( "/dev/mem", O_RDWR | O_SYNC | O_SYNC);
	if (fd == -1)
	{
		printf("impossible to open /dev/mem\n");
		return -1;
	}
    
    for(i = 0; i < num_dma; i++) {
        unsigned offs;
        if (i == 0 && offsets == NULL)
        {
            offs = 0;
        } else
        {
            offs = offsets[i];
        }
        engines[i].fd = fd;
    	engines[i].vaddr = result = mmap(NULL, DESCRIPTOR_REGISTERS_SIZE,
    		PROT_READ | PROT_WRITE, MAP_SHARED, fd, AXI_DMA_REGISTER_LOCATION + offs);
    	if ( result == NULL )
    	{
            unsigned j;
    		printf("impossible to mmap /dev/mem\n");
            for( j = 0; j < i; j++) {
                munmap(engines[j].vaddr, DESCRIPTOR_REGISTERS_SIZE);
            }
    		close(fd);
    		return -1;
    	}
        xdma_engine_init(engines + i);
    }
	return 0;
}

static void destroy_dma_interface(struct dma_engine *engine)
{
    munmap(engine->vaddr, DESCRIPTOR_REGISTERS_SIZE);
    close(engine->fd);
}

void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines)
{
    unsigned i;
    for( i = 0; i < num_dma; i++) {
        destroy_dma_interface(engines + i);
    }
}

static void check_transfer_alignment(phys_addr_t addr)
{
#ifdef CHECK_ALIGN
    if (addr % DEF_ALIGN != 0) {
        printf("physical address %lx is not aligned to %lx\n", (unsigned long)addr,
            (unsigned long)DEF_ALIGN);
#endif
}

static void set_simple_transfer(volatile uint32_t *reg_addr, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    *(reg_addr + 6) = (uint32_t)buf->paddr + offset;
#ifdef __64BIT__
    *(reg_addr + 7) = (uint32_t)( (buf->paddr + offset) >> 32);
#endif

    SET_BIT(*reg_addr, 0);

    SET_BITFIELD(*(reg_addr + 10), 0, 25, (uint32_t)length);
}

void set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    check_transfer_alignment(buf->paddr + offset);
    set_simple_transfer(&regs->mm2s_control, buf, offset, length);
}

void set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    check_transfer_alignment(buf->paddr + offset);
    set_simple_transfer(&regs->s2mm_control, buf, offset, length);
}

void start_simple_transfer_to_device(struct dma_engine *engine)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    SET_BIT(regs->mm2s_control, 0);
}

void wait_simple_transfer_to_device(struct dma_engine *engine, unsigned usleep_timeout)
{
    while( !engine_to_device_is_idle(engine)
        /* || !engine_to_device_is_halted(engine) */ ) {
        if (usleep_timeout != 0) {
            usleep((useconds_t)usleep_timeout);
        }
    }
}

void start_simple_transfer_from_device(struct dma_engine *engine)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    SET_BIT(regs->s2mm_control, 0);
}

void wait_simple_transfer_from_device(struct dma_engine *engine, unsigned usleep_timeout)
{
    while( !engine_from_device_is_idle(engine)
        /* || !engine_from_device_is_halted(engine) */ ) {
        if (usleep_timeout != 0) {
            usleep((useconds_t)usleep_timeout);
        }
    }
}

unsigned err_status_to_device(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;

    uint32_t value = regs->mm2s_status;
    SET_BITFIELD(value, 0, 3, 0);
    SET_BITFIELD(value, 11, 31, 0);
    UNSET_BIT(value, 7);
    return value;
}

unsigned err_status_from_device(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;

    uint32_t value = regs->s2mm_status;
    SET_BITFIELD(value, 0, 3, 0);
    SET_BITFIELD(value, 11, 31, 0);
    UNSET_BIT(value, 7);
    return value;
}

void print_err_mask(unsigned mask)
{
    if ( mask & (1U << 4) ) {
        printf("DMAIntErr\n");
    }
    if ( mask & (1U << 5) ) {
        printf("DMASlvErr\n");
    }
    if ( mask & (1U << 6) ) {
        printf("DMADecErr\n");
    }
    if ( mask & (1U << 8) ) {
        printf("SGIntErr\n");
    }
    if ( mask & (1U << 9) ) {
        printf("SGSlvErr\n");
    }
    if ( mask & (1U << 10) ) {
        printf("SGDecErr\n");
    }
}

void print_engine( struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;

    printf(
        "regs dump:\n"
        "mm2s_control %x\n"
        "mm2s_status %x\n"
        "mm2s_source_addr_low %x\n"
        "mm2s_source_addr_high %x\n"
        "mm2s_length %x\n"

        "s2mm_control %x\n"
        "s2mm_status %x\n"
        "s2mm_dest_addr_low %x\n"
        "s2mm_dest_addr_high %x\n"
        "s2mm_length %x\n",
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
