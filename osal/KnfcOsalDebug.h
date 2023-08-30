/*
 * KnfcOsalDebug.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_OSAL_DEBUG_H_
#define _KNFC_OSAL_DEBUG_H_

#include "KnfcOsalConfig.h"

//////////////////////////////////////////////////
// NFC Error Types
//////////////////////////////////////////////////

typedef enum {
	KNFC_SUCCESS = 0,

	// Errors for OS Abstract Layer
	KNFC_ERROR_TOO_SHORT_BUFFER,
	KNFC_ERROR_MEMORY_ALLOC,
	KNFC_ERROR_INVALID_PARAMETER,

	// Errors for Device Abstract Layer
	KNFC_ERROR_DAL_UNKNOWN_TYPE,
	KNFC_ERROR_DAL_NOT_CONFIGURED,
	KNFC_ERROR_DAL_OPEN,
	KNFC_ERROR_DAL_READ,
	KNFC_ERROR_DAL_WRITE,
	KNFC_ERROR_DAL_CLOSE,

	// Errors for Host Interface Layer
	KNFC_ERROR_HOST_UNKNOWN_TYPE,
	KNFC_ERROR_HOST_NOT_CONFIGURED,
	KNFC_ERROR_HOST_OPEN,
	KNFC_ERROR_HOST_READ,
	KNFC_ERROR_HOST_WRITE,
	KNFC_ERROR_HOST_CLOSE,
	KNFC_ERROR_HOST_UNKNOWN_COMMAND,
	KNFC_ERROR_HOST_INVALID_MESSAGE,
	KNFC_ERROR_HOST_INVALID_LAZY_MESSAGE,
	KNFC_ERROR_HOST_UNKNOWN_READER_ERROR,

	// Error for Reader
	KNFC_ERROR_READER_UNREGISTERED_PROTOCOL,
	KNFC_ERROR_READER_INVALID_CONNECTION,

	// Error for Card Emulation
	KNFC_ERROR_CARD_INVALID_CONNECTION_INFO,
	KNFC_ERROR_CARD_INVALID_CONNECTION,
	KNFC_ERROR_CARD_ACTIVATE,
	KNFC_ERROR_CARD_DEACTIVATE,
	KNFC_ERROR_CARD_TOO_SHORT_BUFFER,
	KNFC_ERROR_CARD_UNSUPPORT_TYPE,

	// Error for ISO 14443 Part 3 - Type A
	KNFC_ERROR_P14P3A_GET_ATQA,
	KNFC_ERROR_P14P3A_SET_ATQA,
	KNFC_ERROR_P14P3A_GET_SAK,
	KNFC_ERROR_P14P3A_SET_SAK,
	KNFC_ERROR_P14P3A_GET_UID,
	KNFC_ERROR_P14P3A_SET_UID,
	KNFC_ERROR_P14P3A_UID_SIZE,

	// Error for ISO 14443 Part 4 - Type A
	KNFC_ERROR_P14P4A_GET_ATQA,
	KNFC_ERROR_P14P4A_SET_ATQA,
	KNFC_ERROR_P14P4A_GET_SAK,
	KNFC_ERROR_P14P4A_SET_SAK,
	KNFC_ERROR_P14P4A_GET_ATS,
	KNFC_ERROR_P14P4A_SET_ATS,
	KNFC_ERROR_P14P4A_GET_UID,
	KNFC_ERROR_P14P4A_SET_UID,
	KNFC_ERROR_P14P4A_UID_SIZE,
	KNFC_ERROR_P14P4A_EXCHANGE_DATA,

	// Error for ISO 14443 Part 3 - Type B
	KNFC_ERROR_P14P3B_GET_ATQB,
	KNFC_ERROR_P14P3B_SET_ATQB,
	KNFC_ERROR_P14P3B_GET_MBLInCID,
	KNFC_ERROR_P14P3B_SET_MBLInCID,
	KNFC_ERROR_P14P3B_GET_HLRESP,	/* Higher Layer Response */
	KNFC_ERROR_P14P3B_SET_HLRESP,	/* Higher Layer Response */

	// Mode Swithcing
	KNFC_ERROR_MODESW_INVALID_PROTOCOL,
	
	KNFC_FAILURE,
} KnfcError_t;

//////////////////////////////////////////////////
// NFC Debug Function Declaration
//////////////////////////////////////////////////

void KnfcDebugPrintln(const char * sTag, const char * sFmt,...);

//////////////////////////////////////////////////
// Debug Interface Macros for NFC Debugging Functions
//////////////////////////////////////////////////

#ifndef ARM926EJS_SUPPORT
#define KNFC_ENABLE_DEBUG
#define KNFC_ENABLE_ERROR
#ifdef LINUX_SUPPORT
#define KNFC_ENABLE_ASSERT
#endif
#define KNFC_ENABLE_CHECK
#endif

#ifdef KNFC_ENABLE_DEBUG
#define KNFC_DEBUG(TAG, FMT, ...)		KnfcDebugPrintln(TAG, FMT, ##__VA_ARGS__)
#else
#define KNFC_DEBUG(TAG, FMT, ...)		//
#endif

#ifdef KNFC_ENABLE_DEBUG
#define KNFC_ERROR(FMT, ...)			KnfcDebugPrintln("ERROR", FMT, ##__VA_ARGS__)
#else
#define KNFC_ERROR(FMT, ...)			//
#endif

#ifdef KNFC_ENABLE_CHECK
#define KNFC_CHECK(EXPR, ERROR)		\
do{									\
	if(!(EXPR)) {					\
		return ERROR;				\
	}								\
} while(0)
#else
#define KNFC_CHECK(EXPR, ERROR)		//
#endif

#ifdef KNFC_ENABLE_ASSERT
#define KNFC_ASSERT(EXPR, MSG)		\
do{									\
	if(!(EXPR)) {					\
		KNFC_ERROR("%s", MSG);		\
		assert(EXPR);				\
	}								\
} while(0)
#else
#define KNFC_ASSERT(EXPR, MSG)		//
#endif

#endif /* _KNFC_OSAL_DEBUG_H_ */
