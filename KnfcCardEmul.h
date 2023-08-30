/*
 * KnfcCardEmul.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

/**
 * This file is going to contain the functions
 * for the card emulation using SE.
 */

// YOUNGSUN - CHKME
// Not fully supported in this version
// since the SWP and SE are not supported
// in the underlying NFC controller hardware.

#ifndef _KNFC_CARD_EMUL_H_
#define _KNFC_CARD_EMUL_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"
#include "KnfcOsalList.h"

#include "KnfcHostCmd.h"

//////////////////////////////////////////////////
// Macro Definition for NFC Protocol capabilities
//////////////////////////////////////////////////

#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_A		0x00000001
#define W_NFCC_PROTOCOL_CARD_ISO_14443_4_B		0x00000002
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_A		0x00000004
#define W_NFCC_PROTOCOL_CARD_ISO_14443_3_B		0x00000008
#define W_NFCC_PROTOCOL_CARD_ISO_15693_3		0x00000010
#define W_NFCC_PROTOCOL_CARD_ISO_15693_2		0x00000020
#define W_NFCC_PROTOCOL_CARD_FELICA			0x00000040
#define W_NFCC_PROTOCOL_CARD_P2P_TARGET			0x00000080
#define W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP		0x00000100
#define W_NFCC_PROTOCOL_CARD_MIFARE_CLASSIC		0x00000200
#define W_NFCC_PROTOCOL_CARD_BPRIME			0x00000400
#define W_NFCC_PROTOCOL_CARD_KOVIO			0x00000800
#define W_NFCC_PROTOCOL_CARD_MIFARE_PLUS		0x00001000
#define W_NFCC_PROTOCOL_CARD_RESERVED0			0x00002000
#define W_NFCC_PROTOCOL_CARD_RESERVED1			0x00004000
#define W_NFCC_PROTOCOL_CARD_RESERVED2			0x00008000

//////////////////////////////////////////////////
// Callback Function Declaration
//////////////////////////////////////////////////

typedef void (*tCardEmulationEventCB)(KnfcHandle_t hHandle, uint32_t nEventCode);

typedef void (*tCardEmulationCommandCB)(KnfcHandle_t hHandle, uint32_t nBufferLength);

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

#define COMMAND_BUFFER_LENGTH	256

typedef struct {

	bool_t bVirutal;
	uint8_t nCardType;
	tCardEmulationEventCB cbCardEmulationEvent;
	
	tCardEmulationCommandCB cbCardEmulationCommand;
	uint8_t aCommandBuffer[COMMAND_BUFFER_LENGTH];
	uint32_t nCommandBufferLength;

} KnfcEmulConnection_t; 

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

KnfcError_t KnfcOpenEmulConnection(tWEmulConnectionInfo *pEmulConnectionInfo, KnfcEmulConnection_t **ppConnection);

KnfcError_t KnfcCloseEmulConnection(KnfcHandle_t hHandle);

KnfcError_t KnfcRegisterCardEmulationEvent(KnfcHandle_t hHandle);

KnfcError_t KnfcUnregisterCardEmulationEvent(KnfcHandle_t hHandle);

KnfcError_t KnfcProcessCardEmulationEvent(uint32_t nCardType, uint32_t nEventCode);

KnfcError_t KnfcRegisterCardEmulationCommand(KnfcHandle_t hHandle);

KnfcError_t KnfcUnregisterCardEmulationCommand(KnfcHandle_t hHandle);

KnfcError_t KnfcProcessCardEmulationCommand(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcGetEmulMessageData(KnfcHandle_t hHandle, uint8_t **ppBuffer, uint32_t *pnBufferLength);

KnfcError_t KnfcSendEmulAnswer(KnfcHandle_t hHandle, uint8_t *pBuffer, uint32_t nBufferLength);

#endif /* _KNFC_CARD_EMUL_H_ */
