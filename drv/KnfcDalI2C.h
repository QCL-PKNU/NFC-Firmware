/*
 * KnfcDalI2C.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_DAL_I2C_H_
#define _KNFC_DAL_I2C_H_

#include "KnfcOsalConfig.h"

/**
 * This file is going to contain the functions
 * for handling the I2C slave.
 */

// YOUNGSUN - CHKME
// Not supported in this version
// since I2C is not included in the requirement.

//////////////////////////////////////////////////
// I2c Function Declaration
//////////////////////////////////////////////////

bool_t KnfcI2cDataReady(KnfcHandle_t hnd);

KnfcHandle_t KnfcI2cOpen(void *pParam);

int32_t KnfcI2cRead(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

int32_t KnfcI2cWrite(KnfcHandle_t hnd, void *pBuf, uint32_t nLength);

void KnfcI2cClose(KnfcHandle_t hnd);

#endif /* _KNFC_DAL_I2C_H_ */
