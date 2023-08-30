/*
 * KnfcOsalList.c
 *
 *  Created on: Jun 26, 2013
 *      Author: youngsun
 */

#include "KnfcOsalList.h"

//////////////////////////////////////////////////
// Global Function Definitions
//////////////////////////////////////////////////

/**
 * This function inserts a new node with the given item into the head of the list.
 *
 * @param pHeadNode the head node of the list
 * @param pItem an item to insert
 * @return the new node
 */

KnfcNode_t *
KnfcInsertNode(KnfcNode_t *pHeadNode, void *pItem) {

	KnfcNode_t *newNode;

	if(pHeadNode == NULL) {
		return NULL;
	}

	newNode = (KnfcNode_t*)KnfcMemoryAlloc(sizeof(KnfcNode_t));

	if(newNode != NULL) {
		newNode->pNext = pHeadNode->pNext;
		newNode->pPrev = pHeadNode;
		newNode->pItem = pItem;

		if(newNode->pNext != NULL) {
			newNode->pNext->pPrev = newNode;
		}
	}

	pHeadNode->pNext = newNode;
	return newNode;
}

/**
 * This function will be used to insert a new node at the end of the list.
 *
 * @param pHeadNode the head node of the list
 * @param pItem an item to insert
 * @return the new node
 */

KnfcNode_t *
KnfcInsertLastNode(KnfcNode_t *pHeadNode, void *pItem) {
	KnfcNode_t *currNode;
	KnfcNode_t *prevNode;
	KnfcNode_t *nextNode;
	KnfcNode_t *newNode;

	if(pHeadNode == NULL) {
		return NULL;
	}

	newNode = (KnfcNode_t *)KnfcMemoryAlloc(sizeof(KnfcNode_t));

	if(newNode != NULL) {

		// YOUGNSUN - CHKME
		// The implementation could be modified to not use the loop to find the end of the list.
		for(currNode = pHeadNode; currNode->pNext != NULL; currNode = currNode->pNext);

		currNode->pNext = newNode;
		newNode->pPrev = currNode;
		newNode->pNext = NULL;
		newNode->pItem = pItem;
	}

	return newNode;
}

/**
 * This function will delete the node with the given item.
 *
 * @param pHeadNode the head node of the list
 * @param pItem an item to delete
 * @return the deleted node
 */

KnfcNode_t *
KnfcDeleteNode(KnfcNode_t *pHeadNode, void *pItem) {

	KnfcNode_t *currNode;
	KnfcNode_t *prevNode;
	KnfcNode_t *nextNode;

	if(pHeadNode == NULL || pItem == NULL) {
		return NULL;
	}

	currNode = pHeadNode->pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {

		if(currNode->pItem == pItem) {

			prevNode = currNode->pPrev;
			nextNode = currNode->pNext;

			prevNode->pNext = nextNode;

			if(nextNode != NULL) {
				nextNode->pPrev = prevNode;
			}

			return currNode;
		}
	}

	return NULL;
}

/**
 * This function will find and return the node with the given item.
 *
 * @param pHeadNode the head node of the list
 * @param pItem an item to find
 * @return the found node
 */
 
KnfcNode_t *
KnfcFindNode(KnfcNode_t *pHeadNode, void *pItem) {

	KnfcNode_t *currNode;

	if(pHeadNode == NULL || pItem == NULL) {
		return NULL;
	}

	currNode = pHeadNode->pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {
		if(currNode->pItem == pItem) {
			return currNode;
		}
	}

	return NULL;
}

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

/**
 * This function will delete all of the nodes in the list.
 *
 * @param pHeadNode the head node of the list
 */
 
void
KnfcDeleteList(KnfcNode_t *pHeadNode) {

	KnfcNode_t *currNode;
	KnfcNode_t *prevNode;

	if(pHeadNode == NULL) {
		return;
	}

	prevNode = pHeadNode;
	currNode = pHeadNode->pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {

		KnfcMemoryFree(prevNode->pItem);
		KnfcMemoryFree(prevNode);
		prevNode = currNode;
	}
}

/**
 * This function will return the size of the list.
 *
 * @param pHeadNode the head node of the list
 * @return the size
 */
 
int32_t
KnfcGetListSize(KnfcNode_t *pHeadNode) {

	int32_t listSize;
	KnfcNode_t *currNode;

	if(pHeadNode == NULL) {
		return -1;
	}

	listSize = 0;
	currNode = pHeadNode->pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {
		listSize++;
	}

	return listSize;
}

/**
 * This function will return the next node of the given node.
 *
 * @param pNode the current node
 * @return the next node
 */
 
KnfcNode_t *
KnfcGetNextItem(KnfcNode_t *pNode) {

	if(pNode == NULL) {
		return NULL;
	}

	if(pNode->pNext == NULL) {
		return NULL;
	}

	return pNode->pNext->pItem;
}

/**
 * This function will print out all the items of the list.
 *
 * @param pHeadNode the head node of the list
 */
 
void
KnfcPrintAllItems(KnfcNode_t *pHeadNode) {

	KnfcNode_t *currNode;

	if(pHeadNode == NULL) {
		return;
	}

	currNode = pHeadNode->pNext;

	for(; currNode != NULL; currNode = currNode->pNext) {

		KNFC_DEBUG("PrintAllItems", "Item: %X", currNode->pItem);
	}
}

#endif
