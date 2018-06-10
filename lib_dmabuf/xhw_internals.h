
#ifndef XHW_INTERNALS_H_
#define XHW_INTERNALS_H_

/*
 * from https://www.xilinx.com/support/documentation/ip_documentation/axi_dma/v7_1/pg021_axi_dma.pdf
 * pag 12
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "dma_engine_buf.h"

#define DEF_ALIGN 64
/* #define CHECK_ALIGN */

#define BIT(v, offs) ( ((unsigned)((v) >> (offs))) & 1U)

#define SET_BIT(v, offs) ( (v) |= (1U << offs) )

#define UNSET_BIT(v, offs) ( (v) &= (~(1U << offs)) )

#define BITFIELD(v, start, last) ( ((unsigned)((v) >> (start))) & ((1U << (last - start + 1)) - 1) )

#define SET_BITFIELD(v, start, last, new_val) do {                     \
    unsigned __base_mask = ((1U << (last - start + 1)) - 1) << start;  \
    (v) &= ~__base_mask;                                               \
    (v) |= ( (new_val) << start) & __base_mask;                        \
    } while (0)

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

#ifdef __cplusplus
}
#endif

#endif /* XHW_INTERNALS_H_ */

