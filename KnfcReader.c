/*
 * KnfcReader.c
 *
 *  Created on: Jun 25, 2013
 *      Author: youngsun
 */

#include "KnfcReader.h"

#include "KnfcIso15693.h"
#include "KnfcIso14443A.h"
#include "KnfcIso14443B.h"

#include "KnfcHostCmd.h"
#include "KnfcModeSwitch.h"

#ifdef DUMMY_MODEM_SUPPORT
#include "KnfcDummyModem.h"

static KnfcDummyData_t dummyData;
#endif

//////////////////////////////////////////////////
// Static Variable Definition
//////////////////////////////////////////////////

// The current connection
static KnfcReaderConnection_t *g_pReaderConnection = NULL;

// Registery of the callback functions for Reader Error Events
static KnfcReaderErrorRegistry_t g_hReaderErrorRegistry = {NULL, NULL, NULL, NULL, NULL, NULL};

// Registery for the reader connection
static KnfcNode_t g_hReaderConnectionRegistry = {NULL, NULL, NULL}; 

//////////////////////////////////////////////////
// Static Function Declaration
//////////////////////////////////////////////////

/* Reader Error Event */
static void DetectUnknownCardCB(void *pParam); 

static void DetectCollisionCB(void *pParam);

static void DetectMultipleCardCB(void *pParam);

/* Reader Driver Event */
static void ReaderDriverDetectCardCB(void *pConnection, uint32_t nDetectedProtocol, uint8_t *pBuffer, uint32_t nBufferLength);

static KnfcReaderConnection_t *CreateReaderConnection(uint32_t nDriverProtocol, uint8_t nPriority);

static KnfcReaderConnection_t *FindReaderConnection(uint32_t nDetectedProtocol);

#ifdef HARDWARE_P14P4_SUPPORT
/* Data Exchange - ISO14443-4 */
static void ReaderReceiveP14P4DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);
#endif

/* Data Exchange - ISO14443-4 */
static void ReaderReceiveP14P3DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

#ifdef HARDWARE_P15P3_SUPPORT
/* Data Exchange - ISO15693-3 */
static void ReaderReceiveP15P3DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);
#endif

#ifdef INCLUDE_DEPRECATED_FUNCTIONS
/* Time Out Configuration */
static bool_t SetReaderDriverTimeOut(void *pConnection, uint32_t nTimeOut);
#endif

//////////////////////////////////////////////////
// For Reader Error Event
//////////////////////////////////////////////////

/**
 * Callback function for unknown card detection
 */
 
static void 
DetectUnknownCardCB(void *pParam) {

	KhstRespMessage_t respMessage;
	uint8_t error = W_READER_ERROR_UNKNOWN;

	KNFC_DEBUG("DetectUnknownCardCB", "CALLBACK EXECUTED");

	respMessage.header.nCode = P_Idenfier_PReaderErrorEventRegister;
	respMessage.header.nLength = 1 /*sizeof(uint8_t)*/;
	respMessage.pPayload = (uint8_t *)&error;
		
	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("DetectUnknownCardCB - KnfcHostEnqueueLazyResponse");		
	}

	// YOUNGSUN - CHKME
	// Do something internally for the unknown card detection event 
}

/**
 * Callback function for collision detection
 */
 
static void 
DetectCollisionCB(void *pParam) {

	KhstRespMessage_t respMessage;
	uint8_t error = W_READER_ERROR_COLLISION;

	KNFC_DEBUG("DetectCollisionCB", "CALLBACK EXECUTED");

	respMessage.header.nCode = P_Idenfier_PReaderErrorEventRegister;
	respMessage.header.nLength = 1 /*sizeof(uint8_t)*/;
	respMessage.pPayload = (uint8_t *)&error;

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("DetectCollisionCB - KnfcHostEnqueueLazyResponse");		
	}

	// YOUNGSUN - CHKME
	// Do something internally for the collision detection event 
}

/**
 * Callback function for multiple card detection
 */
 
static void 
DetectMultipleCardCB(void *pParam) {

	KhstRespMessage_t respMessage;
	uint8_t error = W_READER_MULTIPLE_DETECTION;

	KNFC_DEBUG("DetectMultipleCardCB", "CALLBACK EXECUTED");

	respMessage.header.nCode = P_Idenfier_PReaderErrorEventRegister;
	respMessage.header.nLength = 1 /*sizeof(uint8_t)*/;
	respMessage.pPayload = (uint8_t *)&error;

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("DetectMultipleCardCB - KnfcHostEnqueueLazyResponse");		
	}

	// YOUNGSUN - CHKME
	// Do something internally for the multiple card detection event 	
}

/**
 * This function will be used to register an event handler for the specified reader error event
 *
 * @param nEventType error event type
 * @param bCardDetectionRequested unused
 * @return error information
 */
KnfcError_t
KnfcRegisterReaderErrorEvent(uint8_t nEventType, uint8_t bCardDetectionRequested) {

	KNFC_DEBUG("KnfcRegisterReaderErrorEvent", "FUNC STA - %d", nEventType);

	switch(nEventType) {
		case W_READER_ERROR_COLLISION: 
			g_hReaderErrorRegistry.cbCollisionDetect = DetectCollisionCB;
			g_hReaderErrorRegistry.pCollisionDetectedParameter = (void *)bCardDetectionRequested;
			break;

		case W_READER_ERROR_UNKNOWN:
			g_hReaderErrorRegistry.cbUnknownCardDetect = DetectUnknownCardCB;
			g_hReaderErrorRegistry.pUnknownCardDetectedParameter = (void *)bCardDetectionRequested;
			break;

		case W_READER_MULTIPLE_DETECTION:
			g_hReaderErrorRegistry.cbMultipleCardDetect = DetectMultipleCardCB;
			g_hReaderErrorRegistry.pMultipleCardDetectedParameter = (void *)bCardDetectionRequested;
			break;
	}

	//YOUNGSUN - CHKME
	//We used the dummy event generation routine to verify handling Reader Error Events.
#ifdef TEST_DUMMY_READER_ERROR_EVENT	
	KnfcRegisterDummyReaderErrorEvent();
#endif

	//YOUNGSUN - CHKME
	//We have to add some procedures to configure that 
	//the underlying NFC controller will handle the registered error events.
	//The routines should be written by following the implementation of the controller.

	KNFC_DEBUG("KnfcRegisterReaderErrorEvent", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function will be used to unregister the event handler for the specified reader event
 *
 * @param nEventType error event type to be unregistered
 * @return error information
 */
 
KnfcError_t
KnfcUnregisterReaderErrorEvent(uint8_t nEventType) {

	KNFC_DEBUG("KnfcUnregisterReaderErrorEvent", "FUNC STA - %d", nEventType);

	switch(nEventType) {
		case W_READER_ERROR_COLLISION: 
			g_hReaderErrorRegistry.cbCollisionDetect = NULL;
			g_hReaderErrorRegistry.pCollisionDetectedParameter = NULL;
			break;

		case W_READER_ERROR_UNKNOWN:
			g_hReaderErrorRegistry.cbUnknownCardDetect = NULL;
			g_hReaderErrorRegistry.pUnknownCardDetectedParameter = NULL;
			break;

		case W_READER_MULTIPLE_DETECTION:
			g_hReaderErrorRegistry.cbMultipleCardDetect = NULL;
			g_hReaderErrorRegistry.pMultipleCardDetectedParameter = NULL;
			break;
	}

	KNFC_DEBUG("KnfcUnregisterReaderErrorEvent", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function will process the reader error event with the given error type. 
 *
 * @param nErrorType error event type to be handled
 * @return error information
 */
 
KnfcError_t
KnfcProcessReaderErrorEvent(uint8_t nErrorType) {

	switch(nErrorType) {
		case W_READER_ERROR_UNKNOWN:
			if(g_hReaderErrorRegistry.cbUnknownCardDetect != NULL) {
				g_hReaderErrorRegistry.cbUnknownCardDetect(g_hReaderErrorRegistry.pUnknownCardDetectedParameter);
			}
			break;
			
		case W_READER_ERROR_COLLISION:
			if(g_hReaderErrorRegistry.cbCollisionDetect != NULL) {
				g_hReaderErrorRegistry.cbCollisionDetect(g_hReaderErrorRegistry.pCollisionDetectedParameter);
			}
			break;
			
		case W_READER_MULTIPLE_DETECTION:
			if(g_hReaderErrorRegistry.cbMultipleCardDetect != NULL) {
				g_hReaderErrorRegistry.cbMultipleCardDetect(g_hReaderErrorRegistry.pMultipleCardDetectedParameter);
			}
			break;

		default:
			KNFC_ERROR("Unknown Reader Error Event: %d", nErrorType);
			return KNFC_ERROR_HOST_UNKNOWN_READER_ERROR;
	}

	return KNFC_SUCCESS;
}

//////////////////////////////////////////////////
// For Card Detection Event
//////////////////////////////////////////////////

/**
 * Callback function for handling the card detection
 */
static void
ReaderDriverDetectCardCB(void *pConnection, uint32_t nDetectedProtocol, uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t connInfoSize;
	KhstRespMessage_t respMessage;
	KnfcReaderConnection_t *connection = (KnfcReaderConnection_t *)pConnection;

	struct DetectedConnectionInfo_s {
		uint8_t nPriority;
		uint32_t nDriverProtocol;
		uint32_t nDetectedProtocol;
		uint32_t hConnection;
		uint32_t nBufferLength;
	} connInfo;
	
	KNFC_DEBUG("ReaderDriverDetectCardCB", "CALLBACK EXECUTED");
	KNFC_ASSERT(pConnection != NULL, "Invalid Reader Connection");

	// Build the connection information and put it in front of the response message.
	connInfo.nPriority = connection->nPriority;
	connInfo.nDriverProtocol = connection->nDriverProtocol;
	connInfo.nDetectedProtocol = nDetectedProtocol;
	connInfo.hConnection = (uint32_t)pConnection;
	connInfo.nBufferLength = nBufferLength;

	connInfoSize = sizeof(struct DetectedConnectionInfo_s);

	// Build the whole response message including payload
	respMessage.header.nCode = P_Idenfier_PReaderDriverRegister;
	respMessage.header.nLength = nBufferLength + connInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(respMessage.header.nLength);

	if(respMessage.pPayload == NULL) {
		KNFC_ERROR("ReaderDriverDetectCardCB - Memory Alloc");		
		return;
	}

	KnfcMemoryCopy(&respMessage.pPayload[0], &connInfo, connInfoSize);

	if(pBuffer != NULL && nBufferLength > 0) {
		KnfcMemoryCopy(&respMessage.pPayload[connInfoSize], pBuffer, nBufferLength);
	}

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("ReaderDriverDetectCardCB - KnfcHostEnqueueLazyResponse");		
	}

	// YOUNGSUN - CHKME
	// Do something internally for the card detection event 
	g_pReaderConnection = pConnection;
}

/**
 * Create a new reader connection and append to the registry
 */
 
static KnfcReaderConnection_t *
CreateReaderConnection(uint32_t nDriverProtocol, uint8_t nPriority) {

	KnfcReaderConnection_t *connection;

	KNFC_CHECK(nDriverProtocol != 0, NULL);
	
	connection = (KnfcReaderConnection_t *)KnfcMemoryAlloc(sizeof(KnfcReaderConnection_t));

	if(connection != NULL) {
		connection->cbCardDetect = ReaderDriverDetectCardCB;
		connection->nDriverProtocol = nDriverProtocol;
		connection->nPriority = nPriority;
		
		KnfcInsertNode(&g_hReaderConnectionRegistry, connection);
	}

	return connection;
}

/**
 * Find the reader connection for the current card detection event.
 */

static KnfcReaderConnection_t *
FindReaderConnection(uint32_t nDetectedProtocol) {

	KnfcReaderConnection_t *connection = NULL;
	KnfcNode_t *currNode = g_hReaderConnectionRegistry.pNext;

	KNFC_CHECK(nDetectedProtocol != 0, NULL); 

	for(; currNode != NULL; currNode = currNode->pNext) {
		
		KnfcReaderConnection_t *tmpConn = (KnfcReaderConnection_t *)currNode->pItem;

		if((tmpConn->nDriverProtocol & nDetectedProtocol) == 0) {
			continue;
		}

		if((connection == NULL) || (connection->nPriority < tmpConn->nPriority)) {
			connection = tmpConn;
		}
	}

	return connection;
}

/**
 * This function will be used to register a reader driver to listen the card detection.
 *
 * @param nDriverProtocol reader protocols to detect the card
 * @param nPriority the priority of the drivers
 * @return error information
 */
 
KnfcError_t
KnfcRegisterReaderDriver(uint32_t nDriverProtocol, uint8_t nPriority) {

	KnfcReaderConnection_t *connection;

	KNFC_DEBUG("KnfcRegisterReaderDriver", "FUNC STA - Protocol: %X, Priority: %d", nDriverProtocol, nPriority);

	connection = CreateReaderConnection(nDriverProtocol, nPriority);

	if(connection == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// Register the driver protocol to be activated by the mode switching.
	KnfcRegisterNfccProtocol(nDriverProtocol, NULL);

	//YOUNGSUN - CHKME
	//We used the dummy event generation routine to verify handling Reader Driver Events.
#ifdef TEST_DUMMY_READER_DRIVER_EVENT	
	KnfcRegisterDummyReaderDriverEvent();
#endif
	
	KNFC_DEBUG("KnfcRegisterReaderDriver", "FUNC END");
	return KNFC_SUCCESS;
}

/**
 * This function will unregister the specified reader driver.
 *
 * @param nDriverProtocol reader protocols to be unregistered
 * @return error information
 */
 
KnfcError_t
KnfcUnregisterReaderDriver(KnfcReaderConnection_t *pConnection) {

	KnfcError_t error;
	KnfcNode_t *currNode = NULL;
	uint32_t nOldDriverProtocol = 0;
	uint32_t nNewDriverProtocol = 0;
	KnfcReaderConnection_t *connection = NULL;

	KNFC_DEBUG("KnfcUnregisterReaderDriver", "FUNC STA - Connection: %X", pConnection);

	if(g_pReaderConnection == NULL) {
		return KnfcUnregisterNfccProtocol(0xFFFFFFFF);
	}

	// The old driver protocols
	nOldDriverProtocol = g_pReaderConnection->nDriverProtocol;

	if(pConnection == g_pReaderConnection) {
		g_pReaderConnection = NULL;
	}

	// Delete the connection from the registry
	if(KnfcDeleteNode(&g_hReaderConnectionRegistry, pConnection) == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// Kick out the protocols of only the unregistered driver
	currNode = g_hReaderConnectionRegistry.pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {

		connection = (KnfcReaderConnection_t *)currNode->pItem;

		if(connection != NULL) {
			nNewDriverProtocol |= connection->nDriverProtocol;
		}
	}
	
	// Configure the mode switching
	KNFC_DEBUG("KnfcUnregisterReaderDriver", "OLD: %x", nOldDriverProtocol);
	KNFC_DEBUG("KnfcUnregisterReaderDriver", "NEW: %x", nNewDriverProtocol);
	
	error = KnfcUnregisterNfccProtocol((nOldDriverProtocol ^ nNewDriverProtocol));
	KNFC_CHECK(error == KNFC_SUCCESS, error);

	KNFC_DEBUG("KnfcUnregisterReaderDriver", "FUNC END");
	return error;
}

/**
 * This function will process the card detection event with the specified protocol. 
 *
 * @param nDetectedProtocol the detected protocol
 * @param pBuffer a data buffer for the detected tag information
 * @param nBufferLength the buffer length
 */
 
KnfcError_t
KnfcProcessReaderDriverEvent(uint32_t nDetectedProtocol, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcReaderConnection_t *connection = FindReaderConnection(nDetectedProtocol);

	if(connection == NULL) {
		KNFC_ERROR("Unregistered Reader Protocol Detected: 0x%X", nDetectedProtocol);		
		return KNFC_ERROR_READER_UNREGISTERED_PROTOCOL;
	}

	if(connection->cbCardDetect != NULL) {
		connection->cbCardDetect(connection, nDetectedProtocol, pBuffer, nBufferLength);
	}

	return KNFC_SUCCESS;
}

/**
 * This function will close the reader connection. 
 *
 * @param pConnection the connection to be closed
 * @return error information
 */
 
KnfcError_t 
KnfcCloseReaderDriver(void) {

	KNFC_DEBUG("KnfcCloseReaderDriver", "FUNC STA - pConnection: %X", g_pReaderConnection);

#ifdef DUMMY_MODEM_SUPPORT
	g_bStopped = TRUE;
#endif

	return KnfcUnregisterReaderDriver(g_pReaderConnection);
}

//////////////////////////////////////////////////
// For Data Exchange for P14P4
//////////////////////////////////////////////////

#ifdef HARDWARE_P14P4_SUPPORT

/**
 * Callback function for handling to receive the P14P4 data
 */
static void
ReaderReceiveP14P4DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t dataInfoSize;
	KhstRespMessage_t respMessage;

	struct ReceivedP14P4DataInfo_s {
		uint32_t hConnection;
		uint32_t nBufferLength;
	} dataInfo;
	
	KNFC_DEBUG("ReaderReceiveP14P4DataCB", "CALLBACK EXECUTED");
	KNFC_ASSERT(pConnection != NULL, "Invalid Reader Connection");

	// Build the connection information and put it in front of the response message.
	dataInfo.hConnection = (uint32_t)pConnection;
	dataInfo.nBufferLength = nBufferLength;

	dataInfoSize = sizeof(struct ReceivedP14P4DataInfo_s);

	// Build the whole response message including payload
	respMessage.header.nCode = P_Idenfier_P14P4DriverExchangeData;
	respMessage.header.nLength = nBufferLength + dataInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(respMessage.header.nLength);

	if(respMessage.pPayload == NULL) {
		KNFC_ERROR("ReaderReceiveP14P4DataCB - Memory Alloc");		
		return;
	}

	KnfcMemoryCopy(&respMessage.pPayload[0], &dataInfo, dataInfoSize);

	if(pBuffer != NULL && nBufferLength > 0) {
		KnfcMemoryCopy(&respMessage.pPayload[dataInfoSize], pBuffer, nBufferLength);
	}

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("ReaderReceiveP14P4DataCB - KnfcHostEnqueueLazyResponse");		
	}
}

/**
 * This function will process the received data from the PICC. 
 */
 
KnfcError_t
KnfcProcessReaderExchangeP14P4Data(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcReaderConnection_t *connection = g_pReaderConnection;

	if(connection == NULL) {
		KNFC_ERROR("Invalid Reader Connection Error: 0x%X", connection);		
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	if(connection->cbReaderReceiveP14P4 != NULL) {
		connection->cbReaderReceiveP14P4(connection, pBuffer, nBufferLength);
	}

	return KNFC_SUCCESS;
}

/**
 * This function will be used to exchange the data with the currently connected PICC by the PCD. 
 */
 
KnfcError_t
KnfcExchangeP14P4DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength, bool_t bSendNAD, uint8_t nNAD) {

	KnfcError_t error;
	KnfcReaderConnection_t *connection;

	KNFC_DEBUG("KnfcExchangeP14P4DriverData", "FUNC STA - pConnection: %X, bSendNAD: %d, nNAD: %d", pConnection, bSendNAD, nNAD);
	
	if(pConnection == NULL || pConnection != g_pReaderConnection) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	connection = g_pReaderConnection;

	connection->cbReaderReceiveP14P4 = ReaderReceiveP14P4DataCB;
	connection->bSendNAD = bSendNAD;
	connection->nNAD = nNAD;

	// Exchange data with the NFC Controller
	error = KnfcExchangeData(connection, pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		return error;
	}

	//YOUNGSUN - CHKME
#ifdef TEST_DUMMY_READER_EXCHANGE_DATA	
	KnfcDummyExchangeP14P4Data(NULL);
#endif
		
	KNFC_DEBUG("KnfcExchangeP14P4DriverData", "FUNC END");	
	return KNFC_SUCCESS;
}

#endif

//////////////////////////////////////////////////
// For Setting Timeout of ISO 14443 Part 3 
//////////////////////////////////////////////////

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

static bool_t 
SetReaderDriverTimeOut(void *pConnection, uint32_t nTimeOut) {

	KnfcReaderConnection_t *connection = NULL;
	KnfcNode_t *currNode = g_hReaderConnectionRegistry.pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {
		if(pConnection == currNode->pItem) {
			connection = (KnfcReaderConnection_t *)pConnection;
			break;
		}
	}
	
	if(connection != NULL) {
		KNFC_DEBUG("SetDriverTimeOut", "Connection found");		
		connection->nTimeout = nTimeOut;
	}
	else {
		KNFC_ERROR("SetDriverTimeOut - Connection not found.");
		return FALSE;
	}

	return TRUE;
}

KnfcError_t
KnfcSetP14P3DriverTimeout(void *pConnection, uint32_t nTimeOut) {

	if(SetReaderDriverTimeOut(pConnection, nTimeOut) == FALSE) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	//YOUNGSUN - CHKME
	//Do something only for the protocol: 14443-3

	return KNFC_SUCCESS;
}

KnfcError_t
KnfcSetP14P4DriverTimeout(void *pConnection, uint32_t nTimeOut) {

	if(SetReaderDriverTimeOut(pConnection, nTimeOut) == FALSE) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	//YOUNGSUN - CHKME
	//Do something only for the protocol: 14443-4

	return KNFC_SUCCESS;
	}
	

KnfcError_t
KnfcSetP15P3DriverTimeout(void *pConnection, uint32_t nTimeOut) {

	if(SetReaderDriverTimeOut(pConnection, nTimeOut) == FALSE) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	//YOUNGSUN - CHKME
	//Do something only for the protocol: 15693-3
	
	return KNFC_SUCCESS;
}

#endif

//////////////////////////////////////////////////
// For Data Exchange for P14P3
//////////////////////////////////////////////////

/**
 * Callback function for handling to receive the P14P3 data
 */
static void
ReaderReceiveP14P3DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t dataInfoSize;
	KhstRespMessage_t respMessage;

	struct ReceivedP14P3DataInfo_s {
		uint32_t hConnection;
		uint32_t nBufferLength;
	} dataInfo;
	
	KNFC_DEBUG("ReaderReceiveP14P3DataCB", "CALLBACK EXECUTED");
	KNFC_ASSERT(pConnection != NULL, "Invalid Reader Connection");

	// Build the connection information and put it in front of the response message.
	dataInfo.hConnection = (uint32_t)pConnection;
	dataInfo.nBufferLength = nBufferLength;

	dataInfoSize = sizeof(struct ReceivedP14P3DataInfo_s);

	// Build the whole response message including payload
	respMessage.header.nCode = P_Idenfier_P14P3DriverExchangeData;
	respMessage.header.nLength = nBufferLength + dataInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(respMessage.header.nLength);

	if(respMessage.pPayload == NULL) {
		KNFC_ERROR("ReaderReceiveP14P3DataCB - Memory Alloc");		
		return;
	}

	KnfcMemoryCopy(&respMessage.pPayload[0], &dataInfo, dataInfoSize);

	if(pBuffer != NULL && nBufferLength > 0) {
		KnfcMemoryCopy(&respMessage.pPayload[dataInfoSize], pBuffer, nBufferLength);
	}

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("ReaderReceiveP14P3DataCB - KnfcHostEnqueueLazyResponse");		
	}
}

/**
 * This function will process the received data from the PICC. 
 *
 * @param pBuffer a data buffer to be transmitted to the detected PICC
 * @param nBufferLength the buffer length
 * @return error information
 */
 
KnfcError_t
KnfcProcessReaderExchangeP14P3Data(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcReaderConnection_t *connection = g_pReaderConnection;

	KNFC_DEBUG("KnfcProcessReaderExchangeP14P3Data", "FUNC STA - Length: %d", nBufferLength);

	if(connection == NULL) {
		KNFC_ERROR("Invalid Reader Connection Error: 0x%X", connection);		
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	if(connection->cbReaderReceiveP14P3 != NULL) {
		connection->cbReaderReceiveP14P3(connection, pBuffer, nBufferLength);
	}

	return KNFC_SUCCESS;
}

/**
 * This function will be used to exchange the data with the currently connected PICC by the PCD. 
 *
 * @param pConnection the currently established connection to an adjacent PICC
 * @param pBuffer the data buffer to be transmitted to the selected PICC
 * @param nBufferLength the length of the data buffer
 * @param bCheckCRC if checking CRC is needed
 * @param bCheckAck if checking ACK is needed
 * @return error information
 */

KnfcError_t
KnfcExchangeP14P3DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength, bool_t bCheckCRC, bool_t bCheckAck) {

	KnfcError_t error;
	KnfcReaderConnection_t *connection;

	KNFC_DEBUG("KnfcExchangeP14P3DriverData", "FUNC STA - pConnection: %X", pConnection);
	
	if(pConnection == NULL || pConnection != g_pReaderConnection) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	connection = g_pReaderConnection;

	connection->cbReaderReceiveP14P3 = ReaderReceiveP14P3DataCB;
	connection->bCheckCRC = bCheckCRC;
	connection->bCheckAck = bCheckAck;

	// Exchange data with the NFC Controller
	error = KnfcExchangeData(connection, pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		return error;
	}	

	//YOUNGSUN - CHKME
#ifdef TEST_DUMMY_READER_EXCHANGE_DATA	
	dummyData.pBuffer = (uint8_t *)malloc(nBufferLength);
	memcpy(dummyData.pBuffer, pBuffer, nBufferLength);
	dummyData.nBufferLength = nBufferLength;

	KnfcDummyExchangeP14P3Data(&dummyData);
#endif
		
	KNFC_DEBUG("KnfcExchangeP14P3DriverData", "FUNC END");	
	return KNFC_SUCCESS;
}

//////////////////////////////////////////////////
// For Data Exchange for P15P3
//////////////////////////////////////////////////

#ifdef HARDWARE_P15P3_SUPPORT

/**
 * Callback function for handling to receive the P15P3 data
 */
static void
ReaderReceiveP15P3DataCB(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t dataInfoSize;
	KhstRespMessage_t respMessage;

	struct ReceivedP15P3DataInfo_s {
		uint32_t hConnection;
		uint32_t nBufferLength;
	} dataInfo;
	
	KNFC_DEBUG("ReaderReceiveP15P3DataCB", "CALLBACK EXECUTED");
	KNFC_ASSERT(pConnection != NULL, "Invalid Reader Connection");

	// Build the connection information and put it in front of the response message.
	dataInfo.hConnection = (uint32_t)pConnection;
	dataInfo.nBufferLength = nBufferLength;

	dataInfoSize = sizeof(struct ReceivedP15P3DataInfo_s);

	// Build the whole response message including payload
	respMessage.header.nCode = P_Idenfier_P15P3DriverExchangeData;
	respMessage.header.nLength = nBufferLength + dataInfoSize;
	respMessage.pPayload = (uint8_t *)KnfcMemoryAlloc(respMessage.header.nLength);

	if(respMessage.pPayload == NULL) {
		KNFC_ERROR("ReaderReceiveP15P3DataCB - Memory Alloc");		
		return;
	}

	KnfcMemoryCopy(&respMessage.pPayload[0], &dataInfo, dataInfoSize);

	if(pBuffer != NULL && nBufferLength > 0) {
		KnfcMemoryCopy(&respMessage.pPayload[dataInfoSize], pBuffer, nBufferLength);
	}

	if(KnfcHostEnqueueLazyResponse(&respMessage) != KNFC_SUCCESS) {
		KNFC_ERROR("ReaderReceiveP15P3DataCB - KnfcHostEnqueueLazyResponse");		
	}
}

/**
 * This function will process the received data from the VICC. 
 *
 * @param pBuffer a data buffer to be transmitted to the detected VICC
 * @param nBufferLength the buffer length
 * @return error information
 */
 
KnfcError_t
KnfcProcessReaderExchangeP15P3Data(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcReaderConnection_t *connection = g_pReaderConnection;

	KNFC_DEBUG("KnfcProcessReaderExchangeP15P3Data", "FUNC STA - Length: %d", nBufferLength);

	if(connection == NULL) {
		KNFC_ERROR("Invalid Reader Connection Error: 0x%X", connection);		
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	if(connection->cbReaderReceiveP15P3 != NULL) {
		connection->cbReaderReceiveP15P3(connection, pBuffer, nBufferLength);
	}

	return KNFC_SUCCESS;
}

/**
 * This function will be used to exchange the data with the currently connected VICC by the VCD. 
 *
 * @param pConnection the currently established connection to an adjacent VICC
 * @param pBuffer the data buffer to be transmitted to the selected VICC
 * @param nBufferLength the length of the data buffer
 * @return error information
 */

KnfcError_t
KnfcExchangeP15P3DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KnfcReaderConnection_t *connection;

	KNFC_DEBUG("KnfcExchangeP15P3DriverData", "FUNC STA - pConnection: %X", pConnection);
	
	if(pConnection == NULL || pConnection != g_pReaderConnection) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	connection = g_pReaderConnection;

	connection->cbReaderReceiveP15P3 = ReaderReceiveP15P3DataCB;

	// Exchange data with the NFC Controller
	error = KnfcExchangeData(connection, pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		return error;
	}	

	//YOUNGSUN - CHKME
#ifdef TEST_DUMMY_READER_EXCHANGE_DATA	
	dummyData.pBuffer = (uint8_t *)malloc(nBufferLength);
	memcpy(dummyData.pBuffer, pBuffer, nBufferLength);
	dummyData.nBufferLength = nBufferLength;

	KnfcDummyExchangeP15P3Data(&dummyData);
#endif
		
	KNFC_DEBUG("KnfcExchangeP15P3DriverData", "FUNC END");	
	return KNFC_SUCCESS;
}

#endif
