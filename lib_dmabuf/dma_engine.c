
/**
 * @file dma_engine.c
 * @author Alberto Scolari
 * @brief Implementation of utilities to work with DMa interfaces and AXI control interfaces
 * for custom kernels.
 */

#define _POSIX_C_SOURCE 199309L
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "dma_engine_buf.h"
#include "xhw_internals.h"

static const char sg_err_msg[] = "ERROR: DMA engine is set in Scatter/Gather mode; can handle only Direct Register Mode";

static void xdma_engine_init(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs = (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;

    /* reset everything, no interrupt mode */
    engine->to_dev.status = NOT_STARTED;
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

    engine->from_dev.status = NOT_STARTED;
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

#define LINUX_MEM_DEV "/dev/mem"

int get_dma_interfaces(unsigned num_dma, phys_addr_t *offsets,
    unsigned *lengths, struct dma_engine *engines)
{
	char *result;
	int fd;
    unsigned i;

    if (offsets == NULL)
    {
        printf("function %s requires offsets to be given\n", __func__);
        exit(-1);
    }

	fd = open(LINUX_MEM_DEV, O_RDWR | O_SYNC);
	if (fd == -1)
	{
		printf("%s: impossible to open %s\n", __func__, LINUX_MEM_DEV);
		return -1;
	}
    
    for(i = 0; i < num_dma; i++) {
        unsigned __offset, __length;
        if (offsets == NULL)
        {
            __offset = AXI_DMA_REGISTER_LOCATION ;
        } else
        {
            __offset = offsets[i];
        }
        if ( lengths == NULL )
        {
            __length = DESCRIPTOR_REGISTERS_SIZE;
        } else
        {
            __length = lengths[i];
        }
        engines[i].fd = fd;
        engines[i].regs_vaddr = result = mmap(NULL, __length,
    	    PROT_READ | PROT_WRITE, MAP_SHARED, fd, __offset);
        engines[i].length = __length;
        if ( result == NULL )
        {
            unsigned j;
    	    printf("%s: impossible to mmap %s\n", __func__, LINUX_MEM_DEV);
            for( j = 0; j < i; j++) {
                munmap((void*)engines[j].regs_vaddr, DESCRIPTOR_REGISTERS_SIZE);
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
    munmap((void*)engine->regs_vaddr, engine->length);
    close(engine->fd);
}

void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines)
{
    unsigned i;
    for( i = 0; i < num_dma; i++) {
        destroy_dma_interface(engines + i);
    }
}

#ifndef __unused__
#if defined(__GNUC__)
#define __unused__ __attribute__((unused))
#else
#define __unused__
#endif
#endif

static void check_transfer_alignment( __unused__ phys_addr_t addr)
{
#ifdef CHECK_ALIGN
    if (addr % DEF_ALIGN != 0) {
        printf("physical address %lx is not aligned to %lx\n", (unsigned long)addr,
            (unsigned long)DEF_ALIGN);
#endif
}

static enum dma_err_status set_simple_transfer_common(volatile uint32_t *reg_addr,
    struct dma_transaction *trans, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    uint32_t low;
#ifdef __64BIT__
    uint32_t high = (uint32_t)( (buf->paddr + offset) >> 32 );
#endif

    if ( trans->status == STARTED )
    {
        return DMA_TRANS_RUNNING;
    }
    low = (uint32_t)(buf->paddr + offset);
    *(reg_addr + 6) = trans->addr_low = low;
#ifdef __64BIT__
    *(reg_addr + 7) = trans->addr_high = high;
#endif
    __mem_full_barrier();

    trans->length = length;
    trans->status = PROGRAMMED;
    return NO_ERROR;
}

enum dma_err_status set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;
    check_transfer_alignment(buf->paddr + offset);
    return set_simple_transfer_common(&regs->mm2s_control, &engine->to_dev, buf, offset, length);
}

enum dma_err_status set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length)
{
    
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;
    check_transfer_alignment(buf->paddr + offset);
    return set_simple_transfer_common(&regs->s2mm_control, &engine->from_dev, buf, offset, length);
}

static enum dma_err_status start_simple_transfer_common(volatile uint32_t *regs, struct dma_transaction *trans)
{
    if (trans->status == NOT_STARTED)
    {
        return DMA_TRANS_NOT_PROGRAMMED;
    }
    if (trans->status == STARTED)
    {
        return DMA_TRANS_RUNNING;
    }
    SET_BIT(*regs, 0);
    __mem_full_barrier();

    SET_BITFIELD(*(regs + 10), 0, 25, (uint32_t)trans->length);
    __mem_full_barrier();
    trans->status = STARTED;
    return NO_ERROR;
}

enum dma_err_status start_simple_transfer_to_device(struct dma_engine *engine)
{
    return start_simple_transfer_common((volatile uint32_t *)engine->regs_vaddr, &engine->to_dev);
}

enum dma_err_status start_simple_transfer_from_device(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;
    return start_simple_transfer_common(&regs->s2mm_control, &engine->from_dev);
}

static inline int engine_is_idle(volatile uint32_t *regs)
{
    return BIT(*regs, 0) == 1;
}

static inline int engine_is_halted(volatile uint32_t *regs)
{
    return BIT(*regs, 1) == 1;
}

static void usleep_nano(unsigned utime)
{
	struct timespec __time;
	__time.tv_sec = utime % 1000000;
	__time.tv_nsec = utime * 1000;
	nanosleep(&__time, NULL);
}

static enum dma_err_status wait_simple_transfer_common(volatile uint32_t *regs,
    struct dma_transaction *trans, unsigned usleep_timeout)
{
    if (trans->status != STARTED)
    {
        return DMA_TRANS_NOT_STARTED;
    }
    while( !engine_is_idle(regs)
        /* || !engine_is_halted(regs) */ ) {
        if (usleep_timeout != 0) {
            usleep_nano(usleep_timeout);
        }
    }
    trans->status = PROGRAMMED;
    return NO_ERROR;
}

enum dma_err_status wait_simple_transfer_to_device(struct dma_engine *engine, unsigned usleep_timeout)
{
    return wait_simple_transfer_common((volatile uint32_t *)engine->regs_vaddr,
        &engine->to_dev, usleep_timeout);
}

enum dma_err_status wait_simple_transfer_from_device(struct dma_engine *engine, unsigned usleep_timeout)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;
    return wait_simple_transfer_common(&regs->s2mm_control,
        &engine->from_dev, usleep_timeout);
}

static unsigned err_status_common(volatile uint32_t *regs)
{
    uint32_t value = *regs;
    SET_BITFIELD(value, 0, 3, 0);
    SET_BITFIELD(value, 11, 31, 0);
    UNSET_BIT(value, 7);
    return value;
}

unsigned err_status_to_device(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;

    return err_status_common(&regs->mm2s_status);
}

unsigned err_status_from_device(struct dma_engine *engine)
{
    volatile struct axi_direct_dma_regs *regs =
        (volatile struct axi_direct_dma_regs *)engine->regs_vaddr;

    return err_status_common(&regs->s2mm_status);
}

static inline int kernel_is_idle(volatile struct axi_control_base_regs *regs)
{
    return BIT(regs->control, 2) == 1;
}

static inline int kernel_is_ready(volatile struct axi_control_base_regs *regs)
{
    //return BIT(regs->control, 3) == 1;
    return BIT(regs->control, 0) == 0;
}

static inline int kernel_is_done(volatile struct axi_control_base_regs *regs)
{
    return BIT(regs->control, 1) == 1;
}

static int axi_control_init(struct control_interface *intf)
{
    volatile struct axi_control_base_regs *regs = 
        ( volatile struct axi_control_base_regs *)intf->control_regs_vaddr;
    /*
     * turn interrupts off
     */
    UNSET_BIT(regs->global_int, 0);
    UNSET_BIT(regs->ip_int, 0);
    UNSET_BIT(regs->ip_int, 1);
    UNSET_BIT(regs->global_int, 0);
    if (BIT(regs->ip_int_status, 0) )
    {
        SET_BIT(regs->ip_int_status, 0);
    }
    if (BIT(regs->ip_int_status, 1) )
    {
        SET_BIT(regs->ip_int_status, 1);
    }
    if ( !kernel_is_ready(regs) )
    {
        printf("%s: kernel is not ready\n", __func__);
        return -1;
    }
    return 0;
}

int get_control_interface(phys_addr_t phys_addr, unsigned length,
    struct control_interface *ctrl_intf)
{
    int fd;
    phys_addr_t __phys_addr = phys_addr;
    unsigned __length = length;
    char *result;

	fd = open(LINUX_MEM_DEV, O_RDWR | O_SYNC);
	if (fd == -1)
	{
		printf("%s: impossible to open %s\n", __func__, LINUX_MEM_DEV);
		return -1;
	}
    ctrl_intf->fd = fd;

    if (phys_addr == 0)
    {
        __phys_addr = AXI_CONTROL_REGS_BASE_DEF;
    }
    if (length == 0)
    {
        __length = AXI_CONTROL_REGS_LEN_DEF;
    }
    
    result = mmap(NULL, __length,
    	PROT_READ | PROT_WRITE, MAP_SHARED, fd, __phys_addr);
    if ( result == NULL )
    {
        printf("%s: impossible to mmap %s\n", __func__, LINUX_MEM_DEV);
        close(fd);
        return -1;
    }
    ctrl_intf->length = __length;
    ctrl_intf->control_regs_vaddr = result;
    ctrl_intf->user_args = (volatile char *)( result + AXI_CONTROL_USER_DATA_OFFS);
    return axi_control_init(ctrl_intf);
}

void destroy_control_interface(struct control_interface *ctrl_intf)
{
    munmap((void*)ctrl_intf->control_regs_vaddr, ctrl_intf->length);
    close(ctrl_intf->fd);
}

void start_kernel(struct control_interface *ctrl_intf)
{
    volatile struct axi_control_base_regs *regs = 
        ( volatile struct axi_control_base_regs *)ctrl_intf->control_regs_vaddr;
    __mem_full_barrier();
    SET_BIT(regs->control, 0);
    __mem_full_barrier();
}

void wait_kernel(struct control_interface *ctrl_intf, unsigned usleep_timeout)
{
    volatile struct axi_control_base_regs *regs = 
        ( volatile struct axi_control_base_regs *)ctrl_intf->control_regs_vaddr;
    while( !kernel_is_ready(regs) )
    {
        if (usleep_timeout != 0)
        {
            usleep_nano(usleep_timeout);
        }
    }
}

