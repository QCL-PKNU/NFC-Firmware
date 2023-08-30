/*
 * KnfcHost.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#include "KnfcHostIntf.h"

// An unified interface to the several communication devices 
static KHostIntferface_t g_hHostInterface;

// A handle of the communication port
static KnfcHandle_t g_hHostHandle;

bool_t
KnfcHostDataReady(void) {

	KNFC_CHECK(g_hHostInterface.isConfigured, KNFC_ERROR_HOST_NOT_CONFIGURED);
	return g_hHostInterface.DataReady(g_hHostHandle);
}

KnfcError_t
KnfcHostConfigureAndOpen(KHostType_t eType, void *pParam) {
	KnfcError_t error = KNFC_SUCCESS;

	KNFC_DEBUG("KnfcHostConfigureAndOpen", "FUNC STA: eType - %d", eType);	

	error = KnfcHostConfigure(eType);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostConfigureAndOpen: %d", error);
		return error;
	}

	error = KnfcHostOpen(pParam);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostOpenAndOpen: %d", error);
		return error;
	}	

	KNFC_DEBUG("KnfcHostConfigureAndOpen", "FUNC END");	
	return error;
}

/** 
 * This function is used to configure the communication interface to the protocol stack.
 *
 * @param eType interface type: socket, uart, i2c, and so on
 * @return error information
 */
KnfcError_t
KnfcHostConfigure(KHostType_t eType) {

	KNFC_DEBUG("KnfcHostConfigure", "FUNC STA: eType - %d", eType);

	// Configure the interface functions with the given Host type
	switch(eType) {
		
		case KNFC_HOST_UART:
			g_hHostInterface.DataReady = KnfcUartDataReady;
			g_hHostInterface.Open = KnfcUartOpen;
			g_hHostInterface.Read = KnfcUartRead;
			g_hHostInterface.Write = KnfcUartWrite;
			g_hHostInterface.Close = KnfcUartClose;
			break;

#ifdef LINUX_SUPPORT
		case KNFC_HOST_SOCKET:
			g_hHostInterface.DataReady = KnfcSocketDataReady;
			g_hHostInterface.Open = KnfcSocketOpen;
			g_hHostInterface.Read = KnfcSocketRead;
			g_hHostInterface.Write = KnfcSocketWrite;
			g_hHostInterface.Close = KnfcSocketClose;
			break;
#endif
		default:
			KNFC_ERROR("Unknown Host Type was given: %d", eType);
			g_hHostInterface.isConfigured = FALSE;
			return KNFC_ERROR_HOST_UNKNOWN_TYPE;
	}

	g_hHostInterface.isConfigured = TRUE;

	KNFC_DEBUG("KnfcHostConfigure", "FUNC END");
	return KNFC_SUCCESS;
}


/**
 * This function will be used to establish the communication with the protocol stack.
 *
 * @param pParam a parameter to open the communication
 * @return error information
 */
KnfcError_t 
KnfcHostOpen(void *pParam) {

	KNFC_DEBUG("KnfcHostOpen", "FUNC STA");

	KNFC_CHECK(g_hHostInterface.isConfigured, KNFC_ERROR_HOST_NOT_CONFIGURED);

	g_hHostHandle = g_hHostInterface.Open(pParam);

	if(g_hHostHandle == 0) {
		g_hHostInterface.isOpened = FALSE;
		return KNFC_ERROR_HOST_OPEN;
	}

	g_hHostInterface.isOpened = TRUE;
	
	KNFC_DEBUG("KnfcHostOpen", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function is used to read data from the connected communication.
 *
 * @param pBuffer a buffer to read bytes
 * @param nBufferLength the number of bytes to read
 * @return error information
 */

KnfcError_t 
KnfcHostRead(void *pBuffer, uint32_t nBufferLength) {

	int32_t i;
	int32_t nRead = 0;
	int32_t nOffset = 0;

	KNFC_DEBUG("KnfcHostRead", "FUNC STA: pBuffer - %x, nBufferLength - %d", pBuffer, nBufferLength);
	KNFC_CHECK(g_hHostInterface.isConfigured, KNFC_ERROR_HOST_NOT_CONFIGURED);

	while(nBufferLength != 0) {

		nRead = g_hHostInterface.Read(g_hHostHandle, (uint8_t*)pBuffer + nOffset, nBufferLength);
	
		if(nRead < 0) {
			KNFC_ERROR("KnfcHostRead: read data failed (nRead - %d, nOffset - %d)", nRead, nOffset);
			return KNFC_ERROR_HOST_READ;
		}
		else if(nRead == 0) {
			KNFC_DEBUG("KnfcHostRead", "no data has been received");
			KnfcUSleep(100);
		}

#ifdef TRACE_HOST_RW_BYTES
		for(i = 0; i < nRead; i++) {
			KNFC_DEBUG("KnfcHostRead", "Read Byte [%d] = %X", i, ((uint8_t*)pBuffer)[i]);
		}
#endif
		nOffset += nRead;
		nBufferLength -= nRead;
	}

	KNFC_DEBUG("KnfcHostRead", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function is used to write data via the connected communication.
 *
 * @param pBuffer a buffer for the bytes to be transmitted
 * @param nBufferLength  the number of bytes to be transmitted
 * @return error information
 */

KnfcError_t 
KnfcHostWrite(void *pBuffer, uint32_t nBufferLength) {

	int32_t i;
	int32_t nWrite = 0;
	int32_t nOffset = 0;

	KNFC_DEBUG("KnfcHostWrite", "FUNC STA: pBuffer - %x, nBufferLength - %d", pBuffer, nBufferLength);
	KNFC_CHECK(g_hHostInterface.isConfigured, KNFC_ERROR_HOST_NOT_CONFIGURED);

	while(nBufferLength != 0) {
		
		nWrite = g_hHostInterface.Write(g_hHostHandle, (uint8_t*)pBuffer + nOffset, nBufferLength);
		
		if(nWrite < 0) {
			KNFC_ERROR("KnfcHostWrite: write data failed (nWrite - %d, nOffset - %d)\n", nWrite, nOffset);
			return KNFC_ERROR_HOST_WRITE;
		}
		else if(nWrite == 0) {
			KNFC_DEBUG("KnfcHostWrite", "no data has been sent");
			KnfcUSleep(100);
		}

#ifdef TRACE_HOST_RW_BYTES
		for(i = 0; i < nWrite; i++) {
			KNFC_DEBUG("KnfcHostWrite", "Write Byte [%d] = %X", i, ((uint8_t*)pBuffer)[i]);
		}
#endif

		nOffset += nWrite;
		nBufferLength -= nWrite;
	}
	
	KNFC_DEBUG("KnfcHostWrite", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function is used to close the connected communication.
 */

void 
KnfcHostClose(void) {

	KNFC_DEBUG("KnfcHostClose", "FUNC STA");

	if(g_hHostInterface.isOpened) {
		g_hHostInterface.Close(g_hHostHandle);
	}

	g_hHostInterface.isConfigured = FALSE;
	g_hHostInterface.isOpened = FALSE;

	KNFC_DEBUG("KnfcHostClose", "FUNC END");
}

