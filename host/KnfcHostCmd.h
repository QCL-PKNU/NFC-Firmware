/*
 * KnfcHostCmd.h
 *
 *  Created on: Jun 20, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_HOST_CMD_H_
#define _KNFC_HOST_CMD_H_

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalLibrary.h"

#include "KnfcHostIntf.h"
#include "KnfcDevMgmt.h"

//////////////////////////////////////////////////
// Macro Definition for OPENNFC
//////////////////////////////////////////////////

#ifndef W_ERROR
typedef uint32_t W_ERROR;
#endif

#ifndef W_HANDLE
typedef uint32_t W_HANDLE;
#endif

//////////////////////////////////////////////////
// Data Structure for Common Inferface with OPENNFC
//////////////////////////////////////////////////

/* ------------------------------------------------
      PBasicDriverGetVersion()
------------------------------------------------ */

#define P_Idenfier_PBasicDriverGetVersion			9

typedef struct __tMessage_in_PBasicDriverGetVersion
{
	void * pBuffer;
	uint32_t nBufferSize;
} tMessage_in_PBasicDriverGetVersion;

/* ------------------------------------------------
	PNFCControllerDriverReadInfo()
------------------------------------------------ */

#define P_Idenfier_PNFCControllerDriverReadInfo	52

typedef struct __tMessage_in_PNFCControllerDriverReadInfo {
	void * pBuffer;
	uint32_t nBufferSize;
} tMessage_in_PNFCControllerDriverReadInfo;

/* ------------------------------------------------
      PNFCControllerGetMode()
------------------------------------------------- */

#define P_Idenfier_PNFCControllerGetMode			55

typedef union __tParams_PNFCControllerGetMode {
	uint32_t out;
} tParams_PNFCControllerGetMode;

/* ------------------------------------------------
	PReaderErrorEventRegister()
------------------------------------------------- */

#define P_Idenfier_PReaderErrorEventRegister		97

#define W_READER_ERROR_UNKNOWN				0x00
#define W_READER_ERROR_COLLISION				0x01
#define W_READER_MULTIPLE_DETECTION			0x02

typedef struct __tMessage_in_PReaderErrorEventRegister {
	void *pHandler;
	void *pHandlerParameter;
	uint8_t nEventType;
	bool_t bCardDetectionRequested;
	W_HANDLE *phRegistryHandle;
} tMessage_in_PReaderErrorEventRegister;

/* --------------------------------------------------
      PReaderDriverRegister()
--------------------------------------------------- */

#define P_Idenfier_PReaderDriverRegister			94

typedef struct __tMessage_in_PReaderDriverRegister
{
   void *pCallback;
   void *pCallbackParameter;
   uint8_t nPriority;
   uint32_t nRequestedProtocolsBF;
   uint32_t nDetectionConfigurationLength;
   uint8_t * pBuffer;
   uint32_t nBufferMaxLength;
   W_HANDLE * phListenerHandle;
} tMessage_in_PReaderDriverRegister;

/* ---------------------------------------------------
      PReaderDriverSetWorkPerformedAndClose()
-----------------------------------------------------*/

#define P_Idenfier_PReaderDriverSetWorkPerformedAndClose 95

typedef struct __tMessage_in_PReaderDriverSetWorkPerformedAndClose
{
   W_HANDLE hDriverListener;
} tMessage_in_PReaderDriverSetWorkPerformedAndClose;

/* ---------------------------------------------------
      P14P4DriverExchangeData()
---------------------------------------------------- */

#define P_Idenfier_P14P4DriverExchangeData 		4

typedef struct __tMessage_in_P14P4DriverExchangeData
{
   W_HANDLE hDriverConnection;
   void *pCallback;
   void *pCallbackParameter;
   const uint8_t *pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t *pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
   bool_t bSendNAD;
   uint8_t nNAD;
   bool_t bCreateOperation;
} tMessage_in_P14P4DriverExchangeData;

/* -------------------------------------------------
      P14P3DriverSetTimeout()
-------------------------------------------------- */

#define P_Idenfier_P14P3DriverSetTimeout 		3

typedef struct __tMessage_in_P14P3DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P14P3DriverSetTimeout;

/* -------------------------------------------------
      P14P4DriverSetTimeout()
-------------------------------------------------- */

#define P_Idenfier_P14P4DriverSetTimeout 		5

typedef struct __tMessage_in_P14P4DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P14P4DriverSetTimeout;

/* ---------------------------------------------------
      PCacheConnectionDriverRead()
----------------------------------------------------*/

#define P_Idenfier_PCacheConnectionDriverRead 	12

/* ---------------------------------------------------
      PCacheConnectionDriverWrite()
-----------------------------------------------------*/

#define P_Idenfier_PCacheConnectionDriverWrite	13

/* ---------------------------------------------------
      PContextDriverResetMemoryStatistics()
----------------------------------------------------*/

#define P_Idenfier_PContextDriverResetMemoryStatistics 16

/* ---------------------------------------------------
      PDFCDriverInterruptEventLoop()
----------------------------------------------------*/

#define P_Idenfier_PDFCDriverInterruptEventLoop	17

/* ---------------------------------------------------
      PDFCDriverPumpEvent()
----------------------------------------------------*/

#define P_Identifier_PDFCDriverPumpEvent		125

/* ---------------------------------------------------
      PEmulOpenConnectionDriver1Ex()
----------------------------------------------------*/

#define P_Idenfier_PEmulOpenConnectionDriver1Ex 23

/* Predefined here because used in reader structures */
#define W_EMUL_APPLICATION_DATA_MAX_LENGTH			252
#define W_EMUL_HIGHER_LAYER_DATA_MAX_LENGTH		245
#define W_EMUL_HIGHER_LAYER_RESPONSE_MAX_LENGTH	253

typedef struct
{
   uint8_t nSAK;
   uint16_t nATQA;
   uint8_t nFWI_SFGI;
   uint8_t nDataRateMax;
   bool_t bSetCIDSupport;
   uint8_t nCID;
   uint8_t nNAD;
   uint8_t nUIDLength;
   uint8_t UID[10];
   uint32_t nApplicationDataLength;
   uint8_t aApplicationData[W_EMUL_APPLICATION_DATA_MAX_LENGTH];
} tWEmul14AConnectionInfo;

typedef struct
{
   uint8_t nAFI;
   uint32_t nATQB;
   bool_t bSetCIDSupport;
   uint8_t nCID;
   uint8_t nNAD;
   uint8_t nPUPILength;
   uint8_t PUPI[4];
   uint32_t nHigherLayerResponseLength;
   uint8_t aHigherLayerResponse[W_EMUL_HIGHER_LAYER_RESPONSE_MAX_LENGTH];
} tWEmul14BConnectionInfo;

typedef struct
{
   uint8_t nCardType;

   union
   {
      tWEmul14AConnectionInfo s14A;
      tWEmul14BConnectionInfo s14B;
   } sCardInfo;
} tWEmulConnectionInfo;

typedef struct __tMessage_in_PEmulOpenConnectionDriver1Ex
{
   void * pOpenCallback;
   void * pOpenCallbackParameter;
   const tWEmulConnectionInfo * pEmulConnectionInfo;
   uint32_t nSize;
   W_HANDLE * phHandle;
} tMessage_in_PEmulOpenConnectionDriver1Ex;

/* ---------------------------------------------------
      PEmulOpenConnectionDriver2Ex()
----------------------------------------------------*/

#define W_EMUL_EVENT_SELECTION				2
#define W_EMUL_EVENT_DEACTIVATE				3

#define P_Idenfier_PEmulOpenConnectionDriver2Ex 25

typedef struct __tMessage_in_PEmulOpenConnectionDriver2Ex
{
   W_HANDLE hHandle;
   void * pEventCallback;
   void * pEventCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver2Ex;

/* ---------------------------------------------------
      PEmulOpenConnectionDriver3Ex()
----------------------------------------------------*/

#define P_Idenfier_PEmulOpenConnectionDriver3Ex 27

typedef struct __tMessage_in_PEmulOpenConnectionDriver3Ex
{
   W_HANDLE hHandle;
   void * pCommandCallback;
   void * pCommandCallbackParameter;
} tMessage_in_PEmulOpenConnectionDriver3Ex;

/* ----------------------------------------------------
      PEmulCloseDriver()
------------------------------------------------------*/

#define P_Idenfier_PEmulCloseDriver				20

typedef struct __tMessage_in_PEmulCloseDriver
{
   W_HANDLE hHandle;
   void * pCallback;
   void * pCallbackParameter;
} tMessage_in_PEmulCloseDriver;

/* ---------------------------------------------------
      PEmulGetMessageData()
-----------------------------------------------------*/

#define P_Idenfier_PEmulGetMessageData 			21

typedef struct __tMessage_in_PEmulGetMessageData
{
   W_HANDLE hHandle;
   uint8_t * pDataBuffer;
   uint32_t nDataLength;
   uint32_t * pnActualDataLength;
} tMessage_in_PEmulGetMessageData;

/* ----------------------------------------------------
      PEmulSendAnswer()
------------------------------------------------------*/

#define P_Idenfier_PEmulSendAnswer 				28

typedef struct __tMessage_in_PEmulSendAnswer
{
   W_HANDLE hDriverConnection;
   const uint8_t * pDataBuffer;
   uint32_t nDataLength;
} tMessage_in_PEmulSendAnswer;

/* ------------------------------------------------------
      P14P3DriverExchangeData()
--------------------------------------------------------*/

#define P_Idenfier_P14P3DriverExchangeData 0

typedef struct __tMessage_in_P14P3DriverExchangeData
{
   W_HANDLE hDriverConnection;
   void * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
   bool_t bCheckResponseCRC;
   bool_t bCheckAckOrNack;
} tMessage_in_P14P3DriverExchangeData;

/* -------------------------------------------------------
      P15P3DriverExchangeData()
---------------------------------------------------------*/

#define P_Idenfier_P15P3DriverExchangeData 6

typedef struct __tMessage_in_P15P3DriverExchangeData
{
   W_HANDLE hDriverConnection;
   void * pCallback;
   void * pCallbackParameter;
   const uint8_t * pReaderToCardBuffer;
   uint32_t nReaderToCardBufferLength;
   uint8_t * pCardToReaderBuffer;
   uint32_t nCardToReaderBufferMaxLength;
} tMessage_in_P15P3DriverExchangeData;

/* -------------------------------------------------------
      P15P3DriverSetTimeout()
---------------------------------------------------------*/

#define P_Idenfier_P15P3DriverSetTimeout 7

typedef struct __tMessage_in_P15P3DriverSetTimeout
{
   W_HANDLE hConnection;
   uint32_t nTimeout;
} tMessage_in_P15P3DriverSetTimeout;

/* ------------------------------------------------------
      PHandleCloseDriver()
--------------------------------------------------------*/

#define P_Idenfier_PHandleCloseDriver		32

typedef struct __tMessage_in_PHandleCloseDriver
{
   W_HANDLE hObject;
} tMessage_in_PHandleCloseDriver;

/* --------------------------------------------------
      For all of the lazy events
--------------------------------------------------- */

#define P_Idenfier_KhalLazyEvent					0xFA
#define P_Idenfier_KhalNormalEvent				0xFB
#define P_Idenfier_KhalParameter0				0xFC
#define P_Idenfier_KhalParameter1				0xFD

//////////////////////////////////////////////////
// KNFC Command Datat Structure
//////////////////////////////////////////////////

typedef struct {
	// input parameter size 
	uint32_t nSizeIn;

	// output parameter size 
	uint32_t nSizeOut;

	// function identifier 
	uint8_t nCode;

} KhstCmdHeader_t;

typedef struct {
	
	// message header
	KhstCmdHeader_t header;

	// message body
	uint8_t *pPayload;
	
} KhstCmdMessage_t;

//////////////////////////////////////////////////
// KNFC Response Data Structure
//////////////////////////////////////////////////

typedef struct {
	// Send the same code number with the given command 
	// in order to check whether the response is valid
	uint8_t nCode;
	
	// the number of bytes of the response
	uint32_t nLength;
	
} KhstRespHeader_t;

typedef struct {

	// Response header
	KhstRespHeader_t header;

	// Response message body
	uint8_t *pPayload;
	
} KhstRespMessage_t;

///////////////////////////////////////////////////
// Global Function Declaration
//////////////////////////////////////////////////

KnfcError_t KnfcHostProcessMessage(void);

#ifdef INCLUDE_DEPRECATED_FUNCTIONS

KnfcError_t KnfcHostSendMessageData(uint8_t nCode, uint8_t *pBuffer, uint32_t nBufferLength);

bool_t KnfcHostReadyToProcessMessage(void);

#endif

KnfcError_t KnfcHostEnqueueLazyResponse(KhstRespMessage_t *pRespMessage);

KnfcError_t KnfcHostSendAllLazyResponses(void);

#endif
