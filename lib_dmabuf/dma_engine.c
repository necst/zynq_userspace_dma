#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include "dmabuf.h"

#define AXI_DMA_REGISTER_LOCATION 0x40400000
#define DESCRIPTOR_REGISTERS_SIZE 0xFFFF

int get_dma_interface(struct dma_engine *engine)
{
	char *result;
	int fd;

	engine->fd = fd = open( "/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
	{
		printf("impossible to open /dev/mem\n");
		return -1;
	}
	engine->vaddr = result = mmap(NULL, DESCRIPTOR_REGISTERS_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, AXI_DMA_REGISTER_LOCATION);
	if ( result == NULL )
	{
		printf("impossible to mmap /dev/mem\n");
		close(fd);
		return -1;
	}
	return 0;
}
