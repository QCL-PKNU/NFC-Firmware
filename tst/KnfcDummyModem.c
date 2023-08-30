/*
 * KnfcDummyModem.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcDummyModem.h"
#include "KnfcHostCmd.h"
#include "KnfcReader.h"
#include "KnfcCardEmul.h"
#include "KnfcOsalDebug.h"
#include "KnfcModeSwitch.h"

#include "KnfcIso14443A.h"
#include "KnfcIso14443B.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

static bool_t g_bExecuted = FALSE;
static pthread_t g_hEventThread;
static pthread_mutex_t g_hEventThreadMutex;

bool_t g_bStopped = FALSE;

////////////////////////////////////////////////////////
// Dummy thread initialization
////////////////////////////////////////////////////////

void 
KnfcInitializeDummyEventThread() {
	pthread_mutex_init(&g_hEventThreadMutex, NULL);
}

////////////////////////////////////////////////////////
// To test the Reader Error Event Handling
////////////////////////////////////////////////////////

static void *
DummyReaderErrorEventThread(void *pParam) {

	KNFC_DEBUG("DummyReaderErrorEventThread", "Start Thread...");

	//delay a few seconds before occurring the error event.
	sleep(5);
	
	KnfcProcessReaderErrorEvent(W_READER_ERROR_UNKNOWN);
	return NULL;
}

void
KnfcRegisterDummyReaderErrorEvent() {

	KNFC_DEBUG("KnfcRegisterDummyReaderErrorEvent", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummyReaderErrorEventThread, NULL);

	KNFC_DEBUG("KnfcRegisterDummyReaderErrorEvent", "FUNC END");
}

////////////////////////////////////////////////////////
// To test the Reader Driver Event Handling
////////////////////////////////////////////////////////

static void *
DummyReaderDriverEventThread(void *pParam) {

	pthread_mutex_lock(&g_hEventThreadMutex);

#if 0	//P14P3A - NDEF TYPE 2
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3A];
	uint32_t infoBufferLength = 0;

	KNFC_DEBUG("DummyReaderDriverEventThread", "Start Thread...");

	//delay a few seconds before occurring the event.
	sleep(5);

	KnfcGetProtocolInfoBytes_P14P3A(infoBuffer, &infoBufferLength);	
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_A, infoBuffer, infoBufferLength);
#endif

#if 0	//P14P3B
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3B];
	uint32_t infoBufferLength = 0;
	
	KNFC_DEBUG("DummyReaderDriverEventThread", "Start Thread...");

	//delay a few seconds before occurring the error event.
	sleep(5);

	KnfcGetProtocolInfoBytes_P14P3B(infoBuffer, &infoBufferLength);	
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_B, infoBuffer, infoBufferLength);
#endif

#if 0	//P14P4A - NDEF TYPE 4A - standalone
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P4A];
	uint32_t infoBufferLength = 0;

	KNFC_DEBUG("DummyReaderDriverEventThread", "Start Thread...");

	//delay a few seconds before occurring the event.
	sleep(5);

	KnfcGetProtocolInfoBytes_P14P4A(infoBuffer, &infoBufferLength);
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_4_A, infoBuffer, infoBufferLength);
#endif

#if 1	//P14P3A - NDEF TYPE 4A - not standalone
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3A];
	uint32_t infoBufferLength = 0;

	KNFC_DEBUG("DummyReaderDriverEventThread", "Start Thread...");

	//delay a few seconds before occurring the event.
	sleep(5);

	KNFC_DEBUG("DummyReaderDriverEventThread", "Reader Driver Event... %d", g_bStopped);

	if(!g_bStopped) { 
		while(KnfcGetCurrentMode() != W_NFCC_PROTOCOL_READER_ISO_14443_3_A) {
			//KNFC_DEBUG("DummyReaderDriverEventThread", "WAITING...");
		}

		KNFC_DEBUG("DummyReaderDriverEventThread", "Reader Driver Event... ISO 14443_3_A");
		KnfcStopPollingLoop();

		KnfcGetProtocolInfoBytes_P14P3A(infoBuffer, &infoBufferLength);

		KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_A, infoBuffer, infoBufferLength);
	}
#endif

#if 0	//P14P4B - NDEF TYPE 4B - not standalone
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3B];
	uint32_t infoBufferLength = 0;

	KNFC_DEBUG("DummyReaderDriverEventThread", "Start Thread...");

	//delay a few seconds before occurring the event.
	sleep(5);

	KnfcGetProtocolInfoBytes_P14P3B(infoBuffer, &infoBufferLength);
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_B, infoBuffer, infoBufferLength);
#endif

	pthread_mutex_unlock(&g_hEventThreadMutex);
	return NULL;
}

void
KnfcRegisterDummyReaderDriverEvent() {

	if(g_bExecuted == TRUE) {
		return;
	}

	//g_bExecuted = TRUE;

	KNFC_DEBUG("KnfcRegisterDummyReaderDriverEvent", "FUNC STA");

	g_bStopped = FALSE;

	pthread_create(&g_hEventThread, NULL, &DummyReaderDriverEventThread, NULL);

	KNFC_DEBUG("KnfcRegisterDummyReaderDriverEvent", "FUNC END");
}

////////////////////////////////////////////////////////
// To test the Data Exchange for P14P4
////////////////////////////////////////////////////////

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

static void *
DummyExchangeP14P4DataThread(void *pParam) {

	uint8_t infoBuffer[256];
	uint32_t infoBufferLength = 0;

	KNFC_DEBUG("DummyExchangeP14P4DataThread", "Start Thread...");

	//delay a few seconds before occurring the error event.
	sleep(5);

	infoBuffer[0] = 0x90;
	infoBuffer[1] = 0x00;

	infoBufferLength = 2;
	
	KnfcProcessReaderExchangeP14P4Data(infoBuffer, infoBufferLength);
	return NULL;
}

void
KnfcDummyExchangeP14P4Data(void *pParam) {
	KNFC_DEBUG("KnfcRegisterDummyReaderDriverEvent", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummyExchangeP14P4DataThread, pParam);

	KNFC_DEBUG("KnfcRegisterDummyReaderDriverEvent", "FUNC END");
}

#endif

////////////////////////////////////////////////////////
// To test the Data Exchange for P14P3
////////////////////////////////////////////////////////

static void *
DummyExchangeP14P3DataThread(void *pParam) {

	uint8_t infoBuffer[256];
	uint32_t infoBufferLength = 0;
	KnfcDummyData_t *dummyData = (KnfcDummyData_t *)pParam;

	KNFC_DEBUG("DummyExchangeP14P3DataThread", "Start Thread...");

	//delay a few seconds before occurring the error event.
	sleep(1);

#if 0	// NDEF 2 DATA

	// For NDEF2 READ COMMAND + CC + TLV
	infoBuffer[0] = 0x03;
	infoBuffer[1] = 0x10;
	infoBuffer[2] = 0xFE;

	// LOCK
	infoBuffer[10] = 0x00;
	infoBuffer[11] = 0x00;

	// CC
	infoBuffer[12] = 0xE1;
	infoBuffer[13] = 0x10;
	infoBuffer[14] = 0x10;
	infoBuffer[15] = 0x00;

	infoBufferLength = 16;
#endif

#if 1	// NDEF 4-A/B DATA

	// RATS Command for only NDEF Type A
	if(dummyData->pBuffer[0] == 0xE0) {

		KNFC_DEBUG("DummyExchangeP14P3DataThread", "Send ATS Response");

		// ATS Response (No TA, TB, TC, and Historical Bytes)
		infoBuffer[0] = 0x02;
		infoBuffer[1] = 0x00;

		infoBufferLength = 2;
	}
	// ISO 7816 - APDU Command (Select Application)
	else if(dummyData->pBuffer[0] == 0x00 &&
		    dummyData->pBuffer[1] == 0xA4 &&
		    dummyData->pBuffer[2] == 0x04 &&
		    dummyData->pBuffer[3] == 0x00) {

		// Version
		uint8_t buf[] = {0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00};

		if(memcmp(&dummyData->pBuffer[4], buf, sizeof(buf)) == 0) {

			KNFC_DEBUG("DummyExchangeP14P3DataThread", "ISO 7816 - APDU Command (Select Application)");

			infoBuffer[0] = 0x90;
			infoBuffer[1] = 0x00;
			
			infoBufferLength = 2;
		}
	}
	// ISO 7816 - APDU Command (Select File)
	else if(dummyData->pBuffer[0] == 0x00 &&
		    dummyData->pBuffer[1] == 0xA4 &&
		    dummyData->pBuffer[2] == 0x00 &&
		    dummyData->pBuffer[3] == 0x0C) {

		// Select File
		if(dummyData->pBuffer[4] == 0x02) {

			uint16_t fileID = ((uint16_t)dummyData->pBuffer[5] << 8 | dummyData->pBuffer[6]);

			KNFC_DEBUG("DummyExchangeP14P3DataThread", "ISO 7816 - APDU Command (Select File: %x)", fileID);

			infoBuffer[0] = 0x90;
			infoBuffer[1] = 0x00;
			
			infoBufferLength = 2;
		}
	}	
	// ISO 7816 - APDU Command (Select CC File)
	else if(dummyData->pBuffer[0] == 0x00 &&
		    dummyData->pBuffer[1] == 0xB0) {

		// CC
		if(dummyData->pBuffer[4] == 15) {
			KNFC_DEBUG("DummyExchangeP14P3DataThread", "ISO 7816 - APDU Command (Select File CC)");

			// CC length
			infoBuffer[ 0] = 0x00;
			infoBuffer[ 1] = 0x0F;

			// Version
			infoBuffer[ 2] = 0x02;

			// Max R-APDU Size
			infoBuffer[ 3] = 0x00;
			infoBuffer[ 4] = 0xFF;

			// Max C-APDU Size
			infoBuffer[ 5] = 0x00;
			infoBuffer[ 6] = 0xFF;

			// TLV Type
			infoBuffer[ 7] = 0x04;	// 0x04: NDEF, 0x05: Proprietary

			// TLV Length (2bytes: if > 255, 1byte: if < 255)
			infoBuffer[ 8] = 0x05;

			// NDEF ID
			infoBuffer[ 9] = 0x00;
			infoBuffer[10] = 0x01;

			// NDEF Length
			infoBuffer[11] = 0x00;
			infoBuffer[12] = 0x22;

			// Read Access
			infoBuffer[13] = 0x00;

			// Write Access
			infoBuffer[14] = 0x00;

			infoBuffer[15] = 0x90;		
			infoBuffer[16] = 0x00;		
			
			infoBufferLength = 17;
		}
		// NDEF LENGTH
		else if(dummyData->pBuffer[4] == 2) {

			KNFC_DEBUG("DummyExchangeP14P3DataThread", "ISO 7816 - APDU Command (Select NDEF LEN)");
			
			infoBuffer[0] = 0x00;
			infoBuffer[1] = 0x20;
			
			infoBuffer[2] = 0x90;
			infoBuffer[3] = 0x00;

			infoBufferLength = 4;			
		} 
		// NDEF CONTENT
		else if(dummyData->pBuffer[4] == 32) {
			KNFC_DEBUG("DummyExchangeP14P3DataThread", "ISO 7816 - APDU Command (Select File NDEF)");

			infoBuffer[ 0] = 0xD2;
			infoBuffer[ 1] = 0x0C;
			infoBuffer[ 2] = 0x11;
			infoBuffer[ 3] = 0x74;
			infoBuffer[ 4] = 0x65;
			infoBuffer[ 5] = 0x78;
			infoBuffer[ 6] = 0x74;
			infoBuffer[ 7] = 0x2F;
			infoBuffer[ 8] = 0x78;
			infoBuffer[ 9] = 0x2D;

			infoBuffer[10] = 0x76;
			infoBuffer[11] = 0x43;
			infoBuffer[12] = 0x61;
			infoBuffer[13] = 0x72;
			infoBuffer[14] = 0x64;
			infoBuffer[15] = 0x77;
			infoBuffer[16] = 0x77;
			infoBuffer[17] = 0x77;
			infoBuffer[18] = 0x2E;
			infoBuffer[19] = 0x72;

			infoBuffer[20] = 0x61;
			infoBuffer[21] = 0x6F;
			infoBuffer[22] = 0x6E;
			infoBuffer[23] = 0x2D;
			infoBuffer[24] = 0x74;
			infoBuffer[25] = 0x65;
			infoBuffer[26] = 0x63;
			infoBuffer[27] = 0x68;
			infoBuffer[28] = 0x2E;
			infoBuffer[29] = 0x63;

			infoBuffer[30] = 0x6F;
			infoBuffer[31] = 0x6D;

			// 7816 APDU
			infoBuffer[32] = 0x90;
			infoBuffer[33] = 0x00;

			infoBufferLength = 34;				
		}
	}	
	else {

		infoBuffer[0] = 0x90;
		infoBuffer[1] = 0x00;

		infoBufferLength = 2;
	}

#endif
	
	KnfcProcessReaderExchangeP14P3Data(infoBuffer, infoBufferLength);
	return NULL;
}

void
KnfcDummyExchangeP14P3Data(void *pParam) {

	KNFC_DEBUG("KnfcDummyExchangeP14P3Data", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummyExchangeP14P3DataThread, pParam);

	KNFC_DEBUG("KnfcDummyExchangeP14P3Data", "FUNC END");
}

////////////////////////////////////////////////////////
// To test the Send Emul Answer
////////////////////////////////////////////////////////


/* ----------------------------------------------------------------------------

      ISO 7816 Constants

   ---------------------------------------------------------------------------- */

/** Masterfile first byte */
#define P_7816SM_MF_FIRST_BYTE  0x3F

/** The CLA byte for ISO/IEC 7816-4 compliant commands */
#define P_7816SM_CLA ((uint8_t)0x00)

/** The INS byte of the MANAGE CHANNEL command */
#define P_7816SM_INS_MANAGE_CHANNEL ((uint8_t)0x70)

/** The P1 byte of the MANAGE CHANNEL[open] command */
#define P_7816SM_P1_MANAGE_CHANNEL_OPEN ((uint8_t)0x00)

/** The P1 byte of the MANAGE CHANNEL[close] command */
#define P_7816SM_P1_MANAGE_CHANNEL_CLOSE ((uint8_t)0x80)

/** The INS byte of the SELECT command */
#define P_7816SM_INS_SELECT ((uint8_t)0xA4)

/** The P1 byte of the SELECT[AID] command */
#define P_7816SM_P1_SELECT_AID ((uint8_t)0x04)

/** The P2 byte of the SELECT[AID] command : FCI template in answer */
#define P_7816SM_P2_SELECT_AID ((uint8_t)0x00)

/** The P1 byte of the SELECT[FILE] command */
#define P_7816SM_P1_SELECT_FILE ((uint8_t)0x00)

/** The P2 byte of the SELECT[FILE] command : FCI template in answer */
#define P_7816SM_P2_SELECT_FILE_WITH_FCI ((uint8_t)0x00)

/** The P2 byte of the SELECT[FILE] command : FCP template in answer */
#define P_7816SM_P2_SELECT_FILE_WITH_FCP ((uint8_t)0x04)

/** The P2 byte of the SELECT[FILE] command : No response data if Le field absent, or proprietary if Le field present */
#define P_7816SM_P2_SELECT_FILE_NO_RESPONSE_DATA ((uint8_t)0x0C)

/** The INS byte of the GET RESPONSE command */
#define P_7816SM_INS_GET_RESPONSE ((uint8_t)0xC0)

/** The INS byte of the READ RECORD command */
#define P_7816SM_INS_READ_RECORD ((uint8_t)0xB2)

/** The INS byte of the READ BINARY command */
#define P_7816SM_INS_READ_BINARY ((uint8_t)0xB0)

/** The INS byte of the UPDATE BINARY command */
#define P_7816SM_INS_UPDATE_BINARY ((uint8_t)0xD6)

/** The INS byte of the CREATE EF command */
#define P_7816SM_INS_CREATE ((uint8_t)0xE0)

/** The INS byte of the DELETE EF command */
#define P_7816SM_INS_DELETE ((uint8_t)0xE4)

/** The INS byte of the GET DATA command */
#define P_7816SM_INS_GET_DATA ((uint8_t)0xCA)

/* The specific SW1 response command */
#define P_7816SM_SW1_61              0x61
#define P_7816SM_SW1_6C              0x6C

/** The minimum length (in bytes) of an AID */
#define P_7816SM_MIN_AID_LENGTH 5
/** The maximum length (in bytes) of an AID */
#define P_7816SM_MAX_AID_LENGTH 16

// Emulation State 
#define EMUL_STATE_NONE						0x00
#define EMUL_STATE_RATS						0x01
#define EMUL_STATE_SELECT_APPLICATION		0x02
#define EMUL_STATE_SELECT_CC_FILE				0x03
#define EMUL_STATE_READ_CC_FILE				0x04
#define EMUL_STATE_SELECT_NDEF_FILE			0x05
#define EMUL_STATE_READ_NDEF_LENGTH			0x06
#define EMUL_STATE_READ_NDEF_CONTENT		0x07
#define EMUL_STATE_WRITE_NDEF_MESSAGE		0x08

static uint32_t g_nEmulState = EMUL_STATE_NONE;
static uint32_t g_nNdefLength = 0;

static void *
DummySendEmulAnswerThread(void *pParam) {

	uint32_t i;
	uint8_t infoBuffer[256];
	uint32_t infoBufferLength = 0;
	KnfcDummyData_t *dummyData = (KnfcDummyData_t *)pParam;
	uint32_t dummyLength = dummyData->nBufferLength;

	KNFC_DEBUG("DummySendEmulAnswerThread", "Start Thread...");

	//delay a few seconds before sending the answer.
	sleep(1);

	for(i = 0; i < dummyData->nBufferLength; i++) {
		KNFC_DEBUG("DummySendEmulAnswerThread", "Buffer[%d] = 0x%02X", i, dummyData->pBuffer[i]);
	}	

#if 1	// NDEF 4-A/B DATA

	if(g_nEmulState != EMUL_STATE_RATS) {

		if(dummyData->pBuffer[dummyLength-2] != 0x90 || 
		   dummyData->pBuffer[dummyLength-1] != 0x00) {

			KNFC_DEBUG("DummySendEmulAnswerThread", "Error Response");
		   	return NULL;
		}
	}

	// Waiting the response after send the following commands
	switch(g_nEmulState) {
		case EMUL_STATE_RATS: 
			
			// ATS Response (No TA, TB, TC, and Historical Bytes)
			if(dummyData->pBuffer[0] == 0x02 && dummyData->pBuffer[1] == 0x00) {

				// Version Information
				uint8_t buf[] = {0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01, 0x00};
				
				KNFC_DEBUG("DummySendEmulAnswerThread", "Receive ATS Response");

				// ISO 7816 - APDU Command (Select Application)
				infoBuffer[0] = P_7816SM_CLA;
				infoBuffer[1] = P_7816SM_INS_SELECT;
				infoBuffer[2] = P_7816SM_P1_SELECT_AID;
				infoBuffer[3] = P_7816SM_P2_SELECT_AID;

				infoBufferLength = 4 + sizeof(buf);

				// Application Version
				memcpy(&infoBuffer[4], buf, sizeof(buf));	

				g_nEmulState = EMUL_STATE_SELECT_APPLICATION;
			}
			break;

		case EMUL_STATE_SELECT_APPLICATION:

			// ISO 7816 - APDU Command (Select File)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_SELECT;
			infoBuffer[2] = P_7816SM_P1_SELECT_FILE;
			infoBuffer[3] = P_7816SM_P2_SELECT_FILE_NO_RESPONSE_DATA;

			// Select File
			infoBuffer[4] = 0x02;	

			// CC File (0xE1, 0x03)
			infoBuffer[5] = 0xE1;
			infoBuffer[6] = 0x03;

			infoBufferLength = 7;

			g_nEmulState = EMUL_STATE_SELECT_CC_FILE;
			break;

		case EMUL_STATE_SELECT_CC_FILE: 

			// ISO 7816 - APDU Command (Read CC File)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_READ_BINARY;

			// CC File
			infoBuffer[2] = 0x00;
			infoBuffer[3] = 0x00;
			infoBuffer[4] = 0x0F;

			infoBufferLength = 5;

			g_nEmulState = EMUL_STATE_READ_CC_FILE;
			break;			

		case EMUL_STATE_READ_CC_FILE: 

			// ISO 7816 - APDU Command (Select NDEF File)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_SELECT;
			infoBuffer[2] = P_7816SM_P1_SELECT_FILE;
			infoBuffer[3] = P_7816SM_P2_SELECT_FILE_NO_RESPONSE_DATA;

			// The length of NDEF File ID
			infoBuffer[4] = 0x02;	

			// NDEF File
			infoBuffer[5] = 0x00;
			infoBuffer[6] = 0x01;

			infoBufferLength = 7;

			g_nEmulState = EMUL_STATE_SELECT_NDEF_FILE;
			break;

		case EMUL_STATE_SELECT_NDEF_FILE: 

			// ISO 7816 - APDU Command (Read NDEF Length)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_UPDATE_BINARY;

			// NDEF File Length
			infoBuffer[2] = 0x00;
			infoBuffer[3] = 0x00;
			infoBuffer[4] = 0x04;

			infoBuffer[5] = (uint8_t)'A';
			infoBuffer[6] = (uint8_t)'B';
			infoBuffer[7] = (uint8_t)'C';
			infoBuffer[8] = (uint8_t)'D';
			
			infoBufferLength = 9;

			g_nEmulState = EMUL_STATE_WRITE_NDEF_MESSAGE;
			break;			

		case EMUL_STATE_WRITE_NDEF_MESSAGE: 

			// ISO 7816 - APDU Command (Read NDEF Length)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_READ_BINARY;

			// NDEF File Length
			infoBuffer[2] = 0x00;
			infoBuffer[3] = 0x00;
			infoBuffer[4] = 0x02;

			infoBufferLength = 5;

			g_nEmulState = EMUL_STATE_READ_NDEF_LENGTH;
			break;			

		case EMUL_STATE_READ_NDEF_LENGTH: {

			g_nNdefLength = ((uint32_t)dummyData->pBuffer[0] << 8) | dummyData->pBuffer[1];

			KNFC_DEBUG("DummySendEmulAnswerThread", "NDEF Content Length: %d", g_nNdefLength);

			// ISO 7816 - APDU Command (NDEF Content)
			infoBuffer[0] = P_7816SM_CLA;
			infoBuffer[1] = P_7816SM_INS_READ_BINARY;
			infoBuffer[2] = 0x00;
			infoBuffer[3] = (g_nNdefLength >> 8) & 0xFF;
			infoBuffer[4] = (g_nNdefLength >> 0) & 0xFF;

			infoBufferLength = 5;
			
			g_nEmulState = EMUL_STATE_READ_NDEF_CONTENT;
			break;
		}

		case EMUL_STATE_READ_NDEF_CONTENT: {

			for(i = 0; i < g_nNdefLength; i++) {
				KNFC_DEBUG("DummySendEmulAnswerThread", "NDEF Content[%d]: 0x%02X", i, dummyData->pBuffer[i]);
			}

			g_nEmulState = EMUL_STATE_NONE;
			//KnfcProcessCardEmulationEvent(W_PROP_ISO_14443_3_A, W_EMUL_EVENT_DEACTIVATE);
			return NULL;
		}	

		default: 
			KNFC_DEBUG("DummySendEmulAnswerThread", "Card Emulation Connection is not valid: %d", g_nEmulState);
			break;
	}	
#endif
	
	KnfcProcessCardEmulationCommand(infoBuffer, infoBufferLength);
	return NULL;
}

void
KnfcDummySendEmulAnswer(void *pParam) {

	KNFC_DEBUG("KnfcDummySendEmulAnswer", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummySendEmulAnswerThread, pParam);

	KNFC_DEBUG("KnfcDummySendEmulAnswer", "FUNC END");
}


////////////////////////////////////////////////////////
// Dummy Card Emulation Event
////////////////////////////////////////////////////////

static void *
DummyCardEmulationEventThread(void *pParam) {

	KNFC_DEBUG("DummyCardEmulationEventThread", "Start Thread...");

	//delay a few seconds before occurring the error event.
	sleep(3);

	KnfcStopPollingLoop();
	g_nEmulState = EMUL_STATE_NONE;
	KnfcProcessCardEmulationEvent(W_PROP_ISO_14443_3_A, W_EMUL_EVENT_SELECTION);
	return NULL;
}

void
KnfcDummyCardEmulationEvent() {
	KNFC_DEBUG("KnfcDummyCardEmulationEvent", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummyCardEmulationEventThread, NULL);

	KNFC_DEBUG("KnfcDummyCardEmulationEvent", "FUNC END");
}

////////////////////////////////////////////////////////
// Dummy Card Emulation Command
////////////////////////////////////////////////////////

static void *
DummyCardEmulationCommandThread(void *pParam) {

	uint8_t cmdBuffer[2];
	uint32_t cmdBufferLength = 0;

	KNFC_DEBUG("DummyCardEmulationCommandThread", "Start Thread...");

	//delay a few seconds before sending the RATS command.
	sleep(5);

	cmdBuffer[0] = 0xE0;
	cmdBuffer[1] = 0x00;

	cmdBufferLength = 2;

	g_nEmulState = EMUL_STATE_RATS;
	KnfcProcessCardEmulationCommand(cmdBuffer, cmdBufferLength);
	return NULL;
}

void
KnfcDummyCardEmulationCommand(void) {
	KNFC_DEBUG("KnfcDummyCardEmulationCommand", "FUNC STA");

	pthread_create(&g_hEventThread, NULL, &DummyCardEmulationCommandThread, NULL);

	KNFC_DEBUG("KnfcDummyCardEmulationCommand", "FUNC END");
}

