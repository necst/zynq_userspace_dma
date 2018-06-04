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
    regs->mm2s_control = 0;
    if ( BIT(regs->mm2s_status, 3) )
    {
        printf("%s\n", sg_err_msg);
        exit(-1);
    }
    SET_BITFIELD(regs->mm2s_status, 12, 14, 0);
#ifndef __64BITS__
    regs->mm2s_source_addr_high = 0;
#endif

    regs->s2mm_control = 0;
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

int get_dma_interfaces(unsigned num_dma, struct dma_engine *engines, unsigned *offsets)
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

void set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length, unsigned channel)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    check_transfer_alignment(buf->paddr + offset);

    regs->mm2s_source_addr_low = (uint32_t)buf->paddr + offset;
#ifdef __64BIT__
    regs->mm2s_source_addr_high = (uint32_t)( (buf->paddr + offset) >> 32);
#endif

    SET_BITFIELD(regs->mm2s_length, 0, 25, (uint32_t)length);
}

void set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length, unsigned channel)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->vaddr;
    check_transfer_alignment(buf->paddr + offset);

regs->s2mm_dest_addr_low = (uint32_t)buf->paddr + offset;
#ifdef __64BIT__
    regs->s2mm_dest_addr_high = (uint32_t)( (buf->paddr + offset) >> 32);
#endif

    SET_BITFIELD(regs->s2mm_length, 0, 25, (uint32_t)length);
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

