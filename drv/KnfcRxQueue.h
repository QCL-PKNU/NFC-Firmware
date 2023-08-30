/*
 * KnfcRecvQueue.hx
 *
 *  Created on: 2013. 7. 21.
 *      Author: youngsun
 */

#ifndef _KNFC_RX_QUEUE_H_
#define _KNFC_RX_QUEUE_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"

//////////////////////////////////////////////////
// Rx Queue Data Structure
//////////////////////////////////////////////////

#define RX_QUEUE_SIZE		256

typedef struct {
	int32_t nFront;
	int32_t nRear;
	uint8_t aBuffer[RX_QUEUE_SIZE];
} KnfcRxq_t;

//////////////////////////////////////////////////
// Rx Queue Macro Definition
//////////////////////////////////////////////////

// We could use the following macro instead of "KnfcRxqEnqueue" function 
// in order to elimiate the calling overhead.
#define KNFC_RXQ_ENQUEUE(hQUEUE, nVALUE) 							\
do{																	\
	if((hQUEUE.nFront == 0 && hQUEUE.nRear == RX_QUEUE_SIZE - 1) ||	\
		(hQUEUE.nFront == hQUEUE.nRear + 1)) {						\
		break;														\
	}																\
	if(hQUEUE.nFront == -1) {											\
		hQUEUE.nFront = hQUEUE.nRear = 0;							\
	}																\
	else {															\
		hQUEUE.nRear = (hQUEUE.nRear + 1) & 0xFF;	 					\
	}																\
	hQUEUE.aBuffer[hQUEUE.nRear] = nVALUE;							\
} while(0)

//////////////////////////////////////////////////
// Rx Queue Management Functions
//////////////////////////////////////////////////

void KnfcRxqInit(KnfcRxq_t *pQueue);

bool_t KnfcRxqEmpty(KnfcRxq_t *pQueue);

int32_t KnfcRxqDequeue(KnfcRxq_t *pQueue, uint8_t *pBuffer, uint32_t nBufferLength);

#ifndef KNFC_RXQ_ENQUEUE
bool_t KnfcRxqEnqueue(KnfcRxq_t *pQueue, uint8_t value);
#endif

#endif
