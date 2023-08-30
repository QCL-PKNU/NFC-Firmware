/*
 * KnfcOsalList.h
 *
 *  Created on: Jun 26, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_OSAL_LIST_H_
#define _KNFC_OSAL_LIST_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

//////////////////////////////////////////////////
// Data Structure Definition
//////////////////////////////////////////////////

typedef struct KnfcNode_s {

	struct KnfcNode_s *pPrev;
	struct KnfcNode_s *pNext;

	void *pItem;
	
} KnfcNode_t;

//////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

KnfcNode_t *KnfcInsertNode(KnfcNode_t *pHeadNode, void *pItem);

KnfcNode_t *KnfcInsertLastNode(KnfcNode_t *pHeadNode, void *pItem);

KnfcNode_t *KnfcDeleteNode(KnfcNode_t *pHeadNode, void *pItem);

KnfcNode_t *KnfcFindNode(KnfcNode_t *pHeadNode, void *pItem);

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

void KnfcDeleteList(KnfcNode_t *pHeadNode);

int32_t KnfcGetListSize(KnfcNode_t *pHeadNode);

KnfcNode_t *KnfcGetNextItem(KnfcNode_t *pNode);

void KnfcPrintAllItems(KnfcNode_t *pHeadNode);

#endif

#endif /* _KNFC_OSAL_LIST_H_ */
