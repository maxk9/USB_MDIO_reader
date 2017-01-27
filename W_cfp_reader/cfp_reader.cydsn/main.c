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


#define USBFS_DEVICE    (0u)
/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
* endpoints.
*/
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH     (20u)

#define BASE_MDIO_ADDRESS       0x8000  /* Start address for the incremental address variable */


/* MDIO Host Variables */
uint16 HostAddress    = BASE_MDIO_ADDRESS;    /* Address data for the MDIO frames */
uint16 HostData       = 0x0000;               /* Read data for the MDIO frames */

/* Enable/Disable the MDIO Host */
/* This might be disable in case an external MDIO Host is used */
#define MDIO_HOST_ENABLE 1

#define RANDOM_CONST            0xDEAD  /* Random value to generate write/read data */
/* State Constants */
#define STATE_WRITE 0
#define STATE_READ  1

//static   uint16 myCounter = 0;          /* Global counter incremented in main loop */
//static   uint8  myState = STATE_READ;  /* State variable. Default state is WRITE */
/* Device Address of MDIO Interface (constant) */
static const uint8  MdioDevAddr = 0x01; 
static const uint8  MdioPhyAddr = 0x1f;



int main(void)
{
    CYBIT  first_enter = 0;
    CYBIT test=0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    uint16 count;
    uint8 buffer[USBUART_BUFFER_SIZE];
    

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
    PWM_1_Start();
    PWM_2_Start();
   LCD_PrintString("TEST");
    LCD_Position(1u, 0u);
    LCD_PrintString("MDIO host");
//   ISR_Start();
    
    for(;;)
    {
      //  CyDelay(2000);
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
                    USBUART_1_PutData(buffer, count);

                    /* Send address frame */
MDIO_host_2_WriteDataC45( MDIO_host_2_C45_ADDR, MdioPhyAddr, MdioDevAddr, HostAddress );
                       	/* Start Transmission */
//                    if(test)
//                    {
// //  	MDIO_host_2_CONTROL_REG |= MDIO_host_2_START;
//                    }
//                    else
//                    {
// //  MDIO_host_2_CONTROL_REG &= ~MDIO_host_2_START;
//                    }
//                    test =~test;
//                    LED_3_Write(test);
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
                /* Output on LCD Line Coding settings. */
                if (0u != (state & USBUART_1_LINE_CODING_CHANGED))
                {                  
                    /* Get string to output. */
//                    sprintf(lineStr,"BR:%4ld %d%c%s", USBUART_1_GetDTERate(),\
//                                   (uint16) USBUART_1_GetDataBits(),\
//                                   parity[(uint16) USBUART_1_GetParityType()][0],\
//                                   stop[(uint16) USBUART_1_GetCharFormat()]);

//                    /* Clear LCD line. */
//                    LCD_Position(0u, 0u);
//                    LCD_PrintString("                    ");
//
//                    /* Output string on LCD. */
//                    LCD_Position(0u, 0u);
//                    LCD_PrintString(lineStr);
                }

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

/* [] END OF FILE */
