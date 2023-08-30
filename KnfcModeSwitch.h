/*
 * KnfcModeSwitch.h
 *
 *  Created on: Jul 9, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_MODE_SWITCH_H_
#define _KNFC_MODE_SWITCH_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

#include "KnfcIso14443A.h"
#include "KnfcIso14443B.h"
#include "KnfcIso15693.h"

//////////////////////////////////////////////////
// Macro Definition for NFC Protocol capabilities
//////////////////////////////////////////////////

#define MAX_NUM_NFCC_PROTOCOL 			32

#define KNFC_POLLING_LOOP_OFF		0x00
#define KNFC_POLLING_LOOP_ON		0x01

//////////////////////////////////////////////////
// Data Structure for Mode Switching
//////////////////////////////////////////////////

typedef struct {

	// Protocol ID
	uint32_t nProtocol;

	// Timeout to deactivate the protocol after the activation
	uint32_t nTimeout;

	// Function to activate the protocol
	KnfcError_t (*ActivateProtocol)(void *pParam);
	void *pActivateParameter;
	
	// Function to deactivate the protocol
	KnfcError_t (*DeactivateProtocol)(void *pParam);
	void *pDeactivateParameter;

	// Function to exchange data 
	KnfcError_t (*ExchangeData)(void * pConnection, uint8_t * pBuffer, uint32_t nBufferLength);
	
} KnfcProtocolHandler_t;

typedef struct {

	// the current state of the controller
	uint32_t nState;

	// the handler of the currently operating protocol
	KnfcProtocolHandler_t *pHandler;

	// The configured protocols to be activated 
	uint32_t nRegisteredProtocol;
	uint32_t aRegisteredProtocolArray[MAX_NUM_NFCC_PROTOCOL];
	uint32_t nRegisteredProtocolArrayLength;

	// The current operating protocol
	int32_t nCurrentProtocolIndex;
	
} KnfcControllerStatus_t;

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

uint32_t KnfcGetCurrentMode(void);

KnfcError_t KnfcResetModeSwitch(void);

KnfcError_t KnfcPerformModeSwitch(void);

KnfcError_t KnfcRegisterNfccProtocol(uint32_t nProtocol, void *pParam);
	
KnfcError_t KnfcUnregisterNfccProtocol(uint32_t nProtocol);

KnfcError_t KnfcExchangeData(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcStartPollingLoop(void);

KnfcError_t KnfcStopPollingLoop(void);

#endif /* _KNFC_MODE_SWITCH_H_ */
