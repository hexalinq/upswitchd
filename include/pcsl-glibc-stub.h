#pragma once
// === pcsl/stdinc.h ===
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>
#include <net/ethernet.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <byteswap.h>
#include <ctype.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1

#define IN
#define OUT
#define INOUT
#define IN_OPT
#define OUT_OPT
#define INOUT_OPT

#define __STR_HELPER(x) #x
#define STR(x) __STR_HELPER(x)

#define FOR_IF(entry, loopcond, next, condition) for((entry); (loopcond); (next)) if((condition))
#define FOR_RANGE(variable, limit) for(size_t variable = 0; variable < limit; ++variable)
#define FOR_SRANGE(variable, start, limit) for(size_t variable = (start); variable < limit; ++variable)
#define FOR_CRANGE(variable, limit) for(size_t variable = 0; variable <= limit; ++variable)
#define FOR_SCRANGE(variable, start, limit) for(size_t variable = (start); variable <= limit; ++variable)
#define FOR_RANGE_IF(variable, limit, condition) FOR_RANGE(variable, limit) if((condition))

#define FORCE_INLINE inline __attribute__((always_inline))
#define INLINE FORCE_INLINE
#define EXPORT __attribute__((visibility("default")))
#define NOEXPORT __attribute__((visibility("hidden")))
#define UNUSED_DEF __attribute__((unused))
#define UNUSED(x) (void)(x)
#define NULL_SENTINEL __attribute__((sentinel))

#if !defined(PCSL_PLATFORM_WINDOWS_USER) && !defined(PCSL_DISABLE_MALLOC_ATTR_EXT)
	#define MALLOC_LIKE(freefn, freearg) __attribute__((malloc, malloc(freefn, freearg)))
#else
	#define MALLOC_LIKE(freefn, freearg) __attribute__((malloc))
#endif

#ifndef MIN
	#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
	#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define ABS(a) ((a) < 0 ? -(a) : (a))
#define MIN2(key1, key2, val1, val2) ((key1) < (key2) ? (val1) : (val2))
#define MAX2(key1, key2, val1, val2) ((key1) > (key2) ? (val1) : (val2))

#ifdef PCSL_NO_STRUCT_ATTRIBUTES
	#define TRANSPARENT_UNION union
#else
	#define TRANSPARENT_UNION union __attribute__((transparent_union))
#endif

// === pcsl/util.h ===
#define dbgprintf(format, ...) printf("%s:%u:%s | %s (%s) | " format "\n", __FILE__, __LINE__, __FUNCTION__, strerrorname_np(errno), strerrordesc_np(errno), ##__VA_ARGS__)
#define crash(format, ...) do { \
	dbgprintf(format, ##__VA_ARGS__); \
	fflush(stdout); \
	fflush(stderr); \
	abort(); \
} while(0)

// === pcsl/main.h ===
typedef int32_t pcsl_error_t;

INLINE int8_t prRaiseOOM(void* pContext) { crash("Out of memory"); }
INLINE int8_t prRaise(pcsl_error_t iError, void* pContext) { crash(); }

#define PCSL_SUCCESS 0

#define PCSL_ERROR_MALLOC -0x30000000 // Out of memory
#define PCSL_ERROR_OOM PCSL_ERROR_MALLOC
#define PCSL_ERROR_ARGS -0x30000001 // Invalid function arguments
#define PCSL_ERROR_LENGTH -0x30000002 // Invalid length
#define PCSL_ERROR_TYPE -0x30000003 // Invalid type
#define PCSL_ERROR_VALUE -0x30000004 // Invalid value
#define PCSL_ERROR_SYS -0x30000005 // System error
#define PCSL_ERROR_ISTCORRUPT -0x30000006 // Internal structure corruption detected
#define PCSL_ERROR_UNKNOWN -0x30000007 // Unknown error
#define PCSL_ERROR_NOINIT -0x30000008 // Data structure not initialized
#define PCSL_ERROR_UNSUPPORTED -0x30000009 // Unsupported operation
#define PCSL_ERROR_NOTFOUND -0x3000000A // Not found
#define PCSL_ERROR_EXISTS -0x3000000B // Already exists

//#define PCSL_IS_ERROR(x) (prStatusType((x)) == PCSL_STYPE_ERROR)

#define PCSL_EHMASK_OOM (1 << 0)
#define PCSL_EHMASK_PERMISSION (1 << 1)
#define PCSL_EHMASK_ASSERT (1 << 2)
#define PCSL_EHMASK_SYS (1 << 3)

#define PCSL_EHMASK_ALL (PCSL_EHMASK_OOM | PCSL_EHMASK_PERMISSION | PCSL_EHMASK_ASSERT | PCSL_EHMASK_SYS)
#define PCSL_EHMASK_ANY PCSL_EHMASK_ALL

#define PCSL_EH_ABORT ((void*)-1)

// === pcsl/memory/main.h ===
#define mmAllocateZeroTS(type, var) type* var = mmAllocateZero(sizeof(type))

#define mmZero(ptr, count) memset((ptr), 0, (count))
#define mmZeroT(var) mmZero(&(var), sizeof((var)))
#define mmZeroTA(var) mmZero((var), sizeof((var)))
#define mmZeroTP(dst, count) mmZero((dst), sizeof(*(dst)) * count)

#define mmFree free
#define mmMove memmove

#define mmCopy memcpy
#define mmCopyT(pDest, pSrc) do { _Static_assert(sizeof(pDest) == sizeof(pSrc)); mmCopy(&(pDest), &(pSrc), sizeof(pDest)); } while(0)
#define mmCopyDT(pDest, pSrc) mmCopy(&(pDest), (pSrc), sizeof(pDest))
#define mmCopyST(pDest, pSrc) mmCopy((pDest), &(pSrc), sizeof(pSrc))
#define mmCopyDA(pDest, pSrc) mmCopy((pDest), (pSrc), sizeof(pDest))
#define mmCopySA(pDest, pSrc) mmCopy((pDest), (pSrc), sizeof(pSrc))
#define mmCopySTP(dst, src, count) mmCopy((dst), (src), (count) * sizeof(*(src)))
#define mmCopyDTP(dst, src, count) mmCopy((dst), (src), (count) * sizeof(*(dst)))

INLINE void* mmAllocate(size_t iSize) {
	void* pMemory = malloc(iSize);
	if(!pMemory) prRaiseOOM(NULL);
	return pMemory;
}

INLINE void* mmAllocateZero(size_t iSize) {
	void* pMemory = calloc(1, iSize);
	if(!pMemory) prRaiseOOM(NULL);
	return pMemory;
}

INLINE void* mmReallocate(void* pMemory, size_t iSize) {
	void* pNew = realloc(pMemory, iSize);
	if(!pNew) prRaiseOOM(NULL);
	return pNew;
}

INLINE void* mmReallocateI(void* pMemory, size_t iSize) {
	void* pNew = mmReallocate(*(void**)pMemory, iSize);
	if(!pNew) return NULL;
	*(void**)pMemory = pNew;
	return pNew;
}
