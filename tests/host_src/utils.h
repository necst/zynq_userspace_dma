
/**
 * @file utils.h
 * @author Alberto Scolari
 * @brief Header with various utilities to debug UDMA buffers, DMA interfaces
 * and AXI control logic.
 */

#ifndef DMA_UTILS_H_
#define DMA_UTILS_H_

#include "dma_engine_buf.h"

/*
 * shutd up GCC from complaining about unused
 * function arguments (like argc and argv)
 */

#ifndef __unused__
#if defined(__GNUC__)
#define __unused__ __attribute__((unused))
#else
#define __unused__
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


int flash_bitstream(const char *path);

void check_err(enum dma_err_status err);

void print_engine( struct dma_engine *engine);

void print_err_mask(unsigned mask);

void print_buffer_status(int buf_id, struct udmabuf *buf);

void print_kernel_status(struct control_interface *ctrl_intf);

#ifdef __cplusplus
}
#endif

#endif /* DMA_UTILS_H_ */

