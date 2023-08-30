/*
 * KnfcOsalConfig.h
 *
 *  Created on: Jun 17, 2013
 *      Author: youngsun
 */

#ifndef _KNFC_OSAL_CONFIG_H_
#define _KNFC_OSAL_CONFIG_H_

//#define ARM926EJS_SUPPORT

#ifndef ARM926EJS_SUPPORT

// Define if the firmware will operate on a Linux system.
#define LINUX_SUPPORT

// Define if the dummy modem is employed for the test.
#define DUMMY_MODEM_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#endif

// Maximum buffer length
#define MAX_BUF_LENGTH		256

// User-defined data types
#ifndef _bool_t_defined_
#define _bool_t_defined_
typedef unsigned char bool_t;
#endif

#ifndef _uint8_t_defined_
#define _uint8_t_defined_
typedef unsigned char uint8_t;
#endif

#ifndef _int8_t_defined_
#define _int8_t_defined_
typedef signed char int8_t;
#endif

#ifndef _uint16_t_defined_
#define _uint16_t_defined_
typedef unsigned short uint16_t;
#endif

#ifndef _int16_t_defined_
#define _int16_t_defined_
typedef signed short int16_t;
#endif

#ifndef _uint32_t_defined_
#define _uint32_t_defined_
typedef unsigned int uint32_t;
#endif

#ifndef _int32_t_defined_
#define _int32_t_defined_
typedef signed int int32_t;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef _knfc_handle_t_defined_
#define _knfc_handle_t_defined_
typedef uint32_t KnfcHandle_t;
#endif

#ifndef LINUX_SUPPORT
#ifndef _knfc_size_t_defined_
#define _knfc_size_t_defined_
typedef uint32_t size_t;
#endif
#endif

#endif /* _KNFC_OSAL_CONFIG_H_ */
