#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include "dmabuf.h"
#include "xdma_internals.h"

#define AXI_DMA_REGISTER_LOCATION 0x40400000
/* #define DESCRIPTOR_REGISTERS_SIZE 0xFFFF */
#define DESCRIPTOR_REGISTERS_SIZE 92

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


int transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, unsigned length, unsigned channel)
{
   return 0;
}
