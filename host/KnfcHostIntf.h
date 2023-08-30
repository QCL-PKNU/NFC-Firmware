/*
 * KnfcHostIntf.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_HOST_INTERFACE_H_
#define _KNFC_HOST_INTERFACE_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

#include "KnfcDalUart.h"
#ifdef LINUX_SUPPORT
#include "KnfcDalSocket.h"
#endif

typedef enum {
	KNFC_HOST_UART = 0,
	KNFC_HOST_SOCKET,
	KNFC_HOST_INVALID
	
} KHostType_t;

//#define TRACE_HOST_RW_BYTES

//////////////////////////////////////////////////
// Data Structures for Host Interface
//////////////////////////////////////////////////

typedef struct {

	// Check if the interface functions have been configured
	bool_t isConfigured;

	// Show whether the connection is opened or not
	bool_t isOpened;

	// Show whether any data have been received.
	bool_t (*DataReady)(KnfcHandle_t hnd);	
	
	// Open the connection
	KnfcHandle_t (*Open)(void *pParam);

	// Read data from the connection 
	int32_t (*Read)(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

	// Write data to the connection 
	int32_t (*Write)(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

	// Close the connection 
	void (*Close)(KnfcHandle_t hnd);

} KHostIntferface_t;

//////////////////////////////////////////////////
// Basic Host Interface Function Declaration
//////////////////////////////////////////////////

bool_t KnfcHostDataReady(void);

KnfcError_t KnfcHostConfigureAndOpen(KHostType_t eType, void *pParam);

KnfcError_t KnfcHostConfigure(KHostType_t eType);

KnfcError_t KnfcHostOpen(void *pParam);

KnfcError_t KnfcHostRead(void *pBuf, uint32_t nBufLength);

KnfcError_t KnfcHostWrite(void *pBuf, uint32_t nBufLength);

void KnfcHostClose(void);

#endif /* _KNFC_HOST_INTERFACE_H_ */
