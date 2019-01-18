#include <stdint-gcc.h>
#include "xparameters.h"
#include <string.h>


typedef uintptr_t UINTPTR;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;


//#define ULONG64_HI_MASK	0xFFFFFFFF00000000U
//#define ULONG64_LO_MASK	~ULONG64_HI_MASK

#define LINUX_MEM_DEV "/dev/mem"

#define XIL_COMPONENT_IS_READY     0x11111111U  /**< In device drivers, This macro will be
                                                 assigend to "IsReady" member of driver
												 instance to indicate that driver
												 instance is initialized and ready to use. */
#define XIL_COMPONENT_IS_STARTED   0x22222222U  /**< In device drivers, This macro will be assigend to
                                                 "IsStarted" member of driver instance
												 to indicate that driver instance is
												 started and it can be enabled. */

/*********************** Common statuses 0 - 500 *****************************/
/**
@name Common Status Codes for All Device Drivers
@{
*/
#define XST_SUCCESS                     0L
#define XST_FAILURE                     1L
#define XST_DEVICE_NOT_FOUND            2L
#define XST_DEVICE_BLOCK_NOT_FOUND      3L
#define XST_INVALID_VERSION             4L
#define XST_DEVICE_IS_STARTED           5L
#define XST_DEVICE_IS_STOPPED           6L
#define XST_FIFO_ERROR                  7L	/*!< An error occurred during an
						   operation with a FIFO such as
						   an underrun or overrun, this
						   error requires the device to
						   be reset */
#define XST_RESET_ERROR                 8L	/*!< An error occurred which requires
						   the device to be reset */
#define XST_DMA_ERROR                   9L	/*!< A DMA error occurred, this error
						   typically requires the device
						   using the DMA to be reset */
#define XST_NOT_POLLED                  10L	/*!< The device is not configured for
						   polled mode operation */
#define XST_FIFO_NO_ROOM                11L	/*!< A FIFO did not have room to put
						   the specified data into */
#define XST_BUFFER_TOO_SMALL            12L	/*!< The buffer is not large enough
						   to hold the expected data */
#define XST_NO_DATA                     13L	/*!< There was no data available */
#define XST_REGISTER_ERROR              14L	/*!< A register did not contain the
						   expected value */
#define XST_INVALID_PARAM               15L	/*!< An invalid parameter was passed
						   into the function */
#define XST_NOT_SGDMA                   16L	/*!< The device is not configured for
						   scatter-gather DMA operation */
#define XST_LOOPBACK_ERROR              17L	/*!< A loopback test failed */
#define XST_NO_CALLBACK                 18L	/*!< A callback has not yet been
						   registered */
#define XST_NO_FEATURE                  19L	/*!< Device is not configured with
						   the requested feature */
#define XST_NOT_INTERRUPT               20L	/*!< Device is not configured for
						   interrupt mode operation */
#define XST_DEVICE_BUSY                 21L	/*!< Device is busy */
#define XST_ERROR_COUNT_MAX             22L	/*!< The error counters of a device
						   have maxed out */
#define XST_IS_STARTED                  23L	/*!< Used when part of device is
						   already started i.e.
						   sub channel */
#define XST_IS_STOPPED                  24L	/*!< Used when part of device is
						   already stopped i.e.
						   sub channel */
#define XST_DATA_LOST                   26L	/*!< Driver defined error */
#define XST_RECV_ERROR                  27L	/*!< Generic receive error */
#define XST_SEND_ERROR                  28L	/*!< Generic transmit error */
#define XST_NOT_ENABLED                 29L	/*!< A requested service is not
						   available because it has not
						   been enabled */

/************************** Constant Definitions *****************************/

/** @name Configuration options
 *
 * The following options may be specified or retrieved for the device and
 * enable/disable additional features of the SPI.  Each of the options
 * are bit fields, so more than one may be specified.
 *
 * @{
 */
/**
 * <pre>
 * The Master option configures the SPI device as a master. By default, the
 * device is a slave.
 *
 * The Active Low Clock option configures the device's clock polarity. Setting
 * this option means the clock is active low and the SCK signal idles high. By
 * default, the clock is active high and SCK idles low.
 *
 * The Clock Phase option configures the SPI device for one of two transfer
 * formats.  A clock phase of 0, the default, means data if valid on the first
 * SCK edge (rising or falling) after the slave select (SS) signal has been
 * asserted. A clock phase of 1 means data is valid on the second SCK edge
 * (rising or falling) after SS has been asserted.
 *
 * The Loopback option configures the SPI device for loopback mode.  Data is
 * looped back from the transmitter to the receiver.
 *
 * The Manual Slave Select option, which is default, causes the device not
 * to automatically drive the slave select.  The driver selects the device
 * at the start of a transfer and deselects it at the end of a transfer.
 * If this option is off, then the device automatically toggles the slave
 * select signal between bytes in a transfer.
 * </pre>
 */
#define XSP_MASTER_OPTION		0x1
#define XSP_CLK_ACTIVE_LOW_OPTION	0x2
#define XSP_CLK_PHASE_1_OPTION		0x4
#define XSP_LOOPBACK_OPTION		0x8
#define XSP_MANUAL_SSELECT_OPTION	0x10

/*********************** SPI statuses 1151 - 1175 ****************************/

#define XST_SPI_MODE_FAULT          1151	/*!< master was selected as slave */
#define XST_SPI_TRANSFER_DONE       1152	/*!< data transfer is complete */
#define XST_SPI_TRANSMIT_UNDERRUN   1153	/*!< slave underruns transmit register */
#define XST_SPI_RECEIVE_OVERRUN     1154	/*!< device overruns receive register */
#define XST_SPI_NO_SLAVE            1155	/*!< no slave has been selected yet */
#define XST_SPI_TOO_MANY_SLAVES     1156	/*!< more than one slave is being
						 * selected */
#define XST_SPI_NOT_MASTER          1157	/*!< operation is valid only as master */
#define XST_SPI_SLAVE_ONLY          1158	/*!< device is configured as slave-only
						 */
#define XST_SPI_SLAVE_MODE_FAULT    1159	/*!< slave was selected while disabled */
#define XST_SPI_SLAVE_MODE          1160	/*!< device has been addressed as slave */
#define XST_SPI_RECEIVE_NOT_EMPTY   1161	/*!< device received data in slave mode */

#define XST_SPI_COMMAND_ERROR       1162	/*!< unrecognised command - qspi only */
#define XST_SPI_POLL_DONE           1163        /*!< controller completed polling the
						   device for status */

#ifndef TRUE
#  define TRUE		1U
#endif

#ifndef FALSE
#  define FALSE		0U
#endif

/** @name Data Width Definitions
 *
 * @{
 */
#define XSP_DATAWIDTH_BYTE	 8  /**< Tx/Rx Reg is Byte Wide */
#define XSP_DATAWIDTH_HALF_WORD	16  /**< Tx/Rx Reg is Half Word (16 bit)
						Wide */
#define XSP_DATAWIDTH_WORD	32  /**< Tx/Rx Reg is Word (32 bit)  Wide */

/**
 * XSpi statistics
 */
typedef struct {
    u32 ModeFaults;		/**< Number of mode fault errors */
    u32 XmitUnderruns;	/**< Number of transmit underruns */
    u32 RecvOverruns;	/**< Number of receive overruns */
    u32 SlaveModeFaults;	/**< Num of selects as slave while disabled */
    u32 BytesTransferred;	/**< Number of bytes transferred */
    u32 NumInterrupts;	/**< Number of transmit/receive interrupts */
} XSpi_Stats;

typedef void (*XSpi_StatusHandler) (void *CallBackRef, u32 StatusEvent,
                                    unsigned int ByteCount);

/**
 * This typedef contains configuration information for the device.
 */
typedef struct {
    u16 DeviceId;		/**< Unique ID  of device */
    UINTPTR BaseAddress;	/**< Base address of the device */
    int HasFifos;		/**< Does device have FIFOs? */
    u32 SlaveOnly;		/**< Is the device slave only? */
    u8 NumSlaveBits;	/**< Num of slave select bits on the device */
    u8 DataWidth;		/**< Data transfer Width */
    u8 SpiMode;		/**< Standard/Dual/Quad mode */
    u8 AxiInterface;	/**< AXI-Lite/AXI Full Interface */
    u32 AxiFullBaseAddress;	/**< AXI Full Interface Base address of
					the device */
    u8 XipMode;             /**< 0 if Non-XIP, 1 if XIP Mode */
    u8 Use_Startup;		/**< 1 if Starup block is used in h/w */
} XSpi_Config;

/**
 * The XSpi driver instance data. The user is required to allocate a
 * variable of this type for every SPI device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct {
    XSpi_Stats Stats;	/**< Statistics */
    UINTPTR BaseAddr;		/**< Base address of device (IPIF) */
    int IsReady;		/**< Device is initialized and ready */
    int IsStarted;		/**< Device has been started */
    int HasFifos;		/**< Device is configured with FIFOs or not */
    u32 SlaveOnly;		/**< Device is configured to be slave only */
    u8 NumSlaveBits;	/**< Number of slave selects for this device */
    u8 DataWidth;		/**< Data Transfer Width 8 or 16 or 32 */
    u8 SpiMode;		/**< Standard/Dual/Quad mode */
    u32 SlaveSelectMask;	/**< Mask that matches the number of SS bits */
    u32 SlaveSelectReg;	/**< Slave select register */

    u8 *SendBufferPtr;	/**< Buffer to send  */
    u8 *RecvBufferPtr;	/**< Buffer to receive */
    unsigned int RequestedBytes; /**< Total bytes to transfer (state) */
    unsigned int RemainingBytes; /**< Bytes left to transfer (state) */
    int IsBusy;		/**< A transfer is in progress (state) */

    XSpi_StatusHandler StatusHandler; /**< Status Handler */
    void *StatusRef;	/**< Callback reference for status handler */
    u32 FlashBaseAddr;    	/**< Used in XIP Mode */
    u8 XipMode;             /**< 0 if Non-XIP, 1 if XIP Mode */
    UINTPTR vAddr; //virtual add to be used in mmap
    u32 vAddr_length; // lenght of address memory
    int fd; //file descriptor for mmap
} XSpi;

XSpi_Config *XSpi_LookupConfig(u16 DeviceId);

XSpi_Config XSpi_ConfigTable[] =
{
        {
                XPAR_SPI_0_DEVICE_ID,
                XPAR_SPI_0_BASEADDR,
                XPAR_SPI_0_FIFO_EXIST,
                XPAR_SPI_0_SPI_SLAVE_ONLY,
                XPAR_SPI_0_NUM_SS_BITS,
                XPAR_SPI_0_NUM_TRANSFER_BITS,
                XPAR_SPI_0_SPI_MODE,
                XPAR_SPI_0_TYPE_OF_AXI4_INTERFACE,
                XPAR_SPI_0_AXI4_BASEADDR,
                XPAR_SPI_0_XIP_MODE,
                XPAR_SPI_0_USE_STARTUP
        }
};

/**
 * XSPI register offsets
 */
/** @name Register Map
 *
 * Register offsets for the XSpi device.
 * @{
 */
#define XSP_DGIER_OFFSET	0x1C	/**< Global Intr Enable Reg */
#define XSP_IISR_OFFSET		0x20	/**< Interrupt status Reg */
#define XSP_IIER_OFFSET		0x28	/**< Interrupt Enable Reg */
#define XSP_SRR_OFFSET	 	0x40	/**< Software Reset register */
#define XSP_CR_OFFSET		0x60	/**< Control register */
#define XSP_SR_OFFSET		0x64	/**< Status Register */
#define XSP_DTR_OFFSET		0x68	/**< Data transmit */
#define XSP_DRR_OFFSET		0x6C	/**< Data receive */
#define XSP_SSR_OFFSET		0x70	/**< 32-bit slave select */
#define XSP_TFO_OFFSET		0x74	/**< Tx FIFO occupancy */
#define XSP_RFO_OFFSET		0x78	/**< Rx FIFO occupancy */

/**
 * SPI Software Reset Register (SRR) mask.
 */
#define XSP_SRR_RESET_MASK		0x0000000A

/** @name SPI Control Register (CR) masks
 *
 * @{
 */
#define XSP_CR_LOOPBACK_MASK	   0x00000001 /**< Local loopback mode */
#define XSP_CR_ENABLE_MASK	   0x00000002 /**< System enable */
#define XSP_CR_MASTER_MODE_MASK	   0x00000004 /**< Enable master mode */
#define XSP_CR_CLK_POLARITY_MASK   0x00000008 /**< Clock polarity high
								or low */
#define XSP_CR_CLK_PHASE_MASK	   0x00000010 /**< Clock phase 0 or 1 */
#define XSP_CR_TXFIFO_RESET_MASK   0x00000020 /**< Reset transmit FIFO */
#define XSP_CR_RXFIFO_RESET_MASK   0x00000040 /**< Reset receive FIFO */
#define XSP_CR_MANUAL_SS_MASK	   0x00000080 /**< Manual slave select
								assert */
#define XSP_CR_TRANS_INHIBIT_MASK  0x00000100 /**< Master transaction
								inhibit */


/** @name SPI Device Interrupt Status/Enable Registers
 *
 * <b> Interrupt Status Register (IPISR) </b>
 *
 * This register holds the interrupt status flags for the Spi device.
 *
 * <b> Interrupt Enable Register (IPIER) </b>
 *
 * This register is used to enable interrupt sources for the Spi device.
 * Writing a '1' to a bit in this register enables the corresponding Interrupt.
 * Writing a '0' to a bit in this register disables the corresponding Interrupt.
 *
 * ISR/IER registers have the same bit definitions and are only defined once.
 * @{
 */
#define XSP_INTR_MODE_FAULT_MASK	0x00000001 /**< Mode fault error */
#define XSP_INTR_SLAVE_MODE_FAULT_MASK	0x00000002 /**< Selected as slave while
						     *  disabled */
#define XSP_INTR_TX_EMPTY_MASK		0x00000004 /**< DTR/TxFIFO is empty */
#define XSP_INTR_TX_UNDERRUN_MASK	0x00000008 /**< DTR/TxFIFO underrun */
#define XSP_INTR_RX_FULL_MASK		0x00000010 /**< DRR/RxFIFO is full */
#define XSP_INTR_RX_OVERRUN_MASK	0x00000020 /**< DRR/RxFIFO overrun */
#define XSP_INTR_TX_HALF_EMPTY_MASK	0x00000040 /**< TxFIFO is half empty */
#define XSP_INTR_SLAVE_MODE_MASK	0x00000080 /**< Slave select mode */
#define XSP_INTR_RX_NOT_EMPTY_MASK	0x00000100 /**< RxFIFO not empty */

/**
 * The following bits are available only in axi_qspi Interrupt Status and
 * Interrupt Enable registers.
 */
#define XSP_INTR_CPOL_CPHA_ERR_MASK	0x00000200 /**< CPOL/CPHA error */
#define XSP_INTR_SLAVE_MODE_ERR_MASK	0x00000400 /**< Slave mode error */
#define XSP_INTR_MSB_ERR_MASK		0x00000800 /**< MSB Error */
#define XSP_INTR_LOOP_BACK_ERR_MASK	0x00001000 /**< Loop back error */
#define XSP_INTR_CMD_ERR_MASK		0x00002000 /**< 'Invalid cmd' error */



/**
 * Mask for all the interrupts in the IP Interrupt Registers.
 */
#define XSP_INTR_ALL		(XSP_INTR_MODE_FAULT_MASK | \
				 XSP_INTR_SLAVE_MODE_FAULT_MASK | \
				 XSP_INTR_TX_EMPTY_MASK | \
				 XSP_INTR_TX_UNDERRUN_MASK | \
				 XSP_INTR_RX_FULL_MASK | \
				 XSP_INTR_TX_HALF_EMPTY_MASK | \
				 XSP_INTR_RX_OVERRUN_MASK | \
				 XSP_INTR_SLAVE_MODE_MASK | \
				 XSP_INTR_RX_NOT_EMPTY_MASK | \
				 XSP_INTR_CMD_ERR_MASK | \
				 XSP_INTR_LOOP_BACK_ERR_MASK | \
				 XSP_INTR_MSB_ERR_MASK | \
				 XSP_INTR_SLAVE_MODE_ERR_MASK | \
				 XSP_INTR_CPOL_CPHA_ERR_MASK)

/**
* The interrupts we want at startup. We add the TX_EMPTY interrupt in later
* when we're getting ready to transfer data.  The others we don't care
* about for now.
*/
#define XSP_INTR_DFT_MASK	(XSP_INTR_MODE_FAULT_MASK |	\
				 XSP_INTR_TX_UNDERRUN_MASK |	\
				 XSP_INTR_RX_OVERRUN_MASK |	\
				 XSP_INTR_SLAVE_MODE_FAULT_MASK | \
				 XSP_INTR_CMD_ERR_MASK)

/**
* @name Global Interrupt Enable Register (GIER) mask(s)
* @{
*/
#define XSP_GINTR_ENABLE_MASK	0x80000000	/**< Global interrupt enable */


/** @name Status Register (SR) masks
 *
 * @{
 */
#define XSP_SR_RX_EMPTY_MASK	   0x00000001 /**< Receive Reg/FIFO is empty */
#define XSP_SR_RX_FULL_MASK	   0x00000002 /**< Receive Reg/FIFO is full */
#define XSP_SR_TX_EMPTY_MASK	   0x00000004 /**< Transmit Reg/FIFO is
								empty */
#define XSP_SR_TX_FULL_MASK	   0x00000008 /**< Transmit Reg/FIFO is full */
#define XSP_SR_MODE_FAULT_MASK	   0x00000010 /**< Mode fault error */
#define XSP_SR_SLAVE_MODE_MASK	   0x00000020 /**< Slave mode select */

/*
 * The following bits are available only in axi_qspi Status register.
 */
#define XSP_SR_CPOL_CPHA_ERR_MASK  0x00000040 /**< CPOL/CPHA error */
#define XSP_SR_SLAVE_MODE_ERR_MASK 0x00000080 /**< Slave mode error */
#define XSP_SR_MSB_ERR_MASK	   0x00000100 /**< MSB Error */
#define XSP_SR_LOOP_BACK_ERR_MASK  0x00000200 /**< Loop back error */
#define XSP_SR_CMD_ERR_MASK	   0x00000400 /**< 'Invalid cmd' error */

/* @} */

/** @name Status Register (SR) masks for XIP Mode
 *
 * @{
 */
#define XSP_SR_XIP_RX_EMPTY_MASK	0x00000001 /**< Receive Reg/FIFO
								is empty */
#define XSP_SR_XIP_RX_FULL_MASK		0x00000002 /**< Receive Reg/FIFO
								is full */
#define XSP_SR_XIP_MASTER_MODF_MASK	0x00000004 /**< Receive Reg/FIFO
								is full */
#define XSP_SR_XIP_CPHPL_ERROR_MASK	0x00000008 /**< Clock Phase,Clock
							 Polarity Error */
#define XSP_SR_XIP_AXI_ERROR_MASK	0x00000010 /**< AXI Transaction
								Error */


/************************** Variable Definitions *****************************/

/*
 * Create the table of options which are processed to get/set the device
 * options. These options are table driven to allow easy maintenance and
 * expansion of the options.
 */
typedef struct {
    u32 Option;
    u32 Mask;
} OptionsMap;

static OptionsMap OptionsTable[] = {
        {XSP_LOOPBACK_OPTION, XSP_CR_LOOPBACK_MASK},
        {XSP_CLK_ACTIVE_LOW_OPTION, XSP_CR_CLK_POLARITY_MASK},
        {XSP_CLK_PHASE_1_OPTION, XSP_CR_CLK_PHASE_MASK},
        {XSP_MASTER_OPTION, XSP_CR_MASTER_MODE_MASK},
        {XSP_MANUAL_SSELECT_OPTION, XSP_CR_MANUAL_SS_MASK}
};

#define XSP_NUM_OPTIONS		(sizeof(OptionsTable) / sizeof(OptionsMap))

/*****************************************************************************/
/**
*
* This is a stub for the status callback. The stub is here in case the upper
* layers forget to set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference
* @param	StatusEvent is the event that just occurred.
* @param	ByteCount is the number of bytes transferred up until the event
*		occurred.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void StubStatusHandler(void *CallBackRef, u32 StatusEvent,
                              unsigned int ByteCount)
{
        (void )CallBackRef;
        (void )StatusEvent;
        (void ) ByteCount;
        //Xil_AssertVoidAlways();
}


void XSpi_Reset(XSpi *InstancePtr);


/*****************************************************************************/
/**
*
* @brief    Performs an input operation for a memory location by
*           reading from the specified address and returning the 32 bit Value
*           read  from that address.
*
* @param	Addr: contains the address to perform the input operation
*
* @return	The 32 bit Value read from the specified input address.
*
******************************************************************************/
static inline u32 Xil_In32(UINTPTR Addr)
{
        return *(volatile u32 *) Addr;
}
/*****************************************************************************/
/**
*
* @brief    Performs an output operation for a memory location by writing the
*           32 bit Value to the the specified address.
*
* @param	Addr contains the address to perform the output operation
* @param	Value contains the 32 bit Value to be written at the specified
*           address.
*
* @return	None.
*
******************************************************************************/
static inline void Xil_Out32(UINTPTR Addr, u32 Value)
{
        //todo check who sets this
#ifndef ENABLE_SAFETY
        volatile u32 *LocalAddr = (volatile u32 *)Addr;
        *LocalAddr = Value;
#else
        XStl_RegUpdate(Addr, Value);
#endif
}

/*****************************************************************************/
/**
* @brief    This assert macro is to be used for functions that do return a
*           value. This in conjunction with the Xil_AssertWait boolean can be
*           used to accomodate tests so that asserts which fail allow execution
*           to continue.
*
* @param    Expression: expression to be evaluated. If it evaluates to false,
*           the assert occurs.
*
* @return   Returns 0 unless the Xil_AssertWait variable is true, in which
* 	        case no return is made and an infinite loop is entered.
*
******************************************************************************/
#define Xil_AssertNonvoid(Expression)             \
{                                                  \
    if (!Expression){                                       \
        printf("%s --- %s \n",__FILE__, __LINE__);            \
    }                                              \
}

/*
 * Basic function to read specific registers
 * todo
 */
static inline u32 XSpi_ReadReg(UINTPTR VirtualAddress, u32 RegOffset){
       return Xil_In32(VirtualAddress + RegOffset);
}


static inline u32 XSpi_GetControlReg(XSpi *InstancePtr){
        return XSpi_ReadReg(InstancePtr->vAddr, XSP_CR_OFFSET);
}

//return here is
static inline void XSpi_WriteReg(UINTPTR VirtualAddress, u32 RegOffset, u32 RegisterValue){
        Xil_Out32(VirtualAddress + RegOffset, (RegisterValue));
}

static inline void XSpi_SetControlReg(XSpi *InstancePtr, u32 mask){
        XSpi_WriteReg(InstancePtr->vAddr, XSP_CR_OFFSET, (mask));
}


/****************************************************************************/
/**
*
* Set the contents of the slave select register. Each bit in the mask
* corresponds to a slave select line. Only one slave should be selected at
* any one time.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	Mask is the 32-bit value to write to the slave select register.
*
* @return	None.
*
* @note		C-Style signature:
* 		void XSpi_SetSlaveSelectReg(XSpi *InstancePtr, u32 Mask);
*
*****************************************************************************/
static inline void XSpi_SetSlaveSelectReg(XSpi *InstancePtr, u32 Mask) {
        XSpi_WriteReg(((InstancePtr)->BaseAddr), XSP_SSR_OFFSET, (Mask));
}

int XSpi_CfgInitialize(XSpi *InstancePtr, XSpi_Config *Config,
                       UINTPTR EffectiveAddr);

int XSpi_SetOptions(XSpi *InstancePtr, u32 Options);

/******************************************************************************/
/**
*
* This function sets the contents of the Interrupt Enable Register.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	EnableMask is the bitmask of the interrupts to be enabled.
*		Bit positions of 1 will be enabled. Bit positions of 0 will
*		keep the previous setting. This mask is formed by OR'ing
*		XSP_INTR_* bits defined in xspi_l.h.
*
* @return 	None.
*
* @note		C-Style signature:
*		void XSpi_IntrEnable(XSpi *InstancePtr, u32 EnableMask);
*
******************************************************************************/
void XSpi_IntrEnable(XSpi *InstancePtr, u32 EnableMask){
    XSpi_WriteReg(((InstancePtr)->BaseAddr), XSP_IIER_OFFSET,
                  (XSpi_ReadReg(((InstancePtr)->BaseAddr),
                                XSP_IIER_OFFSET)) | (((EnableMask) & XSP_INTR_ALL )));
}


/******************************************************************************/
/**
*
* This macro writes to the global interrupt enable register to enable
* interrupts from the device.
*
* Interrupts enabled using XSpi_IntrEnable() will not occur until the global
* interrupt enable bit is set by using this function.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	None.
*
* @note		C-Style signature:
*		void XSpi_IntrGlobalEnable(XSpi *InstancePtr);
*
******************************************************************************/
void XSpi_IntrGlobalEnable(XSpi *InstancePtr){
    XSpi_WriteReg(((InstancePtr)->BaseAddr),  XSP_DGIER_OFFSET,
                  XSP_GINTR_ENABLE_MASK);
}

int XSpi_Start(XSpi *InstancePtr);

/******************************************************************************/
/**
*
* This macro disables all interrupts for the device by writing to the Global
* interrupt enable register.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	None.
*
* @note		C-Style signature:
*		void XSpi_IntrGlobalDisable(XSpi *InstancePtr);
*
******************************************************************************/
void XSpi_IntrGlobalDisable(XSpi *InstancePtr){
    XSpi_WriteReg(((InstancePtr)->BaseAddr),  XSP_DGIER_OFFSET, 0);
}

int XSpi_SetSlaveSelect(XSpi *InstancePtr, u32 SlaveMask);

/*****************************************************************************/
/**
*
* This function determines if interrupts are enabled at the global level by
* reading the global interrupt register.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return
*		- TRUE if global interrupts are enabled.
*		- FALSE if global interrupts are disabled.
*
* @note		C-Style signature:
*		int XSpi_IsIntrGlobalEnabled(XSpi *InstancePtr);
*
******************************************************************************/
int XSpi_IsIntrGlobalEnabled(XSpi *InstancePtr){
    return (XSpi_ReadReg(((InstancePtr)->BaseAddr), XSP_DGIER_OFFSET) == XSP_GINTR_ENABLE_MASK);
}

/***************************************************************************/
/**
*
* Get the contents of the status register. Use the XSP_SR_* constants defined
* above to interpret the bit-mask returned.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	An 32-bit value representing the contents of the status
*		register.
*
* @note		C-Style signature:
* 		u8 XSpi_GetStatusReg(XSpi *InstancePtr);
*
*****************************************************************************/
u32 XSpi_GetStatusReg(XSpi *InstancePtr) {
    return XSpi_ReadReg(((InstancePtr)->BaseAddr), XSP_SR_OFFSET);
}

/*****************************************************************************/
/**
*
* This function gets the contents of the Interrupt Status Register.
* This register indicates the status of interrupt sources for the device.
* The status is independent of whether interrupts are enabled such
* that the status register may also be polled when interrupts are not enabled.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	A status which contains the value read from the Interrupt
*		Status Register.
*
* @note		C-Style signature:
*		u32 XSpi_IntrGetStatus(XSpi *InstancePtr);
*
******************************************************************************/
u32 XSpi_IntrGetStatus(XSpi *InstancePtr){
    return XSpi_ReadReg(((InstancePtr)->BaseAddr), XSP_IISR_OFFSET);
}

/*****************************************************************************/
/**
*
* This function clears the specified interrupts in the Interrupt status
* Register. The interrupt is cleared by writing to this register with the bits
* to be cleared set to a one and all others bits to zero. Setting a bit which
* is zero within this register causes an interrupt to be generated.
*
* This function writes only the specified value to the register such that
* some status bits may be set and others cleared.  It is the caller's
* responsibility to get the value of the register prior to setting the value
* to prevent an destructive behavior.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	ClearMask is the Bitmask for interrupts to be cleared.
*		Bit positions of "1" clears the interrupt. Bit positions of 0
*		will keep the previous setting. This mask is formed by OR'ing
*		XSP_INTR_* bits defined in xspi_l.h.
*
* @return	None.
*
* @note		C-Style signature:
*		void XSpi_IntrClear(XSpi *InstancePtr, u32 ClearMask);
*
******************************************************************************/
void XSpi_IntrClear(XSpi *InstancePtr, u32 ClearMask){
    XSpi_WriteReg(((InstancePtr)->BaseAddr),  XSP_IISR_OFFSET, XSpi_IntrGetStatus(InstancePtr) | (ClearMask));
}
int XSpi_Transfer(XSpi *InstancePtr, u8 *SendBufPtr,
                  u8 *RecvBufPtr, unsigned int ByteCount);