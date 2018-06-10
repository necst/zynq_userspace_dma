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

struct udmabuf {
	int fd;
	unsigned long size;
	void *vaddr;
	phys_addr_t paddr;
};

int load_udma_buffers(unsigned int num, const unsigned long *sizes, struct udmabuf *buffers);

void unload_udma_buffers(unsigned int num, struct udmabuf *buffers);

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
    volatile char *regs_vaddr;
    struct dma_transaction to_dev, from_dev;
};

enum dma_err_status { NO_ERROR = 0, DMA_TRANS_RUNNING, DMA_TRANS_NOT_PROGRAMMED, DMA_TRANS_NOT_STARTED };

#define AXI_DMA_REGISTER_LOCATION 0x40400000
#define DESCRIPTOR_REGISTERS_SIZE 0x10000

int get_dma_interfaces(unsigned num_dma, unsigned *offsets,
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

#ifdef __cplusplus
}
#endif

#endif /* DMA_ENGINE_BUF_H_ */
