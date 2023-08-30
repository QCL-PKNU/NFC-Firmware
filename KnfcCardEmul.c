/*
 * KnfcCardEmul.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcCardEmul.h"

#include "KnfcIso14443A.h"
#include "KnfcIso14443B.h"
#include "KnfcModeSwitch.h"

#ifdef DUMMY_MODEM_SUPPORT
#include "KnfcDummyModem.h"

static KnfcDummyData_t dummyData;
#endif

//////////////////////////////////////////////////
// Global & Static Variable Definition
//////////////////////////////////////////////////

extern KnfcMutex_t g_hHostCommMutex;

// The currently operating card emulation connection
static KnfcEmulConnection_t *g_pEmulConnection = NULL;

// Card Emulation Connection
static KnfcNode_t g_hEmulConnectionRegistry = {NULL, NULL, NULL};

//////////////////////////////////////////////////
// Static Function Declaration
//////////////////////////////////////////////////

static KnfcError_t ActivateCardEmulation(uint8_t nCardType, bool_t bVirtual, void *pParam);

static KnfcError_t DeactivateCardEmulation(uint8_t nCardType, bool_t bVirtual);

static KnfcEmulConnection_t *CreateEmulConnection(uint8_t nCardType, bool_t bVirtual);

static KnfcEmulConnection_t *FindEmulConnectionByType(uint32_t nCardType);

static KnfcEmulConnection_t *FindEmulConnection(KnfcHandle_t hHandle);

static uint8_t *GetProtocolParameter(void *pProtocolInfo, uint32_t nProtocolInfoLength);

//////////////////////////////////////////////////
// Static Function Definition
//////////////////////////////////////////////////

static KnfcError_t
ActivateCardEmulation(uint8_t nCardType, bool_t bVirtual, void *pParam) {

	KnfcError_t error;
	uint32_t nProtocol = 0;
	
	KNFC_DEBUG("ActivateCardEmulation", "FUNC STA: nCardType - %d, bVirtual - %d", nCardType, bVirtual);

	switch(nCardType) {
		case W_PROP_ISO_14443_3_A:
		case W_PROP_NFC_TAG_TYPE_1:
		case W_PROP_NFC_TAG_TYPE_2:

			if(bVirtual == TRUE) {
				KNFC_ERROR("ActivateCardEmulation - Unsupported Virtual Card Type: %d", nCardType);
				return KNFC_ERROR_CARD_UNSUPPORT_TYPE;
			}

			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_3_A;
			break;

		case W_PROP_ISO_14443_4_A:
		case W_PROP_NFC_TAG_TYPE_4_A:
			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
			break;

		case W_PROP_ISO_14443_4_B:
		case W_PROP_NFC_TAG_TYPE_4_B:
			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_B;
			break;

		// YOUNGSUN - CHKME
		// Append additional supported card types here.

		default:
			KNFC_ERROR("ActivateCardEmulation - Unsupported Card Type: %d", nCardType);
			return KNFC_ERROR_CARD_UNSUPPORT_TYPE;
	}
	
	// Activate the card emulation mode with the specified type.
	error = KnfcRegisterNfccProtocol(nProtocol, pParam);

	KNFC_DEBUG("ActivateCardEmulation", "FUNC END: error - %d", error);
	return error;
}

static KnfcError_t
DeactivateCardEmulation(uint8_t nCardType, bool_t bVirtual) {

	KnfcError_t error;
	uint32_t nProtocol = 0;
	
	KNFC_DEBUG("DeactivateCardEmulation", "FUNC STA: nCardType - %d, bVirtual - %d", nCardType, bVirtual);

	switch(nCardType) {
		case W_PROP_ISO_14443_3_A:
		case W_PROP_NFC_TAG_TYPE_1:
		case W_PROP_NFC_TAG_TYPE_2:

			if(bVirtual == TRUE) {
				KNFC_ERROR("DeactivateCardEmulation - Unsupported Virtual Card Type: %d", nCardType);
				return KNFC_ERROR_CARD_UNSUPPORT_TYPE;
			}

			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_3_A;
			break;

		case W_PROP_ISO_14443_4_A:
		case W_PROP_NFC_TAG_TYPE_4_A:
			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_A;
			break;

		case W_PROP_ISO_14443_4_B:
		case W_PROP_NFC_TAG_TYPE_4_B:
			nProtocol = W_NFCC_PROTOCOL_CARD_ISO_14443_4_B;
			break;

		// YOUNGSUN - CHKME
		// Append additional supported card types here.

		default:
			nProtocol = 0;
			break;
	}
	
	// Activate the card emulation mode with the specified type.
	error = KnfcUnregisterNfccProtocol(nProtocol);
	KNFC_CHECK(error == KNFC_SUCCESS, error);

	KNFC_DEBUG("DeactivateCardEmulation", "FUNC END: error - %d", error);
	return error;
}

/**
 * Create a new card emulation connection and append to the registry
 */
 
static KnfcEmulConnection_t *
CreateEmulConnection(uint8_t nCardType, bool_t bVirtual) {

	KnfcEmulConnection_t *connection;

	connection = (KnfcEmulConnection_t *)KnfcMemoryAlloc(sizeof(KnfcEmulConnection_t));	

	if(connection != NULL) {
		connection->bVirutal = bVirtual;
		connection->nCardType = nCardType;
		connection->cbCardEmulationEvent = NULL;

		KnfcInsertNode(&g_hEmulConnectionRegistry, connection);
	}

	return connection;
}

/**
 * Find the card emulation connection for the current card event.
 */

static KnfcEmulConnection_t *
FindEmulConnectionByType(uint32_t nCardType) {

	KnfcNode_t *currNode = g_hEmulConnectionRegistry.pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {
		
		KnfcEmulConnection_t *connection = (KnfcEmulConnection_t *)currNode->pItem;

#ifndef HARDWARE_P14P4_SUPPORT		
		switch(connection->nCardType) {
			// ISO 14443-3 compliant Card Type
			// Part 4 will be supported by the protocol stack in software-manner. 
			case W_PROP_ISO_14443_4_A:

			// NFC Forum Tag Type (ISO 14443-3 compliant)
			case W_PROP_NFC_TAG_TYPE_1:
			case W_PROP_NFC_TAG_TYPE_2:
			case W_PROP_NFC_TAG_TYPE_4_A:
				if(nCardType == W_PROP_ISO_14443_3_A) {
					return connection;
				}
				break;

			// ISO 14443-3 compliant Card Type
			case W_PROP_ISO_14443_4_B:

			// NFC Forum Tag Type (ISO 14443-3 compliant)				
			case W_PROP_NFC_TAG_TYPE_4_B:				
				if(nCardType == W_PROP_ISO_14443_3_B) {
					return connection;
				}				
				break;
		}
#endif			

		if(connection->nCardType == nCardType) {
			return connection;
		}
	}

	return NULL;
}

/**
 * Find the card emulation connection for the connection handle.
 */

static KnfcEmulConnection_t *
FindEmulConnection(KnfcHandle_t hHandle) {

	KnfcNode_t *findNode;

	findNode = KnfcFindNode(&g_hEmulConnectionRegistry, (void *)hHandle);

	if(findNode == NULL) {
		return NULL;
	}

	return (KnfcEmulConnection_t *)findNode->pItem;
}

/**
 * Convert the protocol info data structure into a byte array.
 */
static uint8_t *
GetProtocolParameter(void *pProtocolInfo, uint32_t nProtocolInfoLength) {

	uint8_t *buffer = (uint8_t *)KnfcMemoryAlloc(nProtocolInfoLength);

	if(buffer == NULL) {
		return NULL;
	}
	
	return KnfcMemoryCopy(buffer, pProtocolInfo, nProtocolInfoLength);	
}

//////////////////////////////////////////////////
// Card Emulation Open Event
//////////////////////////////////////////////////

/**
 * This function is designed to open the emulation connection using the given connection information.
 *
 * @param pEmulConnectionInfo the information to open the card emulation connection
 * @param ppConnection the handle of the connection
 * @return error information
 */
 
KnfcError_t
KnfcOpenEmulConnection(tWEmulConnectionInfo *pEmulConnectionInfo, KnfcEmulConnection_t **ppConnection) {

	KnfcError_t error = KNFC_SUCCESS;
	uint8_t *paramBuffer = NULL;
	uint8_t cardType = pEmulConnectionInfo->nCardType;
	KNFC_DEBUG("KnfcOpenEmulConnection", "FUNC STA - nCardType: %d, Info: %X", cardType, pEmulConnectionInfo);
	
	switch(cardType) {
#ifndef HARDWARE_P14P4_SUPPORT		
		case W_PROP_ISO_14443_4_A: 
#endif
		case W_PROP_ISO_14443_3_A: {
			KnfcProtocolInfo_P14P3A_t protocolInfo;
			tWEmul14AConnectionInfo *connectionInfo = &pEmulConnectionInfo->sCardInfo.s14A; 

			// ATQA
			protocolInfo.aAtqa[0] = (connectionInfo->nATQA >> 8) & 0xFF;
			protocolInfo.aAtqa[1] = (connectionInfo->nATQA >> 0) & 0xFF;

			// SAK
			protocolInfo.nSak = connectionInfo->nSAK;

			// UID
			protocolInfo.nUidLength = connectionInfo->nUIDLength;
			KnfcMemoryCopy(protocolInfo.aUid, connectionInfo->UID, protocolInfo.nUidLength);
				
			error = KnfcSetProtocolInfo_P14P3A(&protocolInfo);
			KNFC_CHECK(error == KNFC_SUCCESS, error);

			paramBuffer = GetProtocolParameter((void *)&protocolInfo, sizeof(KnfcProtocolInfo_P14P3A_t));
			break;
		}

#ifdef HARDWARE_P14P4_SUPPORT
		case W_PROP_ISO_14443_4_A: {
			KnfcProtocolInfo_P14P4A_t protocolInfo;
			tWEmul14AConnectionInfo *connectionInfo = &pEmulConnectionInfo->sCardInfo.s14A; 

			// ATQA
			protocolInfo.aAtqa[0] = (connectionInfo->nATQA >> 8) & 0xFF;
			protocolInfo.aAtqa[1] = (connectionInfo->nATQA >> 0) & 0xFF;

			// SAK
			protocolInfo.nSak = connectionInfo->nSAK;

			// ATS

			// UID
			protocolInfo.nUidLength = connectionInfo->nUIDLength;
			KnfcMemoryCopy(protocolInfo.aUid, connectionInfo->UID, protocolInfo.nUidLength);
				
			error = KnfcSetProtocolInfo_P14P4A(&protocolInfo);		
			KNFC_CHECK(error == KNFC_SUCCESS, error);

			paramBuffer = GetProtocolParameter((void *)&protocolInfo, sizeof(KnfcProtocolInfo_P14P4A_t));
			break;
		}
#endif

		case W_PROP_ISO_14443_4_B:
		case W_PROP_ISO_14443_3_B: {
			KnfcProtocolInfo_P14P3B_t protocolInfo;
			tWEmul14BConnectionInfo *connectionInfo = &pEmulConnectionInfo->sCardInfo.s14B;

			// ATQB - PUPI
			protocolInfo.aAtqb[0] = connectionInfo->PUPI[0];
			protocolInfo.aAtqb[1] = connectionInfo->PUPI[1];
			protocolInfo.aAtqb[2] = connectionInfo->PUPI[2];
			protocolInfo.aAtqb[3] = connectionInfo->PUPI[3];

			// ATQB - Application Data (4, 5, 6, 7)
			protocolInfo.aAtqb[4] = connectionInfo->nAFI;
			protocolInfo.aAtqb[5] = (connectionInfo->nATQB >> 24) & 0xFF;
			protocolInfo.aAtqb[6] = (connectionInfo->nATQB >> 16) & 0xFF;
			protocolInfo.aAtqb[7] = (connectionInfo->nATQB >> 8) & 0xFF;	
			
			// ATQB - Protocol Info (8, 9, 10)
			protocolInfo.aAtqb[8] = 0x80;	// Same bit rate between PCD and PICC
			protocolInfo.aAtqb[9] = 0x81;	// 256 bytes size frame

			if(cardType == W_PROP_ISO_14443_4_B) {
				// Compliant ISO 14443-4
				protocolInfo.aAtqb[9] |= 0x01;
			}

			protocolInfo.aAtqb[10] = (connectionInfo->nATQB) & 0xFF;

			// Higher Layer Response
			protocolInfo.nHigherLayerResponseLength = connectionInfo->nHigherLayerResponseLength;

			if(protocolInfo.nHigherLayerResponseLength != 0) {
				KnfcMemoryCopy(protocolInfo.aHigherLayerResponse, 
								connectionInfo->aHigherLayerResponse,
								protocolInfo.nHigherLayerResponseLength);
			}

			error = KnfcSetProtocolInfo_P14P3B(&protocolInfo);	
			KNFC_CHECK(error == KNFC_SUCCESS, error);
			
			paramBuffer = GetProtocolParameter((void *)&protocolInfo, sizeof(KnfcProtocolInfo_P14P3B_t));
			break;
		}

		default:
			KNFC_ERROR("KnfcOpenEmulConnection - Invalid Card Type: %d", cardType);
			return KNFC_ERROR_CARD_INVALID_CONNECTION_INFO;
	}

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcOpenEmulConnection - Error: %d", error);
		return error;
	}

	// In only the case of the Virtual Card Emulation,
	// we could directly configure the protocol information.
	*ppConnection = CreateEmulConnection(cardType, TRUE);
		
	if(*ppConnection != NULL) {
		error = ActivateCardEmulation(cardType, TRUE, paramBuffer);
	}

	KNFC_DEBUG("KnfcOpenEmulConnection", "FUNC END - Error: %d", error);
	return error;
}

/**
 * This function is designed to close the emulation connection indicated by the given handle.
 *
 * @param hHandle the the handle of the connection to be closed
 * @return error information
 */
 
KnfcError_t
KnfcCloseEmulConnection(KnfcHandle_t hHandle) {

	KnfcNode_t *deleteNode;
	KnfcError_t error = KNFC_SUCCESS;
	KNFC_DEBUG("KnfcCloseEmulConnection", "FUNC STA - hHandle: %X", hHandle);

	deleteNode = KnfcDeleteNode(&g_hEmulConnectionRegistry, (void *)hHandle);

	if(deleteNode != NULL) {
		KnfcEmulConnection_t *connection = (KnfcEmulConnection_t *)hHandle;
		error = DeactivateCardEmulation(connection->nCardType, connection->bVirutal);
		
		KnfcMemoryFree(deleteNode->pItem);
		KnfcMemoryFree(deleteNode);
	}

	KNFC_DEBUG("KnfcCloseEmulConnection", "FUNC END - error: %d", error);
	return error;
}

//////////////////////////////////////////////////
// Card Emulation Event
//////////////////////////////////////////////////

/**
 * Callback function for the card emulation events: reader left and reader selection.
 */
 
static void 
CardEmulationEventCB(KnfcHandle_t hHandle, uint32_t nEventCode) {

	uint32_t eventInfoSize;
	KhstRespMessage_t respMessage;

	struct EmulEventInfo_s {
		uint32_t hHandle;			
		uint32_t nEventCode;
	} eventInfo;

	KNFC_DEBUG("CardEmulationEventCB", "CALLBACK EXECUTED");

	eventInfo.hHandle = hHandle;
	eventInfo.nEventCode = nEventCode;

	eventInfoSize = 8 /* sizeof(struct EmulEventInfo_s) */;

	respMessage.header.nCode = P_Idenfier_PEmulOpenConnectionDriver2Ex;
	respMessage.header.nLength = eventInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(eventInfoSize);
	KNFC_ASSERT(respMessage.pPayload != NULL, "CardEmulationEventCB - Memory Alloc");		

	KnfcMemoryCopy(&respMessage.pPayload[0], &eventInfo, eventInfoSize);	

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("CardEmulationEventCB - KnfcHostEnqueueLazyResponse");		
	}
		
	// YOUNGSUN - CHKME
	// Do something internally for the card emulation events.

	// 2: W_EMUL_EVENT_SELECTION
	// 3: W_EMUL_EVENT_DEACTIVATE
	switch(nEventCode) {
		case W_EMUL_EVENT_SELECTION:
			g_pEmulConnection = (KnfcEmulConnection_t *)hHandle;
			break;

		case W_EMUL_EVENT_DEACTIVATE:
			KnfcCloseEmulConnection(hHandle);
			g_pEmulConnection = NULL;			
			break;

		default:
			KNFC_ERROR("CardEmulationEventCB - Unknown Event Code: %d", nEventCode);
			break;
	}
}

/**
 * This function is used to process the card emulation event.
 * 
 * @param nCardType the card type of the event
 * @param nEventCode the event code from the NFC modem
 * @return error information
 */

KnfcError_t
KnfcProcessCardEmulationEvent(uint32_t nCardType, uint32_t nEventCode) {

	KnfcEmulConnection_t *connection = FindEmulConnectionByType(nCardType);
	
	KnfcMutexLock(&g_hHostCommMutex);
	if(connection != NULL && connection->cbCardEmulationEvent != NULL) {
		connection->cbCardEmulationEvent((KnfcHandle_t)connection, nEventCode);
	}
	KnfcMutexUnlock(&g_hHostCommMutex);
	
	return KNFC_SUCCESS;
}

/**
 * This function is used to register the card emulation event.
 * 
 * @param hHandle the handle of the connection to register the event handler
 * @return error information
 */
 
KnfcError_t
KnfcRegisterCardEmulationEvent(KnfcHandle_t hHandle){

	KnfcEmulConnection_t *connection = NULL;
	KNFC_DEBUG("KnfcRegisterCardEmulationEvent", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);
	
	if(connection == NULL) {
		KNFC_ERROR("KnfcRegisterCardEmulationEvent - Connection not found.");
		return KNFC_ERROR_CARD_INVALID_CONNECTION;
	}
	
	connection->cbCardEmulationEvent = CardEmulationEventCB;

#ifdef TEST_DUMMY_CARD_EMULATION_EVENT
	KnfcDummyCardEmulationEvent();
#endif

	KNFC_DEBUG("KnfcRegisterCardEmulationEvent", "FUNC END");	
	return KNFC_SUCCESS;
}

/**
 * This function is used to unregister the card emulation event.
 * 
 * @param hHandle the handle of the connection to unregister the event handler
 * @return error information
 */
 
KnfcError_t
KnfcUnregisterCardEmulationEvent(KnfcHandle_t hHandle){

	KnfcEmulConnection_t *connection;
	KNFC_DEBUG("KnfcUnregisterCardEmulationEvent", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);

	if(connection != NULL) {
		connection->cbCardEmulationEvent = NULL;
	}

	KNFC_DEBUG("KnfcUnregisterCardEmulationEvent", "FUNC END");	
	return KNFC_SUCCESS;
}

//////////////////////////////////////////////////
// Card Emulation Command
//////////////////////////////////////////////////

/**
 * Callback function for the card emulation commands
 */
 
static void 
CardEmulationCommandCB(KnfcHandle_t hHandle, uint32_t nBufferLength) {

	uint32_t cmdInfoSize;
	KhstRespMessage_t respMessage;

	struct EmulCmdInfo_s {
		uint32_t hHandle;			
		uint32_t nBufferLength;
	} cmdInfo;

	KNFC_DEBUG("CardEmulationCommandCB", "CALLBACK EXECUTED");

	cmdInfo.hHandle = hHandle;
	cmdInfo.nBufferLength = nBufferLength;

	cmdInfoSize = 8 /* sizeof(struct EmulCmdInfo_s) */;

	respMessage.header.nCode = P_Idenfier_PEmulOpenConnectionDriver3Ex;
	respMessage.header.nLength = cmdInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(cmdInfoSize);
	KNFC_ASSERT(respMessage.pPayload != NULL, "CardEmulationCommandCB - Memory Alloc");		

	KnfcMemoryCopy(&respMessage.pPayload[0], &cmdInfo, cmdInfoSize);

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("CardEmulationEventCB - KnfcHostEnqueueLazyResponse");		
	}

	// YOUNGSUN - CHKME
	// Do something internally for the card emulation events.
}

/**
 * This function is used to process the card emulation command from the PCD.
 *
 * @param pBuffer a command buffer
 * @param nBufferLength the buffer length
 * @return error information
 */

KnfcError_t
KnfcProcessCardEmulationCommand(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcEmulConnection_t *connection = g_pEmulConnection;

	if(connection == NULL) {
		return KNFC_ERROR_CARD_INVALID_CONNECTION;
	}

	// Temporarily keep the command data to the connection.
	connection->nCommandBufferLength = nBufferLength;

	if(nBufferLength <= COMMAND_BUFFER_LENGTH) {
		KnfcMemoryCopy(connection->aCommandBuffer, pBuffer, nBufferLength);
	}
	else {
		connection->aCommandBuffer[0] = 0xFE;
		connection->aCommandBuffer[1] = 0xFE;
	}

	KnfcMutexLock(&g_hHostCommMutex);

	// Send the command to the protocol stack
	if(connection->cbCardEmulationCommand != NULL) {
		connection->cbCardEmulationCommand((KnfcHandle_t)connection, nBufferLength);
	}
	
	KnfcMutexUnlock(&g_hHostCommMutex);
	
	return KNFC_SUCCESS;
}

/**
 * This function is used to register the card emulation command.
 * 
 * @param hHandle the handle of the connection to register the command handler
 * @return error information
 */
 
KnfcError_t
KnfcRegisterCardEmulationCommand(KnfcHandle_t hHandle){

	KnfcEmulConnection_t *connection = NULL;
	KNFC_DEBUG("KnfcRegisterCardEmulationCommand", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);
	
	if(connection == NULL) {
		KNFC_ERROR("KnfcRegisterCardEmulationCommand - Connection not found.");
		return KNFC_ERROR_CARD_INVALID_CONNECTION;
	}
	
	connection->cbCardEmulationCommand = CardEmulationCommandCB;

#ifdef TEST_DUMMY_CARD_EMULATION_COMMAND
	KnfcDummyCardEmulationCommand();
#endif

	KNFC_DEBUG("KnfcRegisterCardEmulationCommand", "FUNC END");	
	return KNFC_SUCCESS;
}

/**
 * This function is used to unregister the card emulation command.
 * 
 * @param hHandle the handle of the connection to unregister the command handler
 * @return error information
 */
 
KnfcError_t
KnfcUnregisterCardEmulationCommand(KnfcHandle_t hHandle){

	KnfcEmulConnection_t *connection;
	KNFC_DEBUG("KnfcUnregisterCardEmulationCommand", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);

	if(connection != NULL) {
		connection->cbCardEmulationCommand = NULL;
	}

	KNFC_DEBUG("KnfcUnregisterCardEmulationCommand", "FUNC END");	
	return KNFC_SUCCESS;
}

//////////////////////////////////////////////////
// Get Card Emulation Message Data
//////////////////////////////////////////////////

/** 
 * This function will be used to send the card emulation message data to the protocol stack.
 * 
 * @param hHandle the handle of the current connection
 * @param ppBuffer a data buffer to transfer the command up to the protocol stack.
 * @param pnBufferLength the command length
 * @return error information
 */
 
KnfcError_t 
KnfcGetEmulMessageData(KnfcHandle_t hHandle, uint8_t **ppBuffer, uint32_t *pnBufferLength) {

	KnfcEmulConnection_t *connection;
	KNFC_DEBUG("KnfcGetEmulMessageData", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);

	if(connection == NULL) {
		return KNFC_ERROR_CARD_INVALID_CONNECTION;
	}

	*pnBufferLength = connection->nCommandBufferLength;
	*ppBuffer = connection->aCommandBuffer;
	
	KNFC_DEBUG("KnfcGetEmulMessageData", "FUNC END");			
	return KNFC_SUCCESS;
}


//////////////////////////////////////////////////
// Send Card Emulation Answer
//////////////////////////////////////////////////

/**
 * This function will be used to process the answer from the protocol stack for the previously transmitted command.
 *
 * @param hHandle the handle of the current connection
 * @param pBuffer a buffer of the answer
 * @param pnBufferLength the buffer length
 * @return error information
 */

KnfcError_t
KnfcSendEmulAnswer(KnfcHandle_t hHandle, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KnfcEmulConnection_t *connection;
	KNFC_DEBUG("KnfcSendEmulAnswer", "FUNC STA - hHandle: %X", hHandle);

	connection = FindEmulConnection(hHandle);

	if(connection == NULL) {
		return KNFC_ERROR_CARD_INVALID_CONNECTION;
	}

	// Send the answer to the reader on the given connection.
	error = KnfcExchangeData(connection, pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		return error;
	}	

	//YOUNGSUN - CHKME
#ifdef TEST_DUMMY_CARD_SEND_EMUL_ANSWER
	dummyData.pBuffer = (uint8_t *)malloc(nBufferLength);
	memcpy(dummyData.pBuffer, pBuffer, nBufferLength);
	dummyData.nBufferLength = nBufferLength;

	KnfcDummySendEmulAnswer(&dummyData);
#endif	

	KNFC_DEBUG("KnfcSendEmulAnswer", "FUNC END");			
	return KNFC_SUCCESS;
}

