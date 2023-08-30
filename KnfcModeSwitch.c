/*
 * KnfcModeSwitch.c
 *
 *  This file will contain the functions to control the state machine for handling the NFC controller.  
 *
 *  Created on: Jul 9, 2013
 *      Author: youngsun
 */

#include "KnfcModeSwitch.h"

#ifdef ARM926EJS_SUPPORT
#include "KnfcDalTimer.h"
#endif

//////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////

/* Card Mode Timeout */
#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_A_TIMEOUT		500
#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_B_TIMEOUT		500
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_A_TIMEOUT		500
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_B_TIMEOUT		500
#define W_NFCC_PROTOCOL_CARD_ISO_15293_3_TIMEOUT			500		
#define W_NFCC_PROTOCOL_CARD_ISO_15293_2_TIMEOUT			500		
#define W_NFCC_PROTOCOL_CARD_FELICA_TIMEOUT				500
#define W_NFCC_PROTOCOL_CARD_P2P_TARGET_TIMEOUT			500
#define W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP_TIMEOUT			500		
#define W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC_TIMEOUT		500
#define W_NFCC_PROTOCOL_CARD_BPRIME_TIMEOUT				500
#define W_NFCC_PROTOCOL_CARD_KOVIO_TIMEOUT					500				
#define W_NFCC_PROTOCOL_CARD_MIFARE_PLUS_TIMEOUT			500		

/* Reader Mode Timeout */
#define W_NFCC_PROTOCOL_READER_ISO_14443_4_A_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_ISO_14443_4_B_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_A_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_B_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_ISO_15693_3_TIMEOUT		200
#define W_NFCC_PROTOCOL_READER_ISO_15693_2_TIMEOUT		200
#define W_NFCC_PROTOCOL_READER_FELICA_TIMEOUT				200			
#define W_NFCC_PROTOCOL_READER_P2P_INITIATOR_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_TYPE_1_CHIP_TIMEOUT		200
#define W_NFCC_PROTOCOL_READER_MIFARE_CLASSIC_TIMEOUT		200	
#define W_NFCC_PROTOCOL_READER_BPRIME_TIMEOUT				200			
#define W_NFCC_PROTOCOL_READER_KOVIO_TIMEOUT				200			
#define W_NFCC_PROTOCOL_READER_MIFARE_PLUS_TIMEOUT		200

// Default delay time when there is no registered protocol.
#define DEFAULT_POLLING_DELAY									800

//////////////////////////////////////////////////
// Static Variable Definition
//////////////////////////////////////////////////

// Connection Status
static KnfcControllerStatus_t g_hControllerStatus;

// Handlers for each of the protocols
static KnfcProtocolHandler_t g_aProtocolHandlerRegistry[MAX_NUM_NFCC_PROTOCOL] = {

	/* ISO 14443-4A */
	{
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_A,
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_A_TIMEOUT,
#ifdef HARDWARE_P14P4_SUPPORT
		KnfcPiccActivate_P14P4A, NULL,
		KnfcPiccDeactivate_P14P4A, NULL,
		KnfcPiccExchangeData_P14P4A
#else
		KnfcPiccActivate_P14P3A, NULL,
		KnfcPiccDeactivate_P14P3A, NULL,
		KnfcPiccExchangeData_P14P3A
#endif
	},
	/* ISO 14443-4B */
	{
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_B,
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_B_TIMEOUT,
		KnfcPiccActivate_P14P3B, NULL,
		KnfcPiccDeactivate_P14P3B,  NULL,
		KnfcPiccExchangeData_P14P3B
	},
	/* ISO 14443-3A */
	{
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_A,
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_A_TIMEOUT,
		KnfcPiccActivate_P14P3A, NULL,
		KnfcPiccDeactivate_P14P3A, NULL,
		KnfcPiccExchangeData_P14P3A
	},
	/* ISO 14443-3B */
	{
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_B,
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_B_TIMEOUT,
		KnfcPiccActivate_P14P3B, NULL,
		KnfcPiccDeactivate_P14P3B, NULL,
		KnfcPiccExchangeData_P14P3B
	},
	/* ISO 15693-3 : VICC will never be supported. */
	{	W_NFCC_PROTOCOL_CARD_ISO_15693_3,	 0, NULL, NULL, NULL, NULL, NULL },
	/* ISO 15693-2 : VICC will never be supported. */
	{	W_NFCC_PROTOCOL_CARD_ISO_15693_2, 0, NULL, NULL, NULL, NULL, NULL },
	/* FELICA */
	{	W_NFCC_PROTOCOL_CARD_FELICA, 0, NULL, NULL, NULL, NULL, NULL },
	/* P2P TARGET */
	{	W_NFCC_PROTOCOL_CARD_P2P_TARGET, 0, NULL, NULL, NULL, NULL, NULL },
	/* TYPE 1 - TOPAZ */
	{
		W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP,
		W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP_TIMEOUT,
		KnfcPiccActivate_P14P3A, NULL,
		KnfcPiccDeactivate_P14P3A, NULL,
		KnfcPiccExchangeData_P14P3A
	},
	/* MIFARE CLASSIC */
	{	W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC, 0, NULL, NULL, NULL, NULL, NULL },
	/* B PRIME */
	{	W_NFCC_PROTOCOL_CARD_BPRIME, 0, NULL, NULL, NULL, NULL, NULL },
	/* KIVIO */
	{	W_NFCC_PROTOCOL_CARD_KOVIO, 0, NULL, NULL, NULL, NULL, NULL },
	/* MIFARE PLUS */
	{	W_NFCC_PROTOCOL_CARD_MIFARE_PLUS, 0, NULL, NULL, NULL, NULL, NULL },
	/* RESERVED 0, 1, 2 */
	{	W_NFCC_PROTOCOL_CARD_RESERVED0, 0, NULL, NULL, NULL, NULL, NULL },
	{	W_NFCC_PROTOCOL_CARD_RESERVED1, 0, NULL, NULL, NULL, NULL, NULL },
	{	W_NFCC_PROTOCOL_CARD_RESERVED2, 0, NULL, NULL, NULL, NULL, NULL },

	/* ISO 14443-4A */
	{
		W_NFCC_PROTOCOL_READER_ISO_14443_4_A,
		W_NFCC_PROTOCOL_READER_ISO_14443_4_A_TIMEOUT,
#ifdef HARDWARE_P14P4_SUPPORT
		KnfcPcdActivate_P14P4A, NULL,
		KnfcPcdDeactivate_P14P4A, NULL,
		KnfcPcdExchangeData_P14P4A
#else
		KnfcPcdActivate_P14P3A, NULL,
		KnfcPcdDeactivate_P14P3A, NULL,
		KnfcPcdExchangeData_P14P3A
#endif
	},
	/* ISO 14443-4B */
	{
		W_NFCC_PROTOCOL_READER_ISO_14443_4_B,
		W_NFCC_PROTOCOL_READER_ISO_14443_4_B_TIMEOUT,
		KnfcPcdActivate_P14P3B, NULL,
		KnfcPcdDeactivate_P14P3B, NULL,
		KnfcPcdExchangeData_P14P3B
	},
	/* ISO 14443-3A */
	{
		W_NFCC_PROTOCOL_READER_ISO_14443_3_A,
		W_NFCC_PROTOCOL_READER_ISO_14443_3_A_TIMEOUT,
		KnfcPcdActivate_P14P3A, NULL,
		KnfcPcdDeactivate_P14P3A, NULL,
		KnfcPcdExchangeData_P14P3A
	},
	/* ISO 14443-3B */
	{
		W_NFCC_PROTOCOL_READER_ISO_14443_3_B,
		W_NFCC_PROTOCOL_READER_ISO_14443_3_B_TIMEOUT,
		KnfcPcdActivate_P14P3B, NULL,
		KnfcPcdDeactivate_P14P3B, NULL,
		KnfcPcdExchangeData_P14P3B
	},
	/* ISO 15693-3 */
	{
		W_NFCC_PROTOCOL_READER_ISO_15693_3,
		W_NFCC_PROTOCOL_READER_ISO_15693_3_TIMEOUT,
#ifdef HARDWARE_P15P3_SUPPORT		
		KnfcVcdActivate_P15P3, NULL,
		KnfcVcdDeactivate_P15P3, NULL,
		KnfcVcdExchangeData_P15P3
#else
		NULL, NULL, NULL, NULL, NULL
#endif
	},
	/* ISO 15693-2 */
	{	W_NFCC_PROTOCOL_READER_ISO_15693_2, 0, NULL, NULL, NULL, NULL, NULL},
	/* FELICA */
	{	W_NFCC_PROTOCOL_READER_FELICA, 0, NULL, NULL, NULL, NULL, NULL },
	/* P2P INITIATOR */
	{	W_NFCC_PROTOCOL_READER_P2P_INITIATOR, 0, NULL, NULL, NULL, NULL, NULL },
	/* TYPE 1 - TOPAZ */
	{
		W_NFCC_PROTOCOL_READER_TYPE_1_CHIP,
		W_NFCC_PROTOCOL_READER_TYPE_1_CHIP_TIMEOUT,
		KnfcPcdActivate_P14P3A, NULL,
		KnfcPcdDeactivate_P14P3A, NULL,
		KnfcPcdExchangeData_P14P3A
	},
	/* MIFARE CLASSIC */
	{	W_NFCC_PROTOCOL_READER_MIFARE_CLASSIC, 0, NULL, NULL, NULL, NULL, NULL },
	/* B PRIME */
	{	W_NFCC_PROTOCOL_READER_BPRIME, 0, NULL, NULL, NULL, NULL, NULL },
	/* KIVIO */
	{	W_NFCC_PROTOCOL_READER_KOVIO, 0, NULL, NULL, NULL, NULL, NULL },
	/* MIFARE PLUS */
	{	W_NFCC_PROTOCOL_READER_MIFARE_PLUS, 0, NULL, NULL, NULL, NULL, NULL },
	/* RESERVED 0, 1, 2 */
	{	W_NFCC_PROTOCOL_READER_RESERVED0, 0, NULL, NULL, NULL, NULL, NULL },
	{	W_NFCC_PROTOCOL_READER_RESERVED1, 0, NULL, NULL, NULL, NULL, NULL },
	{	W_NFCC_PROTOCOL_READER_RESERVED2, 0, NULL, NULL, NULL, NULL, NULL }
};

//////////////////////////////////////////////////
// Static Function Declaration
//////////////////////////////////////////////////

#ifdef INCLUDE_DEPRECATED_FUNCTIONS
static KnfcProtocolHandler_t *GetProtocolHandler(uint32_t nProtocol);
#endif

static uint32_t UpdateRegisteredProtocolArray(void);

static void PollingLoopEventHandler(void);

//////////////////////////////////////////////////
// Static Function Definition
//////////////////////////////////////////////////

#ifdef INCLUDE_DEPRECATED_FUNCTIONS
/**
 * Return the protocol handler of the given protocol.
 */
 
static KnfcProtocolHandler_t *
GetProtocolHandler(uint32_t nProtocol) {
	uint32_t i;

	for(i = 0; i < MAX_NUM_NFCC_PROTOCOL; i++) {
		if(g_aProtocolHandlerRegistry[i].nProtocol == nProtocol) {
			return &g_aProtocolHandlerRegistry[i];
		}
	}

	return NULL;
}
#endif

/**
 * This function will update the current registered protocol array with the given protocol.
 */
static uint32_t
UpdateRegisteredProtocolArray() {

	uint32_t i;
	uint32_t numProtocols = 0;
	uint32_t protocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
	uint32_t registeredProtocol = g_hControllerStatus.nRegisteredProtocol;
	uint32_t *registeredProtocolArray = (uint32_t *)g_hControllerStatus.aRegisteredProtocolArray;

	for(i = 0; i < MAX_NUM_NFCC_PROTOCOL; i++) {

		// Put the currently registered protocol into the buffer
		if(registeredProtocol & protocol) {
			registeredProtocolArray[numProtocols++] = i;
		}
		
		// Check the next protocol
		protocol <<= 1;
	}

	// Restart the mode switching
	g_hControllerStatus.nCurrentProtocolIndex = -1;

	if(numProtocols > 0) {
		KnfcStartPollingLoop();
	}
	else {
		KnfcStopPollingLoop();
	}

	return g_hControllerStatus.nRegisteredProtocolArrayLength = numProtocols;
}

/**
 * An event handler for performing the mode switching.
 */
 
static void 
PollingLoopEventHandler(void) {

	KnfcProtocolHandler_t *handler = NULL;

#ifdef LINUX_SUPPORT
	// Due to the limitation of the number of available threads,
	// the following code had been re-written for 32 bits ubuntu linux.
	while(g_hControllerStatus.nState == KNFC_POLLING_LOOP_ON) {
		
		if(KnfcPerformModeSwitch() != KNFC_SUCCESS) {
			KNFC_ERROR("PollingLoopEventHandler - Mode Switch Error");
			return;
		}

		if((handler = g_hControllerStatus.pHandler) != NULL) {
			KnfcUSleep(handler->nTimeout);
		}
		else {
			KnfcUSleep(DEFAULT_POLLING_DELAY);
		}		
	}
#else
	if(g_hControllerStatus.nState == KNFC_POLLING_LOOP_OFF) {
		return;
	}
		
	if(KnfcPerformModeSwitch() != KNFC_SUCCESS) {
		KNFC_ERROR("PollingLoopEventHandler - Mode Switch Error");
		return;
	}

	if((handler = g_hControllerStatus.pHandler) != NULL) {
		KnfcTimerSetTime(handler->nTimeout);
	}
	else {
		KnfcTimerSetTime(DEFAULT_POLLING_DELAY);
	}
#endif
}

//////////////////////////////////////////////////
// Global Function Definition
//////////////////////////////////////////////////

#ifdef DUMMY_MODEM_SUPPORT
/**
 * Return the currently operating protocol.
 */
 
uint32_t 
KnfcGetCurrentMode(void) {

	if(g_hControllerStatus.pHandler != 0) {
		return g_hControllerStatus.pHandler->nProtocol;
	}

	return 0;
}
#endif

/**
 * This function will be used to exchange data with the current protocol.
 *
 * @param pConnection the currently established connection to an adjacent PCD or PICC
 * @param pBuffer the data buffer to be transmitted to the selected PCD or PICC
 * @param nBufferLength the length of the data buffer
 * @return error information
 */
 
KnfcError_t 
KnfcExchangeData(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcProtocolHandler_t *handler = g_hControllerStatus.pHandler;

	if(handler == NULL) {
		return KNFC_ERROR_MODESW_INVALID_PROTOCOL;
	}

	KNFC_DEBUG("KnfcExchangeData", "Connection: %X, Protocol: %X", pConnection, handler->nProtocol);
	return handler->ExchangeData(pConnection, pBuffer, nBufferLength);
}

/** 
 * This function reset the NFC controller and restart the mode switching.
 *
 * @return error information
 */

KnfcError_t 
KnfcResetModeSwitch(void) {

	uint32_t i = 0;

	KNFC_DEBUG("KnfcResetModeSwitch", "FUNC CALL");

	g_hControllerStatus.pHandler = NULL;
	g_hControllerStatus.nRegisteredProtocol = 0;
	g_hControllerStatus.nCurrentProtocolIndex = 0;
	g_hControllerStatus.nState = KNFC_POLLING_LOOP_OFF;

	KnfcTimerInit();

	return KNFC_SUCCESS;
}

/** 
 * This function will be used to switch the operating mode using the current status and 
 * the given requests from the NFC controller.
 *
 * @return error information
 */
 
KnfcError_t
KnfcPerformModeSwitch(void) {

	uint32_t i;
	uint32_t handlerIndex = 0;
	KnfcError_t error = KNFC_SUCCESS;
	KnfcProtocolHandler_t *nextHandler = NULL;
	KnfcProtocolHandler_t *currHandler = g_hControllerStatus.pHandler;

#if 0
	KNFC_DEBUG("KnfcPerformModeSwitch", "FUNC STA - Registered Protocol: %X, Current Index: %d", 
					g_hControllerStatus.nRegisteredProtocol,
					g_hControllerStatus.nCurrentProtocolIndex);
#endif

	// If there is no registered NFCC protocols to be activated
	if(g_hControllerStatus.nRegisteredProtocol == 0) {
		return KNFC_SUCCESS;
	}

	// Find the next operating mode and Activate it	
	i = g_hControllerStatus.nCurrentProtocolIndex + 1;

	if(i >= g_hControllerStatus.nRegisteredProtocolArrayLength) {
		i = 0;
	}

	handlerIndex = g_hControllerStatus.aRegisteredProtocolArray[i];
	nextHandler = &g_aProtocolHandlerRegistry[handlerIndex];

	if(nextHandler == NULL) {
		return KNFC_ERROR_MODESW_INVALID_PROTOCOL;
	}
	
	g_hControllerStatus.nCurrentProtocolIndex = i;
	g_hControllerStatus.pHandler = nextHandler;
	
	// Deactivate the currently operating protocol and Activate the next protocol
	if(currHandler != nextHandler) {

		if(currHandler != NULL) {
			error = currHandler->DeactivateProtocol(nextHandler->pDeactivateParameter);
			KNFC_CHECK(error == KNFC_SUCCESS, error);
		}

		error = nextHandler->ActivateProtocol(nextHandler->pActivateParameter);
		KNFC_CHECK(error == KNFC_SUCCESS, error);
	}

	return error;
}

/*
 * This function will be used to register the given protocol.
 *
 * @param nProtocol protocols to be registered
 * @param pParam a parameter for the given protocols
 * @return error information
 */
 
KnfcError_t
KnfcRegisterNfccProtocol(uint32_t nProtocol, void *pParam) {

	KNFC_DEBUG("KnfcRegisterNfccProtocol", "FUNC CALL - Protocol: %X", nProtocol);

	if(nProtocol != 0) {
		
		uint32_t i;
		uint32_t protocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;

		// Set the bits for the given protocols
		g_hControllerStatus.nRegisteredProtocol |= (nProtocol);

		// Reset the protocol to be successively activated.
		UpdateRegisteredProtocolArray();

		if(pParam == NULL) {
			return KNFC_SUCCESS;
		}

		// Set the parameter of each protocol handler
		for(i = 0; i < MAX_NUM_NFCC_PROTOCOL; i++, protocol <<= 1) {

			if(nProtocol & protocol) {
				KnfcProtocolHandler_t *handler = &g_aProtocolHandlerRegistry[i];

				if(handler != NULL) {
					handler->pActivateParameter = pParam;
					handler->pDeactivateParameter = NULL;
				}

				nProtocol &= (~protocol);
			}

			// Stop the iteration if no protocols left.
			if(nProtocol == 0) {
				break;
			}
		}		
	}

	return KNFC_SUCCESS;
}

/*
 * This function will be used to unregister the given protocol.
 *
 * @param nProtocol protocols to be unregistered
 * @return error information
 */
 
KnfcError_t
KnfcUnregisterNfccProtocol(uint32_t nProtocol) {

	KNFC_DEBUG("KnfcUnregisterNfccProtocol", "FUNC CALL - Protocol: %X", nProtocol);
	
	if(nProtocol != 0) {

		uint32_t i;
		uint32_t protocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
		uint32_t currentProtocol = 0;
		KnfcProtocolHandler_t *handler = g_hControllerStatus.pHandler;
		
		if(handler != NULL) {
			currentProtocol = handler->nProtocol;
		}

		// Clear the bits for the given protocols
		g_hControllerStatus.nRegisteredProtocol &= (~nProtocol);

		// Reset the protocol to be successively activated.
		UpdateRegisteredProtocolArray();	

		if(currentProtocol & nProtocol) {
			g_hControllerStatus.pHandler = NULL;
			return handler->DeactivateProtocol(handler->pDeactivateParameter);
		}

		// Clear the parameter of each protocol handler
		for(i = 0; i < MAX_NUM_NFCC_PROTOCOL; i++, protocol <<= 1) {

			if(nProtocol & protocol) {
				KnfcProtocolHandler_t *handler = &g_aProtocolHandlerRegistry[i];

				if(handler != NULL) {
					handler->pActivateParameter = NULL;
					handler->pDeactivateParameter = NULL;
				}

				nProtocol &= (~protocol);
			}

			// Stop the iteration if no protocols left.
			if(nProtocol == 0) {
				break;
			}
		}	
	}
	
	return KNFC_SUCCESS;
}

#ifdef LINUX_SUPPORT
static pthread_t g_hPollingLoopThread;
#endif

/**
 * This function starts the polling loop.
 *
 * @return error information
 */

KnfcError_t
KnfcStartPollingLoop(void) {

	if(g_hControllerStatus.nState == KNFC_POLLING_LOOP_OFF) {
		g_hControllerStatus.nState = KNFC_POLLING_LOOP_ON;
#ifdef LINUX_SUPPORT
		pthread_create(&g_hPollingLoopThread, NULL, &PollingLoopEventHandler, NULL);
#else
		KnfcTimerStart(DEFAULT_POLLING_DELAY, PollingLoopEventHandler);
#endif
	}

	return KNFC_SUCCESS;
}

/**
 * This function stops the polling loop.
 *
 * @return error information
 */
 
KnfcError_t
KnfcStopPollingLoop(void) {

	g_hControllerStatus.nState = KNFC_POLLING_LOOP_OFF;

#ifndef LINUX_SUPPORT
	KnfcTimerStop();	
#endif

	return KNFC_SUCCESS;
}

