/*
 * KnfcDevMgmt.c
 *
 *  Created on: Jun 24, 2013
 *      Author: youngsun
 */

#include "KnfcDevMgmt.h"
#include "KnfcModeSwitch.h"
#ifdef ARM926EJS_SUPPORT
#include "KnfcDalConfig.h"
#endif

#ifndef KNFC_DRIVER_VERSION
#define KNFC_DRIVER_VERSION	OPEN_NFC_PRODUCT_VERSION_BUILD_S
#endif

/**
 * This function is designed to return the NFC driver version.
 *
 * @param ppBuffer a buffer for the driver version information
 * @param pnLength the buffer length
 * @return error information
 */
 
KnfcError_t
KnfcGetDriverVersion(uint8_t **ppBuffer, uint32_t *pnLength) {

	uint16_t aVersion[] = KNFC_DRIVER_VERSION;
	uint32_t nVersionSize = sizeof(aVersion);
	uint8_t *pBuffer = (uint8_t*)KnfcMemoryAlloc(nVersionSize);

	if(pBuffer == NULL) {
		return KNFC_ERROR_MEMORY_ALLOC;
	}

	*ppBuffer = pBuffer;
	*pnLength = nVersionSize;

	KnfcMemoryCopy(pBuffer, aVersion, nVersionSize);
	return KNFC_SUCCESS;
}

/**
 * This function will return the information of the NFC controller including max datarate.
 *
 * @param ppBuffer a buffer for the controller information
 * @param pnLength the buffer length
 * @return error information
 */
 
KnfcError_t
KnfcGetControllerInfo(uint8_t **ppBuffer, uint32_t *pnLength) {

	uint32_t nfccInfoSize = sizeof(tNFCControllerInfo);
	tNFCControllerInfo *nfccInfo = (tNFCControllerInfo *)KnfcMemoryAlloc(nfccInfoSize);

	if(nfccInfo == NULL) {
		return KNFC_ERROR_MEMORY_ALLOC;
	}

	*ppBuffer = (uint8_t*)nfccInfo;
	*pnLength = nfccInfoSize;

	// Configure our controller information
	nfccInfo->nProtocolCapabilities = 
		/* Supported P2P Protocols - Not supported yet */
		/* Supported Card Emulation Protocols */
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_A |
		W_NFCC_PROTOCOL_CARD_ISO_14443_4_B |
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_A |
		W_NFCC_PROTOCOL_CARD_ISO_14443_3_B |
		W_NFCC_PROTOCOL_CARD_TYPE_1_CHIP |
		/* Supported Reader Protocols */
		W_NFCC_PROTOCOL_READER_ISO_14443_4_A |
		W_NFCC_PROTOCOL_READER_ISO_14443_4_B |
		W_NFCC_PROTOCOL_READER_ISO_14443_3_A |
		W_NFCC_PROTOCOL_READER_ISO_14443_3_B |
		W_NFCC_PROTOCOL_READER_ISO_15693_2 |
		W_NFCC_PROTOCOL_READER_ISO_15693_3 |
		W_NFCC_PROTOCOL_READER_TYPE_1_CHIP;

	return KNFC_SUCCESS;
}

/**
 * This function will return the current operating mode of the NFC controller.
 *
 * @return the current controller mode
 */
 
uint32_t
KnfcGetControllerMode() {
	KNFC_DEBUG("KDM - CHKME", "Return the current operating mode of the NFC controller...");

	// YOUNGSUN - CHKME
	// The functionality of this function has not implemented yet.
	
	return W_NFCC_MODE_ACTIVE;
}

/**
 * This function will be used to update the firmware with the data given from the protocol stack
 *
 * @param pBuffer a buffer for the new firmware bytes
 * @param nBufferLength the buffer length
 * @return error information
 */
 
KnfcError_t 
KnfcUpdateFirmware(uint8_t *pBuffer, uint32_t nBufferLength) {
	KNFC_DEBUG("KDM - CHKME", "Update the NFC controller firmware...");

	// YOUNGSUN - CHKME
	// This function has not implemented yet.
	
	return KNFC_SUCCESS;
}

/**
 * This function is designed to electrically reset the NFC controller.
 *
 * @return error information
 */

KnfcError_t
KnfcResetController() {
	KNFC_DEBUG("KDM - CHKME", "Reset the NFC controller...");

	// YOUNGSUN - CHKME
	// This function has not implemented yet.
	
	return KNFC_SUCCESS;
}

/**
 * This function configure the NFCC to bypass the data transmitted from PCD to PICC, and vice versa.
 *
 * @return error information
 */

KnfcError_t
KnfcConfigureBypass(void) {
	KNFC_DEBUG("KnfcConfigureBypass", "Configure Bypass...");
#ifdef ARM926EJS_SUPPORT
	*(volatile unsigned short int *)NFC_CTRL_BASE_ADDR = 0x0A;
#endif
	return KNFC_SUCCESS;
}
