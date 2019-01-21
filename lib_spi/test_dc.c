//
// Created by lorenzo on 18/01/19.
//

#include "xspi.h"
#include <stdio.h>
#include "utils.h"
#include "dma_engine_buf.h"
//#include "xhw_internals.h"


#define CONFIG_WRITE_BYTES 17
#define CONFIG_READ_BYTES 17
#define CONFIG_DATA_BYTES 25
#define REFCLK_DIVISIONS 1048575 // 2^20 -1
#define REFID_BITS 24
#define STOP_BITS 20
#define TEST_PATTERN 0 // SET 1 if using LVDS TEST PATTERN
#define NUM_SAMPLES (2020000 * 2)
#define NUM_BUFFERS 1
#define BUFSIZE 100 * 8 // in BYTE


u8 TdcGpx2PowerOnReset(XSpi *InstancePtr)
{
    int ok;
    u8 sendBuffer[1] = {0x30};
    u8 recvBuffer[1] = {0x02};
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    //if(ok == XST_SUCCESS) print("Select ok\n\r");
    //sleep(1);
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    ok = XSpi_Transfer(InstancePtr, sendBuffer, recvBuffer, 1);
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x0);
    if(ok != XST_SUCCESS)
    {
        printf("Error %d\n\r", ok);
    }
    else
    {
        printf("TDC Reset done ");
        printf("return %02hhX\n\r", recvBuffer[0]);
    }
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x0);
    /*if(ok == XST_SUCCESS)
    	print("deselect ok\n\r");
    else
    	print("deselect not done\n\r");*/
    return recvBuffer[0];
}

u8 TdcGpx2WriteConfig(XSpi *InstancePtr)
{
    int ok;
    //SPI CONFIG
    u8 TEST_LVDS = TEST_PATTERN ? 0xD0 : 0xC0; // 0xC0 normal functioning
    //u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0x31, 0xFA, 0xF1, 0x84, 0x8E, 0x10, 0x0C, 0x3D, 0x1A, 0x31, 0x00, 0xA0, 0xCC, 0xCC, 0x1F, 0xD7, 0x00}; // SPI Mode
//	u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0x13, 0xAF, 0x1F, 0x48, 0xE8, 0x01, 0xC0, 0xD3, 0xA1, 0x13, 0x00, 0x0A, 0xCC, 0xCC, 0xF1, 0x7D, 0x00}; // SPI Mode
//	u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0x31, 0xAF, 0x1D, 0x48, 0xE8, 0x01, 0xD0, 0xD3, 0xA1, 0x13, 0x00, 0x0A, 0xCC, 0xCC, 0xF1, 0x7D, 0x00}; //TUCCIO
    u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0x31, 0xAF, 0x1D, 0x48, 0xE8, 0x01, TEST_LVDS, 0xD3, 0xA1, 0x13, 0x00, 0x0A, 0xCC, 0xCC, 0xF1, 0x7D, 0x00}; //TUCCIO
    //u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0xFF, 0x8F, 0x1F, 0x48, 0xE8, 0x01, 0x53, 0xD3, 0xA1, 0x13, 0x00, 0x0A, 0xCC, 0xCC, 0x1F, 0xD7, 0x00}; // TEST CONFIG TO BE CHECKED
//	u8 sendBuffer[CONFIG_WRITE_BYTES+1] = {0x80, 0x31, 0x01, 0x1F, 0x40, 0x0D, 0x03, 0xC0, 0x53, 0xA1, 0x13, 0x00, 0x0A, 0xCC, 0XCC, 0xF1, 0xD3, 0x00}; // LVDS CONFIG
    u8 sendCommand[1] = {0x80};
    u8 recvCommand[1] = {0x02};
    u8 recvBuffer[CONFIG_WRITE_BYTES+1];
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    //if(ok == XST_SUCCESS) print("Select ok\n\r");

    // Send Write config command
    //ok = XSpi_Transfer(InstancePtr, sendCommand, recvCommand, 1);
    if(ok != XST_SUCCESS)
    {
        printf("Error %d\n\r", ok);
    }
    else
    {
        printf("Write config command done ");
        printf("return %x\n\r", recvCommand[0]);
        // Send configuration
        ok = XSpi_SetSlaveSelect(InstancePtr,0x1);
        ok = XSpi_Transfer(InstancePtr, sendBuffer, recvBuffer, CONFIG_WRITE_BYTES+1);
        ok = XSpi_SetSlaveSelect(InstancePtr,0x0);
        if(ok != XST_SUCCESS)
        {
            printf("Error write config %d\n\r", ok);
        }
        else
        {
            printf("Write config buffer \n\r");
            for(int i=0; i<CONFIG_WRITE_BYTES+1; i++)
            {
                printf("0x%02hhX ", recvBuffer[i]);
            }
            printf("\n\r");
        }
    }

    // Deselect TDC
    ok = XSpi_SetSlaveSelect(InstancePtr,0x0);
    /*if(ok == XST_SUCCESS)
    	print("deselect ok\n\r");
    else
    	print("deselect not done\n\r");*/
    return recvBuffer[0];
}

u8 TdcGpx2ReadConfig(XSpi *InstancePtr)
{
    int ok;
    u8 sendBuffer[CONFIG_READ_BYTES+1] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u8 sendCommand[1] = {0x40};
    u8 recvCommand[1] = {0x02};
    u8 recvBuffer[CONFIG_READ_BYTES+1] = {0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    //if(ok == XST_SUCCESS) print("Select ok\n\r");

    // Send Write config command
    //ok = XSpi_Transfer(InstancePtr, sendCommand, recvCommand, 1);
    if(ok != XST_SUCCESS)
    {
        printf("Error %d\n\r", ok);
    }
    else
    {
        printf("Read config command done ");
        printf("return %x\n\r", recvCommand[0]);
        // Send configuration
        ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
        ok = XSpi_Transfer(InstancePtr, sendBuffer, recvBuffer, CONFIG_READ_BYTES+1);
        ok = XSpi_SetSlaveSelect(InstancePtr,0x0);
        if(ok != XST_SUCCESS)
        {
            printf("Error read config %d\n\r", ok);
        }
        else
        {
            printf("Read config buffer \n\r");
            for(int i=0; i<CONFIG_READ_BYTES+1; i++)
            {
                printf("0x%02hhX ", recvBuffer[i]);
            }
            printf("\n\r");
            fflush(stdout);
        }
    }

    // Deselect TDC
    ok = XSpi_SetSlaveSelect(InstancePtr,0x0);
    /*if(ok == XST_SUCCESS)
    	print("deselect ok\n\r");
    else
    	print("deselect not done\n\r");*/
    return recvBuffer[0];
}

u8 TdcGpx2InitStartMeas(XSpi *InstancePtr)
{
    int ok;
    u8 sendBuffer[1] = {0x18};
    u8 recvBuffer[1] = {0x02};
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    //if(ok == XST_SUCCESS) print("Select ok\n\r");
    //sleep(1);
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x1);
    ok = XSpi_Transfer(InstancePtr, sendBuffer, recvBuffer, 1);
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x0);
    if(ok != XST_SUCCESS)
    {
        printf("Error %d\n\r", ok);
    }
    else
    {
        printf("TDC Init done ");
        printf("return %x\n\r", recvBuffer[0]);
    }
    ok = XSpi_SetSlaveSelect(InstancePtr, 0x0);
    /*if(ok == XST_SUCCESS)
    	print("deselect ok\n\r");
    else
    	print("deselect not done\n\r");*/
    return recvBuffer[0];
}



int main(int argc, char *argv[]){
    XSpi_Config *spi_config;
    XSpi Spi_Instance;
    int status = 0;

    /****** DMA vars *********/
    u64 buffer_out[100] = {0}; //todo change here
    unsigned long sizes[NUM_BUFFERS] = {BUFSIZE};
    struct udmabuf buffers[NUM_BUFFERS];
    struct dma_engine engine;

    enum dma_err_status err_retval;

    load_udma_buffers(NUM_BUFFERS, sizes, buffers);
    printf("DMA buffers created\n");

    printf("Test SPI program \n");
    spi_config = XSpi_LookupConfig(XPAR_AXI_QUAD_SPI_0_DEVICE_ID);
    printf("Configuration Looked Up \n");

    status = XSpi_CfgInitialize(&Spi_Instance, spi_config, spi_config->BaseAddress);
    if(status != XST_SUCCESS)
    {
        printf("Failed to init SPI. Error %d, trying to reset/restart again...\n\r", status);
        XSpi_Reset(&Spi_Instance);

        status = XSpi_CfgInitialize(&Spi_Instance, spi_config, spi_config->BaseAddress);
        if(status != XST_SUCCESS){
            printf("Failed to STOP/Restart SPI \n\r");
            fflush(stdout);
            return 1;
        }else{
            printf("SUCCESS!\n\r");
            fflush(stdout);
        }

    }

    status = XSpi_SetOptions(&Spi_Instance, XSP_MASTER_OPTION | XSP_MANUAL_SSELECT_OPTION | XSP_CLK_ACTIVE_LOW_OPTION);
    if(status != XST_SUCCESS){
        printf("Failed to set SPI \n");
    }

    // Start SPI
    XSpi_Start(&Spi_Instance);

    // Disable interrupt
    XSpi_IntrGlobalDisable(&Spi_Instance);

    // Reset TDC
    TdcGpx2PowerOnReset(&Spi_Instance);

    // Config TDC
    TdcGpx2WriteConfig(&Spi_Instance);
    printf("\n\r");
    TdcGpx2ReadConfig(&Spi_Instance);

    // Init measurement
    TdcGpx2InitStartMeas(&Spi_Instance);


    print_buffer_status(0, buffers);
    get_dma_interfaces(1, NULL, NULL, &engine);
    printf("DMA engine created\n");
    long int * b1 = (int *)buffers->vaddr;
    for(int i = 0; i < BUFSIZE / sizeof(long int); i++){
        b1[i] = 0;
    }

    /*
     * initiate DMA transaction from device
     */
    printf("\nstarting transfer from device...\n");
    err_retval = set_simple_transfer_from_device(&engine, buffers , 0, BUFSIZE);
    check_err(err_retval);
    err_retval = start_simple_transfer_from_device(&engine);
    check_err(err_retval);
    printf("transfer from device started\n");

    printf("\nwaiting for transfer from device...\n");
    err_retval = wait_simple_transfer_from_device(&engine, 0);
    check_err(err_retval);
    print_engine(&engine);

    //print results

    for(int i = 0; i < BUFSIZE/sizeof(long int); i++){
        printf("data read %lu \n", b1[i]);
    }


    destroy_dma_interfaces(1, &engine);
    unload_udma_buffers(NUM_BUFFERS, buffers);
    clean(&Spi_Instance);
}