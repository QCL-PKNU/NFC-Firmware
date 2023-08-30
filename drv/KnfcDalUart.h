/*
 * KnfcDalUart.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_DAL_UART_H_
#define _KNFC_DAL_UART_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"

//////////////////////////////////////////////////
// Uart Function Declaration
//////////////////////////////////////////////////

bool_t KnfcUartDataReady(KnfcHandle_t hnd);

KnfcHandle_t KnfcUartOpen(void *pParam);

int32_t KnfcUartRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

int32_t KnfcUartWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

void KnfcUartClose(KnfcHandle_t hnd);

#endif /* _KNFC_DAL_UART_H_ */
