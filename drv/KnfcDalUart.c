/*
 * KnfcDalUart.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */
 
#include "KnfcDalUart.h"

#ifdef ARM926EJS_SUPPORT
#include "SocM3_reg.h"
#endif

//////////////////////////////////////////////////
// Macro
//////////////////////////////////////////////////

//#define DEFAULT_BAUD	38400
#define DEFAULT_BAUD	115200
#define UART_CLK		40750000	// 40.75MHz

#define UART0			0
#define UART1			1

//////////////////////////////////////////////////
// Static Variable Declaration
//////////////////////////////////////////////////

static void ClearUartFifo(uint32_t nUartId);

//////////////////////////////////////////////////
// Static Function Definition
//////////////////////////////////////////////////

/**
 * Clear the reception fifo
 */

static void
ClearUartFifo(uint32_t nUartId) {

#ifdef ARM926EJS_SUPPORT
	uint8_t tmp;

	if(nUartId != (uint32_t)UART0) {
		return;
	}

	while(!(FPGA_UART0_REG(UARTFR) & 0x02)) {
		tmp = FPGA_UART0_REG(UARTDR);
	}
#endif
}

//////////////////////////////////////////////////
// Global Function Definition
//////////////////////////////////////////////////

/**
 *  This function will indicate whether any data has been received or not.
 */

bool_t
KnfcUartDataReady(KnfcHandle_t hnd) {

#ifdef ARM926EJS_SUPPORT
	uint32_t uartId = hnd;

	switch(uartId) {
		case UART0 :
			if((FPGA_UART0_REG(UARTFR) & (1<<3)) != 0) {
				return TRUE;
			}
			break;

		default:
			KNFC_ERROR("KnfcUartDataReady - Invalid Uart Handle: %X", hnd);
			break;
	}
#endif

	return FALSE;
}

/**
 * Open an UART connection
 */

KnfcHandle_t
KnfcUartOpen(void *pParam) {

	uint32_t uartId = (uint32_t)pParam;

	ClearUartFifo(uartId);

#ifdef ARM926EJS_SUPPORT
	switch(uartId) {
		case UART0 : 
			//Configure the registers for UART communication
			FPGA_UART0_REG(UARTLCR) = 0x70;	//date 8bit, fifo(16byte) enabled,none parity,1 stop bit
			FPGA_UART0_REG(UARTBRD) = ((UART_CLK)/DEFAULT_BAUD) & 0x3fff;	// set baud rate
			FPGA_UART0_REG(UARTCR) = 0x02;	//error frame discard				
			FPGA_UART0_REG(UARTCR) |= 0x01;	//uart enable
			FPGA_UART0_REG(UARTCR) |= (0x1 << 3);	//uart rx interrupt enable		
			break;
			
		default :	
			KNFC_ERROR("KnfcUartDataReady - Invalid Uart Port: UART%d", uartId);
			return 0;
	}		
#endif

	return uartId;
}

/**
 * Receive the data bytes as many as the given nLength and put them into the given pBuf
 * with the blocking scheme.
 */

int32_t 
KnfcUartRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	uint32_t i;

	if(pBuf == NULL) {
		return -1;
	}

#ifdef ARM926EJS_SUPPORT
	for(i = 0; i < nLength; i++) {
		while(FPGA_UART0_REG(UARTFR) & 0x02);
		((uint8_t *)pBuf)[i] = FPGA_UART0_REG(UARTDR);
	}
#endif

	return (int32_t)nLength;
}

/**
 * Send the data bytes as many as the given nLength from the given pBuf
 * with the non-blocking scheme.
 */

int32_t
KnfcUartWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	uint32_t i;

	if(pBuf == NULL) {
		return -1;
	}

#ifdef ARM926EJS_SUPPORT
	for(i = 0; i < nLength; i++) {
		while(FPGA_UART0_REG(UARTFR) & 0x01);
		FPGA_UART0_REG(UARTDR) = ((uint8_t *)pBuf)[i];
	}
#endif

	return (int32_t)nLength;
}

/**
 * Close the connection indicated by the given handle
 */

void 
KnfcUartClose(KnfcHandle_t hnd) {

	uint32_t uartId = hnd;

#ifdef ARM926EJS_SUPPORT
	switch(uartId) {
		case UART0 : 
			FPGA_UART0_REG(UARTCR) = 0; // disable all
			break;

		default:
			break;
	}
#endif
}
