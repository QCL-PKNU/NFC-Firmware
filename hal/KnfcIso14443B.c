/*
 * KnfcIso14443B.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcIso14443B.h"
#ifdef ARM926EJS_SUPPORT
#include "KnfcDalConfig.h"
#include "KnfcRxQueue.h"
#endif

////////////////////////////////////////////////////////
// Static Variable Definition
////////////////////////////////////////////////////////

#ifdef ARM926EJS_SUPPORT
//PCD APB Registers
static volatile NFC_TypeB_PCD_Struct * const g_pPcdTypeB =
	(volatile NFC_TypeB_PCD_Struct *)NFC_TYPEB_PCD_BASE_ADDR;

static volatile NFC_TypeB_RCV_PICC_Info_Struct * const g_pPiccRcvInfoTypeB =
	(volatile NFC_TypeB_RCV_PICC_Info_Struct *)NFC_TYPEB_PCD_RCV_PICC_INFO_ADDR;

static volatile NFC_TypeB_Conf_PCD_Info_Struct * const g_pPcdConfInfoTypeB =
	(volatile NFC_TypeB_Conf_PCD_Info_Struct *)NFC_TYPEB_CONF_PCD_INFO_ADDR;

//PICC APB Registers
static volatile NFC_TypeB_PICC_Struct * const g_pPiccTypeB =
	(volatile NFC_TypeB_PICC_Struct *)NFC_TYPEB_PICC_BASE_ADDR;

static volatile NFC_TypeB_Conf_PICC_Info_Struct * const g_pPiccConfInfoTypeB =
	(volatile NFC_TypeB_Conf_PICC_Info_Struct *)NFC_TYPEB_CONF_PICC_INFO_ADDR;

static volatile NFC_TypeB_RCV_PCD_Info_Struct * const g_pPcdRcvInfoTypeB =
	(volatile NFC_TypeB_RCV_PCD_Info_Struct *)NFC_TYPEB_PICC_RCV_PCD_INFO_ADDR;

static volatile NFC_Tx_Queue_Type * const g_pPcdTxqTypeB	 =
	(volatile NFC_Tx_Queue_Type *) NFC_TYPEB_PCD_TX_QUEUE_ADDR;

static volatile NFC_Rx_Queue_Type * const g_pPcdRxqTypeB =
	(volatile NFC_Rx_Queue_Type *) NFC_TYPEB_PCD_RX_QUEUE_ADDR;

static volatile NFC_Tx_Queue_Type * const g_pPiccTxqTypeB =
	(volatile NFC_Tx_Queue_Type *) NFC_TYPEB_PICC_TX_QUEUE_ADDR;

static volatile NFC_Rx_Queue_Type * const g_pPiccRxqTypeB =
	(volatile NFC_Rx_Queue_Type *) NFC_TYPEB_PICC_RX_QUEUE_ADDR;

static volatile NFC_TypeB_PCD_Filter_Control * const g_pPcdFilterTypeB =
	(volatile NFC_TypeB_PCD_Filter_Control *) NFC_TYPEB_PCD_FILTER_ADDR;

static volatile NFC_TypeB_PICC_Filter_Control * const g_pPiccFilterTypeB =
	(volatile NFC_TypeB_PICC_Filter_Control *) NFC_TYPEB_PICC_FILTER_ADDR;
#endif

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

/* Accessors & Mutators for P14P3B */

static KnfcError_t GetAtqb(uint8_t *pAtqb);

static KnfcError_t SetAtqb(uint8_t *pAtqb);

static KnfcError_t GetMbliCid(uint8_t *pnMbliCid);

static KnfcError_t SetMbliCid(uint8_t nMBbliCid);

static KnfcError_t GetHigherLayerResponse(uint8_t *pBuffer, uint32_t *pnBufferLength);

static KnfcError_t SetHigherLayerResponse(uint8_t *pBuffer, uint32_t nBufferLength);

static KnfcError_t GetResponseToAttribCommand(uint8_t *pBuffer, uint32_t *pnBufferLength);

 ////////////////////////////////////////////////////////
// Internal interface functions to access the information of P14P3B 
//
// These functions will set or get the information 
// by accessing the NFC controller.
////////////////////////////////////////////////////////

static KnfcError_t
GetAtqb(uint8_t *pAtqb){

	if(pAtqb == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	//Based on Big Endian Coding: 
	//Atqb - {Atqb[0], Atqb[1], ... ,Atqb[10]}

#ifdef ARM926EJS_SUPPORT
	pAtqb[0] = (uint8_t)(g_pPiccRcvInfoTypeB->PUPI_Low >> 8);
	pAtqb[1] = (uint8_t)(g_pPiccRcvInfoTypeB->PUPI_Low >> 0);
	pAtqb[2] = (uint8_t)(g_pPiccRcvInfoTypeB->PUPI_High >> 8);
	pAtqb[3] = (uint8_t)(g_pPiccRcvInfoTypeB->PUPI_High >> 0);
	pAtqb[4] = g_pPiccRcvInfoTypeB->AppDataLow.BitField.AFI;
	pAtqb[5] = g_pPiccRcvInfoTypeB->AppDataLow.BitField.CRC_B_AID_Low;
	pAtqb[6] = g_pPiccRcvInfoTypeB->AppDataHigh.BitField.CRC_B_AID_High;
	pAtqb[7] = g_pPiccRcvInfoTypeB->AppDataHigh.BitField.NumApps;
	pAtqb[8] = g_pPiccRcvInfoTypeB->ProtocolInfoLow.BitField.BitRate & 0xFF;
	pAtqb[9] = (((uint8_t)g_pPiccRcvInfoTypeB->ProtocolInfoLow.BitField.MaxFrameSize << 4) & 0xF0) |
				((uint8_t)g_pPiccRcvInfoTypeB->ProtocolInfoLow.BitField.ProtocolType & 0x0F);
	pAtqb[10]= (((uint8_t)g_pPiccRcvInfoTypeB->ProtocolInfoHigh.BitField.FWI << 4) & 0xF0) |
			   (((uint8_t)g_pPiccRcvInfoTypeB->ProtocolInfoHigh.BitField.ADC << 2) & 0x0C) |
				((uint8_t)g_pPiccRcvInfoTypeB->ProtocolInfoHigh.BitField.FWI & 0x03);
#endif
	
	return KNFC_SUCCESS;
}

static KnfcError_t 
SetAtqb(uint8_t *pAtqb){

	if(pAtqb == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	//Based on Big Endian Coding: 
	//Atqb - {Atqb[0], Atqb[1], ... ,Atqb[10]}

#ifdef ARM926EJS_SUPPORT
	g_pPiccConfInfoTypeB->PUPI_Low  = (((uint16_t)pAtqb[0] << 8) & 0xFF00) | pAtqb[1];
	g_pPiccConfInfoTypeB->PUPI_High = (((uint16_t)pAtqb[2] << 8) & 0xFF00) | pAtqb[3];
	g_pPiccConfInfoTypeB->AppDataLow.BitField.AFI = pAtqb[4];
	g_pPiccConfInfoTypeB->AppDataLow.BitField.CRC_B_AID_Low = pAtqb[5];
	g_pPiccConfInfoTypeB->AppDataHigh.BitField.CRC_B_AID_High = pAtqb[6];
	g_pPiccConfInfoTypeB->AppDataHigh.BitField.NumApps = pAtqb[7];
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.BitRate = pAtqb[8];
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.ProtocolType = pAtqb[9] & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.MaxFrameSize = (pAtqb[9] >> 4) & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.FWI = (pAtqb[10] >> 4) & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.ADC = (pAtqb[10] >> 2) & 0x03;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.FO = pAtqb[10] & 0x03;
#endif
	
	return KNFC_SUCCESS;
}

static KnfcError_t
GetMbliCid(uint8_t *pnMbliCid){

	if(pnMbliCid == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	*pnMbliCid = (((uint8_t)g_pPiccRcvInfoTypeB->MBLI_CID.BitField.MBLI << 4) & 0xF0) |
				  ((uint8_t)g_pPiccRcvInfoTypeB->MBLI_CID.BitField.CID & 0x0F);
#endif

	return KNFC_SUCCESS;
}

static KnfcError_t 
SetMbliCid(uint8_t nMbliCid){

#ifdef ARM926EJS_SUPPORT
	g_pPiccConfInfoTypeB->MBLI_CID.BitField.MBLI = (nMbliCid >> 4) & 0x0F;
	g_pPiccConfInfoTypeB->MBLI_CID.BitField.CID = (nMbliCid >> 0) & 0x0F;
#endif

	return KNFC_SUCCESS;
}

static KnfcError_t
GetHigherLayerResponse(uint8_t *pBuffer, uint32_t *pnBufferLength) {

	if(pBuffer == NULL || pnBufferLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// Get the higher layer response from the NFCC

	// Get the size of the higher layer response
	*pnBufferLength = 128;

	return KNFC_SUCCESS;
}

static KnfcError_t
SetHigherLayerResponse(uint8_t *pBuffer, uint32_t nBufferLength) {

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// Set the higher layer response to the NFCC

	return KNFC_SUCCESS;
}

static KnfcError_t 
GetResponseToAttribCommand(uint8_t *pBuffer, uint32_t *pnBufferLength) {

	if(pBuffer == NULL || pnBufferLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	// Get the answer to ATTRIB command from the NFCC

	// Get the size of the higher layer response
	*pnBufferLength = 128;

	return KNFC_SUCCESS;
}

////////////////////////////////////////////////////////
// External interface functions to access the information of P14P3B 
////////////////////////////////////////////////////////

/**
 * This function will return a byte array of the protocol information including ATQB, MBLICID, and HigherLayerResponse.
 *
 * @param pBuffer a byte array to get the protocol information
 * @param pnBufferLength the length of the byte array
 * @return error information
 */

KnfcError_t
KnfcGetProtocolInfoBytes_P14P3B(uint8_t *pBuffer, uint32_t *pnBufferLength) {

	KnfcError_t error;
	uint32_t bufferLength;
	KnfcProtocolInfo_P14P3B_t protocolInfo;

	if(pBuffer == NULL || pnBufferLength == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = KnfcGetProtocolInfo_P14P3B(&protocolInfo);

	if(error == KNFC_SUCCESS) {

		bufferLength = ATQB_LENGTH + MLBInCID_LENGTH + protocolInfo.nHigherLayerResponseLength;
		KnfcMemoryCopy(pBuffer, &protocolInfo, bufferLength);
		*pnBufferLength = bufferLength;
	}

	return error;
}

/**
 * This function will return the protocol information including ATQB, MBLICID, and HigherLayerResponse.
 *
 * @param pProtocolInfo a data structure for protocol information of ISO 14443-3B
 * @return error information
 */

KnfcError_t
KnfcGetProtocolInfo_P14P3B(KnfcProtocolInfo_P14P3B_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = GetAtqb(pProtocolInfo->aAtqb);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_GET_ATQB);

	error = GetMbliCid(&pProtocolInfo->nMlbiCid);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_GET_MBLInCID);
	
	error = GetHigherLayerResponse(pProtocolInfo->aHigherLayerResponse, 
									&pProtocolInfo->nHigherLayerResponseLength);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_GET_HLRESP);
	
	return KNFC_SUCCESS;
}

/**
 * This function will be used to configure the protocol information such as ATQB, MBLICID, and HigherLayerResponse in the given byte array.
 *
 * @param pBuffer a byte array of the protocol information
 * @param nBufferLength the length of the byte array 
 * @return error information
 */

KnfcError_t
KnfcSetProtocolInfoBytes_P14P3B(uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KnfcProtocolInfo_P14P3B_t protocolInfo;

	if(pBuffer == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	if(nBufferLength < MIN_BUF_LENGTH_P14P3B || nBufferLength > MAX_BUF_LENGTH_P14P3B) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	KnfcMemoryCopy(&protocolInfo, pBuffer, nBufferLength);

	error = KnfcSetProtocolInfo_P14P3B(&protocolInfo);

	if(error == KNFC_SUCCESS) {
		protocolInfo.nHigherLayerResponseLength = (uint8_t)(nBufferLength - ATQB_LENGTH);
	}

	return error;
}

/**
 * This function will be used to configure the protocol information such as ATQB, MBLICID, and HigherLayerResponse .
 *
 * @param pProtocolInfo a data structure for protocol information of ISO 14443-3B 
 * @return error information
 */

KnfcError_t
KnfcSetProtocolInfo_P14P3B(KnfcProtocolInfo_P14P3B_t *pProtocolInfo) {

	KnfcError_t error;

	if(pProtocolInfo == NULL) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

	error = SetAtqb(pProtocolInfo->aAtqb);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_SET_ATQB);

	error = SetMbliCid(pProtocolInfo->nMlbiCid);
	KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_SET_MBLInCID);

	if(pProtocolInfo->nHigherLayerResponseLength > 0) {

		error = SetHigherLayerResponse(pProtocolInfo->aHigherLayerResponse, 
										pProtocolInfo->nHigherLayerResponseLength);
		KNFC_CHECK(error == KNFC_SUCCESS, KNFC_ERROR_P14P3B_SET_HLRESP);
	}

	return KNFC_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Hardware-dependent Implemenation
////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This function will send a HLTB command.
 *
 * @return error information
 */

KnfcError_t 
KnfcPcdSendHalt_P14P3B(void) {
	unsigned short int pcd_state;
	unsigned short int picc_state;
	int cnt=16;

#ifdef ARM926EJS_SUPPORT
	pcd_state = g_pPcdTypeB->State.BitField.State;

	if(pcd_state!=0x6) {
		return KNFC_FAILURE;
	}

	g_pPcdTypeB->Control.BitField.EnableHalt = 1;

	while((cnt--) > 0)	{
		pcd_state=g_pPcdTypeB->State.BitField.State;

		if(pcd_state==0x0) {
			return KNFC_SUCCESS;
		}
	}

	g_pPcdTypeB->Control.BitField.EnableHalt = 0;
#endif

	return KNFC_SUCCESS;
}

/* PCD - ISO 14443 Part 3 - Type B */

/**
 * This function will be used to activate the NFCC to operate as PCD mode with P14P3B.
 *
 * @param pParam a parameter for activating the PCD mode
 * @return error information
 */
 
KnfcError_t
KnfcPcdActivate_P14P3B(void *pParam) {
	unsigned short int pcd_state=0;
	int cnt=1000;

	KNFC_DEBUG("KnfcPcdActivate_P14P3B", "PCD ACTIVIATE...");

#ifdef ARM926EJS_SUPPORT

	KnfcPcdSendHalt_P14P3B();
	// Rx Queue Reset
	KnfcRxqInit(&g_hNfcRxQueue);
	KnfcAfeConfigure(0,0,0,2,0);

	// Interrupt Enable
	NFC_INTR_IER = NFC_INTR_MASK(NFC_INTR6_ID) 
				| NFC_INTR_MASK(NFC_INTR7_ID) 
				| NFC_INTR_MASK(NFC_INTR8_ID);
					
	// Modem Enable
	g_pPcdTypeB->Control.Reg = 0;
	g_pPcdTypeB->Control.BitField.Clear = 1;
	KnfcUSleep(100);
	g_pPcdTypeB->Control.BitField.Clear = 0;
	g_pPcdTypeB->Control.BitField.ConfEGT = 2;

	g_pPcdTypeB->AFI_Rule.BitField.ConfFwiTimer=0xf;

	// Configure Filter
	g_pPcdFilterTypeB->Low.BitField.FilterSelection = 0;
	g_pPcdFilterTypeB->Low.BitField.FixedThreshold = 0x1;
	g_pPcdFilterTypeB->High.BitField.RLF_CountMax = 255;
	g_pPcdFilterTypeB->High.BitField.RLF_OffsetMargin = 0;
	g_pPcdFilterTypeB->High.BitField.RLF_ThresholdMargin = 0;

	// Tx Queue Init
	g_pPcdTxqTypeB->BitField.Clear=1;
	g_pPcdTxqTypeB->BitField.Clear=0;

	// Rx Queue Init
	g_pPcdRxqTypeB->BitField.Clear=1;
	g_pPcdRxqTypeB->BitField.Clear=0;

	g_pPcdConfInfoTypeB->ParamLow.BitField.EOF = 1;
	g_pPcdConfInfoTypeB->ParamLow.BitField.SOF = 1;
	g_pPcdConfInfoTypeB->ParamLow.BitField.MinTR1 = 3;
	g_pPcdConfInfoTypeB->ParamLow.BitField.MinTR0 = 3;
	g_pPcdConfInfoTypeB->ParamLow.BitField.MaxFrameSize = 0;
	g_pPcdConfInfoTypeB->ParamLow.BitField.PCDTxRate = 0;
	g_pPcdConfInfoTypeB->ParamLow.BitField.PICCTxRate = 0;
	g_pPcdConfInfoTypeB->ParamHigh.BitField.ProtocolType = 1;
	g_pPcdConfInfoTypeB->ParamHigh.BitField.CID = 0;	

	//Start Anti-collision Loop
	if(g_pPcdTypeB->Control.BitField.EnableAnticollision==1)
	{
		g_pPcdTypeB->Control.BitField.EnableAnticollision=0;
		KnfcUSleep(10000);
	}
	g_pPcdTypeB->Control.BitField.EnableAnticollision=1;

	while(pcd_state==0x0 && (cnt--))
	{
		pcd_state=g_pPcdTypeB->State.BitField.State;
		KnfcUSleep(100);
	}

	g_pPcdTypeB->Control.BitField.EnableAnticollision=0;	
#endif

	return KNFC_SUCCESS;
}

/**
 * This function will be used to deactivate the NFCC operating as PCD mode with P14P3B.
 *
 * @param pParam a parameter for deactivating the PCD mode
 * @return error information
 */
 
KnfcError_t
KnfcPcdDeactivate_P14P3B(void *pParam) {

	KNFC_DEBUG("KnfcPcdDeactivate_P14P3B", "PCD DEACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	NFC_INTR_IER = 0;
	KnfcAfeConfigure(0,0,0,0,0);
#endif

	return KNFC_SUCCESS;
}

/**
 * This function will be used to activate the NFCC to operate as PICC mode with P14P3B.
 *
 * @param pParam a parameter for activating the PICC mode 
 * @return error information
 */

KnfcError_t
KnfcPiccActivate_P14P3B(void *pParam) {

	KnfcError_t error;
	KnfcProtocolInfo_P14P3B_t *protocolInfo = (KnfcProtocolInfo_P14P3B_t *)pParam;

	KNFC_DEBUG("KnfcPiccActivate_P14P3B", "PICC ACTIVIATE...");

#ifdef ARM926EJS_SUPPORT		
	// Rx Queue Reset
	KnfcRxqInit(&g_hNfcRxQueue);
	KnfcAfeConfigure(0,0,0,2,1);

	// Interrupt Enable
	NFC_INTR_IER = NFC_INTR_MASK(NFC_INTR10_ID) 
				| NFC_INTR_MASK(NFC_INTR11_ID) 
				| NFC_INTR_MASK(NFC_INTR12_ID);

	// Modem Enable
	g_pPiccTypeB->Control.Reg=0;
	g_pPiccTypeB->Control.BitField.Clear=1;
	KnfcUSleep(100);
	g_pPiccTypeB->Control.BitField.Clear=0;
	g_pPiccTypeB->Control.BitField.ConfEGT=2;

	// Tx Queue Init
	g_pPiccTxqTypeB->BitField.Clear=1;
	g_pPiccTxqTypeB->BitField.Clear=0;

	// Rx Queue Init
	g_pPiccRxqTypeB->BitField.Clear=1;
	g_pPiccRxqTypeB->BitField.Clear=0;
	
	// Configure Filter
	g_pPiccFilterTypeB->Low.BitField.FilterSelection=0;
	g_pPiccFilterTypeB->Low.BitField.FixedThreshold=0x1;
	g_pPcdFilterTypeB->High.BitField.RLF_CountMax=64;
	g_pPcdFilterTypeB->High.BitField.RLF_OffsetMargin=0;
	g_pPcdFilterTypeB->High.BitField.RLF_ThresholdMargin=0;	

	// Configure PICC Parameter
	g_pPiccConfInfoTypeB->PUPI_High = (((uint16_t)protocolInfo->aAtqb[0] << 8) & 0xFF00) | protocolInfo->aAtqb[1];
	g_pPiccConfInfoTypeB->PUPI_Low = (((uint16_t)protocolInfo->aAtqb[2] << 8) & 0xFF00) | protocolInfo->aAtqb[3];
	g_pPiccConfInfoTypeB->AppDataLow.BitField.AFI = protocolInfo->aAtqb[4];
	g_pPiccConfInfoTypeB->AppDataLow.BitField.CRC_B_AID_Low = protocolInfo->aAtqb[5];
	g_pPiccConfInfoTypeB->AppDataHigh.BitField.CRC_B_AID_High = protocolInfo->aAtqb[6];
	g_pPiccConfInfoTypeB->AppDataHigh.BitField.NumApps = protocolInfo->aAtqb[7];
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.BitRate = protocolInfo->aAtqb[8];
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.ProtocolType = protocolInfo->aAtqb[9] & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoLow.BitField.MaxFrameSize = (protocolInfo->aAtqb[9] >> 4) & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.FO = (protocolInfo->aAtqb[10] >> 4) & 0x0F;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.ADC = (protocolInfo->aAtqb[10] >> 2) & 0x03;
	g_pPiccConfInfoTypeB->ProtocolInfoHigh.BitField.FWI = protocolInfo->aAtqb[10] & 0x03;
#endif

	return KNFC_SUCCESS;
}

/**
 * This function will be used to activate the NFCC operating as PICC mode with P14P3B.
 *
 * @param pParam a parameter for deactivating the PICC mode
 * @return error information
 */

KnfcError_t
KnfcPiccDeactivate_P14P3B(void *pParam) {

	KNFC_DEBUG("KnfcPiccDeactivate_P14P3B", "PICC DEACTIVIATE...");

#ifdef ARM926EJS_SUPPORT
	NFC_INTR_IER = 0;
	KnfcAfeConfigure(0,0,0,0,0);
#endif

	return KNFC_SUCCESS;
}

/**
 * This function will be used to transmit the given data with P14P3B protocol from PCD to PICC.
 */
 
static KnfcError_t 
TransmitPcdData_P14P3B(uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t nTransmit = 0;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	for(; nTransmit < nBufferLength; ) {
		if(!g_pPcdTxqTypeB->BitField.Full) {
			g_pPcdTxqTypeB->BitField.Data = pBuffer[nTransmit] & 0xFF;
			g_pPcdTxqTypeB->BitField.WriteEn = 1;
			g_pPcdTxqTypeB->BitField.WriteEn = 0;
			nTransmit++;
		}
	}
#endif

	return KNFC_SUCCESS;
}

/**
 * This is an event handler function to process the data received after transmitting data on P14P3B.
 */
 
void 
KnfcPcdReceiveDataHandler_P14P3B(void) {

	uint32_t rxBufferLength = 0;
	uint8_t rxBuffer[MAX_BUF_LENGTH];

#ifdef ARM926EJS_SUPPORT
	rxBufferLength = KnfcRxqDequeue(&g_hNfcRxQueue, rxBuffer, MAX_BUF_LENGTH);
	
	if(rxBufferLength >= MAX_BUF_LENGTH) {
		KNFC_ERROR("KnfcPcdReceiveDataHandler_P14P3B - Too Short Rx Buffer");
		return;
	}
#endif

	// YOUNGSUN - CHKME
	// In order to eliminate 2 CRC bytes,
	// we are going to employ rxBufferLength - 2 as the length of the data received.
	KnfcProcessReaderExchangeP14P3Data(rxBuffer, rxBufferLength);
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
KnfcPcdExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KNFC_DEBUG("KnfcPcdExchangeData_P14P3B", "PCD DATA EXCHANGE...");

	if(pConnection == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// send data 
	error = TransmitPcdData_P14P3B(pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcPcdExchangeData_P14P3B - Error: %d", error);
	}

	// If the data will be received, KnfcPcdReceiveDataHandler_P14P3B will be invoked.

	return error;
}

/* PICC - ISO 14443 Part 3 - Type A */

/**
 * This function will be used to transmit the given data with P14P3B protocol from PICC to PCD.
 */

static KnfcError_t
TransmitPiccData_P14P3B(uint8_t *pBuffer, uint32_t nBufferLength) {

	uint32_t nTransmit = 0;

	if(pBuffer == NULL || nBufferLength == 0) {
		return KNFC_ERROR_INVALID_PARAMETER;
	}

#ifdef ARM926EJS_SUPPORT
	for(; nTransmit < nBufferLength; ) {
		if(!g_pPiccTxqTypeB->BitField.Full) {
			g_pPiccTxqTypeB->BitField.Data = pBuffer[nTransmit] & 0xFF;
			g_pPiccTxqTypeB->BitField.WriteEn = 1;
			g_pPiccTxqTypeB->BitField.WriteEn = 0;
			nTransmit++;
		}
	}
#endif

	return KNFC_SUCCESS;
}

/**
 * This is an event handler function to process the data received after transmitting data on P14P3B.
 */
 
void 
KnfcPiccReceiveDataHandler_P14P3B(void) {

	uint32_t rxBufferLength = 0;
	uint8_t rxBuffer[MAX_BUF_LENGTH];

#ifdef ARM926EJS_SUPPORT
	rxBufferLength = KnfcRxqDequeue(&g_hNfcRxQueue, rxBuffer, MAX_BUF_LENGTH);
	
	if(rxBufferLength >= MAX_BUF_LENGTH) {
		KNFC_ERROR("KnfcPcdReceiveDataHandler_P14P3B - Too Short Rx Buffer");
		return;
	}
#endif

	// YOUNGSUN - CHKME
	// In order to eliminate 2 CRC bytes,
	// we are going to employ rxBufferLength - 2 as the length of the data received.
	KnfcProcessCardEmulationCommand(rxBuffer, rxBufferLength);
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
KnfcPiccExchangeData_P14P3B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KnfcError_t error;
	KNFC_DEBUG("KnfcPiccExchangeData_P14P3B", "PICC DATA EXCHANGE...");

	if(pConnection == NULL) {
		return KNFC_ERROR_READER_INVALID_CONNECTION;
	}

	// send data 
	error = TransmitPiccData_P14P3B(pBuffer, nBufferLength);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcPiccExchangeData_P14P3B - Error: %d", error);
	}

	// If the data will be received, KnfcPiccReceiveDataHandler_P14P3B will be invoked.

	return error;
}

/**
 * This function will be invoked when the PCD selects a PICC to communicate.
 */

void 
KnfcPcdSelectHandler_P14P3B(void) {
	uint8_t infoBuffer[MAX_BUF_LENGTH_P14P3B];
	uint32_t infoBufferLength = 0;
	
	KNFC_DEBUG("KnfcPcdErrorHandler_P14P3B", "HANDLER INVOKED...");

#ifdef ARM926EJS_SUPPORT
	// Enable to handle the interrupts by the error events
	NFC_INTR_IER |= NFC_INTR_MASK(NFC_INTR8_ID); 
#endif

	KnfcStopPollingLoop();
	KnfcGetProtocolInfoBytes_P14P3B(infoBuffer, &infoBufferLength);
	KnfcProcessReaderDriverEvent(W_NFCC_PROTOCOL_READER_ISO_14443_3_B, infoBuffer, infoBufferLength);
}

/**
 * This function will handle the error event from the NFC modem.
 */

void 
KnfcPcdErrorEventHandler_P14P3B(void) {
	KNFC_DEBUG("KnfcPcdErrorHandler_P14P3B", "PCD Error");

	// Not support yet.
}

/**
 * This function will handle the error event from the NFC modem.
 */
 
void 
KnfcPiccErrorEventHandler_P14P3B(void) {
	KNFC_DEBUG("KnfcPiccErrorHandler_P14P3B", "PCD Error");

	// Not support yet.
}

/**
 * This function will be invoked after the PICC is activated.
 */
 
void 
KnfcPiccActivateHandler_P14P3B(void) {
	KNFC_DEBUG("KnfcPiccActivateHandler_P14P3B", "PICC Active");

#ifdef ARM926EJS_SUPPORT
	// Enable to handle the interrupts by the error events
	NFC_INTR_IER |= NFC_INTR_MASK(NFC_INTR11_ID); 
#endif
	
	KnfcProcessCardEmulationEvent(W_PROP_ISO_14443_4_B, W_EMUL_EVENT_SELECTION);
}

#ifdef HARDWARE_P14P4_SUPPORT

/* ISO 14443 Part 4 Compliant - Type B */

KnfcError_t
KnfcPcdActivate_P14P4B(void *pParam) {

	KNFC_DEBUG("KnfcPcdActivate_P14P4B", "PCD ACTIVIATE...");
	return KNFC_SUCCESS;
}
 
KnfcError_t
KnfcPcdDeactivate_P14P4B(void *pParam) {

	KNFC_DEBUG("KnfcPcdDeactivate_P14P4B", "PCD DEACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccActivate_P14P4B(void *pParam) {
	
	KNFC_DEBUG("KnfcPiccActivate_P14P4B", "PICC ACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccDeactivate_P14P4B(void *pParam) {

	KNFC_DEBUG("KnfcPiccDeactivate_P14P4B", "PICC DEACTIVIATE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPcdExchangeData_P14P4B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcPiccExchangeData_P14P4B", "PCD DATA EXCHANGE...");
	return KNFC_SUCCESS;
}

KnfcError_t
KnfcPiccExchangeData_P14P4B(void *pConnection, uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcPiccExchangeData_P14P4B", "PICC DATA EXCHANGE...");
	return KNFC_SUCCESS;
}

#endif
