/*
 * KnfcDalKnfcI2c.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */
 
#include "KnfcDalI2C.h"

// Not supported in this version

bool_t
KnfcI2cDataReady(KnfcHandle_t hnd) {
	return TRUE;
}

KnfcHandle_t
KnfcI2cOpen(void *pParam) {
	return 0;
}

int32_t 
KnfcI2cRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	if(hnd < 0) {
		return -1;
	}

	return 0; //recv(g_KnfcI2c, pBuf, nLength, 0);
}

int32_t
KnfcI2cWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength) {

	if(hnd < 0) {
		return -1;
	}
	
	return 0; //send(g_KnfcI2c, pBuf, nLength, MSG_NOSIGNAL);
}

void 
KnfcI2cClose(KnfcHandle_t hnd) {

	if(hnd < 0) {
		return;
	}

	//close(g_I2c);
}
