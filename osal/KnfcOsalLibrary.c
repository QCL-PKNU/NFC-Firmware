/*
 * KnfcOsalLibrary.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#include "KnfcOsalLibrary.h"

#ifdef LINUX_SUPPORT
#include <unistd.h>
#else
#include "KnfcDalMemory.h"
#include "KnfcDalTimer.h"
#endif

//////////////////////////////////////////////////
// OS-independent interface to Memory Functions
//////////////////////////////////////////////////

/**
 * This function will initialize the memory interface.
 */
 
void 
KnfcMemoryInit(void) {

#ifndef LINUX_SUPPORT
	KdalMemoryInit();
#endif
}

/**
 * This function has the same functionality with "malloc".
 */
 
void *
KnfcMemoryAlloc(uint32_t nSize) {

#ifdef LINUX_SUPPORT
	return (void *)malloc((size_t)nSize);
#else
	return (void *)KdalMemoryAlloc(nSize);
#endif
}

/**
 * This function has the same functionality with "memcpy".
 */
 
void *
KnfcMemoryCopy(void *tPtr, void *sPtr, uint32_t nSize) {

#ifdef LINUX_SUPPORT
	return memcpy(tPtr, sPtr, nSize);
#else
	uint32_t i;
	uint8_t *srcByteArray = (uint8_t *)sPtr;
	uint8_t *dstByteArray = (uint8_t *)tPtr;

	if(sPtr == NULL || sPtr == NULL || nSize == 0) {
		return NULL;
	}

	// We are able to optimize the routine by unrolling the loop,
	// but we will not do it since the size might usually be small.

	for(i = 0; i < nSize; i++) {
		dstByteArray[i] = srcByteArray[i];
	}

	return tPtr;
#endif
}

/**
 * This function has the same functionality with "free".
 */

void 
KnfcMemoryFree(void *pPtr) {
	
	if(pPtr == NULL) {
		return;
	}

#ifdef LINUX_SUPPORT
	free(pPtr);
#else
	KdalMemoryFree(pPtr);
#endif
}

/**
 * This function has the same functionality with "memset".
 */

void *
KnfcMemorySet(void *pPtr, int32_t nC, uint32_t nSize) {

#ifdef LINUX_SUPPORT
	return memset(pPtr, nC, (size_t)nSize);
#else
	uint32_t i;
	uint8_t byteValue = (uint8_t)nC;
	uint8_t *byteArray = (uint8_t *)pPtr;

	if(pPtr == NULL || nSize == 0) {
		return NULL;
	}

	// We are able to optimize the routine by unrolling the loop,
	// but we will not do it since the size might usually be small.

	for(i = 0; i < nSize; i++) {
		byteArray[i] = (uint8_t)byteValue;
	}

	return pPtr;
#endif
}

//////////////////////////////////////////////////
// OS-independent interface to Sleep Function
//////////////////////////////////////////////////

/**
 * This function has the same functionality with "usleep".
 */

void 
KnfcUSleep(uint32_t nUSec) {

#ifdef LINUX_SUPPORT
	usleep(nUSec);
#else
	volatile uint32_t i;
	for(i = 0; i < nUSec; i++);
#endif
}

//////////////////////////////////////////////////
// OS-independent interface to Mutex Functions
//////////////////////////////////////////////////

/**
 * This function has the same functionality with "pthread_mutex_init".
 */

void
KnfcMutexInit(KnfcMutex_t *pMutex) {
	pMutex->bLock = FALSE;
}

/**
 * This function has the same functionality with "pthread_mutex_lock".
 */

void KnfcMutexLock(KnfcMutex_t *pMutex) {
	while(pMutex->bLock) {
		KnfcUSleep(100);
	}

	pMutex->bLock = TRUE;
}

/**
 * This function has the same functionality with "pthread_mutex_unlock".
 */

void KnfcMutexUnlock(KnfcMutex_t *pMutex) {
	pMutex->bLock = FALSE;
}

/**
 * This function will wait until the given mutex is unlocked.
 * 
 * @param pMutex the handle of the mutex lock
 */

void KnfcMutexWait(KnfcMutex_t *pMutex) {
	while(pMutex->bLock) {
		KnfcUSleep(100);
	}
}

//////////////////////////////////////////////////
// OS-independent interface to Timer Functions
//////////////////////////////////////////////////

static KnfcTimer_t g_hTimer;

/**
 * Timer Interrupt Handler Routine
 */
 
void 
KnfcTimerEventHandler(void) {

	// Stop Timer
	KnfcTimerStop();

	if(g_hTimer.pEventHandler != NULL) {
		g_hTimer.pEventHandler();
	}

	// Resume Timer
	KnfcTimerResume();
}

/**
 * This function initializes the timer.
 */
 
void
KnfcTimerInit(void) {

	g_hTimer.nTimeout = 0;
	g_hTimer.pEventHandler = NULL;

#ifndef LINUX_SUPPORT
	// Configure to operate on Periodic Timer Mode
	KdalTimerInit();
#endif
}

/**
 * This function starts the timer, and it will be time out and invoke the handler after the given time.
 * 
 * @param nTime the value of timeout
 * @param pHandler the handler which will be executed at the timeout
 */
 
void
KnfcTimerStart(uint32_t nTime, tTimerEventHandler pHandler) {

	g_hTimer.nTimeout = nTime;
	g_hTimer.pEventHandler = pHandler;

#ifndef LINUX_SUPPORT
	KdalTimerStart((uint16_t)nTime);
#endif
}


/**
 * This function starts the timer without changing the value of timeout.
 */
 
void
KnfcTimerResume(void) {

#ifndef LINUX_SUPPORT
	KdalTimerResume();
#endif
}

/**
 * This function will stop the timer.
 */
 
void
KnfcTimerStop(void) {

#ifndef LINUX_SUPPORT
	KdalTimerStop();
#endif
}

/**
 * This function can be used to change the value of timeout with the given time.
 *
 * @param nTime the value of timeout
 */
 
void
KnfcTimerSetTime(uint32_t nTime) {

#ifndef LINUX_SUPPORT
	KdalTimerSetTime((uint16_t)nTime);
#endif
}

