/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"



/* Firmware Constants */
#define SPECIAL_COR_REGISTER    0xA000  /* Special COR Register address */
#define SPECIAL_COR_MODIFIER    0xA200  /* Special address to modify the COR Register */
#define BASE_MDIO_ADDRESS       0x8201  /* Start address for the incremental address variable */
#define MAX_MDIO_ADDRESS        0xA500  /* Maximum address supported in this firmware */
#define INC_ADDRESS             0x0100  /* Incremental value for the adddress */
#define RANDOM_CONST            0xDEAD  /* Random value to generate write/read data */
#define ENABLE_MDIO             0x20    /* Enable bit for the MDIO Component */


#define USBFS_DEVICE    (0u)
/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)

volatile uint16 myAddress = 0;          /* Current address variable */
         uint16 myData = 0;             /* Register value in current address */
volatile uint8 dataFlag = 0;           /* Internal data flag for ISR*/
volatile uint8 corFlag = 0;            /* Internal COR flag for ISR */
static   uint8 status = 0;                 /* Status of API calls */

/* MDIO Host Variables */
uint16 HostAddress    = BASE_MDIO_ADDRESS;    /* Address data for the MDIO frames */
uint16 HostData       = 0x0000;               /* Read data for the MDIO frames */

/* Enable/Disable the MDIO Host */
/* This might be disable in case an external MDIO Host is used */
#define MDIO_HOST_ENABLE 1

/* State Constants */
#define STATE_WRITE 0
#define STATE_READ  1

//static   uint16 myCounter = 0;          /* Global counter incremented in main loop */
//static   uint8  myState = STATE_READ;  /* State variable. Default state is WRITE */
/* Device Address of MDIO Interface (constant) */
static const uint8  MdioDevAddr = 0x01; //PMA/PMD
static const uint8  MdioPhyAddr = 0x04;
/* Local prototypes */
void fillUpReadOnlyRegisters(void);

/* Internal Status Register */
extern uint8  MDIO_host_2_StatusRegister;

uint16 read_MDIO_data=0;



/* Interrupt Prototypes */
CY_ISR_PROTO(DAT_ISR_Handler);
CY_ISR_PROTO(ADDR_ISR_Handler);
CY_ISR_PROTO(COR_ISR_Handler);



int main(void)
{
    CYBIT  first_enter = 0;
    CYBIT test=0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    uint16 count;
    uint8 buffer[USBUART_BUFFER_SIZE], strk[10];
    

    uint8 state;
    //char8 lineStr[LINE_STR_LENGTH];
    
    LCD_Start();

    VDAC8_1_Start();
    Opamp_1_Start();
    CyGlobalIntEnable;

    
     /* Start USBFS operation with 5-V operation. */
    USBUART_1_Start(USBFS_DEVICE, USBUART_1_5V_OPERATION);
    
    #if MDIO_HOST_ENABLE    
    MDIO_host_2_Init(); 	
    #endif
    
   /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Clock_2_Enable();
    Clock_1_Enable();
    Clock_3_Enable();
    PWM_1_Start();
    PWM_2_Start();
//   LCD_PrintString("TEST");
//    LCD_Position(1u, 0u);
//    LCD_PrintString("MDIO host");
     ISR_Start();
    
    /* Start MDIO Components */
    MDIO_Interface_Start();


    /* Start ISRs */
    DAT_ISR_StartEx (DAT_ISR_Handler);
    ADDR_ISR_StartEx(ADDR_ISR_Handler);
    COR_ISR_StartEx (COR_ISR_Handler);

    /* Set read only registers */
    fillUpReadOnlyRegisters();
    
    
    /* Display Information on LCD*/
    LCD_ClearDisplay(); 
    LCD_PrintString("A:0000  T:(N/A)");
    LCD_Position(1,0);
	LCD_PrintString("R:0000");
    
    for(;;)
    {
        /* Get the data from the current address */
        status = MDIO_Interface_GetData(myAddress, &myData, 1);
    
        if (status)
        {   /* Invalid data or address not supported */
            myData = 0x0000;
        }  
        
        
        /* Print info on LCD [REG: XXXX] */
        LCD_Position(1,2);
        LCD_PrintInt16(myData);     
    
        /* Print info about access type [T:(XXX)] */
        LCD_Position(0,11);
        
        /* CFP NVR/Vendor NVR Register Space */
        if ((myAddress & ~(MDIO_Interface_REG_PAGE_1_SIZE-1)) == MDIO_Interface_CFP_NVR_START ||
            (myAddress & ~(MDIO_Interface_REG_PAGE_2_SIZE-1)) == MDIO_Interface_VENDOR_NVR_START)
        {
            LCD_PrintString("RO) ");
        }
        /* Other Register spaces */
        else if ((myAddress & ~(MDIO_Interface_REG_PAGE_3_SIZE-1)) == MDIO_Interface_USER_NVR_START ||
                 (myAddress & ~(MDIO_Interface_REG_PAGE_4_SIZE-1)) == MDIO_Interface_CFP_MODULE_VR_START ||
                 (myAddress & ~(MDIO_Interface_REG_PAGE_5_SIZE-1)) == MDIO_Interface_NETWORK_LANE_VR_START ||
                 (myAddress & ~(MDIO_Interface_REG_PAGE_6_SIZE-1)) == MDIO_Interface_HOST_LANE_VR_START)
        {
            /* Special COR case */
            if (myAddress == SPECIAL_COR_REGISTER)
            {
                LCD_PrintString("COR)");
            }
            else
            {
                LCD_PrintString("R/W)");
            }
        }
        else /* Not Available Register */
        {
            LCD_PrintString("N/A)");
        }
        
        
        
        /* Host can send double SET_INTERFACE request. */
        if (0u != USBUART_1_IsConfigurationChanged())
        {
            /* Initialize IN endpoints when device is configured. */
            if (0u != USBUART_1_GetConfiguration())
            {
                /* Enumeration is done, enable OUT endpoint to receive data 
                 * from host. */
                USBUART_1_CDC_Init();
            }
        }

        /* Service USB CDC when device is configured. */
        if (0u != USBUART_1_GetConfiguration())
        {
            /* Check for input data from host. */
            if (0u != USBUART_1_DataIsReady())
            {
                /* Read received data and re-enable OUT endpoint. */
                count = USBUART_1_GetAll(buffer);

                if (0u != count)
                {
                    /* Wait until component is ready to send data to host. */
                    while (0u == USBUART_1_CDCIsReady())
                    {
                    }
                    
                    /* Send data back to host. */
   //                 USBUART_1_PutData( buffer, count);
                    if(buffer[0] == 'a')/* Send address frame */                    
                    {
                         USBUART_1_PutString("SET MDIO address: 0x8000\n\r");
                        MDIO_host_2_SetAddrC45( MdioPhyAddr, MdioDevAddr, 0x8000u );
//                        LCD_Position(0,0);
//                        LCD_PrintString("ADR:");
//                        LCD_PrintInt16(BASE_MDIO_ADDRESS);
                    }
                    else
                    if(buffer[0] == 'q')/* Send address frame */                    
                    {
                         USBUART_1_PutString("SET MDIO address: 0x8201\n\r");
                        MDIO_host_2_SetAddrC45( MdioPhyAddr, MdioDevAddr, 0x8201u );
//                        LCD_Position(0,0);
//                        LCD_PrintString("ADR:");
//                        LCD_PrintInt16(BASE_MDIO_ADDRESS);
                    }
                    else
                    if(buffer[0] == 'w')/* Send address frame */                    
                    {
                         USBUART_1_PutString("Read MDIO:");
                        MDIO_host_2_ReadDataC45( MdioPhyAddr, MdioDevAddr, &read_MDIO_data );
//                        LCD_Position(1,0);
//                        LCD_PrintString("RD: ");
//                        LCD_PrintInt16(read_MDIO_data);
                        sprintf((char *)strk,"0x%04X\n\r",read_MDIO_data);
                         USBUART_1_PutString((const char *)strk);
                    }
                    else
                    {
                         USBUART_1_PutString("unknown data (only 'q', 'w' implemented)\n\r");
                    }
                    /* If the last sent packet is exactly the maximum packet 
                    *  size, it is followed by a zero-length packet to assure
                    *  that the end of the segment is properly identified by 
                    *  the terminal.
                    */
                    if (USBUART_BUFFER_SIZE == count)
                    {
                        /* Wait until component is ready to send data to PC. */
                        while (0u == USBUART_1_CDCIsReady())
                        {
                        }

                        /* Send zero-length packet to PC. */
                        USBUART_1_PutData(NULL, 0u);
                    }
                }
            }


            /* Check for Line settings change. */
            state = USBUART_1_IsLineChanged();
            if (0u != state)
            {

                /* Output on LCD Line Control settings. */
                if (0u != (state & USBUART_1_LINE_CONTROL_CHANGED))
                {                   
                    /* Get string to output. */
                    state = USBUART_1_GetLineControl();
                    
                    if(0u != (state & USBUART_1_LINE_CONTROL_DTR))//connect terminal
                    {
                       if(!first_enter)
                        {
                          first_enter = 1;  
                          USBUART_1_PutString("\r\nHello from Cypress!\r\n");
                        } 
                    }
                    else    //disconnect terminal
                    {
                        first_enter = 0;
                    }
                    
//                    sprintf(lineStr,"DTR:%s,RTS:%s",  (0u != (state & USBUART_1_LINE_CONTROL_DTR)) ? "ON" : "OFF",
//                                                      (0u != (state & USBUART_1_LINE_CONTROL_RTS)) ? "ON" : "OFF");
//
//                    /* Clear LCD line. */
//                    LCD_Position(1u, 0u);
//                    LCD_PrintString("                    ");
//
//                    /* Output string on LCD. */
//                    LCD_Position(1u, 0u);
//                    LCD_PrintString(lineStr);
                }
            }
        }
        /******* ISR Flags instructions ********/

        /* If flag set, update special COR register */
        if (dataFlag)
        {
            /* Clear flag */
            dataFlag = 0;          
            
            /* Add condition to set special COR register */
            if (myAddress == SPECIAL_COR_MODIFIER)
            {
                /* Get data from current address */
                status = MDIO_Interface_GetData(myAddress, &myData, 1);
            
                /* Write in special COR register with current data */
                MDIO_Interface_SetBits(SPECIAL_COR_REGISTER, myData);
            }
        }
          
        /* If COR is detected, wait a moment to update value on LCD */
        if (corFlag)
        {
            /* Clear Flag */
            corFlag = 0;

            CyDelay(250);
        }
                    
        /* Print current address */
        LCD_Position(0,2);
        LCD_PrintInt16(myAddress);
        /******* Host Handler Frames ********/
//#if MDIO_HOST_ENABLE
//        /* Send write frames in this state */
//        if (myState == STATE_WRITE )
//        {
//            /* Send address frame */
//            MDIO_Host_1_WriteDataC45( MDIO_Host_1_C45_ADDR, MdioPhyAddr, MdioDevAddr, HostAddress );
//            
//            CyDelay(1);
//            
//            /* Generate random data periodically */
//            if ((myCounter % 200) == 0)
//            {
//                HostData = (HostData + RANDOM_CONST)*(myCounter + 1);
//            }
//            
//            /* Send write frame */
//            MDIO_Host_1_WriteDataC45(MDIO_Host_1_C45_WRITE, MdioPhyAddr, MdioDevAddr, HostData);
//            
//            /* Print LCD info [WR:XXXX] */
//            LCD_Position(0,4);
//            LCD_PrintInt16(HostAddress);
//            LCD_PrintString(" WR:");
//            LCD_PrintInt16(HostData);
//            
//        }
//        else /* Send read frames in this state */
//        {
//            /* Send address frame */
//            MDIO_Host_1_WriteDataC45(MDIO_Host_1_C45_ADDR, MdioPhyAddr, MdioDevAddr, HostAddress);
//            
//            CyDelay(1);
//            
//            /* Send read frame */
//            MDIO_Host_1_ReadDataC45(MDIO_Host_1_C45_READ, MdioPhyAddr, MdioDevAddr, &HostData);
//            
//            /* Print LCD info [@:XXXX RD:XXXX]*/
//            LCD_Position(1,4);
//            LCD_PrintInt16(HostAddress);
//            LCD_PrintString(" RD:");
//            LCD_PrintInt16(HostData);
//        }
//        
//#else
//        /* Print current address */
//        LCD_Position(0,12);
//        LCD_PrintInt16(myAddress);
//#endif
        
       // myState = !myState;
    }
}

/*******************************************************************************
* Function Name: fillUpReadOnlyRegisters
********************************************************************************
* Summary:
*  Fill all the the read only registers from the CFP Register Allocation table.
*  The values are generated based on the initial RANDOM_CONST
* 
* Parameters:
*  None
*
* Return:
*  None
*
********************************************************************************/
void fillUpReadOnlyRegisters(void)
{
    uint16 index;
    uint16 randomData = RANDOM_CONST;
    
    /* Fill up CFP NVR Register set */
    for (index=0; index < (MDIO_Interface_REG_PAGE_1_SIZE-1); index+=2)
    {
        MDIO_Interface_SetData(MDIO_Interface_CFP_NVR_START+index, &randomData, 1);    
        randomData = randomData + index;
        CyDelayUs(10);
    }
    
    /* Fill up Vendor NVR Register set */
    for (index=0; index < (MDIO_Interface_REG_PAGE_2_SIZE-1); index+=2)
    {
        MDIO_Interface_SetData(MDIO_Interface_VENDOR_NVR_START+index, &randomData, 1);    
        randomData = randomData + index;
        CyDelayUs(10);
    }   
}

/*******************************************************************************
* Function Name: DAT_ISR_Handler
********************************************************************************
* Summary:
*  The interrupt handler for DAT_ISR. It only sets an internal data flag.
* 
* Parameters:
*  None
*
* Return:
*  None
*
********************************************************************************/
CY_ISR(DAT_ISR_Handler)
{
    dataFlag = 1;
}

/*******************************************************************************
* Function Name: ADR_ISR_Handler
********************************************************************************
* Summary:
*  The interrupt handler for ADR_ISR. It updates the current address.
* 
* Parameters:
*  None
*
* Return:
*  None
*
********************************************************************************/
CY_ISR(ADDR_ISR_Handler)
{
    /* Get Current address */
    myAddress = MDIO_Interface_GetAddress(); 
}

/*******************************************************************************
* Function Name: COR_ISR_Handler
********************************************************************************
* Summary:
*  Interrupt handler for COR_ISR. It only sets an internal COR flag.
* 
* Parameters:
*  None
*
* Return:
*  None
*
********************************************************************************/
CY_ISR(COR_ISR_Handler)
{
    corFlag = 1;
}



/* [] END OF FILE */
