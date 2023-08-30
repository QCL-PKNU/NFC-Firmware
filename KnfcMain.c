/*
 * KnfcMain.c
 *
 *  Created on: Jun 19, 2013
 *      Author: youngsun
 */

#include "KnfcOsalConfig.h"
#include "KnfcOsalDebug.h"
#include "KnfcOsalList.h"

#include "KnfcHostCmd.h"
#include "KnfcHostIntf.h"
#include "KnfcModeSwitch.h"

extern KnfcMutex_t g_hHostCommMutex;

#ifdef ARM926EJS_SUPPORT
#include "KnfcDalUart.h"
#include "KnfcDalTimer.h"
#include "KnfcDalConfig.h"
#include "KnfcIrqHandler.h"

int arm926ejs_main(void) {

	KnfcError_t error = KNFC_SUCCESS;

	error = KnfcHostConfigureAndOpen(KNFC_HOST_UART, (void *)KNFC_UART0);

#ifdef NFC_ERROR_CHECK_UART
	KnfcUartOpen((void *)KNFC_UART1);
#endif

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostConfigureAndOpen: %d", error);
		return 0;
	}

	KnfcMutexInit(&g_hHostCommMutex);
	KnfcMemoryInit();
	KnfcResetModeSwitch();
	KnfcIrqInit();
	KnfcStartPollingLoop();	

	while(1) {
		// Process the command from the host
		if(KnfcHostDataReady()) {

			error = KnfcHostProcessMessage();

			if(error != KNFC_SUCCESS) {
				KNFC_ERROR("KnfcHostProcessMessage: %d", error);
				break;
			}
		}

		// Send all the suspended lazy responses by the interrupt handlers. 
		KnfcHostSendAllLazyResponses();
	}

	KnfcStopPollingLoop();
	KnfcIrqDeinit();
	KnfcHostClose();
	return 0;
}

#else

int main(int argc , char *argv[])
{
	KnfcError_t error = KNFC_SUCCESS;

	error = KnfcHostConfigureAndOpen(KNFC_HOST_SOCKET, NULL);

	if(error != KNFC_SUCCESS) {
		KNFC_ERROR("KnfcHostConfigureAndOpen: %d", error);
		return 0;
	}

	KnfcMutexInit(&g_hHostCommMutex);
	KnfcMemoryInit();
	KnfcResetModeSwitch();

	while(1) {
		// Process the command from the host
		if(KnfcHostDataReady()) {
			error = KnfcHostProcessMessage();

			if(error != KNFC_SUCCESS) {
				KNFC_ERROR("KnfcHostProcessMessage: %d", error);
				break;
			}
		}

		// YOUNGSUN - CHKME
		// We have to append the routine for handling the mode switch thread.
	}

	KnfcHostClose();
	return 0;
}

#endif
