/*
 * KnfcIso14443B.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_ISO14443B_H_
#define _KNFC_ISO14443B_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

#include "KnfcReader.h"
#include "KnfcCardEmul.h"

//////////////////////////////////////////////////
// Macro Definition
//////////////////////////////////////////////////

#define ATQB_LENGTH				11 // 12 bytes - 1 byte (0x50)
#define MLBInCID_LENGTH			1
#define MAX_HIGHER_LAYER_RESP		253
#define MAX_BUF_LENGTH_P14P3B		(ATQB_LENGTH + MLBInCID_LENGTH + MAX_HIGHER_LAYER_RESP)
#define MIN_BUF_LENGTH_P14P3B		(ATQB_LENGTH + MLBInCID_LENGTH)

//////////////////////////////////////////////////
// Data Structure for ISO 14443-3 B 
//////////////////////////////////////////////////

typedef struct {

	// ATQB (12 bytes)
	uint8_t aAtqb[ATQB_LENGTH];

	// MBLI & CID (1 byte)
	uint8_t nMlbiCid;

	// Higher Layer Response (1~253 bytes)
	uint8_t aHigherLayerResponse[MAX_HIGHER_LAYER_RESP];

	// Additional Info (volatile)	
	uint32_t nHigherLayerResponseLength;

} KnfcProtocolInfo_P14P3B_t;

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* ISO 14443 Part 3 - Type B */

KnfcError_t KnfcGetProtocolInfoBytes_P14P3B(uint8_t *pBuffer, uint32_t *pnBufferLength);

KnfcError_t KnfcSetProtocolInfoBytes_P14P3B(uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcGetProtocolInfo_P14P3B(KnfcProtocolInfo_P14P3B_t *pProtocolInfo);

KnfcError_t KnfcSetProtocolInfo_P14P3B(KnfcProtocolInfo_P14P3B_t *pProtocolInfo);

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* NFC Controller Management */

/* ISO 14443-3 Type B */

KnfcError_t KnfcPcdExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPiccExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPcdActivate_P14P3B(void *pParam);

KnfcError_t KnfcPcdDeactivate_P14P3B(void *pParam);

KnfcError_t KnfcPiccActivate_P14P3B(void *pParam);

KnfcError_t KnfcPiccDeactivate_P14P3B(void *pParam);

KnfcError_t KnfcPcdSendHalt_P14P3B(void);

/* Interrupt Service Routines */

void KnfcPcdReceiveDataHandler_P14P3B(void);
 
void KnfcPiccReceiveDataHandler_P14P3B(void);

void KnfcPcdSelectHandler_P14P3B(void);

void KnfcPcdErrorEventHandler_P14P3B(void);

void KnfcPiccErrorEventHandler_P14P3B(void);
 
void KnfcPiccActivateHandler_P14P3B(void);

/*******************************************************************************************
 * Hardware-dependent Implemenation
 *******************************************************************************************/

//////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
#define NFC_TYPEB_PCD_BASE_ADDR				(NFC_CTRL_BASE_ADDR + (0x20<<2))
#define NFC_TYPEB_PCD_RCV_PICC_INFO_ADDR		(NFC_TYPEB_PCD_BASE_ADDR + (0x3<<2))
#define NFC_TYPEB_CONF_PCD_INFO_ADDR			(NFC_TYPEB_PCD_BASE_ADDR + (0xa<<2))

#define NFC_TYPEB_PICC_BASE_ADDR				(NFC_CTRL_BASE_ADDR + (0x30<<2))
#define NFC_TYPEB_CONF_PICC_INFO_ADDR		(NFC_TYPEB_PICC_BASE_ADDR + (0x3<<2))
#define NFC_TYPEB_PICC_RCV_PCD_INFO_ADDR		(NFC_TYPEB_PICC_BASE_ADDR + (0xa<<2))

#define NFC_TYPEB_PCD_TX_QUEUE_ADDR			(NFC_TYPEB_PCD_BASE_ADDR  + (0xc<<2))
#define NFC_TYPEB_PCD_RX_QUEUE_ADDR			(NFC_TYPEB_PCD_BASE_ADDR  + (0xd<<2))
#define NFC_TYPEB_PICC_TX_QUEUE_ADDR			(NFC_TYPEB_PICC_BASE_ADDR + (0xc<<2))
#define NFC_TYPEB_PICC_RX_QUEUE_ADDR			(NFC_TYPEB_PICC_BASE_ADDR + (0xd<<2))

#define NFC_TYPEB_PCD_FILTER_ADDR				(NFC_TYPEB_PCD_BASE_ADDR + (0xe<<2))
#define NFC_TYPEB_PICC_FILTER_ADDR			(NFC_TYPEB_PICC_BASE_ADDR + (0xe<<2))
#endif

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
//NFC_TypeB_PCD_Struct
typedef struct
{
	union
	{
		struct
		{
			unsigned short int EnableAnticollision : 1;
			unsigned short int Reserved0 : 1;
			unsigned short int WakeUp : 1;
			unsigned short int EnableHalt : 1;
			unsigned short int Clear : 1;
			unsigned short int Reset : 1;
			unsigned short int Reseved1 : 1;
			unsigned short int NoSOFEOF : 1;
			unsigned short int ConfSOF : 2;
			unsigned short int ConfEOF : 1;
			unsigned short int ConfEGT : 3;
			unsigned short int GuardTime : 2;
		}BitField;
		unsigned short int Reg;
	} Control;

	unsigned short int Reserved0;

	union
	{
		struct
		{
			unsigned short int AFI_Rule : 8;
			unsigned short int ConfFwiTimer : 4;
			unsigned short int ConfReqbRfu : 4;
		}BitField;
		unsigned short int Reg;
	} AFI_Rule;

	unsigned short int Reserved1;

	union
	{
		struct
		{
			unsigned short int State : 4;
			unsigned short int ResponseFlag : 3;
			unsigned short int Reserved0 : 1;
			unsigned short int ErrorCRC : 1;
			unsigned short int ErrorCollision : 1;
			unsigned short int ErrorFWI : 1;
			unsigned short int Reserved1 : 5;
		} BitField;
		unsigned short int Reg;
	} State;

	unsigned short int Reserved2;
} NFC_TypeB_PCD_Struct;

//NFC_TypeB_PICC_Struct
typedef struct
{
	union
	{
		struct
		{
			unsigned short int ConfTR0 : 2;
			unsigned short int ConfTR1 : 2;
			unsigned short int Clear : 1;
			unsigned short int Reset : 1;
			unsigned short int Reseved0 : 1;
			unsigned short int NoSOFEOF : 1;
			unsigned short int ConfSOF : 2;
			unsigned short int ConfEOF : 1;
			unsigned short int ConfEGT : 3;
			unsigned short int Reseved1 : 2;
		}BitField;
		unsigned short int Reg;
	} Control;

	unsigned short int Reserved0;

	union
	{
		struct
		{
			unsigned short int TR0TimerCompare : 8;
			unsigned short int TR1TimerCompare : 8;
		}BitField;
		unsigned short int Reg;
	} TR;

	unsigned short int Reserved1;

	union
	{
		struct
		{
			unsigned short int State : 3;
			unsigned short int CommandFlag : 5;
			unsigned short int ErrorCRC : 1;
			unsigned short int MatchedAFI : 1;
			unsigned short int MatchedPUPI : 1;
			unsigned short int Reserved : 5;
		} BitField;
		unsigned short int Reg;
	} State;

	unsigned short int Reserved2;
} NFC_TypeB_PICC_Struct;

//NFC_TypeB_RCV_PICC_Info_Struct
typedef struct
{
	const unsigned short int PUPI_Low;
	const unsigned short int Reserved3;
	const unsigned short int PUPI_High;
	const unsigned short int Reserved4;

	union
	{
		struct
		{
			const unsigned short int AFI : 8;
			const unsigned short int CRC_B_AID_Low : 8;
		}BitField;
		const unsigned short int Reg;
	} AppDataLow;

	const unsigned short int Reserved5;

	union
	{
		struct
		{
			const unsigned short int CRC_B_AID_High : 8;
			const unsigned short int NumApps : 8;
		}BitField;
		const unsigned short int Reg;
	} AppDataHigh;
	const unsigned short int Reserved6;

	union
	{
		struct
		{
			const unsigned short int BitRate : 8;
			const unsigned short int ProtocolType : 4;
			const unsigned short int MaxFrameSize : 4;
		}BitField;
		const unsigned short int Reg;
	} ProtocolInfoLow;
	const unsigned short int Reserved7;

	union
	{
		struct
		{
			const unsigned short int FO : 2;
			const unsigned short int ADC : 2;
			const unsigned short int FWI : 4;
			const unsigned short int Reserved : 8;
		}BitField;
		const unsigned short int Reg;
	} ProtocolInfoHigh;
	const unsigned short int Reserved8;

	union
	{
		struct
		{
			const unsigned short int MBLI : 4;
			const unsigned short int CID : 4;
			const unsigned short int Reserved : 8;
		}BitField;
		const unsigned short int Reg;
	} MBLI_CID;
	const unsigned short int Reserved9;
} NFC_TypeB_RCV_PICC_Info_Struct;

//NFC_TypeB_Conf_PICC_Info_Struct
typedef struct
{
	unsigned short int PUPI_Low;
	unsigned short int Reserved3;
	unsigned short int PUPI_High;
	unsigned short int Reserved4;

	union
	{
		struct
		{
			unsigned short int AFI : 8;
			unsigned short int CRC_B_AID_Low : 8;
		}BitField;
		unsigned short int Reg;
	} AppDataLow;

	unsigned short int Reserved5;

	union
	{
		struct
		{
			unsigned short int CRC_B_AID_High : 8;
			unsigned short int NumApps : 8;
		}BitField;
		unsigned short int Reg;
	} AppDataHigh;
	unsigned short int Reserved6;

	union
	{
		struct
		{
			unsigned short int BitRate : 8;
			unsigned short int ProtocolType : 4;
			unsigned short int MaxFrameSize : 4;
		}BitField;
		unsigned short int Reg;
	} ProtocolInfoLow;
	unsigned short int Reserved7;

	union
	{
		struct
		{
			unsigned short int FO : 2;
			unsigned short int ADC : 2;
			unsigned short int FWI : 4;
			unsigned short int Reserved : 8;
		}BitField;
		unsigned short int Reg;
	} ProtocolInfoHigh;
	unsigned short int Reserved8;
	union
	{
		struct
		{
			unsigned short int MBLI : 4;
			unsigned short int CID : 4;
			unsigned short int Reserved : 8;
		}BitField;
		unsigned short int Reg;
	} MBLI_CID;
	unsigned short int Reserved9;
} NFC_TypeB_Conf_PICC_Info_Struct;

//NFC_TypeB_RCV_PCD_Info_Struct
typedef struct
{
	union
	{
		struct
		{
			const unsigned short int Reserved : 2;
			const unsigned short int SOF : 1;
			const unsigned short int EOF : 1;
			const unsigned short int MinTR1 : 2;
			const unsigned short int MinTR0 : 2;
			const unsigned short int MaxFrameSize : 4;
			const unsigned short int PCDTxRate : 2;
			const unsigned short int PICCTxRate : 2;
		}BitField;
		const unsigned short int Reg;
	} ParamLow;
	const unsigned short int Reserved0;

	union
	{
		struct
		{
			const unsigned short int ProtocolType : 4;
			const unsigned short int Reserved0 : 4;
			const unsigned short int CID : 4;
			const unsigned short int Reserved1 : 4;
		}BitField;
		const unsigned short int Reg;
	} ParamHigh;
	const unsigned short int Reserved1;
} NFC_TypeB_RCV_PCD_Info_Struct;

//NFC_TypeB_Conf_PCD_Info_Struct
typedef struct
{
	union
	{
		struct
		{
			unsigned short int Reserved : 2;
			unsigned short int SOF : 1;
			unsigned short int EOF : 1;
			unsigned short int MinTR1 : 2;
			unsigned short int MinTR0 : 2;
			unsigned short int MaxFrameSize : 4;
			unsigned short int PCDTxRate : 2;
			unsigned short int PICCTxRate : 2;
		}BitField;
		unsigned short int Reg;
	} ParamLow;
	unsigned short int Reserved0;

	union
	{
		struct
		{
			unsigned short int ProtocolType : 4;
			unsigned short int Reserved0 : 4;
			unsigned short int CID : 4;
			unsigned short int Reserved1 : 4;
		}BitField;
		unsigned short int Reg;
	} ParamHigh;
	unsigned short int Reserved1;
} NFC_TypeB_Conf_PCD_Info_Struct;

typedef struct
{
	union
	{
		struct
		{
			unsigned short int FilterSelection : 2;
			unsigned short int Reserved : 2;
			unsigned short int FixedThreshold : 4;
			unsigned short int MatchedFilterThreshold : 8;
		} BitField;
		unsigned short int Reg;
	} Low;

	unsigned short int Reserved0;

	union
	{
		struct
		{
			unsigned short int RLF_ThresholdMargin : 4;
			unsigned short int RLF_OffsetMargin : 4;
			unsigned short int RLF_CountMax : 8;
		} BitField;
		unsigned short int Reg;
	} High;

	unsigned short int Reserved1;

}NFC_TypeB_PCD_Filter_Control;

typedef struct
{
	union
	{
		struct
		{
			unsigned short int FilterSelection : 2;
			unsigned short int Reserved0 : 2;
			unsigned short int FixedThreshold : 4;
			unsigned short int MMFThresdhold : 4;
			unsigned short int Reserved1 : 4;
		} BitField;
		unsigned short int Reg;
	} Low;

	unsigned short int Reserved0;

	union
	{
		struct
		{
			unsigned short int RLF_ThresholdMargin : 4;
			unsigned short int RLF_OffsetMargin : 4;
			unsigned short int RLF_CountMax : 8;
		} BitField;
		unsigned short int Reg;
	} High;

	unsigned short int Reserved1;

}NFC_TypeB_PICC_Filter_Control;
#endif

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

/* NFC Controller Management */

/* ISO 14443-4 Type B */

#ifdef HARDWARE_P14P4_SUPPORT

KnfcError_t KnfcPcdActivate_P14P4B(void *pParam);
 
KnfcError_t KnfcPcdDeactivate_P14P4B(void *pParam);

KnfcError_t KnfcPiccActivate_P14P4B(void *pParam);

KnfcError_t KnfcPiccDeactivate_P14P4B(void *pParam);

KnfcError_t KnfcPcdExchangeData_P14P4B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPiccExchangeData_P14P4B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

#endif

/* ISO 14443-3 Type B */

KnfcError_t KnfcPcdExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPiccExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength);

KnfcError_t KnfcPcdActivate_P14P3B(void *pParam);

KnfcError_t KnfcPcdDeactivate_P14P3B(void *pParam);

KnfcError_t KnfcPiccActivate_P14P3B(void *pParam);

KnfcError_t KnfcPiccDeactivate_P14P3B(void *pParam);

/* Interrupt Service Routines */

void KnfcPcdReceiveDataHandler_P14P3B(void);
 
void KnfcPiccReceiveDataHandler_P14P3B(void);

void KnfcPcdSelectHandler_P14P3B(void);

void KnfcPcdErrorEventHandler_P14P3B(void);

void KnfcPiccErrorEventHandler_P14P3B(void);
 
void KnfcPiccActivateHandler_P14P3B(void);


#endif /* _KNFC_ISO14443B_H_ */
