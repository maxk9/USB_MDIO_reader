/* ========================================
 *
 * Copyright FiberTreid, 2017
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

//#include <device.h>
#include <project.h>
#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "`$INSTANCE_NAME`_my.h"

/*******************************************************************************
* Variables
********************************************************************************/
uint8 `$INSTANCE_NAME`_initVar = 0u;

/* Data received MDIO Host */
volatile uint16 `$INSTANCE_NAME`_data_bits=0u;	 

/* Internal Status Register */
volatile uint8  `$INSTANCE_NAME`_StatusRegister=0u;

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
* Summary:
*  Initializes the MDIO Host component.
* 
* Parameters:
*  None
*
* Return:
*  None
* 
* Side Effects:
*
********************************************************************************/
void `$INSTANCE_NAME`_Start(void)
{ 
    /* If not already initialized, then initialize hardware and software */
    if(`$INSTANCE_NAME`_initVar == 0u)
    {
        `$INSTANCE_NAME`_Init();
        `$INSTANCE_NAME`_initVar = 1u;
    }
    `$INSTANCE_NAME`_Enable();
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
* Summary:
*  Disable the MDIO Host component and the internal interrupt
* 
* Parameters:
*  None
* 
* Return:
*  None
* 
* Side Effects:
*  
********************************************************************************/
void `$INSTANCE_NAME`_Stop(void)
{
	/* Disable the hardware component */
    `$INSTANCE_NAME`_CONTROL_REG = 0u;	// Set Enable and Valid bits to zero
	
	/* Disable Count7 hardware block */
    `$INSTANCE_NAME`_CNT7_AUX_CTL_REG &= ~`$INSTANCE_NAME`_COUNT7_EN;
	
	/* Disable Interrupt */
//	`$INSTANCE_NAME`_DisableInt();
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Init
********************************************************************************
* Summary:
*  Initializes internal variables and resets the datapath FIFO.
* 
* Parameters:
*  None
*
* Return:
*  None
* 
* Side Effects:
*  
********************************************************************************/
void `$INSTANCE_NAME`_Init(void)
{ 	
    `$INSTANCE_NAME`_Enable();
	
	/* Reset Internal FIFOs */
	`$INSTANCE_NAME`_ClearInternalFIFOs();

    /* Initialize interrupts */
//	`$INSTANCE_NAME`_EnableInt();
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Enable()
********************************************************************************
* Summary:
*  Enables the hardware and internal interrupt.
* 
* Parameters:
*  None
*
* Return:
*  None
* 
* Side Effects:
*  None
********************************************************************************/
void `$INSTANCE_NAME`_Enable(void) `=ReentrantKeil($INSTANCE_NAME . "_Enable")`
{
	/* Enable Count7 hardware block */
    `$INSTANCE_NAME`_CNT7_AUX_CTL_REG |= `$INSTANCE_NAME`_COUNT7_EN;

	/* Enable the block */
    `$INSTANCE_NAME`_CONTROL_REG = `$INSTANCE_NAME`_ENABLE;
	
    /*Reset counter */
   // `$INSTANCE_NAME`_PERIOD_REG = 127;
    `$INSTANCE_NAME`_COUNT_REG = 0;
    
	/* Enable the interrupt */
	//CyIntEnable(`$INSTANCE_NAME`_ISR_NUMBER);   
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_EnableInt
********************************************************************************
* Summary:
*  Enables and configure the interrupt from this component. 
* 
* Parameters:
*  None
* 
* Return:
*  None
* 
* Side Effects:
*  None
********************************************************************************/
//void `$INSTANCE_NAME`_EnableInt(void)
//{   
//    /* Enable interrupt handler service routine */
//	CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);
//	CyIntSetVector(`$INSTANCE_NAME`_ISR_NUMBER, `$INSTANCE_NAME`_ISR_InterruptHandler);
//	CyIntSetPriority(`$INSTANCE_NAME`_ISR_NUMBER, `$INSTANCE_NAME`_ISR_PRIORITY);
//	CyIntEnable(`$INSTANCE_NAME`_ISR_NUMBER);        
//}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_DisableInt
********************************************************************************
* Summary:
*  Disables interrupts from this component
*
* Parameters:
*  None
* 
* Return:
*  None
* 
* Side Effects:
*  None
********************************************************************************/
//void `$INSTANCE_NAME`_DisableInt(void)
//{  
//    /* Disable interrupt handler service routine */
//	CyIntDisable(`$INSTANCE_NAME`_ISR_NUMBER);
//}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_GetStatus
********************************************************************************
* Summary:
*  Returns the internal status of the hardware.
*
* Parameters:
*  None
* 
* Return:
*  uint8
*		b0   = Set when component is busy 
*		b1 	 = Set when PHY address matches in the right timing 
*		b7:2 = Reserved. Returns all zeros
*
* Side Effects:
*  None
********************************************************************************/
uint8 `$INSTANCE_NAME`_GetStatus(void) CYREENTRANT
{
	return `$INSTANCE_NAME`_StatusRegister;
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_ClearInternalFIFOs
********************************************************************************
* Summary:
*  Clear the internal FIFOs from the data path 
*
* Parameters:
*  dataPtr: Pointer to the data buffer
* 
* Return:
*  None
*
* Side Effects:
*  The size of this buffer should be at least 32 x 16 bits (64 bytes), otherwise
*  memory violation may occurs.
*
********************************************************************************/
void `$INSTANCE_NAME`_ClearInternalFIFOs(void)
{
	/* Reset F0 FIFO  */
	CY_SET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR, (CY_GET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR) |  `$INSTANCE_NAME`_F0_CLR));
    CY_SET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR, (CY_GET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR) & ~`$INSTANCE_NAME`_F0_CLR));
	
	/* Reset F1 FIFO  */
	CY_SET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR, (CY_GET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR) |  `$INSTANCE_NAME`_F1_CLR));
    //CY_SET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR, (CY_GET_REG16(`$INSTANCE_NAME`_DP_AUX_CTL_PTR) & ~`$INSTANCE_NAME`_F1_CLR));	
}

/*******************************************************************************
*
* `$INSTANCE_NAME`_WriteData_C45()
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_WriteDataC45(uint8 op_mode, uint8 phy_addr, uint8 dev_addr, uint16 reg_data) // Writes data to the bus
{
   	uint16	control_bits;
	uint8   status_reg;

	/* Clear and Set the control register for writing */
	`$INSTANCE_NAME`_CONTROL_REG = `$INSTANCE_NAME`_ENABLE |`$INSTANCE_NAME`_WRITE;
		
   	/* Set the control bits and write in the fifo f0 */
   	control_bits = 0x0002+((uint16)op_mode<<12)+(((uint16)phy_addr)<<7)+(((uint16)dev_addr)<<2);
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, control_bits);
   
   	/* Write data bits in the fifo f0 */
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, reg_data);
    
   	/* Start Transmission */
   	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_START;
	
	/* Wait till the transmission is completed */
	//while ( `$INSTANCE_NAME`_StatusRegister != `$INSTANCE_NAME`_MDIO_STS_CMPLT );
	while (!(`$INSTANCE_NAME`_STATUS_REG & `$INSTANCE_NAME`_MDIO_END_TRANSFER));
    
    /* Stop Transmission */
    `$INSTANCE_NAME`_CONTROL_REG &= ~`$INSTANCE_NAME`_START;
    
	/* Clear hardware status register */
	status_reg = `$INSTANCE_NAME`_STATUS_REG;
	
	/* Clear software status register */
	`$INSTANCE_NAME`_StatusRegister = 0x00u;
    
    return ((status_reg)? 0 : 1);
}

/*******************************************************************************
*
* `$INSTANCE_NAME`_ReadData_C45()
*
*******************************************************************************/
uint8 `$INSTANCE_NAME`_ReadDataC45(uint8 op_mode, uint8 phy_addr, uint8 dev_addr, uint16 *regData) // Read data from the bus
{
   	uint16	control_bits;
	uint8 status_reg;

	/* Clear and Set the control register for writing */
	`$INSTANCE_NAME`_CONTROL_REG &= `$INSTANCE_NAME`_ENABLE;
	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_READ;
	
   	/* Set the control bits and write in the fifo f0 */
   	control_bits = 0x0003+((uint16)op_mode<<12)+(((uint16)phy_addr)<<7)+(((uint16)dev_addr)<<2);
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, control_bits);
    
   	/* Start Transmission */
   	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_START;
	
	/* Wait till the transmission is completed */
	while ( `$INSTANCE_NAME`_StatusRegister != `$INSTANCE_NAME`_MDIO_STS_CMPLT );
	
	/* Check if device acked */
	status_reg = `$INSTANCE_NAME`_STATUS_REG;
	
	`$INSTANCE_NAME`_StatusRegister = 0x00u;
	
	*regData = `$INSTANCE_NAME`_data_bits;
	
	return ((status_reg)? 0 : 1);
}

/*******************************************************************************
*
* MDIO_Host_WriteData_C22()
*
*******************************************************************************/
void 	`$INSTANCE_NAME`_WriteDataC22(uint8 phy_addr, uint8 dev_addr, uint16 reg_data)
{
   	uint16	control_bits;

	/* Clear and Set the control register for writing */
	`$INSTANCE_NAME`_CONTROL_REG &= `$INSTANCE_NAME`_ENABLE;
	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_WRITE;
	
   	/* Set the control bits and write in the fifo f0 */
   	control_bits = 0x5002+(((uint16)phy_addr)<<7)+(((uint16)dev_addr)<<2);
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, control_bits);
   
   	/* Write data bits in the fifo f0 */
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, reg_data);
    
   	/* Start Transmission */
   	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_START;
}

/*******************************************************************************
*
* MDIO_Host_ReadData_C22()
*
*******************************************************************************/
uint16 	`$INSTANCE_NAME`_ReadDataC22(uint8 phy_addr, uint8 dev_addr)
{
   	uint16	control_bits;

	/* Clear and Set the control register for writing */
	`$INSTANCE_NAME`_CONTROL_REG &= `$INSTANCE_NAME`_ENABLE;
	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_READ;
	
   	/* Set the control bits and write in the fifo f0 */
   	control_bits = 0x6003+(((uint16)phy_addr)<<7)+(((uint16)dev_addr)<<2);
   	CY_SET_REG16(`$INSTANCE_NAME`_FIFO_F0_PTR, control_bits);
    
   	/* Start Transmission */
   	`$INSTANCE_NAME`_CONTROL_REG |= `$INSTANCE_NAME`_START;
	
	while ( `$INSTANCE_NAME`_StatusRegister != `$INSTANCE_NAME`_MDIO_STS_CMPLT );
	
	`$INSTANCE_NAME`_StatusRegister = 0x00u;
	
	return `$INSTANCE_NAME`_data_bits;
}

/*******************************************************************************
*
* MDIO_Host__ClearStatus()
*
*******************************************************************************/
void  `$INSTANCE_NAME`_ClearStatus(void)
{
	`$INSTANCE_NAME`_StatusRegister = 0x00u;
}



/* [] END OF FILE */
