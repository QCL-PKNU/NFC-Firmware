/*
 * KnfcVirtualCard.h
 *
 *  Created on: Jun 27, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_VIRTUAL_CARD_H_
#define _KNFC_VIRTUAL_CARD_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

// The functions will be implemented in "KnfcCardEmul.c & .h" to remove the redundancy.

/*******************************************************************************
  NDEF Functions
*******************************************************************************/

#define W_NDEF_TNF_EMPTY			0x00
#define W_NDEF_TNF_WELL_KNOWN	0x01
#define W_NDEF_TNF_MEDIA			0x02
#define W_NDEF_TNF_ABSOLUTE_URI	0x03
#define W_NDEF_TNF_EXTERNAL		0x04
#define W_NDEF_TNF_UNKNOWN		0x05
#define W_NDEF_TNF_UNCHANGED		0x06
#define W_NDEF_TNF_ANY_TYPE		0x08

//////////////////////////////////////////////////
// Data Structure for NDEF2 Tag
//////////////////////////////////////////////////

#define NUM_NDEF2_TLV_BYTES		10
#define NUM_NDEF2_LOCK_BYTES		2
#define NUM_NDEF2_CC_BYTES		4	
#define NUM_NDEF2_SECTOR			16
#define NUM_NDEF2_SECTOR_BYTES	4
#define NUM_NDEF2_SDATA_SECTORS	12
#define NUM_NDEF2_SDATA_BYTES	(NUM_NDEF2_SDATA_SECTORS * NUM_NDEF2_SECTOR_BYTES)

typedef struct {
	uint8_t aInternal[NUM_NDEF2_TLV_BYTES];
	uint8_t aLock[NUM_NDEF2_LOCK_BYTES];
	uint8_t aCC[NUM_NDEF2_CC_BYTES];
	uint8_t *pBuffer;
	uint32_t nBufferLength;	
} Ndef2Memory_t;

#endif /* _KNFC_VIRTUAL_CARD_H_ */
