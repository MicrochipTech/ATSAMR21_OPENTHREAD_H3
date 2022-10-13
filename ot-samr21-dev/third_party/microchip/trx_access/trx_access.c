/**
* \file  trx_access.c
*
* \brief This is the Radio Driver  source file which
*        contains IEEE 802.15.4 Radio Driver Hardware Abstract Layer
*
*
*/
/*******************************************************************************
Copyright (C) 2021-22 released Microchip Technology Inc. and its subsidiaries.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR
*******************************************************************************/

#include "trx_access.h"


static irq_handler_t irq_hdl_trx = NULL;

void AT86RFX_ISR(void);

void AT86RFX_ISR(void)
{
	/*Clearing the RF interrupt*/
	trx_irq_flag_clr();
  	/*Calling the interrupt routines*/
	if (irq_hdl_trx) {
		irq_hdl_trx();
	}
}

void trx_spi_init(void)
{
	/* Initialize SPI in master mode to access the transceiver */
    //SERCOM4_SPI_Initialize();
    
    // TODO_PRVN : Pin config of RF pins here. This may be for reduce power consumption.
    
}

void PhyReset(void)
{
	/* Ensure control lines have correct levels. */
	RST_HIGH();
	SLP_TR_LOW();

	/* Wait typical time of timer TR1. */
    SYSTICK_DelayUs(330);

	RST_LOW();
    SYSTICK_DelayUs(10);
	RST_HIGH();
}

uint8_t trx_reg_read(uint8_t addr)
{
	uint16_t register_value = 0;

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= READ_ACCESS_COMMAND;

	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();

	/* Send the Read command byte */
    SERCOM4_SPI_WriteRead(&addr,1,NULL,0);
    
    /* Read the register value */
    SERCOM4_SPI_WriteRead(NULL,0,&register_value,1);

	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();

	return register_value;
}

void trx_reg_write(uint8_t addr, uint8_t data)
{
	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	/* Prepare the command byte */
	addr |= WRITE_ACCESS_COMMAND;

	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();

	/* Send the Read command byte */
    SERCOM4_SPI_WriteRead(&addr,1,NULL,0);
 
    /* Write the register value */
    SERCOM4_SPI_WriteRead(&data,1,NULL,0);

	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();
    
	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();
}

//void trx_irq_init(FUNC_PTR trx_irq_cb) //TODO_PRVN: Not required in the case of using LW Mesh PHY. SInce PHY uses polling mode operation
//{
//	/*
//	 * Set the handler function.
//	 * The handler is set before enabling the interrupt to prepare for
//	 * spurious
//	 * interrupts, that can pop up the moment they are enabled
//	 */
//	irq_hdl_trx = (irq_handler_t)trx_irq_cb;
//}

uint8_t trx_bit_read(uint8_t addr, uint8_t mask, uint8_t pos)
{
	uint8_t ret;
	ret = trx_reg_read(addr);
	ret &= mask;
	ret >>= pos;
	return ret;
}

void trx_bit_write(uint8_t reg_addr, uint8_t mask, uint8_t pos,
		uint8_t new_value)
{
	uint8_t current_reg_value;
	current_reg_value = trx_reg_read(reg_addr);
	current_reg_value &= ~mask;
	new_value <<= pos;
	new_value &= mask;
	new_value |= current_reg_value;
	trx_reg_write(reg_addr, new_value);
}

void trx_frame_read(uint8_t *data, uint8_t length)
{
	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	uint16_t temp;
	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();
    
	temp = TRX_CMD_FR;

	/* Send the command byte */
    SERCOM4_SPI_WriteRead(&temp,1,NULL,0);

    /* Read the frame from SPI */
    SERCOM4_SPI_WriteRead(NULL,0,data ,length);
    
	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();
}

void trx_frame_write(uint8_t *data, uint8_t length)
{
	uint8_t temp;

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();

	temp = TRX_CMD_FW;

    /* Send the command byte */
    SERCOM4_SPI_WriteRead(&temp,1,NULL,0);
    
    /* Write full data in single spi call */
    SERCOM4_SPI_WriteRead(data,length,NULL,0);
    
	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();
}

/**
 * @brief Writes data into SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver
 *
 * @param addr Start address in the SRAM for the write operation
 * @param data Pointer to the data to be written into SRAM
 * @param length Number of bytes to be written into SRAM
 */
void trx_sram_write(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint8_t temp;

	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();

	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();

	temp = TRX_CMD_SW;
    
    /* Send the command byte */
    SERCOM4_SPI_WriteRead(&temp,1,NULL,0);
    
    /* Write the address byte */
    SERCOM4_SPI_WriteRead(&addr,1,NULL,0);

    /* Write full data in single spi call */
    SERCOM4_SPI_WriteRead(data,length,NULL,0);
    
	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();    

	/*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();
}

/**
 * @brief Reads data from SRAM of the transceiver
 *
 * This function reads from the SRAM of the transceiver
 *
 * @param[in] addr Start address in SRAM for read operation
 * @param[out] data Pointer to the location where data stored
 * @param[in] length Number of bytes to be read from SRAM
 */
void trx_sram_read(uint8_t addr, uint8_t *data, uint8_t length)
{
    uint16_t temp;
    
	/*Saving the current interrupt status & disabling the global interrupt
	**/
	ENTER_TRX_CRITICAL_REGION();
    
	/* Start SPI transaction by pulling SEL low */
    RFSPI_CS_Clear();

	temp = TRX_CMD_SR;

    /* Send the command byte */
    SERCOM4_SPI_WriteRead(&temp,1,NULL,0);
    
    /* Write the address byte */
    SERCOM4_SPI_WriteRead(&addr,1,NULL,0);

    /* Read full data in single spi call */
    SERCOM4_SPI_WriteRead(NULL,0,data,length);
    
	/* Stop the SPI transaction by setting SEL high */
    RFSPI_CS_Set();  

    /*Restoring the interrupt status which was stored & enabling the global
	 * interrupt */
	LEAVE_TRX_CRITICAL_REGION();
}

/**
 * @brief Writes and reads data into/from SRAM of the transceiver
 *
 * This function writes data into the SRAM of the transceiver and
 * simultaneously reads the bytes.
 *
 * @param addr Start address in the SRAM for the write operation
 * @param idata Pointer to the data written/read into/from SRAM
 * @param length Number of bytes written/read into/from SRAM
 */
void trx_aes_wrrd(uint8_t addr, uint8_t *idata, uint8_t length)
{
//	uint8_t *odata;
//#if SAMD || SAMR21 || SAML21 || SAMR30
//	uint16_t odata_var = 0;
//#endif
//	uint8_t temp;
//
//	delay_us(1); /* wap_rf4ce */
//
//	ENTER_TRX_REGION();
//
//#ifdef NON_BLOCKING_SPI
//	while (spi_state != SPI_IDLE) {
//		/* wait until SPI gets available */
//	}
//#endif
//#if SAMD || SAMR21 || SAML21 || SAMR30
//	/* Start SPI transaction by pulling SEL low */
//	spi_select_slave(&master, &slave, true);
//
//	/* Send the command byte */
//	temp = TRX_CMD_SW;
//
//	while (!spi_is_ready_to_write(&master)) {
//	}
//	spi_write(&master, temp);
//	while (!spi_is_write_complete(&master)) {
//	}
//	/* Dummy read since SPI RX is double buffered */
//	while (!spi_is_ready_to_read(&master)) {
//	}
//	spi_read(&master, &dummy_read);
//
//	/* write SRAM start address */
//	while (!spi_is_ready_to_write(&master)) {
//	}
//	spi_write(&master, addr);
//	while (!spi_is_write_complete(&master)) {
//	}
//	/* Dummy read since SPI RX is double buffered */
//	while (!spi_is_ready_to_read(&master)) {
//	}
//	spi_read(&master, &dummy_read);
//
//	/* now transfer data */
//	odata = idata;
//
//	/* write data byte 0 - the obtained value in SPDR is meaningless */
//	while (!spi_is_ready_to_write(&master)) {
//	}
//	spi_write(&master, *idata++);
//	while (!spi_is_write_complete(&master)) {
//	}
//	/* Dummy read since SPI RX is double buffered */
//	while (!spi_is_ready_to_read(&master)) {
//	}
//	spi_read(&master, &dummy_read);
//
//	/* Reading Spi Data for the length specified */
//	while (length > 0) {
//		while (!spi_is_ready_to_write(&master)) {
//		}
//		spi_write(&master, *idata++);
//		while (!spi_is_write_complete(&master)) {
//		}
//		while (!spi_is_ready_to_read(&master)) {
//		}
//
//#if SAMD || SAMR21 || SAML21 || SAMR30
//		spi_read(&master, &odata_var);
//		*odata++ = (uint8_t)odata_var;
//#else
//		spi_read(&master, (uint16_t *)odata++);
//#endif
//		length--;
//	}
//
//	/* To get the last data byte, write some dummy byte */
//	while (!spi_is_ready_to_write(&master)) {
//	}
//	spi_write(&master, 0);
//	while (!spi_is_write_complete(&master)) {
//	}
//	while (!spi_is_ready_to_read(&master)) {
//	}
//#if SAMD || SAMR21 || SAML21 || SAMR30
//	spi_read(&master, &odata_var);
//	*odata = (uint8_t)odata_var;
//#else
//	spi_read(&master, (uint16_t *)odata);
//#endif
//
//	/* Stop the SPI transaction by setting SEL high */
//	spi_select_slave(&master, &slave, false);
//#else
//	/* Start SPI transaction by pulling SEL low */
//	spi_select_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
//
//	/* Send the command byte */
//	temp = TRX_CMD_SW;
//	spi_write_packet(AT86RFX_SPI, &temp, 1);
//	while (!spi_is_tx_empty(AT86RFX_SPI)) {
//	}
//
//	/* write SRAM start address */
//	spi_write_packet(AT86RFX_SPI, &addr, 1);
//	while (!spi_is_tx_empty(AT86RFX_SPI)) {
//	}
//
//	/* now transfer data */
//	odata = idata;
//
//	/* write data byte 0 - the obtained value in SPDR is meaningless */
//	spi_write_packet(AT86RFX_SPI, idata++, 1);
//
//	/* Reading Spi Data for the length specified */
//	while (length > 0) {
//		spi_write_packet(AT86RFX_SPI, idata++, 1);
//		while (!spi_is_tx_empty(AT86RFX_SPI)) {
//		}
//		spi_read_single(AT86RFX_SPI, odata++);
//		length--;
//	}
//
//	/* To get the last data byte, write some dummy byte */
//	spi_read_packet(AT86RFX_SPI, odata, 1);
//
//	/* Stop the SPI transaction by setting SEL high */
//	spi_deselect_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE);
//#endif
//	LEAVE_TRX_REGION();
}

void trx_spi_disable(void)
{
    while(SERCOM4_REGS->SPIM.SERCOM_SYNCBUSY){
    }
    /* Wait until the synchronization is complete */
    /* Disable interrupt */
    SERCOM4_REGS->SPIM.SERCOM_INTENCLR = SERCOM_SPIM_INTENCLR_Msk;
    /* Clear interrupt flag */
    SERCOM4_REGS->SPIM.SERCOM_INTFLAG = SERCOM_SPIM_INTFLAG_Msk;

    while(SERCOM4_REGS->SPIM.SERCOM_SYNCBUSY){
    }

    /* Disable SPI */
    SERCOM4_REGS->SPIM.SERCOM_CTRLA &= ~(SERCOM_SPIM_CTRLA_ENABLE_Msk);
    while(SERCOM4_REGS->SPIM.SERCOM_SYNCBUSY){
    }
}

void trx_spi_enable(void)
{
    while(SERCOM4_REGS->SPIM.SERCOM_SYNCBUSY){
    }
    /* Enable SPI */
    SERCOM4_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;
}