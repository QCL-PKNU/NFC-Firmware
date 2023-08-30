/*
 * KnfcOsalDebug.c
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#include "KnfcOsalDebug.h"

//////////////////////////////////////////////////
// Debugging Function Definition
//////////////////////////////////////////////////

/**
 * This function will be used to print out the debugging message.
 */
 
void 
KnfcDebugPrintln(const char *sTag, const char *sFmt, ...) {

#ifdef LINUX_SUPPORT
	char buf[256];
		
	va_list args;
	va_start(args, sFmt);
	vsprintf(buf, sFmt, args);
	va_end(args);

	printf("%s]: %s\n", sTag, buf); 
#endif
}
