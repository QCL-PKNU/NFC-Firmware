/*
 * KnfcOsalLibrary.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_OSAL_LIBRARY_H_
#define _KNFC_OSAL_LIBRARY_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

/* Mutex Lock */
typedef struct {
	bool_t bLock;
} KnfcMutex_t;

/* Timer */
typedef void (*tTimerEventHandler)(void);

typedef struct {

	// Time to the timeout
	uint32_t nTimeout;

	// Callback function for the timeout
	tTimerEventHandler pEventHandler;

} KnfcTimer_t;

//////////////////////////////////////////////////
// Memory Function Declaration
//////////////////////////////////////////////////

void KnfcMemoryInit(void);

void *KnfcMemoryAlloc(uint32_t nSize);

void *KnfcMemoryCopy(void *tPtr, void *sPtr, uint32_t nSize);

void *KnfcMemorySet(void *pPtr, int32_t nC, uint32_t nSize);

void KnfcMemoryFree(void *pPtr);

//////////////////////////////////////////////////
// Sleep Function Declaration
//////////////////////////////////////////////////

void KnfcUSleep(uint32_t nUSec);

//////////////////////////////////////////////////
// Mutexlock Function Declaration
//////////////////////////////////////////////////

void KnfcMutexInit(KnfcMutex_t *pMutex);

void KnfcMutexLock(KnfcMutex_t *pMutex);

void KnfcMutexUnlock(KnfcMutex_t *pMutex);

void KnfcMutexWait(KnfcMutex_t *pMutex);

//////////////////////////////////////////////////
// Timer Function Declaration
//////////////////////////////////////////////////

void KnfcTimerEventHandler(void);

void KnfcTimerInit(void);

void KnfcTimerStart(uint32_t nTime, tTimerEventHandler pHandler);

void KnfcTimerResume(void);

void KnfcTimerStop(void);

void KnfcTimerTimeSet(uint32_t nTime);

#endif /* _KNFC_OSAL_LIB_H_ */
