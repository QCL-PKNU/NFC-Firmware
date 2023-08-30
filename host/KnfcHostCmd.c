/*
 * KnfcHostCmd.c
 *
 *  Created on: Jun 20, 2013
 *      Author: youngsun
 */
 
#include "KnfcHostCmd.h"
#include "KnfcReader.h"
#include "KnfcCardEmul.h"

#ifdef DUMMY_MODEM_SUPPORT
#include "KnfcDummyModem.h"
#endif

//////////////////////////////////////////////////
// For Communication Locking
//////////////////////////////////////////////////

/// This mutex lock is used to sequentially transmit the lazy responses to the requests from the protocol stack. 
/// Since the lazy responses are built and transmitted by the interrupt handlers, they had to be synchronized with each other. 
KnfcMutex_t g_hHostCommMutex;

//////////////////////////////////////////////////
// Static Variable Definition
//////////////////////////////////////////////////

static KnfcNode_t g_hLazyResponseQueue = {NULL, NULL, NULL};

//////////////////////////////////////////////////
// Static Function Declaration
//////////////////////////////////////////////////

static KnfcError_t SendAckResponse(uint8_t nCode);

static KnfcError_t SendResponseToCommand(KhstCmdMessage_t *pMsg);

static KnfcError_t SendLazyResponse(KhstRespMessage_t *pRespMessage);

//////////////////////////////////////////////////
// Static Function Definitions
//////////////////////////////////////////////////

/** 
 * This function will send both of the received command code and the error result.
 */

static KnfcError_t
SendAckResponse(uint8_t nCode) {

	KnfcError_t error;
	uint8_t extraCode = P_Idenfier_KhalNormalEvent;

	// Send a code to check whether the message is normal one or lazy one.
	error = KnfcHostWrite(&extraCode, 1);

	if(error != KNFC_SUCCESS) {
		return error;
	}

	// Send the given command code to the host for the function identification
	return KnfcHostWrite(&nCode, 1);
}

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

/**
 * This funciton will be used to receive additional data from the protocol stack. 
 */

static KnfcError_t
ReceiveExtraParameter(uint8_t nParamID, uint8_t *pBuffer, uint32_t *pnBufferLength) {

	KnfcError_t error;
	KhstParamHeader_t param;

	// Receive the parameter header first
	error = KnfcHostRead(&param, sizeof(KhstParamHeader_t));

	if(param.nLength > 0) {
		if(param.nCode == nParamID + P_Idenfier_KhalParameter0) {
			error = KnfcHostRead(pBuffer, param.nLength);
			*pnBufferLength = param.nLength;
		}
	}

	return error;
}

#endif

/**
 * This function is used to response to the given command from the protocol stack.
 */
 
static KnfcError_t
SendResponseToCommand(KhstCmdMessage_t *pCmdMessage) {

	KnfcError_t error = KNFC_SUCCESS;
	KhstRespMessage_t respMessage;
	KhstCmdHeader_t *cmdHeader = &pCmdMessage->header;

	uint32_t paramBufferLength = 0;
	uint8_t paramBuffer[MAX_BUF_LENGTH];

	respMessage.header.nCode = cmdHeader->nCode;
	respMessage.header.nLength = 0;
	respMessage.pPayload = NULL;

	KNFC_DEBUG("SendResponseToCommand", "Code - %d", cmdHeader->nCode);
	KNFC_DEBUG("SendResponseToCommand", "SizeIn - %d", cmdHeader->nSizeIn);
	KNFC_DEBUG("SendResponseToCommand", "SizeOut - %d", cmdHeader->nSizeOut);

	// Build a response message for the host command.
	switch(cmdHeader->nCode) {
		case P_Idenfier_PBasicDriverGetVersion: {

			error = KnfcGetDriverVersion(&respMessage.pPayload, &respMessage.header.nLength);
			break;
		}

		case P_Idenfier_PNFCControllerDriverReadInfo: {

			error = KnfcGetControllerInfo(&respMessage.pPayload, &respMessage.header.nLength);
			break;
		}

		case P_Idenfier_PNFCControllerGetMode: {

			uint32_t mode = KnfcGetControllerMode();
			error = SendAckResponse(cmdHeader->nCode);

			if(error == KNFC_SUCCESS) {
				error = KnfcHostWrite(&mode, sizeof(uint32_t));
			}

			goto SRTC_CLEAN_AND_RETURN;
		}

		case P_Idenfier_PReaderErrorEventRegister: {

			tMessage_in_PReaderErrorEventRegister *msg = (tMessage_in_PReaderErrorEventRegister *)pCmdMessage->pPayload;
			error = KnfcRegisterReaderErrorEvent(msg->nEventType, msg->bCardDetectionRequested);
			break;
		}		

		case P_Idenfier_PReaderDriverRegister: {

			tMessage_in_PReaderDriverRegister *msg = (tMessage_in_PReaderDriverRegister *)pCmdMessage->pPayload;
			error = KnfcRegisterReaderDriver(msg->nRequestedProtocolsBF, msg->nPriority);
			break;
		}

#ifdef INCLUDE_DEPRECATED_FUNCTIONS
		case P_Idenfier_P14P3DriverSetTimeout: {

			tMessage_in_P14P3DriverSetTimeout *msg = (tMessage_in_P14P3DriverSetTimeout *)pCmdMessage->pPayload;			
			error = KnfcSetP14P3DriverTimeout((void *)msg->hConnection, msg->nTimeout);
			break;
		}

		case P_Idenfier_P14P4DriverSetTimeout: {

			tMessage_in_P14P4DriverSetTimeout *msg = (tMessage_in_P14P4DriverSetTimeout *)pCmdMessage->pPayload;			
			error = KnfcSetP14P4DriverTimeout((void *)msg->hConnection, msg->nTimeout);
			break;
		}

		case P_Idenfier_P15P3DriverSetTimeout: {

			tMessage_in_P15P3DriverSetTimeout *msg = (tMessage_in_P15P3DriverSetTimeout *)pCmdMessage->pPayload;			
			error = KnfcSetP15P3DriverTimeout((void *)msg->hConnection, msg->nTimeout);
			break;
		}		
#endif

#ifdef HARDWARE_P14P4_SUPPORT
		case P_Idenfier_P14P4DriverExchangeData: {
			tMessage_in_P14P4DriverExchangeData *msg = (tMessage_in_P14P4DriverExchangeData *)pCmdMessage->pPayload;

			// Read the data from the reader to card
			paramBufferLength = msg->nReaderToCardBufferLength;
			error = KnfcHostRead(paramBuffer, paramBufferLength);

			if(error == KNFC_SUCCESS) {
				error = KnfcExchangeP14P4DriverData((void *)msg->hDriverConnection, paramBuffer, paramBufferLength, 
													msg->bSendNAD, msg->nNAD);
			}
			break;
		}
#endif

		case P_Idenfier_P14P3DriverExchangeData: {
			tMessage_in_P14P3DriverExchangeData *msg = (tMessage_in_P14P3DriverExchangeData *)pCmdMessage->pPayload;

			// Read the data from the reader to card
			paramBufferLength = msg->nReaderToCardBufferLength;
			error = KnfcHostRead(paramBuffer, paramBufferLength);

			if(error == KNFC_SUCCESS) {
				error = KnfcExchangeP14P3DriverData((void *)msg->hDriverConnection, paramBuffer, paramBufferLength, 
													msg->bCheckResponseCRC, msg->bCheckAckOrNack);
			}
			break;
		}		

#ifdef HARDWARE_P15P3_SUPPORT
		case P_Idenfier_P15P3DriverExchangeData: {
			tMessage_in_P15P3DriverExchangeData *msg = (tMessage_in_P15P3DriverExchangeData *)pCmdMessage->pPayload;

			// Read the data from the reader to card
			paramBufferLength = msg->nReaderToCardBufferLength;
			error = KnfcHostRead(paramBuffer, paramBufferLength);

			if(error == KNFC_SUCCESS) {
				error = KnfcExchangeP15P3DriverData((void *)msg->hDriverConnection, paramBuffer, paramBufferLength);
			}
			break;
		}			
#endif		
		case P_Idenfier_PEmulOpenConnectionDriver1Ex: {

			uint32_t handle = 0;
			tWEmulConnectionInfo emulConnInfo;
			tMessage_in_PEmulOpenConnectionDriver1Ex *msg;

			msg = (tMessage_in_PEmulOpenConnectionDriver1Ex *)pCmdMessage->pPayload;

			// Read extra information data for the Emulation Connection
			error = KnfcHostRead(&emulConnInfo, msg->nSize);
			KNFC_CHECK(sizeof(tWEmulConnectionInfo) == msg->nSize, KNFC_ERROR_CARD_INVALID_CONNECTION_INFO);

			if(error == KNFC_SUCCESS) {
				error = KnfcOpenEmulConnection(&emulConnInfo, (KnfcEmulConnection_t **)&handle);			
			}

			respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(4/* sizeof(uint32_t) */);
			KnfcMemoryCopy(respMessage.pPayload, &handle, 4);
			respMessage.header.nLength = 4;
			break;
		}

		case P_Idenfier_PEmulOpenConnectionDriver2Ex: {

			tMessage_in_PEmulOpenConnectionDriver2Ex *msg = (tMessage_in_PEmulOpenConnectionDriver2Ex *)pCmdMessage->pPayload;
			error = KnfcRegisterCardEmulationEvent(msg->hHandle);
			break;
		}	

		case P_Idenfier_PEmulOpenConnectionDriver3Ex: {

			tMessage_in_PEmulOpenConnectionDriver3Ex *msg = (tMessage_in_PEmulOpenConnectionDriver3Ex *)pCmdMessage->pPayload;
			error = KnfcRegisterCardEmulationCommand(msg->hHandle);
			break;
		}			

		case P_Idenfier_PEmulCloseDriver: {

			tMessage_in_PEmulCloseDriver *msg = (tMessage_in_PEmulCloseDriver *)pCmdMessage->pPayload;
			error = KnfcCloseEmulConnection(msg->hHandle);
			break;
		}

		case P_Idenfier_PEmulGetMessageData: {
			uint8_t *pBuffer = NULL;
			uint32_t nBufferLength = 0;
			tMessage_in_PEmulGetMessageData *msg = (tMessage_in_PEmulGetMessageData *)pCmdMessage->pPayload;
			error = KnfcGetEmulMessageData(msg->hHandle, &pBuffer, &nBufferLength);

			if(error == KNFC_SUCCESS) {
				respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(nBufferLength);
				KnfcMemoryCopy(respMessage.pPayload, pBuffer, nBufferLength);
				respMessage.header.nLength = nBufferLength;
			}
			break;
		}

		case P_Idenfier_PEmulSendAnswer: {

			tMessage_in_PEmulSendAnswer *msg = (tMessage_in_PEmulSendAnswer *)pCmdMessage->pPayload;

			// Read the data from the reader to card
			paramBufferLength = msg->nDataLength;
			error = KnfcHostRead(paramBuffer, paramBufferLength);

			if(error == KNFC_SUCCESS) {
				error = KnfcSendEmulAnswer(msg->hDriverConnection, paramBuffer, paramBufferLength);
			}	

			break;
		}			

		case P_Idenfier_PHandleCloseDriver: {

			tMessage_in_PHandleCloseDriver *msg = (tMessage_in_PHandleCloseDriver *)pCmdMessage->pPayload;

			// Close Handle
			KNFC_DEBUG("SendResponseToCommand", "P_Idenfier_PHandleCloseDriver");
			break;
		}
		
		case P_Idenfier_PReaderDriverSetWorkPerformedAndClose: {

			KnfcCloseReaderDriver();
			break;
		}		
		
		//Ignore the following commands at this time - We don't need to support them.

#ifndef INCLUDE_DEPRECATED_FUNCTIONS 
		case P_Idenfier_P14P3DriverSetTimeout: 
		case P_Idenfier_P14P4DriverSetTimeout:
		case P_Idenfier_P15P3DriverSetTimeout:
#endif
		case P_Idenfier_PContextDriverResetMemoryStatistics:		
		case P_Idenfier_PCacheConnectionDriverRead:
		case P_Idenfier_PCacheConnectionDriverWrite:
		case P_Idenfier_PDFCDriverInterruptEventLoop:
		case P_Identifier_PDFCDriverPumpEvent:
			KNFC_DEBUG("SendResponseToCommand", "Ignored Command Code: %d", cmdHeader->nCode);
			break;
		
		default:
			KNFC_ERROR("SendResponseToCommand - Unknown Command Code: %d", cmdHeader->nCode);
			error = KNFC_ERROR_HOST_UNKNOWN_COMMAND;
			break;
	}

	// Send an acknowledgment
	error = SendAckResponse(cmdHeader->nCode);

	// Send a quick response,  followed by the payload, for notifying the error result 
	if(cmdHeader->nSizeOut != 0) {
		error = KnfcHostWrite(&error, sizeof(uint32_t));
	}

	// Send the full response to the protocol stack if it is needed.
	if(respMessage.header.nLength > 0 && error == KNFC_SUCCESS) {

		// Send the response message header
		error = KnfcHostWrite(&respMessage.header, sizeof(KhstRespHeader_t));

		if(error != KNFC_SUCCESS) {
			goto SRTC_CLEAN_AND_RETURN;
		}
		
		// Send the response message body
		error = KnfcHostWrite(respMessage.pPayload, respMessage.header.nLength);
	}

SRTC_CLEAN_AND_RETURN:
	
	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostResponseToCommand: error - %d", error);
	}

	KnfcMemoryFree(respMessage.pPayload);
	return error;
}

/**
 * This function will be used to asynchronously send the response for the registered lazy event.
 */
 
static KnfcError_t 
SendLazyResponse(KhstRespMessage_t *pRespMessage) {

	KnfcError_t error;
	uint8_t code = P_Idenfier_KhalLazyEvent;

	KNFC_DEBUG("SendLazyResponse", "Message Code: %d", pRespMessage->header.nCode);

	if(pRespMessage->header.nLength <= 0) {
		return KNFC_ERROR_HOST_INVALID_MESSAGE;
	}

	//Send a byte to identify that the following message is for lazy events. 
	error = KnfcHostWrite(&code, 1);

	if(error == KNFC_SUCCESS) {

		// Send the response message header
		error = KnfcHostWrite(&pRespMessage->header, sizeof(KhstRespHeader_t));

		if(error != KNFC_SUCCESS) {
			return error;
		}
		
		// Send the response message body
		error = KnfcHostWrite(pRespMessage->pPayload, pRespMessage->header.nLength);
	}

	return error;
}

//////////////////////////////////////////////////
// Global Function Definitions
//////////////////////////////////////////////////

/** 
 * This function will parse and process the message from the protocol stack.
 * If the data is ready to be read, the function will check the header first.
 * After then it is going to response to the host for the given command.
 *
 * @return error information
 */

KnfcError_t 
KnfcHostProcessMessage() {

	KnfcError_t error;
	int32_t msgLength;
	KhstCmdMessage_t cmdMessage;
		
	// Read the Message Header
	error = KnfcHostRead(&cmdMessage.header, sizeof(KhstCmdHeader_t));

	if(error != KNFC_SUCCESS) {
		return error;
	}

	msgLength = cmdMessage.header.nSizeIn;

	if(msgLength > 0) {

		cmdMessage.pPayload = (uint8_t*)KnfcMemoryAlloc(msgLength);

		if(cmdMessage.pPayload == NULL) {
			return KNFC_ERROR_MEMORY_ALLOC;
		}

		// Read the Message Body
		error = KnfcHostRead(cmdMessage.pPayload, msgLength);

		if(error != KNFC_SUCCESS) {
			goto KHPM_CLEAN_AND_RETURN;
		}
	}
	else {
		cmdMessage.pPayload = NULL;
	}

	KnfcMutexLock(&g_hHostCommMutex);

	// Handle the read message 
	error = SendResponseToCommand(&cmdMessage);

	KnfcMutexUnlock(&g_hHostCommMutex);

	if(error != KNFC_SUCCESS) {
		goto KHPM_CLEAN_AND_RETURN;
	}			

KHPM_CLEAN_AND_RETURN:

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostProcessMessage: error - %d", error);
	}

	KnfcMemoryFree(cmdMessage.pPayload);
	return error;
}

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

/**
 * This function will be used to send the message data directly.
 */
KnfcError_t 
KnfcHostSendMessageData(uint8_t nCode, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KhstRespMessage_t respMessage;

	if(error == KNFC_SUCCESS) {

		respMessage.header.nCode = nCode;
		respMessage.header.nLength = nBufferLength;

		// Send the response message header
		error = KnfcHostWrite(&respMessage.header, sizeof(KhstRespHeader_t));

		if(error != KNFC_SUCCESS) {
			return error;
		}
		
		// Send the response message body
		error = KnfcHostWrite(pBuffer, nBufferLength);
	}

	return error;
}


/**
 * This function will indicate whether there is any lazy response to be transmitted. 
 *
 * @return if the data is ready to be read, it will returns true. otherwise false.
 */

bool_t
KnfcHostReadyToProcessMessage(void) {

	if(KnfcHostDataReady() == FALSE) {
		return FALSE;
	}

	if(g_hLazyResponseQueue.pNext != NULL) {
		return FALSE;
	}
		
	return TRUE;
}

#endif

/**
 * This function will be used to register a lazy response to be sent to the protocol stack.
 *
 * @return error information
 */
 
KnfcError_t
KnfcHostEnqueueLazyResponse(KhstRespMessage_t *pRespMessage) {

#ifdef LINUX_SUPPORT
	return SendLazyResponse(pRespMessage);
#else
	KhstRespMessage_t *respMessage = (KhstRespMessage_t *)KnfcMemoryAlloc(sizeof(KhstRespMessage_t));
	KNFC_CHECK(respMessage != NULL, KNFC_ERROR_MEMORY_ALLOC);

	KnfcMemoryCopy(respMessage, pRespMessage, sizeof(KhstRespMessage_t));

	if(KnfcInsertLastNode(&g_hLazyResponseQueue, respMessage) == NULL) {
		return KNFC_ERROR_HOST_INVALID_LAZY_MESSAGE;
	}
	return KNFC_SUCCESS;
#endif
}

/**
 * This function will transmit all of the registered lazy responses to the host, NFC protocol stack.
 *
 * @return error information
 */

KnfcError_t
KnfcHostSendAllLazyResponses(void) {

	KnfcError_t error = KNFC_SUCCESS;
	KnfcNode_t *prevNode = NULL;
	KnfcNode_t *nextNode = NULL;
	KnfcNode_t *currNode = g_hLazyResponseQueue.pNext;

	for(; currNode != NULL; currNode = nextNode) {

		prevNode = currNode->pPrev;
		nextNode = currNode->pNext;

		if(currNode->pItem == NULL) {
			continue;
		}
			
		error = SendLazyResponse((KhstRespMessage_t *)currNode->pItem);

		if(error != KNFC_SUCCESS) {
			return error;
		}

		// Delete the node just transmitted from the queue.
		prevNode->pNext = nextNode;
		nextNode->pPrev = prevNode;

		KnfcMemoryFree(currNode->pItem);
		KnfcMemoryFree(currNode);
	}
}
