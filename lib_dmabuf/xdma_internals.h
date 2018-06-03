
#ifndef XDMA_INTERNALS_H_
#define XDMA_INTERNALS_H_

#include <stdint.h>

struct axi_direct_dma_regs {
    uint32_t mm2s_control;
    uint32_t mm2s_status;
    uint32_t mm2s_reserved1[4];
    uint32_t mm2s_source_addr_low;
    uint32_t mm2s_source_addr_high;
    uint32_t mm2s_reserved2[2];
    uint32_t mm2s_length;

    uint32_t reserved3[1];

    uint32_t s2mm_control;
    uint32_t s2mm_status;
    uint32_t s2mm_reserved4[4];
    uint32_t s2mm_dest_addr_low;
    uint32_t s2mm_dest_addr_high;
    uint32_t s2mm_reserved[2];
    uint32_t s2mm_length;
} __attribute__((packed)) ;

#endif /* XDMA_INTERNALS_H_ */
