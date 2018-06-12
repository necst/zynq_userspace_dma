#ifndef DMA_UTILS_H_
#define DMA_UTILS_H_

#include "dma_engine_buf.h"

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

