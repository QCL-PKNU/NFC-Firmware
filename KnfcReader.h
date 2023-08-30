/*
 * KnfcReader.h
 *
 *  Created on: Jun 25, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_READER_H_
#define _KNFC_READER_H_

#include "KnfcOsalDebug.h"
#include "KnfcOsalConfig.h"

//////////////////////////////////////////////////
// Macro Definition for NFC Protocol capabilities
//////////////////////////////////////////////////

#define MAX_RETRANS_BUF_LEGNTH	512

#define W_NFCC_PROTOCOL_READER_ISO_14443_4_A		0x00010000
#define W_NFCC_PROTOCOL_READER_ISO_14443_4_B		0x00020000
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_A		0x00040000
#define W_NFCC_PROTOCOL_READER_ISO_14443_3_B		0x00080000
#define W_NFCC_PROTOCOL_READER_ISO_15693_3			0x00100000
#define W_NFCC_PROTOCOL_READER_ISO_15693_2			0x00200000
#define W_NFCC_PROTOCOL_READER_FELICA				0x00400000
#define W_NFCC_PROTOCOL_READER_P2P_INITIATOR		0x00800000
#define W_NFCC_PROTOCOL_READER_TYPE_1_CHIP			0x01000000
#define W_NFCC_PROTOCOL_READER_MIFARE_CLASSIC		0x02000000
#define W_NFCC_PROTOCOL_READER_BPRIME				0x04000000
#define W_NFCC_PROTOCOL_READER_KOVIO				0x08000000
#define W_NFCC_PROTOCOL_READER_MIFARE_PLUS			0x10000000
#define W_NFCC_PROTOCOL_READER_RESERVED0			0x20000000
#define W_NFCC_PROTOCOL_READER_RESERVED1			0x40000000
#define W_NFCC_PROTOCOL_READER_RESERVED2			0x80000000

//////////////////////////////////////////////////
// Callback Function Declaration
//////////////////////////////////////////////////

typedef void (*tReaderErrorEventCB)(void *pParam);

typedef void (*tReaderDriverCardDetectCB)(void *pConnection, uint32_t nDetectedProtcol, uint8_t *pBuffer, uint32_t nBufferLength);

typedef void (*tReaderReceiveP14P4DataCB)(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

typedef void (*tReaderReceiveP14P3DataCB)(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

typedef void (*tReaderReceiveP15P3DataCB)(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

// Data Re-transmission Buffer

typedef struct {

	// Data buffer
	uint8_t aBuffer[MAX_RETRANS_BUF_LEGNTH];

	// Data buffer length
	uint32_t nBufferLength;

} KnfcRetransBuffer_t;

// Reader Error Registry

typedef struct {

	tReaderErrorEventCB cbUnknownCardDetect;
	void *pUnknownCardDetectedParameter;
	
	tReaderErrorEventCB cbCollisionDetect;
	void *pCollisionDetectedParameter;
	
	tReaderErrorEventCB cbMultipleCardDetect;
	void *pMultipleCardDetectedParameter;
	
} KnfcReaderErrorRegistry_t;

typedef struct {

	// Reader Event
	uint8_t nPriority;
	uint32_t nDriverProtocol;
	tReaderDriverCardDetectCB cbCardDetect;
	uint32_t nTimeout;

	// Exchange Data P14P4
	tReaderReceiveP14P4DataCB cbReaderReceiveP14P4;
	bool_t bSendNAD;
	uint8_t nNAD;

	// Exchange Data P14P3
	tReaderReceiveP14P3DataCB cbReaderReceiveP14P3;
	bool_t bCheckCRC;
	bool_t bCheckAck;

	// Exchange Data P15P3
	tReaderReceiveP15P3DataCB cbReaderReceiveP15P3;

} KnfcReaderConnection_t; 

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* Reader Error Event */
KnfcError_t KnfcProcessReaderErrorEvent(uint8_t nErrorType);

KnfcError_t KnfcRegisterReaderErrorEvent(uint8_t nEventType, uint8_t bCardDetectionRequested);

KnfcError_t KnfcUnregisterReaderErrorEvent(uint8_t nEventType);

/* Reader Driver */
KnfcError_t KnfcRegisterReaderDriver(uint32_t nDriverProtocol, uint8_t nPriority);

KnfcError_t KnfcUnregisterReaderDriver(KnfcReaderConnection_t *pConnection);

KnfcError_t KnfcProcessReaderDriverEvent(uint32_t nDetectedProtocol, uint8_t *pBuffer, uint32_t nBufferLength);

/* Reader Driver Close */
KnfcError_t KnfcCloseReaderDriver(void);

/* Protocol Timeout */
#ifdef INCLUDE_DEPRECATED_FUNCTIONS

KnfcError_t KnfcSetP14P3DriverTimeout(void *pConnection, uint32_t nTimeOut);

KnfcError_t KnfcSetP14P4DriverTimeout(void *pConnection, uint32_t nTimeOut);

KnfcError_t KnfcSetP15P3DriverTimeout(void *pConnection, uint32_t nTimeOut);

#endif

#ifdef HARDWARE_P14P4_SUPPORT

/* 14443-4 Data Exchange */
KnfcError_t KnfcProcessReaderExchangeP14P4Data(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcExchangeP14P4DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength, bool_t bSendNAD, uint8_t nNAD);

#endif

/* 14443-3 Data Exchange */
KnfcError_t KnfcProcessReaderExchangeP14P3Data(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcExchangeP14P3DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength, bool_t bCheckCRC, bool_t bCheckAck);

#ifdef HARDWARE_P15P3_SUPPORT

/* 15693-3 Data Exchange */
KnfcError_t KnfcProcessReaderExchangeP15P3Data(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcExchangeP15P3DriverData(void *pConnection, void *pBuffer, uint32_t nBufferLength);

#endif

#endif /* _KNFC_READER_H_ */
