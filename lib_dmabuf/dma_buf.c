
/**
 * @file dma_buf.c
 * @author Alberto Scolari
 * @brief Implementation of utilities to retrieve and destroy UDMA buffers.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dma_engine_buf.h"

static char param_string[ 145 ];

#define MODPATH "../udmabuf/udmabuf.ko"

static char command_str[ 170 ] = "insmod " MODPATH;

static char rmmod_cmd[] = "sudo rmmod udmabuf";

static int run_command(const char *cmd)
{
    return system(cmd);
}

static void insert_module(unsigned int num, const unsigned long *sizes)
{
    unsigned int i;
    char *buf = param_string;
    int retval = run_command("lsmod | grep udmabuf");
    if (retval == 0)
    {
        retval = run_command(rmmod_cmd);
        if (retval != 0)
        {
            printf("cannot unload the module\n");
            exit(-1);
        }
    }

    for(i = 0; i < num; i++)
    {
        int written = sprintf(buf, " udmabuf%u=%lu", i, sizes[i]);
        buf += written;
    }

    strcat(command_str, param_string);
    printf("running: %s\n", command_str);
    retval = run_command(command_str);
    if (retval != 0)
    {
        printf("cannot insert module\n");
        exit(-1);
    }
}

#define BUFPATH "/dev/udmabuf"
#define PHYSPATH "/sys/class/udmabuf/udmabuf"
#define PHYSNAME "/phys_addr"
#define SYNCMODE "/sync_mode"
#define SYNCDIR "/sync_direction"

static void read_buf_data(unsigned int num, unsigned long size, struct udmabuf *buffer)
{
    int fd;
    unsigned long parsed_size;
    FILE *file;
    char bufname[60];

    /*
     * set sync_mode to 1: If O_SYNC is specified, CPU cache is disabled.
     * If O_SYNC is not specified, CPU cache is enabled.
     */
    sprintf(bufname, "%s%u%s", PHYSPATH, num, SYNCMODE);
    file = fopen(bufname, "w");
    if (file == NULL)
    {
        printf("cannot open file %s\n", bufname);
        exit(-1);
    }
    fprintf(file, "1");
    fclose(file);

    /*
     * set sync_direction to 0: DMA_BIDIRECTIONAL
     */
    sprintf(bufname, "%s%u%s", PHYSPATH, num, SYNCDIR);
    file = fopen(bufname, "w");
    if (file == NULL)
    {
        printf("cannot open file %s\n", bufname);
        exit(-1);
    }
    fprintf(file, "0");
    fclose(file);

    /*
     * mmap buffer
     */
    sprintf(bufname,"%s%u", BUFPATH, num);
    buffer->fd = fd = open(bufname, O_RDWR | O_SYNC);
    if (fd == -1)
    {
        printf("cannot open file %s\n", bufname);
        exit(-1);
    }
    buffer->vaddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    buffer->size = size;

    /*
     * read physical address
     */
    sprintf(bufname, "%s%u%s", PHYSPATH, num, PHYSNAME);
    file = fopen(bufname, "r");
    fscanf(file, "%lx", &parsed_size);
    fclose(file);
    buffer->paddr = (phys_addr_t)parsed_size;
}

int load_udma_buffers(unsigned int num, const unsigned long *sizes, struct udmabuf *buffers)
{
    unsigned int i;
    if (num == 0)
    {
            return 0;
    }
    insert_module(num, sizes);
    for( i = 0; i < num; i++)
    {
        read_buf_data(i, sizes[i], buffers + i);
    }
    return num;
}

void unload_udma_buffers(unsigned int num, struct udmabuf *buffers)
{
    unsigned int i;
    for(i = 0; i < num; i++)
    {
        if (buffers[i].fd < 0) continue;
        munmap(buffers[i].vaddr, buffers[i].size);
        close(buffers[i].fd);
    }
    run_command(rmmod_cmd);
}

