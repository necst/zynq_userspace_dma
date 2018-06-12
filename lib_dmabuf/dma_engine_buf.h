
#ifndef DMA_ENGINE_BUF_H_
#define DMA_ENGINE_BUF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined(__aarch64__) || defined(__amd64__)
    #define __64BITS__
#endif

#ifdef __64BITS__
	typedef unsigned long phys_addr_t;
#else
	typedef unsigned int phys_addr_t;
#endif

/*
 * ========== USERSPACE DMA BUFFER INTERFACES ==========
 */

struct udmabuf {
	int fd;
	unsigned long size;
	void *vaddr;
	phys_addr_t paddr;
};

int load_udma_buffers(unsigned int num, const unsigned long *sizes, struct udmabuf *buffers);

void unload_udma_buffers(unsigned int num, struct udmabuf *buffers);

/*
 * ========== AXI DMA INTERFACES ==========
 */

enum dma_trans_status { NOT_STARTED, PROGRAMMED, STARTED };

struct dma_transaction {
    uint32_t addr_low;
#ifdef __64BITS__
    uint32_t addr_high;
#endif
    uint32_t length;
    enum dma_trans_status status;
};

struct dma_engine {
    int fd;
    unsigned length;
    volatile char *regs_vaddr;
    struct dma_transaction to_dev, from_dev;
};

enum dma_err_status { NO_ERROR = 0, DMA_TRANS_RUNNING, DMA_TRANS_NOT_PROGRAMMED, DMA_TRANS_NOT_STARTED };

int get_dma_interfaces(unsigned num_dma, phys_addr_t *offsets,
    unsigned *lengths, struct dma_engine *engines);

void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines);

enum dma_err_status set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

enum dma_err_status set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

enum dma_err_status start_simple_transfer_to_device(struct dma_engine *engine);

enum dma_err_status start_simple_transfer_from_device(struct dma_engine *engine);

enum dma_err_status wait_simple_transfer_to_device(struct dma_engine *engine, unsigned usleep_timeout);

enum dma_err_status wait_simple_transfer_from_device(struct dma_engine *engine, unsigned usleep_timeout);

/*
 * ========== AXI CONTROL INTERFACES ==========
 */

struct control_interface {
    int fd;
    unsigned length;
    volatile char *control_regs_vaddr;
    volatile char *user_args;
};

int get_control_interface(phys_addr_t phys_addr, unsigned length, struct control_interface *ctrl_intf);

void destroy_control_interface(struct control_interface *ctrl_intf);

#define SET_KERNEL_ARG_UNSAFE(intf, offset, value) do {         \
    volatile typeof(value) *__ptr = (volatile typeof(value) *)  \
        ((intf)->user_args + sizeof(uint64_t) * (offset));      \
    *__ptr = value;                                             \
    } while(0)

#define GET_KERNEL_ARG_UNSAFE(intf, offset, type) *((volatile type *)  \
        ((intf)->user_args + sizeof(uint64_t) * (offset)))

static inline void set_kernel_argument_uint(struct control_interface *ctrl_intf, unsigned offset,
    uint32_t value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

static inline uint32_t get_kernel_argument_uint(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, uint32_t);
}

static inline void set_kernel_argument_ulong(struct control_interface *ctrl_intf, unsigned offset,
    uint64_t value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

static inline uint64_t get_kernel_argument_ulong(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, uint64_t);
}

static inline void set_kernel_argument_char(struct control_interface *ctrl_intf, unsigned offset,
    char value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

static inline char get_kernel_argument_char(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, char);
}

void start_kernel(struct control_interface *ctrl_intf);

void wait_kernel(struct control_interface *ctrl_intf, unsigned usleep_timeout);

#ifdef __cplusplus
}
#endif

#endif /* DMA_ENGINE_BUF_H_ */
