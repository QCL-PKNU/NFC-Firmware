/*
 * KnfcIso14443A.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_ISO14443A_H_
#define _KNFC_ISO14443A_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

#include "KnfcReader.h"
#include "KnfcCardEmul.h"

// YOUNGSUN - CHKME
// ISO 14443-4 is not supported by the hardware,
// but it should be supported in hardware-manner for the battery-off mode.
//#define HARDWARE_P14P4_SUPPORT

//////////////////////////////////////////////////
// Macro Definition
//////////////////////////////////////////////////

#define ATQA_LENGTH				2
#define MAX_UID_LENGTH				10
#define MAX_ATS_LENGTH				256

#define MAX_BUF_LENGTH_P14P3A		13
#define MIN_BUF_LENGTH_P14P3A		7

#define MAX_BUF_LENGTH_P14P4A		33
#define MIN_BUF_LENGTH_P14P4A		8

/* Default timeout value used for ISO 14443 A Part 3 */
/* In part 3, no timeout is exchanged, the default value is always used */
#define P_14443_3_A_DEFAULT_TIMEOUT         0x0B    /* 618 ms */

//////////////////////////////////////////////////
// Data Structure for ISO 14443-3 A 
//////////////////////////////////////////////////

typedef struct {

	// ATQA (2 bytes)
	uint8_t aAtqa[ATQA_LENGTH];

	// SAK (1 byte)
	uint8_t nSak;

	// UID (4, 7, 10 bytes)
	uint8_t aUid[MAX_UID_LENGTH];

	// Additional Info (volatile)
	uint8_t nUidLength;

} KnfcProtocolInfo_P14P3A_t;

typedef struct {

	// ATQA (2 bytes)
	uint8_t aAtqa[ATQA_LENGTH];

	// SAK (1 byte)
	uint8_t nSak;

	// ATS (1~20 byte)
	uint8_t aAts[MAX_ATS_LENGTH];

	// Additional Info (volatile)
	uint8_t nAtsLength;    /* ATS data (0~19 bytes) + ATS length (1 byte ) */

	// UID (4, 7, 10 bytes)
	uint8_t aUid[MAX_UID_LENGTH];

	// Additional Info (volatile)
	uint8_t nUidLength;

} KnfcProtocolInfo_P14P4A_t;

typedef struct {

	// FSDI (4bits)
	uint8_t nFsdi;

	// CID (4bits)
	uint8_t nCid;

} KnfcPcdConnectionInfo_P14P4A_t;

typedef struct {

	// FSCI (4bits)
	uint8_t nFsci;

} KnfcPiccConnectionInfo_P14P4A_t;

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* ISO 14443 Part 3 - Type A */

KnfcError_t KnfcGetProtocolInfoBytes_P14P3A(uint8_t *pBuffer, uint32_t *pnBufferLength);

KnfcError_t KnfcSetProtocolInfoBytes_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcGetProtocolInfo_P14P3A(KnfcProtocolInfo_P14P3A_t *pProtocolInfo);

KnfcError_t KnfcSetProtocolInfo_P14P3A(KnfcProtocolInfo_P14P3A_t *pProtocolInfo);

/* ISO 14443 Part 4 - Type A */

#ifdef HARDWARE_P14P4_SUPPORT
KnfcError_t KnfcGetProtocolInfoBytes_P14P4A(uint8_t *pBuffer, uint32_t *pnBufferLength);

KnfcError_t KnfcSetProtocolInfoBytes_P14P4A(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcGetProtocolInfo_P14P4A(KnfcProtocolInfo_P14P4A_t *pProtocolInfo);

KnfcError_t KnfcSetProtocolInfo_P14P4A(KnfcProtocolInfo_P14P4A_t *pProtocolInfo);
#endif

/*******************************************************************************************
 * Hardware-dependent Implemenation
 *******************************************************************************************/

//////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
#define NFC_TYPEA_PCD_BASE_ADDR			(NFC_CTRL_BASE_ADDR + (0x1<<2))
#define NFC_TYPEA_PICC_BASE_ADDR			(NFC_CTRL_BASE_ADDR + (0x10<<2))

#define NFC_TYPEA_PCD_TX_QUEUE_ADDR		(NFC_CTRL_BASE_ADDR + (0xa<<2))
#define NFC_TYPEA_PCD_RX_QUEUE_ADDR		(NFC_CTRL_BASE_ADDR + (0xc<<2))
#define NFC_TYPEA_PICC_TX_QUEUE_ADDR		(NFC_CTRL_BASE_ADDR + (0x1a<<2))
#define NFC_TYPEA_PICC_RX_QUEUE_ADDR		(NFC_CTRL_BASE_ADDR + (0x1c<<2))

#define NFC_TYPEA_PCD_FILTER_ADDR			(NFC_CTRL_BASE_ADDR + (0xe<<2))
#define NFC_TYPEA_PICC_FILTER_ADDR		(NFC_CTRL_BASE_ADDR + (0x1e<<2))
#endif

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

typedef enum {
	TYPEA_PCD_STATE_IDLE = 0,
	TYPEA_PCD_STATE_SELECT = 12
}typeA_pcd_state_t;

//NFC_TypeA_PCD_Struct
typedef struct {
	union {
		struct {
			unsigned short int EnableAnticollision : 1;
			unsigned short int CollisionBitDecision : 1;
			unsigned short int WakeUp : 1;
			unsigned short int EnableHalt : 1;
			unsigned short int Clear : 1;
			unsigned short int Reset : 1;
			unsigned short int FDT : 2;
			unsigned short int Reserved : 8;
		} BitField;
		unsigned short int Reg;
	} Control;
	unsigned short int Reserved0;

	union {
		struct {
			const unsigned short int State : 4;
			const unsigned short int ErrorOddParity : 1;
			const unsigned short int ErrorCRC : 1;
			const unsigned short int ErrorOverflow : 1;
			const unsigned short int ErrorBCC : 1;
			const unsigned short int ErrorSEL : 1;
			const unsigned short int ErrorSAK : 1;
			const unsigned short int ProprietartyAC : 1;
			const unsigned short int Part4NotCompilant : 1;
			const unsigned short int FrameDelayTimeOut : 1;
			const unsigned short int Reserved : 1;
			const unsigned short int SizeUID : 2;
		} BitField;
		const unsigned short int Reg;
	} State;

	unsigned short int Reserved1;
	const unsigned short int atqa;
	unsigned short int Reserved2;
	const unsigned short int sak;
	unsigned short int Reserved3;
	const unsigned short int UID0;
	unsigned short int Reserved4;
	const unsigned short int UID1;
	unsigned short int Reserved5;
	const unsigned short int UID2;
	unsigned short int Reserved6;
	const unsigned short int UID3;
	unsigned short int Reserved7;
	const unsigned short int UID4;
	unsigned short int Reserved8;

} NFC_TypeA_PCD_Struct;

//NFC_TypeA_PICC_Struct
typedef struct {
	union {
		struct {
			unsigned short int Reserved : 4;
			unsigned short int Clear : 1;
			unsigned short int Reset : 1;
			unsigned short int SizeUID : 2;
			unsigned short int FDT : 8;
		} BitField;
		unsigned short int Reg;
	} Control;

	unsigned short int Reserved0;

	union {
		struct {
			const unsigned short int State : 4;
			const unsigned short int ErrorOddParity : 1;
			const unsigned short int ErrorCRC : 1;
			const unsigned short int Reserved0 : 2;
			const unsigned short int CommandFlag : 7;
			const unsigned short int Reserved1 : 1;
		} BitField;
		const unsigned short int Reg;
	} StateLow;

	unsigned short int Reserved1;

	union {
		struct {
			const unsigned short int CascadeLevelFlag : 3;
			const unsigned short int Reserved0 : 13;
		} BitField;
		const unsigned short int Reg;
	} StateHigh;

	unsigned short int Reserved2;

	unsigned short int atqa;
	unsigned short int Reserved3;
	unsigned short int sak;
	unsigned short int Reserved4;

	unsigned short int UID0;
	unsigned short int Reserved5;
	unsigned short int UID1;
	unsigned short int Reserved6;
	unsigned short int UID2;
	unsigned short int Reserved7;
	unsigned short int UID3;
	unsigned short int Reserved8;
	unsigned short int UID4;
	unsigned short int Reserved9;

} NFC_TypeA_PICC_Struct;

typedef struct {
	union {
		struct {
			unsigned short int FilterSelection : 2;
			unsigned short int Reserved : 2;
			unsigned short int FixedThreshold : 4;
			unsigned short int MatchedFilterThreshold : 8;
		} BitField;
		unsigned short int Reg;
	} Low;

	unsigned short int Reserved0;

	union {
		struct {
			unsigned short int RLF_ThresholdMargin : 4;
			unsigned short int RLF_OffsetMargin : 4;
			unsigned short int RLF_CountMax : 8;
		} BitField;
		unsigned short int Reg;
	} High;

	unsigned short int Reserved1;

}NFC_TypeA_Filter_Control;

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* NFC Controller Management */

/* ISO 14443-4 Type A */

#ifdef HARDWARE_P14P4_SUPPORT

KnfcError_t KnfcPcdActivate_P14P4A(void *pParam);
 
KnfcError_t KnfcPcdDeactivate_P14P4A(void *pParam);

KnfcError_t KnfcPiccActivate_P14P4A(void *pParam);

KnfcError_t KnfcPiccDeactivate_P14P4A(void *pParam);

KnfcError_t KnfcPcdExchangeData_P14P4A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPiccExchangeData_P14P4A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

#endif

/* ISO 14443-3 Type A */

KnfcError_t KnfcPcdExchangeData_P14P3A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPiccExchangeData_P14P3A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPcdActivate_P14P3A(void *pParam);

KnfcError_t KnfcPcdDeactivate_P14P3A(void *pParam);

KnfcError_t KnfcPiccActivate_P14P3A(void *pParam);

KnfcError_t KnfcPiccDeactivate_P14P3A(void *pParam);

/* Interrupt Service Routines */

void KnfcPcdReceiveDataHandler_P14P3A(void);
 
void KnfcPiccReceiveDataHandler_P14P3A(void);

void KnfcPcdSelectHandler_P14P3A(void);

void KnfcPcdErrorEventHandler_P14P3A(void);

void KnfcPiccErrorEventHandler_P14P3A(void);
 
void KnfcPiccActivateHandler_P14P3A(void);

#endif /* _KNFC_ISO14443A_H_ */
