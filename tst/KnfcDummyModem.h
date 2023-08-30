/*
 * KnfcDummyModem.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_DUMMY_MODEM_H_
#define _KNFC_DUMMY_MODEM_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"
#include "KnfcOsalList.h"

///////////////////////////////////////////////////
// Macros for the tests using dummy events
//////////////////////////////////////////////////

#define TEST_DUMMY_READER_ERROR_EVENT
#define TEST_DUMMY_READER_DRIVER_EVENT
#define TEST_DUMMY_READER_EXCHANGE_DATA
#define TEST_DUMMY_CARD_EMULATION_EVENT
#define TEST_DUMMY_CARD_EMULATION_COMMAND
#define TEST_DUMMY_CARD_SEND_EMUL_ANSWER

///////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////

extern bool_t g_bStopped;

///////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

typedef struct {
	uint8_t *pBuffer;
	uint32_t nBufferLength;
} KnfcDummyData_t;

///////////////////////////////////////////////////
// Global Function Declaration
///////////////////////////////////////////////////

void KnfcRegisterDummyReaderErrorEvent();

void KnfcRegisterDummyReaderDriverEvent();

void KnfcInitializeDummyEventThread();

#ifdef INCLUDE_DEPRECATED_FUNCTIONS
void KnfcDummyExchangeP14P4Data(void *pParam);
#endif

void KnfcDummyExchangeP14P3Data(void *pParam);

void KnfcDummyCardEmulationEvent();

void KnfcDummyCardEmulationCommand();

#endif /* _KNFC_DUMMY_MODEM_H_ */
