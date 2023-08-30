/*
 * KnfcDalSocket.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_DAL_SOCKET_H_
#define _KNFC_DAL_SOCKET_H_

#include "KnfcOsalConfig.h"

#ifdef LINUX_SUPPORT

#include <sys/socket.h>
#include <arpa/inet.h>

//////////////////////////////////////////////////
// Socket Function Declaration
//////////////////////////////////////////////////

bool_t KnfcSocketDataReady(KnfcHandle_t hnd);

KnfcHandle_t KnfcSocketOpen(void *pParam);

int32_t KnfcSocketRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

int32_t KnfcSocketWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

void KnfcSocketClose();

#endif /* LINUX_SUPPORT */

#endif /* _KNFC_DAL_SOCKET_H_ */
