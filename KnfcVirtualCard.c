/*
 * KnfcVirtualCard.c
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#include "KnfcVirtualCard.h"

static Ndef2Memory_t g_Ndef2Tag;

KnfcError_t 
KnfcVcardCreateNdef2Tag(uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcVcardCreateNdef2Tag", "FUNC STA - pBuffer: %X, nBufferLength: %d", pBuffer, nBufferLength);

	// TLV
	g_Ndef2Tag.aInternal[0] = 0x03;
	g_Ndef2Tag.aInternal[1] = 0x10;	// Data Length to be transmitted
	g_Ndef2Tag.aInternal[2] = 0xFE;	 
	
	// LOCK
	g_Ndef2Tag.aLock[0] = 0x00;
	g_Ndef2Tag.aLock[1] = 0x00;	
	
	// CC
	g_Ndef2Tag.aCC[0] = 0xE1;
	g_Ndef2Tag.aCC[1] = 0x10;
	g_Ndef2Tag.aCC[2] = 0x04;
	g_Ndef2Tag.aCC[3] = 0x00;

	// DATA Length
	nBufferLength = ((nBufferLength + NUM_NDEF2_SECTOR_BYTES - 1) / NUM_NDEF2_SECTOR_BYTES) * NUM_NDEF2_SECTOR_BYTES;
	if(nBufferLength < NUM_NDEF2_SDATA_BYTES) {
		nBufferLength = NUM_NDEF2_SDATA_BYTES;
	}

	// DATA
	g_Ndef2Tag.pBuffer = (uint8_t *)KnfcMemoryAlloc(nBufferLength);

	if(g_Ndef2Tag.pBuffer != NULL) {
		KnfcMemoryCopy(g_Ndef2Tag.pBuffer, pBuffer, nBufferLength);
	}
	
	KNFC_DEBUG("KnfcVcardCreateNdef2Tag", "FUNC END");		
	return KNFC_SUCCESS;
}

KnfcError_t 
KnfcVcardProcessNdef2Command(uint8_t *pBuffer, uint32_t nBufferLength) {

	KNFC_DEBUG("KnfcVcardParseNdef2Command", "FUNC STA - pBuffer: %X, nBufferLength: %d", pBuffer, nBufferLength);
	
	KNFC_DEBUG("KnfcVcardParseNdef2Command", "FUNC END");
	return KNFC_SUCCESS;
}
