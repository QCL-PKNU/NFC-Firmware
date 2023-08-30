/*
 * KnfcIso15693.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_ISO15693_H_
#define _KNFC_ISO15693_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"

// YOUNGSUN -FIXME
// The anti-collision procedure is not supported by the hardware, 
// but it must be supported in hardware-manner.
//#define HARDWARE_P15P3_SUPPORT 

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* NFC Controller Management */

#ifdef HARDWARE_P15P3_SUPPORT
/* ISO 15693-3 */

KnfcError_t KnfcVcdActivate_P15P3(void *pParam);

KnfcError_t KnfcVcdDeactivate_P15P3(void *pParam);

KnfcError_t KnfcVcdExchangeData_P15P3(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

/* Interrupt Service Routines */

void KnfcVcdReceiveDataHandler_P15P3(void);
#endif

#endif /* _KNFC_ISO15693_H_ */
