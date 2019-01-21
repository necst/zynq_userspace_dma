//
// Created by lorenzo on 18/01/19.
//
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "xspi.h"
#include "stdio.h"

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
    XSpi_WriteReg(((InstancePtr)->vAddr), XSP_SSR_OFFSET, (Mask));
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
    XSpi_WriteReg(((InstancePtr)->vAddr), XSP_IIER_OFFSET,
                  (XSpi_ReadReg(((InstancePtr)->vAddr),
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
    XSpi_WriteReg(((InstancePtr)->vAddr),  XSP_DGIER_OFFSET,
                  XSP_GINTR_ENABLE_MASK);
}

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
    XSpi_WriteReg(((InstancePtr)->vAddr),  XSP_DGIER_OFFSET, 0);
}

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
    return (XSpi_ReadReg(((InstancePtr)->vAddr), XSP_DGIER_OFFSET) == XSP_GINTR_ENABLE_MASK);
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
    return XSpi_ReadReg(((InstancePtr)->vAddr), XSP_SR_OFFSET);
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
    return XSpi_ReadReg(((InstancePtr)->vAddr), XSP_IISR_OFFSET);
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
    XSpi_WriteReg(((InstancePtr)->vAddr),  XSP_IISR_OFFSET, XSpi_IntrGetStatus(InstancePtr) | (ClearMask));
}

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param	DeviceId contains the ID of the device to look up the
*		configuration for.
*
* @return
*
* A pointer to the configuration found or NULL if the specified device ID was
* not found. See xspi.h for the definition of XSpi_Config.
*
* @note		None.
*
******************************************************************************/
XSpi_Config *XSpi_LookupConfig(u16 DeviceId)
{
    XSpi_Config *CfgPtr = NULL;
    u32 Index;

    for (Index = 0; Index < XPAR_XSPI_NUM_INSTANCES; Index++) {
        if (XSpi_ConfigTable[Index].DeviceId == DeviceId) {
            CfgPtr = &XSpi_ConfigTable[Index];
            break;
        }
    }

    return CfgPtr;
}

static u32 init_memory(XSpi *InstancePtr){
    InstancePtr -> vAddr_length = XPAR_SPI_0_AXI4_HIGHADDR - XPAR_SPI_0_AXI4_BASEADDR + 1;

    int fd;
    if((fd = open(LINUX_MEM_DEV, O_RDWR | O_SYNC))){
        InstancePtr -> vAddr = (UINTPTR)mmap(NULL, InstancePtr -> vAddr_length,
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_SPI_0_AXI4_BASEADDR);
        InstancePtr -> fd = fd;
        if(InstancePtr -> vAddr == (UINTPTR)NULL){
            printf("ERROR while performing mmap");
            exit(-1);
        }

    }else{
        printf("ERROR while opening linux dev mem");
        return 1;
    }

    return 0;
}

//to be used within this file
static void deinit_memory(XSpi *InstancePtr){
    int status =  munmap((void *)InstancePtr -> vAddr, InstancePtr -> vAddr_length);
    if(status != 0){
        printf("error when de initializing memory ERROR = %d \n", status);
    }
    status = close(InstancePtr -> fd);
    InstancePtr -> fd = 0;
    InstancePtr -> vAddr = (UINTPTR)NULL;
    if(status != 0){
        printf("error when closing file descriptor ERROR = %d \n", status);
    }
}


/*****************************************************************************/
/**
*
* Aborts a transfer in progress by setting the stop bit in the control register,
* then resetting the FIFOs if present. The byte counts are cleared and the
* busy flag is set to false.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	None.
*
* @note
*
* This function does a read/modify/write of the control register. The user of
* this function needs to take care of critical sections.
*
******************************************************************************/
void XSpi_Abort(XSpi *InstancePtr)
{
    u16 ControlReg;

    /*
     * Deselect the slave on the SPI bus to abort a transfer, this must be
     * done before the device is disabled such that the signals which are
     * driven by the device are changed without the device enabled.
     */
    printf("Prima di set slave select reg \n");
    XSpi_SetSlaveSelectReg(InstancePtr,
                           InstancePtr->SlaveSelectMask);
    printf("Dopo di set slave select reg \n");

    /*
     * Abort the operation currently in progress. Clear the mode
     * fault condition by reading the status register (done) then
     * writing the control register.
     */
    ControlReg = XSpi_GetControlReg(InstancePtr);

    /*
     * Stop any transmit in progress and reset the FIFOs if they exist,
     * don't disable the device just inhibit any data from being sent.
     */
    ControlReg |= XSP_CR_TRANS_INHIBIT_MASK;

    if (InstancePtr->HasFifos) {
        ControlReg |= (XSP_CR_TXFIFO_RESET_MASK |
                       XSP_CR_RXFIFO_RESET_MASK);
    }

    XSpi_SetControlReg(InstancePtr, ControlReg);

    InstancePtr->RemainingBytes = 0;
    InstancePtr->RequestedBytes = 0;
    InstancePtr->IsBusy = FALSE;
}

/*****************************************************************************/
/**
*
* Resets the SPI device by writing to the Software Reset register. Reset must
* only be called after the driver has been initialized. The configuration of the
* device after reset is the same as its configuration after initialization.
* Refer to the XSpi_Initialize function for more details. This is a hard reset
* of the device. Any data transfer that is in progress is aborted.
*
* The upper layer software is responsible for re-configuring (if necessary)
* and restarting the SPI device after the reset.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XSpi_Reset(XSpi *InstancePtr)
{
    if(InstancePtr == NULL){
        printf("NULL %s --- %d \n",__FILE__, __LINE__);
    }

    if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY){
        printf("NOT READY %s --- %d \n",__FILE__, __LINE__);
    }
    /*
     * Abort any transfer that is in progress.
     */
    XSpi_Abort(InstancePtr);

    /*
     * Reset any values that are not reset by the hardware reset such that
     * the software state matches the hardware device.
     */
    InstancePtr->IsStarted = 0;
    InstancePtr->SlaveSelectReg = InstancePtr->SlaveSelectMask;

    /*
     * Reset the device.
     */
    XSpi_WriteReg(InstancePtr->vAddr, XSP_SRR_OFFSET,
                  XSP_SRR_RESET_MASK);
}





/*****************************************************************************/
/**
*
* Initializes a specific XSpi instance such that the driver is ready to use.
*
* The state of the device after initialization is:
*	- Device is disabled
*	- Slave mode
*	- Active high clock polarity
*	- Clock phase 0
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	Config is a reference to a structure containing information
*		about a specific SPI device. This function initializes an
*		InstancePtr object for a specific device specified by the
*		contents of Config. This function can initialize multiple
*		instance objects with the use of multiple calls giving
		different Config information on each call.
* @param	EffectiveAddr is the device base address in the virtual memory
*		address space. The caller is responsible for keeping the
*		address mapping from EffectiveAddr to the device physical base
*		address unchanged once this function is invoked. Unexpected
*		errors may occur if the address mapping changes after this
*		function is called. If address translation is not used, use
*		Config->BaseAddress for this parameters, passing the physical
*		address instead.
*
* @return
*		- XST_SUCCESS if successful.
*		- XST_DEVICE_IS_STARTED if the device is started. It must be
*		  stopped to re-initialize.
*
* @note		None.
*
******************************************************************************/
int XSpi_CfgInitialize(XSpi *InstancePtr, XSpi_Config *Config,
                       UINTPTR EffectiveAddr)
{
    u8  Buffer[3];
    u32 ControlReg;
    u32 ret = 0;

    //Xil_AssertNonvoid(InstancePtr != NULL);
    if(InstancePtr == NULL){
        printf("%s --- %d \n",__FILE__, __LINE__);
    }
    ret = init_memory(InstancePtr);
    if(ret != 0){
        printf("MUOIO \n");
        exit(-1);
    }
    /*
     * If the device is started, disallow the initialize and return a status
     * indicating it is started.  This allows the user to stop the device
     * and reinitialize, but prevents a user from inadvertently
     * initializing.
     */
    if (InstancePtr->IsStarted == XIL_COMPONENT_IS_STARTED) {
        return XST_DEVICE_IS_STARTED;
    }

    /*
     * Set some default values.
     */
    InstancePtr->IsStarted = 0;
    InstancePtr->IsBusy = FALSE;

    InstancePtr->StatusHandler = StubStatusHandler;

    InstancePtr->SendBufferPtr = NULL;
    InstancePtr->RecvBufferPtr = NULL;
    InstancePtr->RequestedBytes = 0;
    InstancePtr->RemainingBytes = 0;
    InstancePtr->BaseAddr = EffectiveAddr;
    InstancePtr->HasFifos = Config->HasFifos;
    InstancePtr->SlaveOnly = Config->SlaveOnly;
    InstancePtr->NumSlaveBits = Config->NumSlaveBits;
    if (Config->DataWidth == 0) {
        InstancePtr->DataWidth = XSP_DATAWIDTH_BYTE;
    } else {
        InstancePtr->DataWidth = Config->DataWidth;
    }

    InstancePtr->SpiMode = Config->SpiMode;

    InstancePtr->FlashBaseAddr = Config->AxiFullBaseAddress;
    InstancePtr->XipMode = Config->XipMode;

    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    /*
     * Create a slave select mask based on the number of bits that can
     * be used to deselect all slaves, initialize the value to put into
     * the slave select register to this value.
     */
    InstancePtr->SlaveSelectMask = (1 << InstancePtr->NumSlaveBits) - 1;
    InstancePtr->SlaveSelectReg = InstancePtr->SlaveSelectMask;

    /*
     * Clear the statistics for this driver.
     */
    InstancePtr->Stats.ModeFaults = 0;
    InstancePtr->Stats.XmitUnderruns = 0;
    InstancePtr->Stats.RecvOverruns = 0;
    InstancePtr->Stats.SlaveModeFaults = 0;
    InstancePtr->Stats.BytesTransferred = 0;
    InstancePtr->Stats.NumInterrupts = 0;

    printf("DEBug ***************** \n Virtual Address is %u \n", (unsigned int)InstancePtr ->vAddr );
    u32 retVal = XSpi_ReadReg(InstancePtr ->vAddr, 0x64);
    printf("RETVAL %x \n", retVal);
    fflush(stdout);

    if(Config->Use_Startup == 1) {
        /*
         * Perform a dummy read this is used when startup block is
         * enabled in the hardware to fix CR #721229.
         */
        ControlReg = XSpi_GetControlReg(InstancePtr);
        ControlReg |= XSP_CR_TXFIFO_RESET_MASK | XSP_CR_RXFIFO_RESET_MASK |
                      XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK ;
        XSpi_SetControlReg(InstancePtr, ControlReg);

        /*
         * Initiate Read command to get the ID. This Read command is for
         * Numonyx flash.
         *
         * NOTE: If user interfaces different flash to the SPI controller
         * this command need to be changed according to target flash Read
         * command.
         */
        Buffer[0] = 0x9F;
        Buffer[1] = 0x00;
        Buffer[2] = 0x00;

        /* Write dummy ReadId to the DTR register */
        XSpi_WriteReg(InstancePtr->vAddr, XSP_DTR_OFFSET, Buffer[0]);
        XSpi_WriteReg(InstancePtr->vAddr, XSP_DTR_OFFSET, Buffer[1]);
        XSpi_WriteReg(InstancePtr->vAddr, XSP_DTR_OFFSET, Buffer[2]);

        /* Master Inhibit enable in the CR */
        ControlReg = XSpi_GetControlReg(InstancePtr);
        ControlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
        XSpi_SetControlReg(InstancePtr, ControlReg);

        /* Master Inhibit disable in the CR */
        ControlReg = XSpi_GetControlReg(InstancePtr);
        ControlReg |= XSP_CR_TRANS_INHIBIT_MASK;
        XSpi_SetControlReg(InstancePtr, ControlReg);

        /* Read the Rx Data Register */
        XSpi_ReadReg(InstancePtr->vAddr, XSP_DRR_OFFSET);
        XSpi_ReadReg(InstancePtr->vAddr, XSP_DRR_OFFSET);
    }

    /*
     * Reset the SPI device to get it into its initial state. It is expected
     * that device configuration will take place after this initialization
     * is done, but before the device is started.
     */
    XSpi_Reset(InstancePtr);

    return XST_SUCCESS;
}

void clean(XSpi *InstancePtr){
    deinit_memory(InstancePtr);
}


/*****************************************************************************/
/**
*
* This function sets the options for the SPI device driver. The options control
* how the device behaves relative to the SPI bus. The device must be idle
* rather than busy transferring data before setting these device options.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	Options contains the specified options to be set. This is a bit
*		mask where a 1 means to turn the option on, and a 0 means to
*		turn the option off. One or more bit values may be contained in
*		the mask.
*		See the bit definitions named XSP_*_OPTIONS in the file xspi.h.
*
* @return
*		-XST_SUCCESS if options are successfully set.
*		- XST_DEVICE_BUSY if the device is currently transferring data.
*		The transfer must complete or be aborted before setting options.
*		- XST_SPI_SLAVE_ONLY if the caller attempted to configure a
*		slave-only device as a master.
*
* @note
*
* This function makes use of internal resources that are shared between the
* XSpi_Stop() and XSpi_SetOptions() functions. So if one task might be setting
* device options while another is trying to stop the device, the user is
* required to provide protection of this shared data (typically using a
* semaphore).
*
******************************************************************************/
int XSpi_SetOptions(XSpi *InstancePtr, u32 Options)
{
    u32 ControlReg;
    u32 Index;

    //Xil_AssertNonvoid(InstancePtr != NULL);
    if(InstancePtr == NULL){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    //Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY){
        printf("NOT READY %s --- %d \n",__FILE__, __LINE__);
    }


    /*
     * Do not allow the slave select to change while a transfer is in
     * progress.
     * No need to worry about a critical section here since even if the Isr
     * changes the busy flag just after we read it, the function will return
     * busy and the caller can retry when notified that their current
     * transfer is done.
     */
    if (InstancePtr->IsBusy) {
        return XST_DEVICE_BUSY;
    }
    /*
     * Do not allow master option to be set if the device is slave only.
     */
    if ((Options & XSP_MASTER_OPTION) && (InstancePtr->SlaveOnly)) {
        return XST_SPI_SLAVE_ONLY;
    }

    ControlReg = XSpi_GetControlReg(InstancePtr);

    /*
     * Loop through the options table, turning the option on or off
     * depending on whether the bit is set in the incoming options flag.
     */
    for (Index = 0; Index < XSP_NUM_OPTIONS; Index++) {
        if (Options & OptionsTable[Index].Option) {
            /*
             *Turn it ON.
             */
            ControlReg |= OptionsTable[Index].Mask;
        }
        else {
            /*
             *Turn it OFF.
             */
            ControlReg &= ~OptionsTable[Index].Mask;
        }
    }

    /*
     * Now write the control register. Leave it to the upper layers
     * to restart the device.
     */
    XSpi_SetControlReg(InstancePtr, ControlReg);

    return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function enables interrupts for the SPI device. If the Spi driver is used
* in interrupt mode, it is up to the user to connect the SPI interrupt handler
* to the interrupt controller before this function is called. If the Spi driver
* is used in polled mode the user has to disable the Global Interrupts after
* this function is called. If the device is configured with FIFOs, the FIFOs are
* reset at this time.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
*
* @return
*		- XST_SUCCESS if the device is successfully started
*		- XST_DEVICE_IS_STARTED if the device was already started.
*
* @note		None.
*
******************************************************************************/
int XSpi_Start(XSpi *InstancePtr)
{
    u32 ControlReg;

    //Xil_AssertNonvoid(InstancePtr != NULL);
    if(InstancePtr == NULL){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    //Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY){
        printf("NOT READY %s --- %d \n",__FILE__, __LINE__);
    }

    /*
     * If it is already started, return a status indicating so.
     */
    if (InstancePtr->IsStarted == XIL_COMPONENT_IS_STARTED) {
        return XST_DEVICE_IS_STARTED;
    }

    /*
     * Enable the interrupts.
     */
    XSpi_IntrEnable(InstancePtr, XSP_INTR_DFT_MASK);

    /*
     * Indicate that the device is started before we enable the transmitter
     * or receiver or interrupts.
     */
    InstancePtr->IsStarted = XIL_COMPONENT_IS_STARTED;

    /*
     * Reset the transmit and receive FIFOs if present. There is a critical
     * section here since this register is also modified during interrupt
     * context. So we wait until after the r/m/w of the control register to
     * enable the Global Interrupt Enable.
     */
    ControlReg = XSpi_GetControlReg(InstancePtr);
    ControlReg |= XSP_CR_TXFIFO_RESET_MASK | XSP_CR_RXFIFO_RESET_MASK |
                  XSP_CR_ENABLE_MASK;
    XSpi_SetControlReg(InstancePtr, ControlReg);

    /*
     * Enable the Global Interrupt Enable just after we start.
     */
    XSpi_IntrGlobalEnable(InstancePtr);

    return XST_SUCCESS;
}
/*****************************************************************************/
/**
*
* Selects or deselect the slave with which the master communicates. Each slave
* that can be selected is represented in the slave select register by a bit.
* The argument passed to this function is the bit mask with a 1 in the bit
* position of the slave being selected. Only one slave can be selected.
*
* The user is not allowed to deselect the slave while a transfer is in progress.
* If no transfer is in progress, the user can select a new slave, which
* implicitly deselects the current slave. In order to explicitly deselect the
* current slave, a zero can be passed in as the argument to the function.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	SlaveMask is a 32-bit mask with a 1 in the bit position of the
*		slave being selected. Only one slave can be selected. The
*		SlaveMask can be zero if the slave is being deselected.
*
* @return
* 		- XST_SUCCESS if the slave is selected or deselected
*		successfully.
*		- XST_DEVICE_BUSY if a transfer is in progress, slave cannot be
*		changed
*		- XST_SPI_TOO_MANY_SLAVES if more than one slave is being
*		selected.
*
* @note
*
* This function only sets the slave which will be selected when a transfer
* occurs. The slave is not selected when the SPI is idle. The slave select
* has no affect when the device is configured as a slave.
*
******************************************************************************/
int XSpi_SetSlaveSelect(XSpi *InstancePtr, u32 SlaveMask)
{
    int NumAsserted;
    int Index;

    //Xil_AssertNonvoid(InstancePtr != NULL);
    if(InstancePtr == NULL){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    //Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY){
        printf("NOT READY %s --- %d \n",__FILE__, __LINE__);
    }

    /*
     * Do not allow the slave select to change while a transfer is in
     * progress.
     * No need to worry about a critical section here since even if the Isr
     * changes the busy flag just after we read it, the function will return
     * busy and the caller can retry when notified that their current
     * transfer is done.
     */
    if (InstancePtr->IsBusy) {
        return XST_DEVICE_BUSY;
    }

    /*
     * Verify that only one bit in the incoming slave mask is set.
     */
    NumAsserted = 0;
    for (Index = (InstancePtr->NumSlaveBits - 1); Index >= 0; Index--) {
        if ((SlaveMask >> Index) & 0x1) {
            /* this bit is asserted */
            NumAsserted++;
        }
    }

    /*
     * Return an error if more than one slave is selected.
     */
    if (NumAsserted > 1) {
        return XST_SPI_TOO_MANY_SLAVES;
    }

    /*
     * A single slave is either being selected or the incoming SlaveMask is
     * zero, which means the slave is being deselected. Setup the value to
     * be  written to the slave select register as the inverse of the slave
     * mask.
     */
    InstancePtr->SlaveSelectReg = ~SlaveMask;

    return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* Transfers the specified data on the SPI bus. If the SPI device is configured
* to be a master, this function initiates bus communication and sends/receives
* the data to/from the selected SPI slave. If the SPI device is configured to
* be a slave, this function prepares the data to be sent/received when selected
* by a master. For every byte sent, a byte is received.
*
* This function/driver operates in interrupt mode and polled mode.
*  - In interrupt mode this function is non-blocking and the transfer is
*    initiated by this function and completed by the interrupt service routine.
*  - In polled mode this function is blocking and the control exits this
*    function only after all the requested data is transferred.
*
* The caller has the option of providing two different buffers for send and
* receive, or one buffer for both send and receive, or no buffer for receive.
* The receive buffer must be at least as big as the send buffer to prevent
* unwanted memory writes. This implies that the byte count passed in as an
* argument must be the smaller of the two buffers if they differ in size.
* Here are some sample usages:
* <pre>
*	XSpi_Transfer(InstancePtr, SendBuf, RecvBuf, ByteCount)
*	The caller wishes to send and receive, and provides two different
*	buffers for send and receive.
*
*	XSpi_Transfer(InstancePtr, SendBuf, NULL, ByteCount)
*	The caller wishes only to send and does not care about the received
*	data. The driver ignores the received data in this case.
*
*	XSpi_Transfer(InstancePtr, SendBuf, SendBuf, ByteCount)
*	The caller wishes to send and receive, but provides the same buffer
*	for doing both. The driver sends the data and overwrites the send
*	buffer with received data as it transfers the data.
*
*	XSpi_Transfer(InstancePtr, RecvBuf, RecvBuf, ByteCount)
*	The caller wishes to only receive and does not care about sending
*	data.  In this case, the caller must still provide a send buffer, but
*	it can be the same as the receive buffer if the caller does not care
*	what it sends. The device must send N bytes of data if it wishes to
*	receive N bytes of data.
* </pre>
* In interrupt mode, though this function takes a buffer as an argument, the
* driver can only transfer a limited number of bytes at time. It transfers only
* one byte at a time if there are no FIFOs, or it can transfer the number of
* bytes up to the size of the FIFO if FIFOs exist.
*  - In interrupt mode a call to this function only starts the transfer, the
*    subsequent transfer of the data is performed by the interrupt service
*    routine until the entire buffer has been transferred.The status callback
*    function is called when the entire buffer has been sent/received.
*  - In polled mode this function is blocking and the control exits this
*    function only after all the requested data is transferred.
*
* As a master, the SetSlaveSelect function must be called prior to this
* function.
*
* @param	InstancePtr is a pointer to the XSpi instance to be worked on.
* @param	SendBufPtr is a pointer to a buffer of data which is to be sent.
*		This buffer must not be NULL.
* @param	RecvBufPtr is a pointer to a buffer which will be filled with
*		received data. This argument can be NULL if the caller does not
*		wish to receive data.
* @param	ByteCount contains the number of bytes to send/receive. The
*		number of bytes received always equals the number of bytes sent.
*
* @return
*		-XST_SUCCESS if the buffers are successfully handed off to the
*		driver for transfer. Otherwise, returns:
*		- XST_DEVICE_IS_STOPPED if the device must be started before
*		transferring data.
*		- XST_DEVICE_BUSY indicates that a data transfer is already in
*		progress. This is determined by the driver.
*		- XST_SPI_NO_SLAVE indicates the device is configured as a
*		master and a slave has not yet been selected.
*
* @notes
*
* This function is not thread-safe.  The higher layer software must ensure that
* no two threads are transferring data on the SPI bus at the same time.
*
******************************************************************************/
int XSpi_Transfer(XSpi *InstancePtr, u8 *SendBufPtr,
                  u8 *RecvBufPtr, unsigned int ByteCount)
{
    u32 ControlReg;
    u32 GlobalIntrReg;
    u32 StatusReg;
    u32 Data = 0;
    u8  DataWidth;

    /*
     * The RecvBufPtr argument can be NULL.
     */
    //Xil_AssertNonvoid(InstancePtr != NULL);
    //Xil_AssertNonvoid(SendBufPtr != NULL);
    //Xil_AssertNonvoid(ByteCount > 0);
    //Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    //Xil_AssertNonvoid(InstancePtr != NULL);
    if(InstancePtr == NULL){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    if(SendBufPtr == NULL){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    if(ByteCount <= 0){
        printf("IS NULL %s --- %d \n",__FILE__, __LINE__);
    }

    //Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
    if(InstancePtr->IsReady != XIL_COMPONENT_IS_READY){
        printf("NOT READY %s --- %d \n",__FILE__, __LINE__);
    }

    if (InstancePtr->IsStarted != XIL_COMPONENT_IS_STARTED) {
        return XST_DEVICE_IS_STOPPED;
    }

    /*
     * Make sure there is not a transfer already in progress. No need to
     * worry about a critical section here. Even if the Isr changes the bus
     * flag just after we read it, a busy error is returned and the caller
     * can retry when it gets the status handler callback indicating the
     * transfer is done.
     */
    if (InstancePtr->IsBusy) {
        return XST_DEVICE_BUSY;
    }

    /*
     * Save the Global Interrupt Enable Register.
     */
    GlobalIntrReg = XSpi_IsIntrGlobalEnabled(InstancePtr);

    /*
     * Enter a critical section from here to the end of the function since
     * state is modified, an interrupt is enabled, and the control register
     * is modified (r/m/w).
     */
    XSpi_IntrGlobalDisable(InstancePtr);

    ControlReg = XSpi_GetControlReg(InstancePtr);

    /*
     * If configured as a master, be sure there is a slave select bit set
     * in the slave select register. If no slaves have been selected, the
     * value of the register will equal the mask.  When the device is in
     * loopback mode, however, no slave selects need be set.
     */
    if (ControlReg & XSP_CR_MASTER_MODE_MASK) {
        if ((ControlReg & XSP_CR_LOOPBACK_MASK) == 0) {
            if (InstancePtr->SlaveSelectReg ==
                InstancePtr->SlaveSelectMask) {
                if (GlobalIntrReg == TRUE) {
                    /* Interrupt Mode of operation */
                    XSpi_IntrGlobalEnable(InstancePtr);
                }
                return XST_SPI_NO_SLAVE;
            }
        }
    }

    /*
     * Set the busy flag, which will be cleared when the transfer
     * is completely done.
     */
    InstancePtr->IsBusy = TRUE;

    /*
     * Set up buffer pointers.
     */
    InstancePtr->SendBufferPtr = SendBufPtr;
    InstancePtr->RecvBufferPtr = RecvBufPtr;

    InstancePtr->RequestedBytes = ByteCount;
    InstancePtr->RemainingBytes = ByteCount;

    DataWidth = InstancePtr->DataWidth;

    /*
     * Fill the DTR/FIFO with as many bytes as it will take (or as many as
     * we have to send). We use the tx full status bit to know if the device
     * can take more data. By doing this, the driver does not need to know
     * the size of the FIFO or that there even is a FIFO. The downside is
     * that the status register must be read each loop iteration.
     */
    StatusReg = XSpi_GetStatusReg(InstancePtr);

    while (((StatusReg & XSP_SR_TX_FULL_MASK) == 0) &&
           (InstancePtr->RemainingBytes > 0)) {
        if (DataWidth == XSP_DATAWIDTH_BYTE) {
            /*
             * Data Transfer Width is Byte (8 bit).
             */
            Data = *InstancePtr->SendBufferPtr;
        } else if (DataWidth == XSP_DATAWIDTH_HALF_WORD) {
            /*
             * Data Transfer Width is Half Word (16 bit).
             */
            Data = *(u16 *)InstancePtr->SendBufferPtr;
        } else if (DataWidth == XSP_DATAWIDTH_WORD){
            /*
             * Data Transfer Width is Word (32 bit).
             */
            Data = *(u32 *)InstancePtr->SendBufferPtr;
        }

        XSpi_WriteReg(InstancePtr->vAddr, XSP_DTR_OFFSET, Data);
        InstancePtr->SendBufferPtr += (DataWidth >> 3);
        InstancePtr->RemainingBytes -= (DataWidth >> 3);
        StatusReg = XSpi_GetStatusReg(InstancePtr);
    }


    /*
     * Set the slave select register to select the device on the SPI before
     * starting the transfer of data.
     */
    XSpi_SetSlaveSelectReg(InstancePtr,
                           InstancePtr->SlaveSelectReg);

    /*
     * Start the transfer by no longer inhibiting the transmitter and
     * enabling the device. For a master, this will in fact start the
     * transfer, but for a slave it only prepares the device for a transfer
     * that must be initiated by a master.
     */
    ControlReg = XSpi_GetControlReg(InstancePtr);
    ControlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
    XSpi_SetControlReg(InstancePtr, ControlReg);

    /*
     * If the interrupts are enabled as indicated by Global Interrupt
     * Enable Register, then enable the transmit empty interrupt to operate
     * in Interrupt mode of operation.
     */
    if (GlobalIntrReg == TRUE) { /* Interrupt Mode of operation */

        /*
         * Enable the transmit empty interrupt, which we use to
         * determine progress on the transmission.
         */
        XSpi_IntrEnable(InstancePtr, XSP_INTR_TX_EMPTY_MASK);

        /*
         * End critical section.
         */
        XSpi_IntrGlobalEnable(InstancePtr);

    } else { /* Polled mode of operation */

        /*
         * If interrupts are not enabled, poll the status register to
         * Transmit/Receive SPI data.
         */
        while(ByteCount > 0) {

            /*
             * Wait for the transfer to be done by polling the
             * Transmit empty status bit
             */
            do {
                StatusReg = XSpi_IntrGetStatus(InstancePtr);
            } while ((StatusReg & XSP_INTR_TX_EMPTY_MASK) == 0);

            XSpi_IntrClear(InstancePtr,XSP_INTR_TX_EMPTY_MASK);

            /*
             * A transmit has just completed. Process received data
             * and check for more data to transmit. Always inhibit
             * the transmitter while the transmit register/FIFO is
             * being filled, or make sure it is stopped if we're
             * done.
             */
            ControlReg = XSpi_GetControlReg(InstancePtr);
            XSpi_SetControlReg(InstancePtr, ControlReg |
                                            XSP_CR_TRANS_INHIBIT_MASK);

            /*
             * First get the data received as a result of the
             * transmit that just completed. We get all the data
             * available by reading the status register to determine
             * when the Receive register/FIFO is empty. Always get
             * the received data, but only fill the receive
             * buffer if it points to something (the upper layer
             * software may not care to receive data).
             */
            StatusReg = XSpi_GetStatusReg(InstancePtr);

            while ((StatusReg & XSP_SR_RX_EMPTY_MASK) == 0) {

                Data = XSpi_ReadReg(InstancePtr->vAddr,
                                    XSP_DRR_OFFSET);
                if (DataWidth == XSP_DATAWIDTH_BYTE) {
                    /*
                     * Data Transfer Width is Byte (8 bit).
                     */
                    if(InstancePtr->RecvBufferPtr != NULL) {
                        *InstancePtr->RecvBufferPtr++ =
                                (u8)Data;
                    }
                } else if (DataWidth ==
                           XSP_DATAWIDTH_HALF_WORD) {
                    /*
                     * Data Transfer Width is Half Word
                     * (16 bit).
                     */
                    if (InstancePtr->RecvBufferPtr != NULL){
                        *(u16 *)InstancePtr->RecvBufferPtr =
                                (u16)Data;
                        InstancePtr->RecvBufferPtr += 2;
                    }
                } else if (DataWidth == XSP_DATAWIDTH_WORD) {
                    /*
                     * Data Transfer Width is Word (32 bit).
                     */
                    if (InstancePtr->RecvBufferPtr != NULL){
                        *(u32 *)InstancePtr->RecvBufferPtr =
                                Data;
                        InstancePtr->RecvBufferPtr += 4;
                    }
                }
                InstancePtr->Stats.BytesTransferred +=
                        (DataWidth >> 3);
                ByteCount -= (DataWidth >> 3);
                StatusReg = XSpi_GetStatusReg(InstancePtr);
            }

            if (InstancePtr->RemainingBytes > 0) {

                /*
                 * Fill the DTR/FIFO with as many bytes as it
                 * will take (or as many as we have to send).
                 * We use the Tx full status bit to know if the
                 * device can take more data.
                 * By doing this, the driver does not need to
                 * know the size of the FIFO or that there even
                 * is a FIFO.
                 * The downside is that the status must be read
                 * each loop iteration.
                 */
                StatusReg = XSpi_GetStatusReg(InstancePtr);

                while(((StatusReg & XSP_SR_TX_FULL_MASK)== 0) &&
                      (InstancePtr->RemainingBytes > 0)) {
                    if (DataWidth == XSP_DATAWIDTH_BYTE) {
                        /*
                         * Data Transfer Width is Byte
                         * (8 bit).
                         */
                        Data = *InstancePtr->
                                SendBufferPtr;

                    } else if (DataWidth ==
                               XSP_DATAWIDTH_HALF_WORD) {

                        /*
                         * Data Transfer Width is Half
                         * Word (16 bit).
                          */
                        Data = *(u16 *)InstancePtr->
                                SendBufferPtr;
                    } else if (DataWidth ==
                               XSP_DATAWIDTH_WORD) {
                        /*
                         * Data Transfer Width is Word
                         * (32 bit).
                          */
                        Data = *(u32 *)InstancePtr->
                                SendBufferPtr;
                    }
                    XSpi_WriteReg(InstancePtr->vAddr,
                                  XSP_DTR_OFFSET, Data);
                    InstancePtr->SendBufferPtr +=
                            (DataWidth >> 3);
                    InstancePtr->RemainingBytes -=
                            (DataWidth >> 3);
                    StatusReg = XSpi_GetStatusReg(
                            InstancePtr);
                }

                /*
                 * Start the transfer by not inhibiting the
                 * transmitter any longer.
                 */
                ControlReg = XSpi_GetControlReg(InstancePtr);
                ControlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
                XSpi_SetControlReg(InstancePtr, ControlReg);
            }
        }

        /*
         * Stop the transfer (hold off automatic sending) by inhibiting
         * the transmitter.
         */
        ControlReg = XSpi_GetControlReg(InstancePtr);
        XSpi_SetControlReg(InstancePtr,
                           ControlReg | XSP_CR_TRANS_INHIBIT_MASK);

        /*
         * Select the slave on the SPI bus when the transfer is
         * complete, this is necessary for some SPI devices,
         * such as serial EEPROMs work correctly as chip enable
         * may be connected to slave select
         */
        XSpi_SetSlaveSelectReg(InstancePtr,
                               InstancePtr->SlaveSelectMask);
        InstancePtr->IsBusy = FALSE;
    }

    return XST_SUCCESS;
}



