#ifndef __COMMON_H__
#define __COMMON_H__

//#define USE_RAMDISK

/* You will define this macro in PA4 */
#define HAS_DEVICE

/* Want to use a simulated cache? Uncomment below. */
#define USE_CACHE
#define USE_TLB

#define DEBUG
#define LOG_FILE

//#define OPTIMIZE_PAL

#include "debug.h"
#include "macro.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;
typedef uint32_t hwaddr_t;
typedef uint32_t lnaddr_t;
typedef uint32_t swaddr_t;

typedef uint16_t ioaddr_t;

#pragma pack (1)
typedef union {
	uint32_t _4;
	uint32_t _3	: 24;
	uint16_t _2;
	uint8_t _1;
} unalign;
#pragma pack ()

#define false 0
#define true 1

#endif
