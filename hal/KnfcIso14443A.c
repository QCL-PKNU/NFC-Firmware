/*
 * KnfcIso14443A.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcIso14443A.h"
#ifdef ARM926EJS_SUPPORT
#include "KnfcDalConfig.h"
#include "KnfcRxQueue.h"
#endif

////////////////////////////////////////////////////////
// Macros for parsing the protocol information
////////////////////////////////////////////////////////

#define UID_SIZE_BIT_FRAME(ATQA)	((ATQA[1] >> 6) & 0x03)
#define BIT_FRAME_ANTICOLL(ATQA)	((ATQA[1] & 0x1F)
#define PROPRIETARY_CODING(ATQA)	((ATQA[0] & 0x0F)

#define CASCADE_BIT(SAK)			(SAK & 0x04)
#define P14P4_COMPLIANT(SAK)		(SAK & 0x20)
#define P14P4_NOT_COMPLIANT(SAK)	((SAK & 0x24) == 0)

////////////////////////////////////////////////////////
// Static Variable Definition
////////////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
static volatile NFC_TypeA_PCD_Struct * const g_pPcdTypeA = 
	(volatile NFC_TypeA_PCD_Struct *)NFC_TYPEA_PCD_BASE_ADDR;

static volatile NFC_TypeA_PICC_Struct * const g_pPiccTypeA = 
	(volatile NFC_TypeA_PICC_Struct *)NFC_TYPEA_PICC_BASE_ADDR;

static volatile NFC_Tx_Queue_Struct * const g_pPcdTxqTypeA  = 
	(volatile NFC_Tx_Queue_Struct *) NFC_TYPEA_PCD_TX_QUEUE_ADDR;

static volatile NFC_Rx_Queue_Struct * const g_pPcdRxqTypeA = 
	(volatile NFC_Rx_Queue_Struct *) NFC_TYPEA_PCD_RX_QUEUE_ADDR;

static volatile NFC_Tx_Queue_Struct * const g_pPiccTxqTypeA = 
	(volatile NFC_Tx_Queue_Struct *) NFC_TYPEA_PICC_TX_QUEUE_ADDR;

static volatile NFC_Rx_Queue_Struct * const g_pPiccRxqTypeA = 
	(volatile NFC_Rx_Queue_Struct *) NFC_TYPEA_PICC_RX_QUEUE_ADDR;

static volatile NFC_TypeA_Filter_Control * const g_pPcdFilterTypeA = 
	(volatile NFC_TypeA_Filter_Control *) NFC_TYPEA_PCD_FILTER_ADDR;

static volatile NFC_TypeA_Filter_Control * const g_pPiccFilterTypeA = 
	(volatile NFC_TypeA_Filter_Control *) NFC_TYPEA_PICC_FILTER_ADDR;

#endif

static KnfcRetransBuffer_t g_RetransBuffer;

//////////////////////////////////////////////////
// Global Variable Declaration
//////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
// Nfc Rx Queue
extern KnfcRxq_t g_hNfcRxQueue;
#endif

////////////////////////////////////////////////////////
// Static Function Declaration
////////////////////////////////////////////////////////

/* Accessors & Mutators for P14P3A & P14P4A */
static KnfcError_t GetAtqa(uint8_t *pAtqa);

static KnfcError_t SetAtqa(uint8_t *pAtqa);

static KnfcError_t GetSak(uint8_t* pnSak);

static KnfcError_t SetSak(uint8_t nSak);

static KnfcError_t GetUid(uint8_t *pUid, uint8_t *nUidLength);

static KnfcError_t SetUid(uint8_t *pUid, uint8_t nUidLength);

#ifdef HARDWARE_P14P4_SUPPORT
static KnfcError_t GetAts(uint8_t* pAts, uint8_t *pnAtsLength);

static KnfcError_t SetAts(uint8_t *pAts, uint8_t nAtsLength);
#endif

/* PCD & PICC - ISO 14443 Part3 - Type A & B */

static KnfcError_t TransmitPcdData_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength);

static KnfcError_t TransmitPiccData_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength);

////////////////////////////////////////////////////////
// Internal interface functions to access the information of P14P3A 
//
// These functions will set or get the information 
// by accessing the NFC controller.
////////////////////////////////////////////////////////

static KnfcError_t
GetAtqa(uint8_t *pAtqa) {
	if(pAtqa == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// YOUNGSUN - FIXME
	// The value of state is not TYPEA_PCD_STATE_SELECT.
#ifdef ARM926EJS_SUPPORT
	//Check if PCD selected PICC
	if(g_pPcdTypeA->State.BitField.State != TYPEA_PCD_STATE_SELECT) {
		return KNFC_ERROR_P14P3A_GET_ATQA;
	}

	//g_pPiccTypeA->atqa[15:0] (2byte APB Register)
	//  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1  0
	//|    RFU        |  Prop Coding  |UIDsize|RFU| Bit Frame AC     |
	pAtqa[0] = (uint8_t)((g_pPcdTypeA->atqa>>8)&0xff);
	pAtqa[1] = (uint8_t)((g_pPcdTypeA->atqa>>0)&0xff);
#else
	//Based on Big Endian Coding:
	//ATQA - {ATQA[0], ATQA[1]}
	pAtqa[0] = 0x00;	//Get ATQA[0] from the NFCC
	pAtqa[1] = 0x01;	//Get ATQA[1] from the NFCC
#endif

	return KNFC_SUCCESS;
}

static KnfcError_t 
SetAtqa(uint8_t *pAtqa) {
	if(pAtqa == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	//g_pPiccTypeA->atqa[15:0] (2byte APB Register)
	//  15  14  13  12  11  10  9   8   7   6   5   4   3   2   1  0
	//|    RFU        |  Prop Coding  |UIDsize|RFU| Bit Frame AC  |
	//Set ATQA[0] to the NFCC
	//Set ATQA[1] to the NFCC
	g_pPiccTypeA->atqa = ((((uint16_t)pAtqa[0])<<8)&0xff00)|
						(((uint16_t)pAtqa[1])&0x00ff);
#endif
	
	return KNFC_SUCCESS;
}

static KnfcError_t 
GetSak(uint8_t *pnSak){
	if(pnSak == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	//You can set only 7,6,5,4,3,1,0
	//Hardware will generate bit position 2 automatically!
	//  7   6   5   4   3   2   1   0
	//  x   x   1   x   x   1   x   x : UID not complete
	//  x   x   1   x   x   0   x   x : UID complete, compliant with part 4
	//  x   x   0   x   x   0   x   x : UID complete, NOT compliant with part 4

	if(g_pPcdTypeA->State.BitField.State != TYPEA_PCD_STATE_SELECT) {
		return KNFC_ERROR_P14P3A_GET_SAK;
	}

	//You always get 0 at bit position 2, which means UID complete,
	//because anti-collision loop already finished.
	//Hardware will try to get UID until receiving UID complete SAK
	*pnSak = (uint8_t)((g_pPcdTypeA->sak)&0xff);	// Get SAK from the NFCC
#else
	*pnSak = 0x20;	//0x20 - Part4	// Get SAK from the NFCC
#endif
	return KNFC_SUCCESS;
}

static KnfcError_t
SetSak(uint8_t nSak) {

#ifdef ARM926EJS_SUPPORT
	//You can set only 7,6,5,4,3,1,0
	//Hardware will generate bit position 2 automatically!
	//  7   6   5   4   3   2   1   0
	//  x   x   1   x   x   1   x   x : UID not complete
	//  x   x   1   x   x   0   x   x : UID complete, compliant with part 4
	//  x   x   0   x   x   0   x   x : UID complete, NOT compliant with part 4

	//Set SAK to the NFCC
	g_pPiccTypeA->sak=nSak;
#endif

	return KNFC_SUCCESS;
}

static KnfcError_t
GetUid(uint8_t *pUid, uint8_t *pnUidLength) {

	//if(pUid == NULL || nUidLength == 0) {
	if(pUid == NULL || pnUidLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	if(g_pPcdTypeA->State.BitField.State!=TYPEA_PCD_STATE_SELECT)
		return KNFC_ERROR_P14P3A_UID_SIZE;

	//Actually g_pPcdTypeA->State.BitField.SizeUID is cascaded level
	//UID size can be obtained with cascade level lastly performed
	switch(g_pPcdTypeA->State.BitField.SizeUID) {
		case 3:	 *pnUidLength=10; break;
		case 2:  *pnUidLength=7;  break;
		case 1:  *pnUidLength=4;  break;
		default: *pnUidLength=0;  break;
	}

	switch(g_pPcdTypeA->State.BitField.SizeUID) {
		/* cascade level 3 */
		case 3:	
			pUid[9] = (uint8_t)((g_pPcdTypeA->UID4>>8)&0xff);
			pUid[8] = (uint8_t)((g_pPcdTypeA->UID4>>0)&0xff);
			pUid[7] = (uint8_t)((g_pPcdTypeA->UID3>>8)&0xff);

		/* cascade level 2 */		
		case 2:	
			pUid[6] = (uint8_t)((g_pPcdTypeA->UID3>>0)&0xff);
			pUid[5] = (uint8_t)((g_pPcdTypeA->UID2>>8)&0xff);
			pUid[4] = (uint8_t)((g_pPcdTypeA->UID2>>0)&0xff);

		/* cascade level 1 */
		case 1:	
			pUid[3] = (uint8_t)((g_pPcdTypeA->UID1>>8)&0xff);
			pUid[2] = (uint8_t)((g_pPcdTypeA->UID1>>0)&0xff);
			pUid[1] = (uint8_t)((g_pPcdTypeA->UID0>>8)&0xff);
			pUid[0] = (uint8_t)((g_pPcdTypeA->UID0>>0)&0xff);
			break;			
		/* case 0: means AC Loop has never been performed. */
		default:
			return KNFC_ERROR_P14P3A_UID_SIZE;
	}
#else
	pUid[3] = 0x04;
	pUid[2] = 0x03;
	pUid[1] = 0x02;
	pUid[0] = 0x01;

	*pnUidLength = 4;
#endif

	return KNFC_SUCCESS;
}

static KnfcError_t
SetUid(uint8_t *pUid, uint8_t nUidLength) {

	if(pUid == NULL || nUidLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	switch(nUidLength) {

		/* triple UID */
		case 10:	
			g_pPiccTypeA->UID4 = (((uint16_t)pUid[9]<<8)&0xff00)|(((uint16_t)pUid[8]<<0)&0x00ff);
			g_pPiccTypeA->UID3 = (((uint16_t)pUid[7]<<8)&0xff00)|(((uint16_t)pUid[6]<<0)&0x00ff);
		/* double UID */		
		case 7:
			g_pPiccTypeA->UID3 = (((uint16_t)pUid[6]<<0)&0x00ff);
			g_pPiccTypeA->UID2 = (((uint16_t)pUid[5]<<8)&0xff00)|(((uint16_t)pUid[4]<<0)&0x00ff);
		/* single UID */
		case 4:
			g_pPiccTypeA->UID1 = (((uint16_t)pUid[3]<<8)&0xff00)|(((uint16_t)pUid[2]<<0)&0x00ff);
			g_pPiccTypeA->UID0 = (((uint16_t)pUid[1]<<8)&0xff00)|(((uint16_t)pUid[0]<<0)&0x00ff);
			break;
			
		default:			
			return KNFC_ERROR_P14P3A_UID_SIZE;
	}
#endif

	return KNFC_SUCCESS;
}

#ifdef HARDWARE_P14P4_SUPPORT
static KnfcError_t 
GetAts(uint8_t *pAts, uint8_t *pnAtsLength){
	if(pAts == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// Get ATS from the NFCC

	return KNFC_SUCCESS;
}

static KnfcError_t
SetAts(uint8_t *pAts, uint8_t nAtsLength) {

	//Set ATS to the NFCC

	return KNFC_SUCCESS;
}
#endif

////////////////////////////////////////////////////////
// External interface functions to access the information of P14P3A 
////////////////////////////////////////////////////////

/**
 * This function will return a byte array of the protocol information including ATQA, SAK, and UID.
 *
 * @param pBuffer a byte array to get the protocol information
 * @param pnBufferLength the length of the byte array
 * @return error information
 */
KnfcError_t
KnfcGetProtocolInfoBytes_P14P3A(uint8_t *pBuffer, uint32_t *pnBufferLength) {

	KnfcError_t error;
	uint32_t bufferLength;
	KnfcProtocolInfo_P14P3A_t protocolInfo;

	if(pBuffer == NULL || pnBufferLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}
	
	error = KnfcGetProtocolInfo_P14P3A(&protocolInfo);
	
	if(error != KNFC_SUCCESS) {
		return error;
	}

	bufferLength = 2 /* ATQA size */ + 1 /* SAK size */ + protocolInfo.nUidLength;

	KnfcMemoryCopy(pBuffer, &protocolInfo, bufferLength);		
	*pnBufferLength = bufferLength;
	return KNFC_SUCCESS;
}

/**
 * This function will return the protocol information including ATQA, SAK, and UID.
 *
 * @param pProtocolInfo a data structure for protocol information of ISO 14443-3A
 * @return error information
 */
KnfcError_t
KnfcGetProtocolInfo_P14P3A(KnfcProtocolInfo_P14P3A_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = GetAtqa(pProtocolInfo->aAtqa);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_GET_ATQA);

	error = GetSak(&pProtocolInfo->nSak);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_GET_SAK);

	error = GetUid(pProtocolInfo->aUid, &pProtocolInfo->nUidLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_GET_UID);
		
	return KNFC_SUCCESS;
}

/**
 * This function will be used to configure the protocol information such as ATQA, SAK, and UID in the given byte array.
 *
 * @param pBuffer a byte array of the protocol information
 * @param nBufferLength the length of the byte array 
 * @return error information
 */
KnfcError_t
KnfcSetProtocolInfoBytes_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KnfcProtocolInfo_P14P3A_t protocolInfo;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	KnfcMemoryCopy(&protocolInfo, pBuffer, nBufferLength);		

	switch(UID_SIZE_BIT_FRAME(protocolInfo.aAtqa)) {
		case 0:
			protocolInfo.nUidLength = (uint8_t)4;
			break;
		case 1:
			protocolInfo.nUidLength = (uint8_t)7;
			break;
		case 2:
			protocolInfo.nUidLength = (uint8_t)10;
			break;
		default:
			return KNFC_ERROR_P14P3A_UID_SIZE;
	}
	
	error = KnfcSetProtocolInfo_P14P3A(&protocolInfo);
	
	if(error != KNFC_SUCCESS) {
		return error;
	}

	return KNFC_SUCCESS;
}

/**
 * This function will be used to configure the protocol information such as ATQA, SAK, and UID.
 *
 * @param pProtocolInfo a data structure for protocol information of ISO 14443-3A 
 * @return error information
 */
KnfcError_t
KnfcSetProtocolInfo_P14P3A(KnfcProtocolInfo_P14P3A_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = SetAtqa(pProtocolInfo->aAtqa);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_SET_ATQA);

	error = SetSak(pProtocolInfo->nSak);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_SET_SAK);

	error = SetUid(pProtocolInfo->aUid, pProtocolInfo->nUidLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3A_SET_UID);
		
	return KNFC_SUCCESS;
}

////////////////////////////////////////////////////////
// External interface functions to access the information of P14P4A 
////////////////////////////////////////////////////////

#ifdef HARDWARE_P14P4_SUPPORT
KnfcError_t
KnfcGetProtocolInfoBytes_P14P4A(uint8_t *pBuffer, uint32_t *pnBufferLength) {

	KnfcError_t error;
	uint32_t bufferLength;
	KnfcProtocolInfo_P14P4A_t protocolInfo;

	if(pBuffer == NULL || pnBufferLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}
	
	error = KnfcGetProtocolInfo_P14P4A(&protocolInfo);
	
	if(error != KNFC_SUCCESS) {
		return error;
	}

	bufferLength = 2 /* ATQA size */ + 1 /* SAK size */ + protocolInfo.nAtsLength + protocolInfo.nUidLength;

	KnfcMemoryCopy(pBuffer, &protocolInfo, protocolInfo.nAtsLength + 3); /* ATQA + SAK + ATS */
	pBuffer += (protocolInfo.nAtsLength + 3);
	KnfcMemoryCopy(pBuffer, &protocolInfo.aUid, protocolInfo.nUidLength);
	
	*pnBufferLength = bufferLength;
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcGetProtocolInfo_P14P4A(KnfcProtocolInfo_P14P4A_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = GetAtqa(pProtocolInfo->aAtqa);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_GET_ATQA);

	error = GetSak(&pProtocolInfo->nSak);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_GET_SAK);

	error = GetAts(pProtocolInfo->aAts, &pProtocolInfo->nAtsLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_GET_ATS);	

	error = GetUid(pProtocolInfo->aUid, &pProtocolInfo->nUidLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_GET_UID);
		
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcSetProtocolInfoBytes_P14P4A(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	uint32_t atsLength;
	KnfcProtocolInfo_P14P4A_t protocolInfo;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// Get the length of ATS first
	atsLength = pBuffer[3] + 1 /* including itself */;

	KnfcMemoryCopy(&protocolInfo, pBuffer, 3 + atsLength); /* ATQA + SAK + ATS */
	pBuffer += (3 + atsLength);

	switch(UID_SIZE_BIT_FRAME(protocolInfo.aAtqa)) {
		case 0:
			protocolInfo.nUidLength = (uint8_t)4;
			break;
		case 1:
			protocolInfo.nUidLength = (uint8_t)7;
			break;
		case 2:
			protocolInfo.nUidLength = (uint8_t)10;
			break;
		default:
			return KNFC_ERROR_P14P4A_UID_SIZE;
	}

	KnfcMemoryCopy(&protocolInfo.aUid, pBuffer, protocolInfo.nUidLength);
	
	error = KnfcSetProtocolInfo_P14P4A(&protocolInfo);
	
	if(error != KNFC_SUCCESS) {
		return error;
	}

	return KNFC_SUCCESS;
}

KnfcError_t
KnfcSetProtocolInfo_P14P4A(KnfcProtocolInfo_P14P4A_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = SetAtqa(pProtocolInfo->aAtqa);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_SET_ATQA);

	error = SetSak(pProtocolInfo->nSak);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_SET_SAK);

	error = SetAts(pProtocolInfo->aAts, pProtocolInfo->nAtsLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_SET_ATS);
	
	error = SetUid(pProtocolInfo->aUid, pProtocolInfo->nUidLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P4A_SET_UID);
		
	return KNFC_SUCCESS;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// Hardware-dependent Implemenation
////////////////////////////////////////////////////////////////////////////////////////////

/* PCD - ISO 14443 Part 3 - Type A */

/**
 * This function will be used to activate the NFCC to operate as PCD mode with P14P3A.
 *
 * @param pParam a parameter for activating the PCD mode on ISO 14443-3A
 * @return error information
 */
 
KnfcError_t
KnfcPcdActivate_P14P3A(void *pParam) {
	unsigned short int pcd_state=0;
	int cnt=1000;

	KNFC_DEBUG("KnfcPcdActivate_P14P3A", "PCD ACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	// Rx Queue Reset
	KnfcRxqInit(&g_hNfcRxQueue);
	
	KnfcAfeConfigure(0,0,0,1,0);

	// Interrupt Enable
	NFC_INTR_IER = NFC_INTR_MASK(NFC_INTR0_ID) | NFC_INTR_MASK(NFC_INTR1_ID);
					
	// Modem Enable
	g_pPcdTypeA->Control.BitField.Clear=1;
	KnfcUSleep(100);
	g_pPcdTypeA->Control.BitField.Clear=0;
	g_pPcdTypeA->Control.BitField.FDT=3;

	//Configure Filter
	g_pPcdFilterTypeA->Low.BitField.FilterSelection=0;
	g_pPcdFilterTypeA->Low.BitField.FixedThreshold=0x1;
	g_pPcdFilterTypeA->Low.BitField.MatchedFilterThreshold=1;
	g_pPcdFilterTypeA->High.BitField.RLF_OffsetMargin=0;
	g_pPcdFilterTypeA->High.BitField.RLF_ThresholdMargin=0;
	g_pPcdFilterTypeA->High.BitField.RLF_CountMax=15;

	// Tx Queue Init
	g_pPcdTxqTypeA->Control.BitField.Clear=1;
	g_pPcdTxqTypeA->Control.BitField.Clear=0;

	// Rx Queue Init
	g_pPcdRxqTypeA->Control.BitField.Clear=1;
	g_pPcdRxqTypeA->Control.BitField.Clear=0;

	//Start Anti-collision Loop
	if(g_pPcdTypeA->Control.BitField.EnableAnticollision==1)
	{
		g_pPcdTypeA->Control.BitField.EnableAnticollision=0;
		KnfcUSleep(10000);
	}
	g_pPcdTypeA->Control.BitField.EnableAnticollision=1;

	while(pcd_state==0x0 && (cnt--))
	{
		pcd_state=g_pPcdTypeA->State.BitField.State;
		KnfcUSleep(100);
	}

	g_pPcdTypeA->Control.BitField.EnableAnticollision=0;
#endif

	return KNFC_SUCCESS;
}

/**
 * This function will be used to deactivate the NFCC operating as PCD mode with P14P3A.
 *
 * @param pParam a parameter for de-activating the PCD mode on ISO 14443-3A
 * @return error information
 */
 
KnfcError_t
KnfcPcdDeactivate_P14P3A(void *pParam) {

	KNFC_DEBUG("KnfcPcdDeactivate_P14P3A", "PCD DEACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	NFC_INTR_IER = 0; 
	KnfcAfeConfigure(0,0,0,0,0);
#endif
	
	return KNFC_SUCCESS;
}

/**
 * This function will be used to activate the NFCC to operate as PICC mode with P14P3A.
 *
 * @param pParam a parameter for activating the PICC mode on ISO 14443-3A
 * @return error information
 */

KnfcError_t
KnfcPiccActivate_P14P3A(void *pParam) {

	KnfcProtocolInfo_P14P3A_t *protocolInfo=(KnfcProtocolInfo_P14P3A_t *)pParam;
	
	KNFC_DEBUG("KnfcPiccActivate_P14P3A", "PICC ACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	// Rx Queue Reset
	KnfcRxqInit(&g_hNfcRxQueue);

	KnfcAfeConfigure(0,0,0,1,1);

	// Interrupt Enable
	NFC_INTR_IER = NFC_INTR_MASK(NFC_INTR3_ID)	| NFC_INTR_MASK(NFC_INTR4_ID);
					
	// Modem Enable
	g_pPiccTypeA->Control.BitField.Clear = 1;
	KnfcUSleep(100);
	g_pPiccTypeA->Control.BitField.Clear = 0;

	g_pPiccTypeA->Control.BitField.SizeUID = UID_SIZE_BIT_FRAME(protocolInfo->aAtqa);
	KnfcSetProtocolInfo_P14P3A(protocolInfo);
	
	//Configure Filter
	g_pPiccFilterTypeA->Low.BitField.FilterSelection = 0;
	g_pPiccFilterTypeA->Low.BitField.FixedThreshold = 0x1;
	g_pPiccFilterTypeA->Low.BitField.MatchedFilterThreshold = 1;
	g_pPiccFilterTypeA->High.BitField.RLF_OffsetMargin = 0;
	g_pPiccFilterTypeA->High.BitField.RLF_ThresholdMargin = 0;
	g_pPiccFilterTypeA->High.BitField.RLF_CountMax = 15;

	// Tx Queue Init
	g_pPiccTxqTypeA->Control.BitField.Clear = 1;
	g_pPiccTxqTypeA->Control.BitField.Clear = 0;

	// Rx Queue Init
	g_pPiccRxqTypeA->Control.BitField.Clear = 1;
	g_pPiccRxqTypeA->Control.BitField.Clear = 0;
#endif
	
	return KNFC_SUCCESS;
}

/**
 * This function will be used to activate the NFCC operating as PICC mode with P14P3A.
 *
 * @param pParam a parameter for de-activating the PICC mode on ISO 14443-3A
 * @return error information
 */

KnfcError_t
KnfcPiccDeactivate_P14P3A(void *pParam) {

	KNFC_DEBUG("KnfcPiccDeactivate_P14P3A", "PICC DEACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	NFC_INTR_IER = 0; 
	KnfcAfeConfigure(0,0,0,0,0);
#endif
	
	return KNFC_SUCCESS;
}

/**
 * This function will be used to transmit the given data with P14P3A protocol from PCD to PICC.
 */
 
static KnfcError_t 
TransmitPcdData_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t nTransmit = 0;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	for(; nTransmit < nBufferLength; ) {
		if(!g_pPcdTxqTypeA->State.BitField.Full) {
			g_pPcdTxqTypeA->Control.BitField.Data = pBuffer[nTransmit] & 0xFF;
			g_pPcdTxqTypeA->Control.BitField.WriteEn=1;
			g_pPcdTxqTypeA->Control.BitField.WriteEn=0;
			nTransmit++;
		}
	}
#endif

	return KNFC_SUCCESS;
}

/**
 * This is an event handler function to process the data received after transmitting data on P14P3A.
 */
 
void 
KnfcPcdReceiveDataHandler_P14P3A(void) {

	uint32_t rxBufferLength = 0;
	uint8_t rxBuffer[MAX_BUF_LENGTH];

#ifdef ARM926EJS_SUPPORT

	// If a CRC error occurs, re-transmit the buffered data.
	if(g_pPcdTypeA->State.BitField.ErrorCRC)
	{
		KnfcError_t error = TransmitPcdData_P14P3A(
							g_RetransBuffer.aBuffer, 
							g_RetransBuffer.nBufferLength);

		if(error != KNFC_SUCCESS) {
			KNFC_ERROR("KnfcPiccReceiveDataHandler_P14P3A - Retransmission Error: %d", error);
		}
		return;
	}

	// Get the received data
	rxBufferLength = KnfcRxqDequeue(&g_hNfcRxQueue, rxBuffer, MAX_BUF_LENGTH);
	
	if(rxBufferLength >= MAX_BUF_LENGTH) {
		KNFC_ERROR("KnfcPcdReceiveDataHandler_P14P3A - Too Short Rx Buffer");
		return;
	}
#endif

	// YOUNGSUN - CHKME
	// In order to eliminate 2 CRC bytes,
	// we are going to employ rxBufferLength - 2 as the length of the data received.
	KnfcProcessReaderExchangeP14P3Data(rxBuffer, rxBufferLength-2);
}

/**
 * This function will be used to perform exchanging data between PCD and PICC.
 *
 * @param pConnection the currently established connection to an adjacent PICC
 * @param pBuffer the data buffer to be transmitted to the selected PICC
 * @param nBufferLength the length of the data buffer
 * @return error information
 */
 
KnfcError_t
KnfcPcdExchangeData_P14P3A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KNFC_DEBUG("KnfcPcdExchangeData_P14P3A", "PCD DATA EXCHANGE...");

	if(pConnection == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// Do buffering the data to be transmitted for the re-transmission, if CRC error occurs. 
	g_RetransBuffer.nBufferLength = nBufferLength;
	KnfcMemoryCopy(g_RetransBuffer.aBuffer, pBuffer, nBufferLength);

	// Send data 
	error = TransmitPcdData_P14P3A(pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcPcdExchangeData_P14P3A - Error: %d", error);
	}

	// If the data will be received, KnfcPcdReceiveDataHandler_P14P3A will be invoked.

	return error;
}

/* PICC - ISO 14443 Part 3 - Type A */

/**
 * This function will be used to transmit the given data with P14P3A protocol from PICC to PCD.
 */

static KnfcError_t
TransmitPiccData_P14P3A(uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t nTransmit = 0;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	for(; nTransmit < nBufferLength; ) {
		if(!g_pPiccTxqTypeA->State.BitField.Full) {
			g_pPiccTxqTypeA->Control.BitField.Data = pBuffer[nTransmit] & 0xFF;
			g_pPiccTxqTypeA->Control.BitField.WriteEn=1;
			g_pPiccTxqTypeA->Control.BitField.WriteEn=0;
			nTransmit++;
		}
	}
#endif

	return KNFC_SUCCESS;
}

/**
 * This is an event handler function to process the data received after transmitting data on P14P3A.
 */
 
void 
KnfcPiccReceiveDataHandler_P14P3A(void) {

	uint32_t rxBufferLength = 0;
	uint8_t rxBuffer[MAX_BUF_LENGTH];

#ifdef ARM926EJS_SUPPORT

	// If a CRC error occurs, re-transmit the buffered data.
	if(g_pPiccTypeA->StateLow.BitField.ErrorCRC) {
		KnfcError_t error = TransmitPiccData_P14P3A(
							g_RetransBuffer.aBuffer, 
							g_RetransBuffer.nBufferLength);

		if(error != KNFC_SUCCESS) {
			KNFC_ERROR("KnfcPiccReceiveDataHandler_P14P3A - Retransmission Error: %d", error);
		}
		
		return;
	}

	// Get the received data
	rxBufferLength = KnfcRxqDequeue(&g_hNfcRxQueue, rxBuffer, MAX_BUF_LENGTH);
	
	if(rxBufferLength >= MAX_BUF_LENGTH) {
		KNFC_ERROR("KnfcPiccReceiveDataHandler_P14P3A - Too Short Rx Buffer");
		return;
	}
#endif

	// YOUNGSUN - CHKME
	// In order to eliminate 2 CRC bytes,
	// we are going to employ rxBufferLength - 2 as the length of the data received.
	KnfcProcessCardEmulationCommand(rxBuffer, rxBufferLength-2);
}

/**
 * This function will be used to perform exchanging data between PICC and PCD.
 *
 * @param pConnection the currently established connection to an adjacent PCD
 * @param pBuffer the data buffer to be transmitted to the connected PCD
 * @param nBufferLength the length of the data buffer
 * @return error information
 */

KnfcError_t
KnfcPiccExchangeData_P14P3A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KNFC_DEBUG("KnfcPiccExchangeData_P14P3A", "PICC DATA EXCHANGE...");

	if(pConnection == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// Send data 
	error = TransmitPiccData_P14P3A(pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcPiccExchangeData_P14P3A - Error: %d", error);
	}

	// If the data will be received, KnfcPiccReceiveDataHandler_P14P3A will be invoked.

	return error;
}

/**
 * This function will be invoked when the PCD selects a PICC to communicate.
 */

void 
KnfcPcdSelectHandler_P14P3A(void) {

	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3A];
	uint32_t infoBufferLength = 0;
	
	KNFC_DEBUG("KnfcPcdErrorHandler_P14P3A", "HANDLER INVOKED...");

#ifdef ARM926EJS_SUPPORT
	// Enable to handle the interrupts by the error events
	NFC_INTR_IER |= NFC_INTR_MASK(NFC_INTR2_ID); 	
#endif

	KnfcStopPollingLoop();
	KnfcGetProtocolInfoBytes_P14P3A(infoBuffer, &infoBufferLength);
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_A, infoBuffer, infoBufferLength);
}

/**
 * This function will handle the error event from the NFC modem.
 */

void 
KnfcPcdErrorEventHandler_P14P3A(void) {
	KNFC_DEBUG("KnfcPcdErrorHandler_P14P3A", "PCD Error");

	// Not supported yet.
}

/**
 * This function will handle the error event from the NFC modem.
 */
 
void 
KnfcPiccErrorEventHandler_P14P3A(void) {
	KNFC_DEBUG("KnfcPiccErrorHandler_P14P3A", "PCD Error");

	// Not supported yet.
}

/**
 * This function will be invoked after the PICC is activated.
 */
 
void 
KnfcPiccActivateHandler_P14P3A(void) {
	KNFC_DEBUG("KnfcPiccActivateHandler_P14P3A", "PICC Active");

#ifdef ARM926EJS_SUPPORT
	// Enable to handle the interrupts by the error events
	NFC_INTR_IER |= NFC_INTR_MASK(NFC_INTR5_ID); 	
#endif
	
	KnfcProcessCardEmulationEvent(W_PROP_ISO_14443_3_A, W_EMUL_EVENT_SELECTION);
}

#ifdef HARDWARE_P14P4_SUPPORT

/* ISO 14443 Part 4 Compliant - Type A */

KnfcError_t
KnfcPcdActivate_P14P4A(void *pParam) {

	KNFC_DEBUG("KnfcPcdActivate_P14P4A", "PCD ACTIVIATE...");
	return KNFC_SUCCESS;
}
 
KnfcError_t
KnfcPcdDeactivate_P14P4A(void *pParam) {

	KNFC_DEBUG("KnfcPcdDeactivate_P14P4A", "PCD DEACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccActivate_P14P4A(void *pParam) {
	
	KNFC_DEBUG("KnfcPiccActivate_P14P4A", "PICC ACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccDeactivate_P14P4A(void *pParam) {

	KNFC_DEBUG("KnfcPiccDeactivate_P14P4A", "PICC DEACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPcdExchangeData_P14P4A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcPiccExchangeData_P14P4A", "PCD DATA EXCHANGE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccExchangeData_P14P4A(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcPiccExchangeData_P14P4A", "PICC DATA EXCHANGE...");
	return KNFC_SUCCESS;
}

#endif
