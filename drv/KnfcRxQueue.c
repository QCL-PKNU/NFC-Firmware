/*
 * KnfcRxQueue.c
 *
 *  Created on: 2013. 7. 21.
 *      Author: youngsun
 */

 #include "KnfcRxQueue.h"

//////////////////////////////////////////////////
// Utility Functions for Rx Queue
//////////////////////////////////////////////////

void
KnfcRxqInit(KnfcRxq_t *pQueue) {
	pQueue->nFront = -1;
	pQueue->nRear  = -1;
}

bool_t
KnfcRxqEmpty(KnfcRxq_t *pQueue) {

	return pQueue->nFront == -1;
}

int32_t
KnfcRxqDequeue(KnfcRxq_t *pQueue, uint8_t *pBuffer, uint32_t nBufferLength) {

	int32_t i;

	for(i = 0; i < nBufferLength; i++) {

		if(pQueue->nFront == -1) {
			break;
		}

		pBuffer[i] = pQueue->aBuffer[pQueue->nFront];

		if(pQueue->nFront == pQueue->nRear) {
			pQueue->nFront = pQueue->nRear = -1;
		}
		else {
			pQueue->nFront = (pQueue->nFront + 1) & 0xFF;//% UART_QUEUE_SIZE;
		}
	}

	return i;
}

#ifndef KNFC_RXQ_ENQUEUE
bool_t
KnfcRxqEnqueue(KnfcRxq_t *pQueue, uint8_t nValue) {

	if((pQueue->nFront == 0 && pQueue->nRear == RX_QUEUE_SIZE - 1) ||
		(pQueue->nFront == pQueue->nRear + 1)) {
		return FALSE;
	}

	if(pQueue->nFront == -1) {
		pQueue->nFront = pQueue->nRear = 0;
	}
	else {
		pQueue->nRear = (pQueue->nRear + 1) & 0xFF;//% RX_QUEUE_SIZE;
	}

	pQueue->aBuffer[pQueue->nRear] = nValue;
	return TRUE;
}
#endif
