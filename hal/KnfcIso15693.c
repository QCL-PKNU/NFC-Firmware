	/*
 * KnfcIso15693.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcIso15693.h"
#ifdef ARM926EJS_SUPPORT
#include "KnfcDalConfig.h"
#include "KnfcRxQueue.h"
#endif

//////////////////////////////////////////////////
// Global Variable Declaration
//////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
// Nfc Rx Queue
extern KnfcRxq_t g_hNfcRxQueue;
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// Hardware-dependent Implemenation
////////////////////////////////////////////////////////////////////////////////////////////

#ifdef HARDWARE_P15P3_SUPPORT

/* VCD - ISO 15693 Part 3 */

// YOUNGSUN - CHKME
// The routines for ISO 15693 Part 3 can be fully implemented if the anti-collision is supported by the hardware. 

/**
 * This function will be used to transmit the given data on P15P3 protocol from VCD to VICC.
 */
 
static KnfcError_t 
TransmitPcdData_P15P3(uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t nTransmit = 0;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	for(; nTransmit < nBufferLength; nTransmit++) {		
		// Transmit a byte
	}
#endif

	return KNFC_SUCCESS;
}

/**
 * This is an event handler function to process the data received after transmitting data on P15P3.
 */
 
void 
KnfcVcdReceiveDataHandler_P15P3(void) {

	uint32_t rxBufferLength = 0;
	uint8_t rxBuffer[MAX_BUF_LENGTH];

#ifdef ARM926EJS_SUPPORT
	rxBufferLength = KnfcRxqDequeue(&g_hNfcRxQueue, rxBuffer, MAX_BUF_LENGTH);
	
	if(rxBufferLength >= MAX_BUF_LENGTH) {
		KNFC_ERROR("KnfcVcdReceiveDataHandler_P15P3 - Too Short Rx Buffer");
		return;
	}
#endif

	// YOUNGSUN - CHKME
	// In order to eliminate 2 CRC bytes,
	// we are going to employ rxBufferLength - 2 as the length of the data received.
	KnfcProcessReaderExchangeP15P3Data(rxBuffer, rxBufferLength-2);
}

/**
 * This function activates the VCD mode on ISO 15693-3.
 *
 * @param pParam a parameter for the activation
 * @return error information
 */
 
KnfcError_t
KnfcVcdActivate_P15P3(void *pParam) {

	KNFC_DEBUG("KnfcVcdActivate_P15P3", "VCD ACTIVIATE...");

	return KNFC_SUCCESS;
}

/**
 * This function deactivates the VCD mode on ISO 15693-2.
 *
 * @param pParam a parameter for the deactivation
 * @return error information
 */

KnfcError_t
KnfcVcdDeactivate_P15P3(void *pParam) {

	KNFC_DEBUG("KnfcVcdDeactivate_P15P3", "VCD DEACTIVIATE...");

	return KNFC_SUCCESS;
}

/**
 * This function will be used to perform exchanging data between VCD and VICC.
 *
 * @param pConnection the currently established connection to an adjacent VICC
 * @param pBuffer the data buffer to be transmitted to the selected VICC
 * @param nBufferLength the length of the data buffer
 * @return error information
 */
 
KnfcError_t
KnfcVcdExchangeData_P15P3(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcVcdExchangeData_P15P3", "VCD DATA EXCHANGE...");

	return KNFC_SUCCESS;
}

#endif
