/*
 * KnfcDevMgmt.h
 *
 *  Created on: Jun 24, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_DEV_MGMT_H_
#define _KNFC_DEV_MGMT_H_

#include "KnfcOsalDebug.h"
#include "KnfcOsalConfig.h"
#include "KnfcOsalLibrary.h"

//////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////

// The version number of the OpenNFC 
#define OPEN_NFC_PRODUCT_VERSION_BUILD_S  { 'v', '4', '.', '5', '.', '2', ' ', '(', 'B', 'u', 'i', 'l', 'd', ' ', '1', '6', '2', '3', '4', ')',  0 }

//////////////////////////////////////////////////
// Data Structure
//////////////////////////////////////////////////

/* The maximum number of standalone slots */
#define P_SE_HAL_MAXIMUM_NUMBER_STANDALONE_SE	4

/* The maximum number of SWP slots */
#define P_SE_HAL_MAXIMUM_NUMBER_SWP_SE			4

/* The maximum number of proprietary slots */
#define P_SE_HAL_MAXIMUM_NUMBER_PROPRIETARY_SE	4

/* The maximum number of Secure Elements */
#define P_SE_HAL_MAXIMUM_SE_NUMBER (P_SE_HAL_MAXIMUM_NUMBER_PROPRIETARY_SE + P_SE_HAL_MAXIMUM_NUMBER_SWP_SE + P_SE_HAL_MAXIMUM_NUMBER_STANDALONE_SE)

#define NAL_SE_DESCRIPTION_STRING_SIZE				0x20
#define NAL_HARDWARE_TYPE_STRING_SIZE              	0x20
#define NAL_HARDWARE_SERIAL_NUMBER_STRING_SIZE		0x20
#define NAL_LOADER_DESCRIPTION_STRING_SIZE			0x20
#define NAL_FIRMWARE_DESCRIPTION_STRING_SIZE		0x20

typedef struct __tNFCControllerSeInfo
{
	uint8_t aDescription[NAL_SE_DESCRIPTION_STRING_SIZE];
	uint32_t nCapabilities;
	uint32_t nSwpTimeout;
	uint32_t nProtocols;
	uint32_t nHalSlotIdentifier;
} tNFCControllerSeInfo;

typedef struct __tNFCControllerInfo
{
	uint32_t nProtocolCapabilities;
	uint8_t aHardwareVersion[NAL_HARDWARE_TYPE_STRING_SIZE];
	uint8_t aHardwareSerialNumber[NAL_HARDWARE_SERIAL_NUMBER_STRING_SIZE];
	uint8_t aLoaderVersion[NAL_LOADER_DESCRIPTION_STRING_SIZE];
	uint8_t aFirmwareVersion[NAL_FIRMWARE_DESCRIPTION_STRING_SIZE];
	uint8_t nNALVersion;

	/* The total number of SE */
	uint32_t nSeNumber;
	tNFCControllerSeInfo aSEInfoArray[P_SE_HAL_MAXIMUM_SE_NUMBER];

	uint32_t nFirmwareCapabilities;

	uint32_t nReaderISO14443_A_MaxRate;
	uint32_t nReaderISO14443_A_InputSize;
	uint32_t nReaderISO14443_B_MaxRate;
	uint32_t nReaderISO14443_B_InputSize;
	uint32_t nCardISO14443_A_MaxRate;
	uint32_t nCardISO14443_B_MaxRate;
	uint32_t nAutoStandbyTimeout;

} tNFCControllerInfo;

/* NFC Properties */
#define W_PROP_ANY					0x00
#define W_PROP_ISO_14443_3_A		0x01
#define W_PROP_ISO_14443_4_A		0x02
#define W_PROP_ISO_14443_3_B		0x03
#define W_PROP_ISO_14443_4_B		0x04
#define W_PROP_ISO_15693_3			0x05
#define W_PROP_ISO_15693_2			0x06
#define W_PROP_ISO_7816_4			0x07
#define W_PROP_BPRIME				0x08

#define W_PROP_NFC_TAG_TYPE_1		0x10
#define W_PROP_NFC_TAG_TYPE_2		0x11
#define W_PROP_NFC_TAG_TYPE_3		0x12
#define W_PROP_NFC_TAG_TYPE_4_A		0x13
#define W_PROP_NFC_TAG_TYPE_4_B		0x14
#define W_PROP_NFC_TAG_TYPE_5		0x15
#define W_PROP_NFC_TAG_TYPE_6		0x16
#define W_PROP_NFC_TAG_TYPE_7		0x17
#define W_PROP_NFC_TAGS				0x1F

/* NFC Controller modes */
#define W_NFCC_MODE_BOOT_PENDING				0x00
#define W_NFCC_MODE_MAINTENANCE					0x01
#define W_NFCC_MODE_NO_FIRMWARE					0x02
#define W_NFCC_MODE_FIRMWARE_NOT_SUPPORTED		0x03
#define W_NFCC_MODE_NOT_RESPONDING				0x04
#define W_NFCC_MODE_LOADER_NOT_SUPPORTED		0x05
#define W_NFCC_MODE_ACTIVE						0x06
#define W_NFCC_MODE_SWITCH_TO_STANDBY			0x07
#define W_NFCC_MODE_STANDBY						0x08
#define W_NFCC_MODE_SWITCH_TO_ACTIVE			0x09

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

KnfcError_t KnfcGetDriverVersion(uint8_t **ppBuffer, uint32_t *pnLength);

KnfcError_t KnfcGetControllerInfo(uint8_t **ppBuffer, uint32_t *pnLength);

uint32_t KnfcGetControllerMode(void);

KnfcError_t KnfcUpdateFirmware(uint8_t *pBuffer, uint32_t nLength);

KnfcError_t KnfcResetController(void);

KnfcError_t KnfcConfigureBypass(void);

#endif /* _KNFC_DEV_MGMT_H_ */
