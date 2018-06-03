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

int get_dma_interfaces(unsigned num_dma, struct dma_engine *engines, unsigned *offsets);

void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines);

#ifdef __cplusplus
}
#endif

#endif /* DMABUF_H_ */
