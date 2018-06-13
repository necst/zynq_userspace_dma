
#ifndef DMA_ENGINE_BUF_H_
#define DMA_ENGINE_BUF_H_

/**
 * @file dma_engine_buf.h
 * @author Alberto Scolari
 * @brief Header with API to get and control UDMA buffers, AXI DMA interfaces and AXI control interfaces.
 *
 * All the basic utilities for managing DMA transactions and running computation on custom FPGA kernels
 * are provided in this header.
 */

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

/*
 * ========== USERSPACE DMA BUFFER INTERFACES ==========
 */

/**
 * @brief The udmabuf struct stores information about the UDMA buffer
 * to be used from userspace
 *
 * UDMA buffers are created by kernel (via the udmabuf module: https://github.com/ikwzm/udmabuf)
 * and made available to applications via files /dev/udmabuf<number>, and additional information
 * about them is available in /sys/class/udmabuf/<number>/. This library reads from these files
 * and retrieves the needed information for each buffer, opens the buffer file and mmap()s each
 * buffer into the proccess virtual memory, also recording its physical address.
 */
struct udmabuf {
        int fd; /**< file descriptor of /dev/ file used to map the buffer from */
        unsigned long size; /**< size of mapped buffer */
        void *vaddr; /**< pointer to access the buffer, in process virtual memory space*/
        phys_addr_t paddr; /**< physical address of the buffer, for DMA transaction initiation */
};

/**
 * @brief load_udma_buffers loads the UDMA buffers according to user parameters and populates
 * the @p buffers array (the user should have allocated it of at least num elements)
 *
 * Loads UDMA buffers: load the udmabuf module with appropriate parameters,
 * read buffers' information, open each buffer file and map() it.
 *
 * @param num number of buffers to create
 * @param sizes size of each buffer
 * @param buffers user-allocated buffer to be filled with information of UDMA buffers
 * @return 0 for success, non-0 for error
 */
int load_udma_buffers(unsigned int num, const unsigned long *sizes, struct udmabuf *buffers);

/**
 * @brief unload_udma_buffers destroys the UDMA buffers and releases the udmabuf module
 *
 * @param num number of buffers to unload
 * @param buffers array with UDMA buffers information
 */
void unload_udma_buffers(unsigned int num, struct udmabuf *buffers);

/*
 * ========== AXI DMA INTERFACES ==========
 */

/**
 * @brief status of DMA transactions
 */
enum dma_trans_status { NOT_STARTED, /**< transaction has not started yet, and was not set */
                        PROGRAMMED, /**< transaction hasbeen programmed, but is not started yet */
                        STARTED }; /**< transaction is currently running */

/**
 * @brief The dma_transaction struct encodes the information of a transaction,
 * either to be run or currently running.
 *
 * This struct stores the information to program and run a DMA transaction.
 */
struct dma_transaction {
    uint32_t addr_low; /**< low 32 bits of source/destination address */
#ifdef __64BITS__
    uint32_t addr_high; /**< high 32 bits of source/destination address */
#endif
    uint32_t length; /**< number of bytes to be transmitted */
    enum dma_trans_status status; /**< current status of the transaction */
};

/**
 * @brief The dma_engine struct stores the information about the entire DMA engine.
 *
 * The DMA engine is mapped from /dev/mem according to the addresses in Vivado Address Editor.
 */
struct dma_engine {
    int fd; /**< file descriptor of /dev/mem */
    unsigned length; /**< length of mmaped() area */
    volatile char *regs_vaddr; /**< pointer to DMA register area */
    struct dma_transaction to_dev; /**< information about transaction towards FPGA logic */
    struct dma_transaction from_dev; /**< information about transaction from FPGA logic */
};

/**
 * @brief error status of DMA-related calls
 */
enum dma_err_status { NO_ERROR = 0, /**< no error occurred */
                      DMA_TRANS_RUNNING, /**< DMa transaction is running */
                      DMA_TRANS_NOT_PROGRAMMED, /**< DMA transaction has not been programmed */
                      DMA_TRANS_NOT_STARTED /**< DMA transaction has not been started */
                    };

/**
 * @brief get_dma_interfaces loads the DMA interfaces from physical memory,
 * making them available to the process
 *
 * @param num_dma number of DMA interfaces
 * @param offsets physical address of DMA interfaces, as from Vivado Address Editor;
 * if num_dma == 1, offsets can be NULL and the default location AXI_DMA_REGISTER_LOCATION
 * is used
 * @param lengths lengths of DMA register areas to be mapped; if NULL, the default length
 * DESCRIPTOR_REGISTERS_SIZE is used
 * @param engines the struct @ref dma_engine
 * @return 0 for success, non-0 otherwise
 */
int get_dma_interfaces(unsigned num_dma, phys_addr_t *offsets,
    unsigned *lengths, struct dma_engine *engines);

/**
 * @brief destroy_dma_interfaces destroys the DMA interfaces by unmmap()ing their memory
 * @param num_dma number of DMA interfaces
 * @param engines pointer to array of DMA engines to be destroyed
 */
void destroy_dma_interfaces(unsigned num_dma, struct dma_engine *engines);

/**
 * @brief set_simple_transfer_to_device programs a transaction on a DMA engine,
 * from the @p buf buffer to the FPGA logic
 * @param engine the DMA engine pointer
 * @param buf the UDMA buf to read data from
 * @param offset the offset within the UDMA buffer
 * @param length how many bytes to trasmit
 * @return an @ref dma_err_status value describing success or failure reason
 */
enum dma_err_status set_simple_transfer_to_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

/**
 * @brief set_simple_transfer_from_device programs a transaction on a DMA engine,
 * from the FPGa logic to the @p buf buffer
 * @param engine the DMA engine pointer
 * @param buf the UDMA buf to write data to
 * @param offset the offset within the UDMA buffer
 * @param length how many bytes to trasmit
 * @return an @ref dma_err_status value describing success or failure reason
 */
enum dma_err_status set_simple_transfer_from_device(struct dma_engine *engine, struct udmabuf *buf, 
    unsigned offset, unsigned length);

/**
 * @brief start_simple_transfer_to_device actually starts the DMA transaction to FPGA logic.
 *
 * It reads the information from the @ref to_dev field of @p engine to program the DMA registers.
 *
 * @param engine the DMA engine pointer
 * @return an @ref dma_err_status value describing success or failure reason
 */
enum dma_err_status start_simple_transfer_to_device(struct dma_engine *engine);

/**
 * @brief start_simple_transfer_from_device actually starts the DMA transaction from FPGA logic.
 *
 * It reads the information from the @ref to_dev field of @p engine to program the DMA registers.
 *
 * @param engine the DMA engine pointer
 * @return an @ref dma_err_status value describing success or failure reason
 */
enum dma_err_status start_simple_transfer_from_device(struct dma_engine *engine);

/**
 * @brief wait_simple_transfer_to_device waits for the completion of the DMA transaction to FPGA logic.
 * Users can optionally specify sleeping time via @p usleep_timeout
 *
 * @param engine the DMA engine pointer
 * @param usleep_timeout sleeping intervals to wait for the transaction end; 0 means busy wait
 * @return an @ref dma_err_status value saying whether the transaction has ended successfully,
 * or describing why it was not possible to wait
 */
enum dma_err_status wait_simple_transfer_to_device(struct dma_engine *engine, unsigned usleep_timeout);

/**
 * @brief wait_simple_transfer_to_device waits for the completion of the DMA transaction from FPGA logic.
 * Users can optionally specify sleeping time via @p usleep_timeout
 *
 * @param engine the DMA engine pointer
 * @param usleep_timeout sleeping intervals to wait for the transaction end; 0 means busy wait
 * @return an @ref dma_err_status value saying whether the transaction has ended successfully,
 * or describing why it was not possible to wait
 */
enum dma_err_status wait_simple_transfer_from_device(struct dma_engine *engine, unsigned usleep_timeout);

/**
 * @brief err_status_to_device retrieves the hardware-related error bitmask after a transaction to FPGA
 * is unseuccessful.
 *
 * @param engine the DMA engine pointer
 */
unsigned err_status_to_device(struct dma_engine *engine);

/**
 * @brief err_status_from_device retrieves the hardware-related error bitmask after a transaction from FPGA
 * is unseuccessful.
 *
 * @param engine the DMA engine pointer
 */
unsigned err_status_from_device(struct dma_engine *engine);

/*
 * ========== AXI CONTROL INTERFACES ==========
 */

/**
 * @brief The control_interface struct stores information about an AXI control interface,
 * allowing users to send parameters to a custom kernel.
 */
struct control_interface {
    int fd; /**< file descriptor of /dem/mem opened to mmap() control interface */
    unsigned length; /**< length of control interface */
    volatile char *control_regs_vaddr; /**< pointer to beginning of memory-mapped control registers */
    volatile char *user_args; /**< pointer to user-logic control registers, where kernel arguments go */
};

/**
 * @brief get_control_interface mmap()s the control interfaces to the process memory.
 *
 * @param phys_addr physical address of control interface registers, as from Vivado Address Editor;
 * if 0, the default value @ref AXI_CONTROL_REGS_BASE_DEF is used
 * @param length length of control interface; if 0, @ref AXI_CONTROL_REGS_LEN_DEF is used
 * @param ctrl_intf pointer of user-allocated @ref struct control_interface to be filled
 * @return 0 for success, non-0 otherwise
 */
int get_control_interface(phys_addr_t phys_addr, unsigned length, struct control_interface *ctrl_intf);

/**
 * @brief destroy_control_interface releases the control interface memory via unmap()
 * @param ctrl_intf control interface to release
 */
void destroy_control_interface(struct control_interface *ctrl_intf);

#define SET_KERNEL_ARG_UNSAFE(intf, offset, value) do {         \
    volatile typeof(value) *__ptr = (volatile typeof(value) *)  \
        ((intf)->user_args + sizeof(uint64_t) * (offset));      \
    *__ptr = value;                                             \
    } while(0)

#define GET_KERNEL_ARG_UNSAFE(intf, offset, type) *((volatile type *)  \
        ((intf)->user_args + sizeof(uint64_t) * (offset)))

/**
 * @brief set_kernel_argument_uint sets the 32 bits @p value argument at offset @p offset
 * in the @p ctrl_intf contorl interface, to pass @p value to the kernel
 */
static inline void set_kernel_argument_uint(struct control_interface *ctrl_intf, unsigned offset,
    uint32_t value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

static inline uint32_t get_kernel_argument_uint(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, uint32_t);
}

/**
 * @brief set_kernel_argument_uint sets the 64 bits @p value argument at offset @p offset
 * in the @p ctrl_intf contorl interface, to pass @p value to the kernel
 */
static inline void set_kernel_argument_ulong(struct control_interface *ctrl_intf, unsigned offset,
    uint64_t value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

/**
 * @brief get_kernel_argument_ulong reads the 64 bits kernel arguments from offset @p offset
 * of @p ctrl_intf control interface
 * @return  the value read from kernel's argument register
 */
static inline uint64_t get_kernel_argument_ulong(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, uint64_t);
}

/**
 * @brief set_kernel_argument_uint sets the 8 bits @p value argument at offset @p offset
 * in the @p ctrl_intf contorl interface, to pass @p value to the kernel
 */
static inline void set_kernel_argument_char(struct control_interface *ctrl_intf, unsigned offset,
    char value)
{
    SET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, value);
}

/**
 * @brief get_kernel_argument_ulong reads the 8 bits kernel arguments from offset @p offset
 * of @p ctrl_intf control interface
 * @return  the value read from kernel's argument register
 */
static inline char get_kernel_argument_char(struct control_interface *ctrl_intf, unsigned offset)
{
    return GET_KERNEL_ARG_UNSAFE(ctrl_intf, offset, char);
}

/**
 * @brief start_kernel starts the kernel computation by writing the contorl signals
 * of the AXI control interface
 *
 * It writes the ap_start bit.
 *
 * @param ctrl_intf the control interface pointer
 */
void start_kernel(struct control_interface *ctrl_intf);

/**
 * @brief wait_kernel waits for the kernel to be done
 *
 * @param ctrl_intf the control interface pointer
 * @param usleep_timeout sleeping intervals to wait for the computation end; 0 means busy wait
 */
void wait_kernel(struct control_interface *ctrl_intf, unsigned usleep_timeout);

#ifdef __cplusplus
}
#endif

#endif /* DMA_ENGINE_BUF_H_ */
