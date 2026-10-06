/*
 * NT Header annotations
 *
 * This file is part of System Informer.
 */

#ifndef _PHNT_H
#define _PHNT_H





#define PHNT_MODE_KERNEL 0
#define PHNT_MODE_USER 1

#define PHNT_WINDOWS_ANCIENT 0
#define PHNT_WINDOWS_XP 51
#define PHNT_WINDOWS_SERVER_2003 52
#define PHNT_WINDOWS_VISTA 60
#define PHNT_WINDOWS_7 61
#define PHNT_WINDOWS_8 62
#define PHNT_WINDOWS_8_1 63
#define PHNT_WINDOWS_10 100
#define PHNT_WINDOWS_10_TH2 101
#define PHNT_WINDOWS_10_RS1 102
#define PHNT_WINDOWS_10_RS2 103
#define PHNT_WINDOWS_10_RS3 104
#define PHNT_WINDOWS_10_RS4 105
#define PHNT_WINDOWS_10_RS5 106
#define PHNT_WINDOWS_10_19H1 107
#define PHNT_WINDOWS_10_19H2 108
#define PHNT_WINDOWS_10_20H1 109
#define PHNT_WINDOWS_10_20H2 110
#define PHNT_WINDOWS_10_21H1 111
#define PHNT_WINDOWS_10_21H2 112
#define PHNT_WINDOWS_10_22H2 113
#define PHNT_WINDOWS_11 114
#define PHNT_WINDOWS_11_22H2 115
#define PHNT_WINDOWS_11_23H2 116
#define PHNT_WINDOWS_11_24H2 117
#define PHNT_WINDOWS_11_25H2 117
#define PHNT_WINDOWS_NEW ULONG_MAX

#ifndef PHNT_MODE
#if defined(_NTDDK_) || defined(_NTIFS_) || defined(_NTDRIVER_)
    #define PHNT_MODE PHNT_MODE_KERNEL
#else
    #define PHNT_MODE PHNT_MODE_USER
#endif
#endif

#ifndef PHNT_VERSION
#define PHNT_VERSION PHNT_WINDOWS_NEW
#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)
#ifndef PHNT_INLINE_TYPEDEFS
#define PHNT_INLINE_TYPEDEFS
#endif
#endif


EXTERN_C_START

#if (PHNT_MODE != PHNT_MODE_KERNEL)
#include <phnt_ntdef.h>
#include <ntnls.h>
#include <ntintsafe.h>
#include <nttypesafe.h>
#endif

#include <ntkeapi.h>
#include <ntldr.h>
#include <ntexapi.h>

#include <ntmmapi.h>
#include <ntobapi.h>
#include <ntpsapi.h>

#if (PHNT_MODE != PHNT_MODE_KERNEL)
#include <ntbcd.h>
#include <ntdbg.h>
#include <ntimage.h>
#include <ntioapi.h>
#include <ntlsa.h>
#include <ntlpcapi.h>
#include <ntmisc.h>
#include <ntpfapi.h>
#include <ntpnpapi.h>
#include <ntpoapi.h>
#include <ntregapi.h>
#include <ntrtl.h>
#include <ntsam.h>
#include <ntseapi.h>
#include <nttmapi.h>
#include <nttp.h>
#include <ntuser.h>
#include <ntwmi.h>
#include <ntwow64.h>
#include <ntxcapi.h>
#endif

EXTERN_C_END

static_assert(__alignof(LARGE_INTEGER) == 8, "Windows headers require the default packing option. Changing the packing can lead to memory corruption.");
static_assert(__alignof(PROCESS_CYCLE_TIME_INFORMATION) == 8, "PHNT headers require the default packing option. Changing the packing can lead to memory corruption.");

#endif
