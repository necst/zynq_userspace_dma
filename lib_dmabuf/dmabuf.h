#ifndef DMABUF_H_
#define DMABUF_H_

#ifdef __cplusplus
extern "C" {
#endif

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

struct dma_engine {
    int fd;
    char *vaddr;
};

int get_dma_interfaces(unsigned num_dma, unsigned *offsets, struct dma_engine *engines);

void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines);

void set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

void set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

void start_simple_transfer_to_device(struct dma_engine *engine);

void wait_simple_transfer_to_device(struct dma_engine *engine, unsigned usleep_timeout);

void start_simple_transfer_from_device(struct dma_engine *engine);

void wait_simple_transfer_from_device(struct dma_engine *engine, unsigned usleep_timeout);

void print_engine( struct dma_engine *engine);

#ifdef __cplusplus
}
#endif

#endif /* DMABUF_H_ */
