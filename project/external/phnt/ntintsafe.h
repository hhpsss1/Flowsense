/******************************************************************
*                                                                 *
*  ntintsafe.h -- This module defines helper functions to prevent *
*                 integer overflow bugs for drivers. A similar    *
*                 file, intsafe.h, is available for applications. *
*                                                                 *
*  Copyright (c) Microsoft Corp.  All rights reserved.            *
*                                                                 *
******************************************************************/

#ifndef _NTINTSAFE_H_INCLUDED_
#define _NTINTSAFE_H_INCLUDED_

#include <winapifamily.h>


#if (_MSC_VER > 1000)
#pragma once
#endif

#pragma region Application Family or Games Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP | WINAPI_PARTITION_SYSTEM | WINAPI_PARTITION_GAMES)

#include <specstrings.h>

#if _MSC_VER >= 1200
#pragma warning(push)
#pragma warning(disable:4668)
#endif

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86) || defined(_ARM_) || defined(_M_ARM)) && (_MSC_VER >= 1300)
#define _W64 __w64
#else
#define _W64
#endif
#endif

typedef char                CHAR;
typedef signed char         INT8;
typedef unsigned char       UCHAR;
typedef unsigned char       UINT8;
typedef unsigned char       BYTE;
typedef short               SHORT;
typedef signed short        INT16;
typedef unsigned short      USHORT;
typedef unsigned short      UINT16;
typedef unsigned short      WORD;
typedef int                 INT;
typedef signed int          INT32;
typedef unsigned int        UINT;
typedef unsigned int        UINT32;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef __int64             LONGLONG;
typedef __int64             LONG64;
typedef signed __int64      RtlINT64;
typedef unsigned __int64    ULONGLONG;
typedef unsigned __int64    DWORDLONG;
typedef unsigned __int64    ULONG64;
typedef unsigned __int64    DWORD64;
typedef unsigned __int64    UINT64;

#if (__midl > 501)
typedef [public]          __int3264 INT_PTR;
typedef [public] unsigned __int3264 UINT_PTR;
typedef [public]          __int3264 LONG_PTR;
typedef [public] unsigned __int3264 ULONG_PTR;
#else
#ifdef _WIN64
typedef __int64             INT_PTR;
typedef unsigned __int64    UINT_PTR;
typedef __int64             LONG_PTR;
typedef unsigned __int64    ULONG_PTR;
#else
typedef _W64 int            INT_PTR;
typedef _W64 unsigned int   UINT_PTR;
typedef _W64 long           LONG_PTR;
typedef _W64 unsigned long  ULONG_PTR;
#endif
#endif

#ifdef _WIN64
typedef __int64             ptrdiff_t;
typedef unsigned __int64    size_t;
#else
typedef _W64 int            ptrdiff_t;
typedef _W64 unsigned int   size_t;
#endif

typedef ULONG_PTR   DWORD_PTR;
typedef LONG_PTR    SSIZE_T;
typedef ULONG_PTR   SIZE_T;

#undef _USE_INTRINSIC_MULTIPLY128

#if !defined(_M_CEE) && ((defined(_AMD64_) && !defined(_ARM64EC_)) || (defined(_IA64_) && (_MSC_VER >= 1400)))
#define _USE_INTRINSIC_MULTIPLY128
#endif

#if defined(_USE_INTRINSIC_MULTIPLY128)
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_ARM64EC_)

#define UnsignedMultiply128 _umul128

#else

#define _umul128 Multiply128

#endif

ULONG64
UnsignedMultiply128(
    _In_ ULONGLONG ullMultiplicand,
    _In_ ULONGLONG ullMultiplier,
    _Out_ ULONGLONG* pullResultHigh);

#if !defined(_ARM64EC_)
#pragma intrinsic(_umul128)
#endif

#ifdef __cplusplus
}
#endif
#endif



typedef _Return_type_success_(return >= 0) long NTSTATUS;

#define NT_SUCCESS(Status)  (((NTSTATUS)(Status)) >= 0)

#define STATUS_SUCCESS  ((NTSTATUS)0x00000000L)
#ifndef SORTPP_PASS
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#else
#define C_ASSERT(e)
#endif

#ifndef UInt32x32To64
#if defined(MIDL_PASS) || defined(RC_INVOKED) || defined(_M_CEE_PURE) \
    || defined(_68K_) || defined(_MPPC_) \
    || defined(_M_IA64) || defined(_M_AMD64) || defined(_M_ARM) || defined(_M_ARM64) \
    || defined(_M_HYBRID_X86_ARM64)
#define UInt32x32To64(a, b) (((unsigned __int64)((unsigned int)(a))) * ((unsigned __int64)((unsigned int)(b))))
#elif defined(_M_IX86)
#define UInt32x32To64(a, b) ((unsigned __int64)(((unsigned __int64)((unsigned int)(a))) * ((unsigned int)(b))))
#else
#error Must define a target architecture.
#endif
#endif

#define INT8_MIN        (-127i8 - 1)
#define SHORT_MIN       (-32768)
#define INT16_MIN       (-32767i16 - 1)
#ifndef INT_MIN
#define INT_MIN         (-2147483647 - 1)
#endif
#define INT32_MIN       (-2147483647i32 - 1)
#ifndef LONG_MIN
#define LONG_MIN        (-2147483647L - 1)
#endif
#define LONGLONG_MIN    (-9223372036854775807i64 - 1)
#define LONG64_MIN      (-9223372036854775807i64 - 1)
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT128_MIN      (-170141183460469231731687303715884105727i128 - 1)

#ifdef _WIN64
#define INT_PTR_MIN     (-9223372036854775807i64 - 1)
#define LONG_PTR_MIN    (-9223372036854775807i64 - 1)
#define PTRDIFF_T_MIN   (-9223372036854775807i64 - 1)
#define SSIZE_T_MIN     (-9223372036854775807i64 - 1)
#else
#define INT_PTR_MIN     (-2147483647 - 1)
#define LONG_PTR_MIN    (-2147483647L - 1)
#define PTRDIFF_T_MIN   (-2147483647 - 1)
#define SSIZE_T_MIN     (-2147483647L - 1)
#endif

#define INT8_MAX        127i8
#define UINT8_MAX       0xffui8
#define BYTE_MAX        0xff
#define SHORT_MAX       32767
#define INT16_MAX       32767i16
#define USHORT_MAX      0xffff
#define UINT16_MAX      0xffffui16
#define WORD_MAX        0xffff
#ifndef INT_MAX
#define INT_MAX         2147483647
#endif
#define INT32_MAX       2147483647i32
#ifndef UINT_MAX
#define UINT_MAX        0xffffffff
#endif
#define UINT32_MAX      0xffffffffui32
#ifndef LONG_MAX
#define LONG_MAX        2147483647L
#endif
#ifndef ULONG_MAX
#define ULONG_MAX       0xffffffffUL
#endif
#define DWORD_MAX       0xffffffffUL
#define LONGLONG_MAX    9223372036854775807i64
#define LONG64_MAX      9223372036854775807i64
#define INT64_MAX       9223372036854775807i64
#define ULONGLONG_MAX   0xffffffffffffffffui64
#define DWORDLONG_MAX   0xffffffffffffffffui64
#define ULONG64_MAX     0xffffffffffffffffui64
#define DWORD64_MAX     0xffffffffffffffffui64
#define UINT64_MAX      0xffffffffffffffffui64
#define INT128_MAX      170141183460469231731687303715884105727i128
#define UINT128_MAX     0xffffffffffffffffffffffffffffffffui128

#undef SIZE_T_MAX

#ifdef _WIN64
#define INT_PTR_MAX     9223372036854775807i64
#define UINT_PTR_MAX    0xffffffffffffffffui64
#define LONG_PTR_MAX    9223372036854775807i64
#define ULONG_PTR_MAX   0xffffffffffffffffui64
#define DWORD_PTR_MAX   0xffffffffffffffffui64
#define PTRDIFF_T_MAX   9223372036854775807i64
#define SIZE_T_MAX      0xffffffffffffffffui64
#define SSIZE_T_MAX     9223372036854775807i64
#define _SIZE_T_MAX     0xffffffffffffffffui64
#else
#define INT_PTR_MAX     2147483647
#define UINT_PTR_MAX    0xffffffff
#define LONG_PTR_MAX    2147483647L
#define ULONG_PTR_MAX   0xffffffffUL
#define DWORD_PTR_MAX   0xffffffffUL
#define PTRDIFF_T_MAX   2147483647
#define SIZE_T_MAX      0xffffffff
#define SSIZE_T_MAX     2147483647L
#define _SIZE_T_MAX     0xffffffffUL
#endif


#define INT8_ERROR      (-1i8)
#define UINT8_ERROR     0xffui8
#define BYTE_ERROR      0xff
#define SHORT_ERROR     (-1)
#define INT16_ERROR     (-1i16)
#define USHORT_ERROR    0xffff
#define UINT16_ERROR    0xffffui16
#define WORD_ERROR      0xffff
#define INT_ERROR       (-1)
#define INT32_ERROR     (-1i32)
#define UINT_ERROR      0xffffffff
#define UINT32_ERROR    0xffffffffui32
#define LONG_ERROR      (-1L)
#define ULONG_ERROR     0xffffffffUL
#define DWORD_ERROR     0xffffffffUL
#define LONGLONG_ERROR  (-1i64)
#define LONG64_ERROR    (-1i64)
#define INT64_ERROR     (-1i64)
#define ULONGLONG_ERROR 0xffffffffffffffffui64
#define DWORDLONG_ERROR 0xffffffffffffffffui64
#define ULONG64_ERROR   0xffffffffffffffffui64
#define UINT64_ERROR    0xffffffffffffffffui64

#ifdef _WIN64
#define INT_PTR_ERROR   (-1i64)
#define UINT_PTR_ERROR  0xffffffffffffffffui64
#define LONG_PTR_ERROR  (-1i64)
#define ULONG_PTR_ERROR 0xffffffffffffffffui64
#define DWORD_PTR_ERROR 0xffffffffffffffffui64
#define PTRDIFF_T_ERROR (-1i64)
#define SIZE_T_ERROR    0xffffffffffffffffui64
#define SSIZE_T_ERROR   (-1i64)
#define _SIZE_T_ERROR   0xffffffffffffffffui64
#else
#define INT_PTR_ERROR   (-1)
#define UINT_PTR_ERROR  0xffffffff
#define LONG_PTR_ERROR  (-1L)
#define ULONG_PTR_ERROR 0xffffffffUL
#define DWORD_PTR_ERROR 0xffffffffUL
#define PTRDIFF_T_ERROR (-1)
#define SIZE_T_ERROR    0xffffffff
#define SSIZE_T_ERROR   (-1L)
#define _SIZE_T_ERROR   0xffffffffUL
#endif


C_ASSERT(sizeof(USHORT) == 2);
C_ASSERT(sizeof(INT) == 4);
C_ASSERT(sizeof(UINT) == 4);
C_ASSERT(sizeof(LONG) == 4);
C_ASSERT(sizeof(ULONG) == 4);
C_ASSERT(sizeof(UINT_PTR) == sizeof(ULONG_PTR));




_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToUChar(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) UCHAR* pch)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pch = (UCHAR)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToUInt8(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) UINT8* pu8Result)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pu8Result = (UINT8)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlInt8ToByte  RtlInt8ToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToUShort(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) USHORT* pusResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pusResult = (USHORT)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlInt8ToUInt16    RtlInt8ToUShort

#define RtlInt8ToWord  RtlInt8ToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToUInt(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) UINT* puResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *puResult = (UINT)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlInt8ToUInt32    RtlInt8ToUInt

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToUIntPtr(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *puResult = (UINT_PTR)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToULong(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) ULONG* pulResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pulResult = (ULONG)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToULongPtr(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pulResult = (ULONG_PTR)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlInt8ToDWord RtlInt8ToULong

#define RtlInt8ToDWordPtr  RtlInt8ToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlInt8ToULongLong(
    _In_ INT8 i8Operand,
    _Out_ _Deref_out_range_(==, i8Operand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (i8Operand >= 0)
    {
        *pullResult = (ULONGLONG)i8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlInt8ToDWordLong RtlInt8ToULongLong

#define RtlInt8ToULong64   RtlInt8ToULongLong

#define RtlInt8ToDWord64   RtlInt8ToULongLong

#define RtlInt8ToUInt64    RtlInt8ToULongLong

#define RtlInt8ToSizeT RtlInt8ToUIntPtr

#define RtlInt8ToSIZET RtlInt8ToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlUInt8ToInt8(
    _In_ UINT8 u8Operand,
    _Out_ _Deref_out_range_(==, u8Operand) INT8* pi8Result)
{
    NTSTATUS status;

    if (u8Operand <= INT8_MAX)
    {
        *pi8Result = (INT8)u8Operand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlUInt8ToChar(
    _In_ UINT8 u8Operand,
    _Out_ _Deref_out_range_(==, u8Operand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    *pch = (CHAR)u8Operand;
    return STATUS_SUCCESS;
#else
    return RtlUInt8ToInt8(u8Operand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlByteToInt8(
    _In_ BYTE bOperand,
    _Out_ _Deref_out_range_(==, bOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (bOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)bOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlByteToChar(
    _In_ BYTE bOperand,
    _Out_ _Deref_out_range_(==, bOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    *pch = (CHAR)bOperand;
    return STATUS_SUCCESS;
#else
    return RtlByteToInt8(bOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToInt8(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((sOperand >= INT8_MIN) && (sOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToUChar(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((sOperand >= 0) && (sOperand <= 255))
    {
        *pch = (UCHAR)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlShortToChar(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlShortToUChar(sOperand, (UCHAR*)pch);
#else
    return RtlShortToInt8(sOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToUInt8(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if ((sOperand >= 0) && (sOperand <= UINT8_MAX))
    {
        *pui8Result = (UINT8)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlShortToByte  RtlShortToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToUShort(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *pusResult = (USHORT)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlShortToUInt16   RtlShortToUShort

#define RtlShortToWord RtlShortToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToUInt(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) UINT* puResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *puResult = (UINT)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlShortToUInt32   RtlShortToUInt

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToUIntPtr(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *puResult = (UINT_PTR)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToULong(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *pulResult = (ULONG)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToULongPtr(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *pulResult = (ULONG_PTR)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlShortToDWord    RtlShortToULong

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToDWordPtr(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) DWORD_PTR* pdwResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *pdwResult = (DWORD_PTR)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pdwResult = DWORD_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortToULongLong(
    _In_ SHORT sOperand,
    _Out_ _Deref_out_range_(==, sOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (sOperand >= 0)
    {
        *pullResult = (ULONGLONG)sOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlShortToDWordLong    RtlShortToULongLong

#define RtlShortToULong64  RtlShortToULongLong

#define RtlShortToDWord64  RtlShortToULongLong

#define RtlShortToUInt64   RtlShortToULongLong

#define RtlShortToSizeT    RtlShortToUIntPtr

#define RtlShortToSIZET    RtlShortToULongPtr

#define RtlInt16ToChar RtlShortToChar

#define RtlInt16ToInt8 RtlShortToInt8

#define RtlInt16ToUChar    RtlShortToUChar

#define RtlInt16ToUInt8    RtlShortToUInt8

#define RtlInt16ToByte RtlShortToUInt8

#define RtlInt16ToUShort   RtlShortToUShort

#define RtlInt16ToUInt16   RtlShortToUShort

#define RtlInt16ToWord RtlShortToUShort

#define RtlInt16ToUInt RtlShortToUInt

#define RtlInt16ToUInt32   RtlShortToUInt

#define RtlInt16ToUIntPtr  RtlShortToUIntPtr

#define RtlInt16ToULong    RtlShortToULong

#define RtlInt16ToULongPtr RtlShortToULongPtr

#define RtlInt16ToDWord    RtlShortToULong

#define RtlInt16ToDWordPtr RtlShortToULongPtr

#define RtlInt16ToULongLong    RtlShortToULongLong

#define RtlInt16ToDWordLong    RtlShortToULongLong

#define RtlInt16ToULong64  RtlShortToULongLong

#define RtlInt16ToDWord64  RtlShortToULongLong

#define RtlInt16ToUInt64   RtlShortToULongLong

#define RtlInt16ToSizeT    RtlShortToUIntPtr

#define RtlInt16ToSIZET    RtlShortToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortToInt8(
    _In_ USHORT usOperand,
    _Out_ _Deref_out_range_(==, usOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (usOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)usOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortToUChar(
    _In_ USHORT usOperand,
    _Out_ _Deref_out_range_(==, usOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (usOperand <= 255)
    {
        *pch = (UCHAR)usOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlUShortToChar(
    _In_ USHORT usOperand,
    _Out_ _Deref_out_range_(==, usOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlUShortToUChar(usOperand, (UCHAR*)pch);
#else
    return RtlUShortToInt8(usOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortToUInt8(
    _In_ USHORT usOperand,
    _Out_ _Deref_out_range_(==, usOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if (usOperand <= UINT8_MAX)
    {
        *pui8Result = (UINT8)usOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUShortToByte    RtlUShortToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortToShort(
    _In_ USHORT usOperand,
    _Out_ _Deref_out_range_(==, usOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (usOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)usOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUShortToInt16   RtlUShortToShort

#define RtlUInt16ToChar    RtlUShortToChar

#define RtlUInt16ToInt8    RtlUShortToInt8

#define RtlUInt16ToUChar   RtlUShortToUChar

#define RtlUInt16ToUInt8   RtlUShortToUInt8

#define RtlUInt16ToByte    RtlUShortToUInt8

#define RtlUInt16ToShort   RtlUShortToShort

#define RtlUInt16ToInt16   RtlUShortToShort

#define RtlWordToInt8  RtlUShortToInt8

#define RtlWordToChar  RtlUShortToChar

#define RtlWordToUChar RtlUShortToUChar

#define RtlWordToUInt8 RtlUShortToUInt8

#define RtlWordToByte  RtlUShortToUInt8

#define RtlWordToShort RtlUShortToShort

#define RtlWordToInt16 RtlUShortToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToInt8(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((iOperand >= INT8_MIN) && (iOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToUChar(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= 255))
    {
        *pch = (UCHAR)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlIntToChar(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlIntToUChar(iOperand, (UCHAR*)pch);
#else
    return RtlIntToInt8(iOperand, (INT8*)pch);
#endif
}

#define RtlIntToByte   RtlIntToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToUInt8(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= UINT8_MAX))
    {
        *pui8Result = (UINT8)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToShort(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) SHORT* psResult)
{
    NTSTATUS status;

    if ((iOperand >= SHORT_MIN) && (iOperand <= SHORT_MAX))
    {
        *psResult = (SHORT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntToInt16  RtlIntToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToUShort(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= USHORT_MAX))
    {
        *pusResult = (USHORT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntToUInt16  RtlIntToUShort

#define RtlIntToWord   RtlIntToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToUInt(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UINT* puResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *puResult = (UINT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlIntToUIntPtr    RtlIntToULongLong
#else
#define RtlIntToUIntPtr    RtlIntToUInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToULong(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *pulResult = (ULONG)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlIntToULongPtr   RtlIntToULongLong
#else
#define RtlIntToULongPtr   RtlIntToULong
#endif

#define RtlIntToDWord  RtlIntToULong

#define RtlIntToDWordPtr   RtlIntToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlIntToULongLong(
    _In_ INT iOperand,
    _Out_ _Deref_out_range_(==, iOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *pullResult = (ULONGLONG)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntToDWordLong  RtlIntToULongLong

#define RtlIntToULong64    RtlIntToULongLong

#define RtlIntToDWord64    RtlIntToULongLong

#define RtlIntToUInt64 RtlIntToULongLong

#define RtlIntToSizeT  RtlIntToUIntPtr

#define RtlIntToSIZET  RtlIntToULongPtr

#define RtlInt32ToChar RtlIntToChar

#define RtlInt32ToInt8 RtlIntToInt8

#define RtlInt32ToUChar    RtlIntToUChar

#define RtlInt32ToByte RtlIntToUInt8

#define RtlInt32ToUInt8    RtlIntToUInt8

#define RtlInt32ToShort    RtlIntToShort

#define RtlInt32ToInt16    RtlIntToShort

#define RtlInt32ToUShort   RtlIntToUShort

#define RtlInt32ToUInt16   RtlIntToUShort

#define RtlInt32ToWord RtlIntToUShort

#define RtlInt32ToUInt RtlIntToUInt

#define RtlInt32ToUInt32   RtlIntToUInt

#define RtlInt32ToUIntPtr  RtlIntToUIntPtr

#define RtlInt32ToULong    RtlIntToULong

#define RtlInt32ToULongPtr RtlIntToULongPtr

#define RtlInt32ToDWord    RtlIntToULong

#define RtlInt32ToDWordPtr RtlIntToULongPtr

#define RtlInt32ToULongLong    RtlIntToULongLong

#define RtlInt32ToDWordLong    RtlIntToULongLong

#define RtlInt32ToULong64  RtlIntToULongLong

#define RtlInt32ToDWord64  RtlIntToULongLong

#define RtlInt32ToUInt64   RtlIntToULongLong

#define RtlInt32ToSizeT    RtlIntToUIntPtr

#define RtlInt32ToSIZET    RtlIntToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToInt8(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((iOperand >= INT8_MIN) && (iOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToUChar(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= 255))
    {
        *pch = (UCHAR)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlIntPtrToChar(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlIntPtrToUChar(iOperand, (UCHAR*)pch);
#else
    return RtlIntPtrToInt8(iOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToUInt8(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= UINT8_MAX))
    {
        *pui8Result = (UINT8)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntPtrToByte    RtlIntPtrToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToShort(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) SHORT* psResult)
{
    NTSTATUS status;

    if ((iOperand >= SHORT_MIN) && (iOperand <= SHORT_MAX))
    {
        *psResult = (SHORT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntPtrToInt16   RtlIntPtrToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToUShort(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if ((iOperand >= 0) && (iOperand <= USHORT_MAX))
    {
        *pusResult = (USHORT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlIntPtrToUInt16  RtlIntPtrToUShort

#define RtlIntPtrToWord    RtlIntPtrToUShort

#ifdef _WIN64
#define RtlIntPtrToInt RtlLongLongToInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToInt(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) INT* piResult)
{
    *piResult = (INT)iOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlIntPtrToInt32   RtlIntPtrToInt

#ifdef _WIN64
#define RtlIntPtrToUInt    RtlLongLongToUInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToUInt(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UINT* puResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *puResult = (UINT)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#define RtlIntPtrToUInt32  RtlIntPtrToUInt

#ifdef _WIN64
#define RtlIntPtrToUIntPtr RtlLongLongToULongLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToUIntPtr(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *puResult = (UINT_PTR)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#ifdef _WIN64
#define RtlIntPtrToLong    RtlLongLongToLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToLong(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) LONG* plResult)
{
    *plResult = (LONG)iOperand;
    return STATUS_SUCCESS;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToLongPtr(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) LONG_PTR* plResult)
{
    *plResult = (LONG_PTR)iOperand;
    return STATUS_SUCCESS;
}

#ifdef _WIN64
#define RtlIntPtrToULong   RtlLongLongToULong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToULong(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *pulResult = (ULONG)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#ifdef _WIN64
#define RtlIntPtrToULongPtr    RtlLongLongToULongLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToULongPtr(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *pulResult = (ULONG_PTR)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#define RtlIntPtrToDWord   RtlIntPtrToULong

#define RtlIntPtrToDWordPtr    RtlIntPtrToULongPtr

#ifdef _WIN64
#define RtlIntPtrToULongLong   RtlLongLongToULongLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrToULongLong(
    _In_ INT_PTR iOperand,
    _Out_ _Deref_out_range_(==, iOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (iOperand >= 0)
    {
        *pullResult = (ULONGLONG)iOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#define RtlIntPtrToDWordLong   RtlIntPtrToULongLong

#define RtlIntPtrToULong64 RtlIntPtrToULongLong

#define RtlIntPtrToDWord64 RtlIntPtrToULongLong

#define RtlIntPtrToUInt64  RtlIntPtrToULongLong

#define RtlIntPtrToSizeT   RtlIntPtrToUIntPtr

#define RtlIntPtrToSIZET   RtlIntPtrToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToInt8(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (uOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToUChar(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (uOperand <= 255)
    {
        *pch = (UCHAR)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlUIntToChar(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlUIntToUChar(uOperand, (UCHAR*)pch);
#else
    return RtlUIntToInt8(uOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToUInt8(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if (uOperand <= UINT8_MAX)
    {
        *pui8Result = (UINT8)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntToByte   RtlUIntToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToShort(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (uOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntToInt16 RtlUIntToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToUShort(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (uOperand <= USHORT_MAX)
    {
        *pusResult = (USHORT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntToUInt16    RtlUIntToUShort

#define RtlUIntToWord  RtlUIntToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToInt(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT* piResult)
{
    NTSTATUS status;

    if (uOperand <= INT_MAX)
    {
        *piResult = (INT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntToInt32 RtlUIntToInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToIntPtr(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT_PTR* piResult)
{
    *piResult = uOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlUIntToIntPtr    RtlUIntToInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToLong(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) LONG* plResult)
{
    NTSTATUS status;

    if (uOperand <= LONG_MAX)
    {
        *plResult = (LONG)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntToLongPtr(
    _In_ UINT uOperand,
    _Out_ _Deref_out_range_(==, uOperand) LONG_PTR* plResult)
{
    *plResult = uOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlUIntToLongPtr   RtlUIntToLong
#endif

#define RtlUIntToPtrdiffT  RtlUIntToIntPtr

#define RtlUIntToSSIZET    RtlUIntToLongPtr

#define RtlUInt32ToChar    RtlUIntToChar

#define RtlUInt32ToInt8    RtlUIntToInt8

#define RtlUInt32ToUChar   RtlUIntToUChar

#define RtlUInt32ToUInt8   RtlUIntToUInt8

#define RtlUInt32ToByte    RtlUInt32ToUInt8

#define RtlUInt32ToShort   RtlUIntToShort

#define RtlUInt32ToInt16   RtlUIntToShort

#define RtlUInt32ToUShort  RtlUIntToUShort

#define RtlUInt32ToUInt16  RtlUIntToUShort

#define RtlUInt32ToWord    RtlUIntToUShort

#define RtlUInt32ToInt RtlUIntToInt

#define RtlUInt32ToIntPtr  RtlUIntToIntPtr

#define RtlUInt32ToInt32   RtlUIntToInt

#define RtlUInt32ToLong    RtlUIntToLong

#define RtlUInt32ToLongPtr RtlUIntToLongPtr

#define RtlUInt32ToPtrdiffT    RtlUIntToPtrdiffT

#define RtlUInt32ToSSIZET  RtlUIntToSSIZET

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToInt8(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (uOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToUChar(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (uOperand <= 255)
    {
        *pch = (UCHAR)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlUIntPtrToChar(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlUIntPtrToUChar(uOperand, (UCHAR*)pch);
#else
    return RtlUIntPtrToInt8(uOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToUInt8(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==,uOperand) UINT8* pu8Result)
{
    NTSTATUS status;

    if (uOperand <= UINT8_MAX)
    {
        *pu8Result = (UINT8)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntPtrToByte   RtlUIntPtrToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToShort(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (uOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToInt16(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT16* pi16Result)
{
    NTSTATUS status;

    if (uOperand <= INT16_MAX)
    {
        *pi16Result = (INT16)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi16Result = INT16_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToUShort(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (uOperand <= USHORT_MAX)
    {
        *pusResult = (USHORT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToUInt16(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) UINT16* pu16Result)
{
    NTSTATUS status;

    if (uOperand <= UINT16_MAX)
    {
        *pu16Result = (UINT16)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu16Result = UINT16_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntPtrToWord   RtlUIntPtrToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToInt(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT* piResult)
{
    NTSTATUS status;

    if (uOperand <= INT_MAX)
    {
        *piResult = (INT)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUIntPtrToInt32  RtlUIntPtrToInt

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToIntPtr(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) INT_PTR* piResult)
{
    NTSTATUS status;

    if (uOperand <= INT_PTR_MAX)
    {
        *piResult = (INT_PTR)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlUIntPtrToUInt   RtlULongLongToUInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToUInt(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) UINT* puResult)
{
    *puResult = (UINT)uOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlUIntPtrToUInt32 RtlUIntPtrToUInt

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToLong(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) LONG* plResult)
{
    NTSTATUS status;

    if (uOperand <= LONG_MAX)
    {
        *plResult = (LONG)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToLongPtr(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) LONG_PTR* plResult)
{
    NTSTATUS status;

    if (uOperand <= LONG_PTR_MAX)
    {
        *plResult = (LONG_PTR)uOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlUIntPtrToULong  RtlULongLongToULong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToULong(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) ULONG* pulResult)
{
    *pulResult = (ULONG)uOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlUIntPtrToDWord  RtlUIntPtrToULong

#ifdef _WIN64
#define RtlUIntPtrToLongLong   RtlULongLongToLongLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrToLongLong(
    _In_ UINT_PTR uOperand,
    _Out_ _Deref_out_range_(==, uOperand) LONGLONG* pllResult)
{
    *pllResult = (LONGLONG)uOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlUIntPtrToLong64 RtlUIntPtrToLongLong

#define RtlUIntPtrToInt64  RtlUIntPtrToLongLong

#define RtlUIntPtrToPtrdiffT   RtlUIntPtrToIntPtr

#define RtlUIntPtrToSSIZET RtlUIntPtrToLongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToInt8(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((lOperand >= INT8_MIN) && (lOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToUChar(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= 255))
    {
        *pch = (UCHAR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlLongToChar(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlLongToUChar(lOperand, (UCHAR*)pch);
#else
    return RtlLongToInt8(lOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToUInt8(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= UINT8_MAX))
    {
        *pui8Result = (UINT8)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongToByte  RtlLongToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToShort(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) SHORT* psResult)
{
    NTSTATUS status;

    if ((lOperand >= SHORT_MIN) && (lOperand <= SHORT_MAX))
    {
       *psResult = (SHORT)lOperand;
       status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongToInt16 RtlLongToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToUShort(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= USHORT_MAX))
    {
        *pusResult = (USHORT)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongToUInt16    RtlLongToUShort

#define RtlLongToWord  RtlLongToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToInt(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT* piResult)
{
    C_ASSERT(sizeof(INT) == sizeof(LONG));
    *piResult = (INT)lOperand;
    return STATUS_SUCCESS;
}

#define RtlLongToInt32 RtlLongToInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlLongToIntPtr(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT_PTR* piResult)
{
    *piResult = lOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlLongToIntPtr    RtlLongToInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToUInt(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT* puResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *puResult = (UINT)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongToUInt32    RtlLongToUInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlLongToUIntPtr(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *puResult = (UINT_PTR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#else
#define RtlLongToUIntPtr   RtlLongToUInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToULong(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pulResult = (ULONG)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlLongToULongPtr(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pulResult = (ULONG_PTR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#else
#define RtlLongToULongPtr  RtlLongToULong
#endif

#define RtlLongToDWord RtlLongToULong

#define RtlLongToDWordPtr  RtlLongToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongToULongLong(
    _In_ LONG lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pullResult = (ULONGLONG)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongToDWordLong RtlLongToULongLong

#define RtlLongToULong64   RtlLongToULongLong

#define RtlLongToDWord64   RtlLongToULongLong

#define RtlLongToUInt64    RtlLongToULongLong

#define RtlLongToPtrdiffT  RtlLongToIntPtr

#define RtlLongToSizeT RtlLongToUIntPtr

#define RtlLongToSIZET RtlLongToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToInt8(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((lOperand >= INT8_MIN) && (lOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToUChar(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= 255))
    {
        *pch = (UCHAR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlLongPtrToChar(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlLongPtrToUChar(lOperand, (UCHAR*)pch);
#else
    return RtlLongPtrToInt8(lOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToUInt8(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= UINT8_MAX))
    {
        *pui8Result = (UINT8)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongPtrToByte   RtlLongPtrToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToShort(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) SHORT* psResult)
{
    NTSTATUS status;

    if ((lOperand >= SHORT_MIN) && (lOperand <= SHORT_MAX))
    {
        *psResult = (SHORT)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongPtrToInt16  RtlLongPtrToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToUShort(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if ((lOperand >= 0) && (lOperand <= USHORT_MAX))
    {
        *pusResult = (USHORT)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongPtrToUInt16 RtlLongPtrToUShort

#define RtlLongPtrToWord   RtlLongPtrToUShort

#ifdef _WIN64
#define RtlLongPtrToInt    RtlLongLongToInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToInt(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT* piResult)
{
    C_ASSERT(sizeof(INT) == sizeof(LONG_PTR));
    *piResult = (INT)lOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlLongPtrToInt32  RtlLongPtrToInt

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToIntPtr(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) INT_PTR* piResult)
{
    C_ASSERT(sizeof(LONG_PTR) == sizeof(INT_PTR));
    *piResult = (INT_PTR)lOperand;
    return STATUS_SUCCESS;
}

#ifdef _WIN64
#define RtlLongPtrToUInt   RtlLongLongToUInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToUInt(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT* puResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *puResult = (UINT)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#define RtlLongPtrToUInt32 RtlLongPtrToUInt

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToUIntPtr(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *puResult = (UINT_PTR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlLongPtrToLong   RtlLongLongToLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToLong(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) LONG* plResult)
{
    *plResult = (LONG)lOperand;
    return STATUS_SUCCESS;
}
#endif

#ifdef _WIN64
#define RtlLongPtrToULong  RtlLongLongToULong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToULong(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pulResult = (ULONG)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToULongPtr(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pulResult = (ULONG_PTR)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongPtrToDWord  RtlLongPtrToULong

#define RtlLongPtrToDWordPtr   RtlLongPtrToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrToULongLong(
    _In_ LONG_PTR lOperand,
    _Out_ _Deref_out_range_(==, lOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (lOperand >= 0)
    {
        *pullResult = (ULONGLONG)lOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongPtrToDWordLong  RtlLongPtrToULongLong

#define RtlLongPtrToULong64    RtlLongPtrToULongLong

#define RtlLongPtrToDWord64    RtlLongPtrToULongLong

#define RtlLongPtrToUInt64 RtlLongPtrToULongLong

#define RtlLongPtrToSizeT  RtlLongPtrToUIntPtr

#define RtlLongPtrToSIZET  RtlLongPtrToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToInt8(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (ulOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToUChar(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (ulOperand <= 255)
    {
        *pch = (UCHAR)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlULongToChar(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlULongToUChar(ulOperand, (UCHAR*)pch);
#else
    return RtlULongToInt8(ulOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToUInt8(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if (ulOperand <= UINT8_MAX)
    {
        *pui8Result = (UINT8)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongToByte RtlULongToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToShort(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (ulOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongToInt16    RtlULongToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToUShort(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (ulOperand <= USHORT_MAX)
    {
        *pusResult = (USHORT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongToUInt16   RtlULongToUShort

#define RtlULongToWord RtlULongToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToInt(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT* piResult)
{
    NTSTATUS status;

    if (ulOperand <= INT_MAX)
    {
        *piResult = (INT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongToInt32    RtlULongToInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlULongToIntPtr(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT_PTR* piResult)
{
    *piResult = (INT_PTR)ulOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlULongToIntPtr   RtlULongToInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToUInt(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT* puResult)
{
    C_ASSERT(sizeof(ULONG) == sizeof(UINT));
    *puResult = (UINT)ulOperand;
    return STATUS_SUCCESS;
}

#define RtlULongToUInt32   RtlULongToUInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlULongToUIntPtr(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT_PTR* puiResult)
{
    C_ASSERT(sizeof(UINT_PTR) > sizeof(ULONG));
    *puiResult = (UINT_PTR)ulOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlULongToUIntPtr  RtlULongToUInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongToLong(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) LONG* plResult)
{
    NTSTATUS status;

    if (ulOperand <= LONG_MAX)
    {
        *plResult = (LONG)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlULongToLongPtr(
    _In_ ULONG ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) LONG_PTR* plResult)
{
    C_ASSERT(sizeof(LONG_PTR) > sizeof(ULONG));
    *plResult = (LONG_PTR)ulOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlULongToLongPtr  RtlULongToLong
#endif

#define RtlULongToPtrdiffT RtlULongToIntPtr

#define RtlULongToSSIZET   RtlULongToLongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToInt8(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (ulOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToUChar(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (ulOperand <= 255)
    {
        *pch = (UCHAR)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlULongPtrToChar(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlULongPtrToUChar(ulOperand, (UCHAR*)pch);
#else
    return RtlULongPtrToInt8(ulOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToUInt8(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT8* pui8Result)
{
    NTSTATUS status;

    if (ulOperand <= UINT8_MAX)
    {
        *pui8Result = (UINT8)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pui8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongPtrToByte  RtlULongPtrToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToShort(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (ulOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongPtrToInt16 RtlULongPtrToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToUShort(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (ulOperand <= USHORT_MAX)
    {
        *pusResult = (USHORT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongPtrToUInt16    RtlULongPtrToUShort

#define RtlULongPtrToWord  RtlULongPtrToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToInt(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT* piResult)
{
    NTSTATUS status;

    if (ulOperand <= INT_MAX)
    {
        *piResult = (INT)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongPtrToInt32 RtlULongPtrToInt

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToIntPtr(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) INT_PTR* piResult)
{
    NTSTATUS status;

    if (ulOperand <= INT_PTR_MAX)
    {
        *piResult = (INT_PTR)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlULongPtrToUInt  RtlULongLongToUInt
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToUInt(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT* puResult)
{
    C_ASSERT(sizeof(ULONG_PTR) == sizeof(UINT));
    *puResult = (UINT)ulOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlULongPtrToUInt32    RtlULongPtrToUInt

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToUIntPtr(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) UINT_PTR* puResult)
{
    *puResult = (UINT_PTR)ulOperand;
    return STATUS_SUCCESS;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToLong(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) LONG* plResult)
{
    NTSTATUS status;

    if (ulOperand <= LONG_MAX)
    {
        *plResult = (LONG)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToLongPtr(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) LONG_PTR* plResult)
{
    NTSTATUS status;

    if (ulOperand <= LONG_PTR_MAX)
    {
        *plResult = (LONG_PTR)ulOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlULongPtrToULong RtlULongLongToULong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToULong(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) ULONG* pulResult)
{
    *pulResult = (ULONG)ulOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlULongPtrToDWord RtlULongPtrToULong

#ifdef _WIN64
#define RtlULongPtrToLongLong  RtlULongLongToLongLong
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrToLongLong(
    _In_ ULONG_PTR ulOperand,
    _Out_ _Deref_out_range_(==, ulOperand) LONGLONG* pllResult)
{
    *pllResult = (LONGLONG)ulOperand;
    return STATUS_SUCCESS;
}
#endif

#define RtlULongPtrToLong64    RtlULongPtrToLongLong

#define RtlULongPtrToInt64 RtlULongPtrToLongLong

#define RtlULongPtrToPtrdiffT  RtlULongPtrToIntPtr

#define RtlULongPtrToSSIZET    RtlULongPtrToLongPtr

#define RtlDWordToInt8 RtlULongToInt8

#define RtlDWordToChar RtlULongToChar

#define RtlDWordToUChar    RtlULongToUChar

#define RtlDWordToUInt8    RtlULongToUInt8

#define RtlDWordToByte RtlULongToUInt8

#define RtlDWordToShort    RtlULongToShort

#define RtlDWordToInt16    RtlULongToShort

#define RtlDWordToUShort   RtlULongToUShort

#define RtlDWordToUInt16   RtlULongToUShort

#define RtlDWordToWord RtlULongToUShort

#define RtlDWordToInt  RtlULongToInt

#define RtlDWordToInt32    RtlULongToInt

#define RtlDWordToIntPtr   RtlULongToIntPtr

#define RtlDWordToUInt RtlULongToUInt

#define RtlDWordToUInt32   RtlULongToUInt

#define RtlDWordToUIntPtr  RtlULongToUIntPtr

#define RtlDWordToLong RtlULongToLong

#define RtlDWordToLongPtr  RtlULongToLongPtr

#define RtlDWordToPtrdiffT RtlULongToIntPtr

#define RtlDWordToSSIZET   RtlULongToLongPtr

#define RtlDWordPtrToInt8  RtlULongPtrToInt8

#define RtlDWordPtrToUChar RtlULongPtrToUChar

#define RtlDWordPtrToChar  RtlULongPtrToChar

#define RtlDWordPtrToUInt8 RtlULongPtrToUInt8

#define RtlDWordPtrToByte  RtlULongPtrToUInt8

#define RtlDWordPtrToShort RtlULongPtrToShort

#define RtlDWordPtrToInt16 RtlULongPtrToShort

#define RtlDWordPtrToUShort    RtlULongPtrToUShort

#define RtlDWordPtrToUInt16    RtlULongPtrToUShort

#define RtlDWordPtrToWord  RtlULongPtrToUShort

#define RtlDWordPtrToInt   RtlULongPtrToInt

#define RtlDWordPtrToInt32 RtlULongPtrToInt

#define RtlDWordPtrToIntPtr    RtlULongPtrToIntPtr

#define RtlDWordPtrToUInt  RtlULongPtrToUInt

#define RtlDWordPtrToUInt32    RtlULongPtrToUInt

#define RtlDWordPtrToUIntPtr   RtlULongPtrToUIntPtr

#define RtlDWordPtrToLong  RtlULongPtrToLong

#define RtlDWordPtrToLongPtr   RtlULongPtrToLongPtr

#define RtlDWordPtrToULong RtlULongPtrToULong

#define RtlDWordPtrToDWord RtlULongPtrToULong

#define RtlDWordPtrToLongLong  RtlULongPtrToLongLong

#define RtlDWordPtrToLong64    RtlULongPtrToLongLong

#define RtlDWordPtrToInt64 RtlULongPtrToLongLong

#define RtlDWordPtrToPtrdiffT  RtlULongPtrToIntPtr

#define RtlDWordPtrToSSIZET    RtlULongPtrToLongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToInt8(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if ((llOperand >= INT8_MIN) && (llOperand <= INT8_MAX))
    {
        *pi8Result = (INT8)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToUChar(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) UCHAR* pch)
{
    NTSTATUS status;

    if ((llOperand >= 0) && (llOperand <= 255))
    {
        *pch = (UCHAR)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlLongLongToChar(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlLongLongToUChar(llOperand, (UCHAR*)pch);
#else
    return RtlLongLongToInt8(llOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToUInt8(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) UINT8* pu8Result)
{
    NTSTATUS status;

    if ((llOperand >= 0) && (llOperand <= UINT8_MAX))
    {
        *pu8Result = (UINT8)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToByte  RtlLongLongToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToShort(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) SHORT* psResult)
{
    NTSTATUS status;

    if ((llOperand >= SHORT_MIN) && (llOperand <= SHORT_MAX))
    {
        *psResult = (SHORT)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToInt16 RtlLongLongToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToUShort(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if ((llOperand >= 0) && (llOperand <= USHORT_MAX))
    {
        *pusResult = (USHORT)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToUInt16    RtlLongLongToUShort

#define RtlLongLongToWord  RtlLongLongToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToInt(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) INT* piResult)
{
    NTSTATUS status;

    if ((llOperand >= INT_MIN) && (llOperand <= INT_MAX))
    {
        *piResult = (INT)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToInt32 RtlLongLongToInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToIntPtr(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) INT_PTR* piResult)
{
    *piResult = llOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlLongLongToIntPtr   RtlLongLongToInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToUInt(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) UINT* puResult)
{
    NTSTATUS status;

    if ((llOperand >= 0) && (llOperand <= UINT_MAX))
    {
        *puResult = (UINT)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToUInt32    RtlLongLongToUInt

#ifdef _WIN64
#define RtlLongLongToUIntPtr  RtlLongLongToULongLong
#else
#define RtlLongLongToUIntPtr  RtlLongLongToUInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToLong(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) LONG* plResult)
{
    NTSTATUS status;

    if ((llOperand >= LONG_MIN) && (llOperand <= LONG_MAX))
    {
        *plResult = (LONG)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToLongPtr(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) LONG_PTR* plResult)
{
    *plResult = (LONG_PTR)llOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlLongLongToLongPtr  RtlLongLongToLong
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToULong(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if ((llOperand >= 0) && (llOperand <= ULONG_MAX))
    {
        *pulResult = (ULONG)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlLongLongToULongPtr RtlLongLongToULongLong
#else
#define RtlLongLongToULongPtr RtlLongLongToULong
#endif

#define RtlLongLongToDWord    RtlLongLongToULong

#define RtlLongLongToDWordPtr RtlLongLongToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongToULongLong(
    _In_ LONGLONG llOperand,
    _Out_ _Deref_out_range_(==, llOperand) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (llOperand >= 0)
    {
        *pullResult = (ULONGLONG)llOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlLongLongToDWordLong RtlLongLongToULongLong

#define RtlLongLongToULong64   RtlLongLongToULongLong

#define RtlLongLongToDWord64   RtlLongLongToULongLong

#define RtlLongLongToUInt64    RtlLongLongToULongLong

#define RtlLongLongToPtrdiffT RtlLongLongToIntPtr

#define RtlLongLongToSizeT    RtlLongLongToUIntPtr

#define RtlLongLongToSSIZET   RtlLongLongToLongPtr

#define RtlLongLongToSIZET    RtlLongLongToULongPtr

#define RtlLong64ToChar    RtlLongLongToChar

#define RtlLong64ToInt8    RtlLongLongToInt8

#define RtlLong64ToUChar   RtlLongLongToUChar

#define RtlLong64ToUInt8   RtlLongLongToUInt8

#define RtlLong64ToByte    RtlLongLongToUInt8

#define RtlLong64ToShort   RtlLongLongToShort

#define RtlLong64ToInt16   RtlLongLongToShort

#define RtlLong64ToUShort  RtlLongLongToUShort

#define RtlLong64ToUInt16  RtlLongLongToUShort

#define RtlLong64ToWord    RtlLongLongToUShort

#define RtlLong64ToInt RtlLongLongToInt

#define RtlLong64ToInt32   RtlLongLongToInt

#define RtlLong64ToIntPtr  RtlLongLongToIntPtr

#define RtlLong64ToUInt    RtlLongLongToUInt

#define RtlLong64ToUInt32  RtlLongLongToUInt

#define RtlLong64ToUIntPtr RtlLongLongToUIntPtr

#define RtlLong64ToLong    RtlLongLongToLong

#define RtlLong64ToLongPtr RtlLongLongToLongPtr

#define RtlLong64ToULong   RtlLongLongToULong

#define RtlLong64ToULongPtr    RtlLongLongToULongPtr

#define RtlLong64ToDWord   RtlLongLongToULong

#define RtlLong64ToDWordPtr    RtlLongLongToULongPtr

#define RtlLong64ToULongLong   RtlLongLongToULongLong

#define RtlLong64ToPtrdiffT    RtlLongLongToIntPtr

#define RtlLong64ToSizeT   RtlLongLongToUIntPtr

#define RtlLong64ToSSIZET  RtlLongLongToLongPtr

#define RtlLong64ToSIZET   RtlLongLongToULongPtr

#define RtlInt64ToChar RtlLongLongToChar

#define RtlInt64ToInt8 RtlLongLongToInt8

#define RtlInt64ToUChar    RtlLongLongToUChar

#define RtlInt64ToUInt8    RtlLongLongToUInt8

#define RtlInt64ToByte RtlLongLongToUInt8

#define RtlInt64ToShort    RtlLongLongToShort

#define RtlInt64ToInt16    RtlLongLongToShort

#define RtlInt64ToUShort   RtlLongLongToUShort

#define RtlInt64ToUInt16   RtlLongLongToUShort

#define RtlInt64ToWord RtlLongLongToUShort

#define RtlInt64ToInt  RtlLongLongToInt

#define RtlInt64ToInt32    RtlLongLongToInt

#define RtlInt64ToIntPtr   RtlLongLongToIntPtr

#define RtlInt64ToUInt RtlLongLongToUInt

#define RtlInt64ToUInt32   RtlLongLongToUInt

#define RtlInt64ToUIntPtr  RtlLongLongToUIntPtr

#define RtlInt64ToLong RtlLongLongToLong

#define RtlInt64ToLongPtr  RtlLongLongToLongPtr

#define RtlInt64ToULong    RtlLongLongToULong

#define RtlInt64ToULongPtr RtlLongLongToULongPtr

#define RtlInt64ToDWord    RtlLongLongToULong

#define RtlInt64ToDWordPtr RtlLongLongToULongPtr

#define RtlInt64ToULongLong    RtlLongLongToULongLong

#define RtlInt64ToDWordLong    RtlLongLongToULongLong

#define RtlInt64ToULong64  RtlLongLongToULongLong

#define RtlInt64ToDWord64  RtlLongLongToULongLong

#define RtlInt64ToUInt64   RtlLongLongToULongLong

#define RtlInt64ToPtrdiffT RtlLongLongToIntPtr

#define RtlInt64ToSizeT    RtlLongLongToUIntPtr

#define RtlInt64ToSSIZET   RtlLongLongToLongPtr

#define RtlInt64ToSIZET    RtlLongLongToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToInt8(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) INT8* pi8Result)
{
    NTSTATUS status;

    if (ullOperand <= INT8_MAX)
    {
        *pi8Result = (INT8)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pi8Result = INT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToUChar(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) UCHAR* pch)
{
    NTSTATUS status;

    if (ullOperand <= 255)
    {
        *pch = (UCHAR)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pch = '\0';
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

__forceinline
NTSTATUS
RtlULongLongToChar(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) CHAR* pch)
{
#ifdef _CHAR_UNSIGNED
    return RtlULongLongToUChar(ullOperand, (UCHAR*)pch);
#else
    return RtlULongLongToInt8(ullOperand, (INT8*)pch);
#endif
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToUInt8(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) UINT8* pu8Result)
{
    NTSTATUS status;

    if (ullOperand <= UINT8_MAX)
    {
        *pu8Result = (UINT8)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToByte RtlULongLongToUInt8

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToShort(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) SHORT* psResult)
{
    NTSTATUS status;

    if (ullOperand <= SHORT_MAX)
    {
        *psResult = (SHORT)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *psResult = SHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToInt16    RtlULongLongToShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToUShort(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) USHORT* pusResult)
{
    NTSTATUS status;

    if (ullOperand <= USHORT_MAX)
    {
        *pusResult = (USHORT)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToUInt16   RtlULongLongToUShort

#define RtlULongLongToWord RtlULongLongToUShort

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToInt(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) INT* piResult)
{
    NTSTATUS status;

    if (ullOperand <= INT_MAX)
    {
        *piResult = (INT)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *piResult = INT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToInt32    RtlULongLongToInt

#ifdef _WIN64
#define RtlULongLongToIntPtr   RtlULongLongToLongLong
#else
#define RtlULongLongToIntPtr   RtlULongLongToInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToUInt(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) UINT* puResult)
{
    NTSTATUS status;

    if (ullOperand <= UINT_MAX)
    {
        *puResult = (UINT)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToUInt32   RtlULongLongToUInt

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToUIntPtr(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) UINT_PTR* puResult)
{
    *puResult = ullOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlULongLongToUIntPtr  RtlULongLongToUInt
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToLong(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) LONG* plResult)
{
    NTSTATUS status;

    if (ullOperand <= LONG_MAX)
    {
        *plResult = (LONG)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToLongPtr(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) LONG_PTR* plResult)
{
    NTSTATUS status;

    if (ullOperand <= LONG_PTR_MAX)
    {
        *plResult = (LONG_PTR)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *plResult = LONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToULong(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) ULONG* pulResult)
{
    NTSTATUS status;

    if (ullOperand <= ULONG_MAX)
    {
        *pulResult = (ULONG)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToULongPtr(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) ULONG_PTR* pulResult)
{
    *pulResult = ullOperand;
    return STATUS_SUCCESS;
}
#else
#define RtlULongLongToULongPtr RtlULongLongToULong
#endif

#define RtlULongLongToDWord    RtlULongLongToULong

#define RtlULongLongToDWordPtr RtlULongLongToULongPtr

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongToLongLong(
    _In_ ULONGLONG ullOperand,
    _Out_ _Deref_out_range_(==, ullOperand) LONGLONG* pllResult)
{
    NTSTATUS status;

    if (ullOperand <= LONGLONG_MAX)
    {
        *pllResult = (LONGLONG)ullOperand;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pllResult = LONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlULongLongToInt64    RtlULongLongToLongLong

#define RtlULongLongToLong64   RtlULongLongToLongLong

#define RtlULongLongToPtrdiffT RtlULongLongToIntPtr

#define RtlULongLongToSizeT    RtlULongLongToUIntPtr

#define RtlULongLongToSSIZET   RtlULongLongToLongPtr

#define RtlULongLongToSIZET    RtlULongLongToULongPtr

#define RtlDWordLongToChar RtlULongLongToChar

#define RtlDWordLongToInt8 RtlULongLongToInt8

#define RtlDWordLongToUChar    RtlULongLongToUChar

#define RtlDWordLongToUInt8    RtlULongLongToUInt8

#define RtlDWordLongToByte RtlULongLongToUInt8

#define RtlDWordLongToShort    RtlULongLongToShort

#define RtlDWordLongToInt16    RtlULongLongToShort

#define RtlDWordLongToUShort   RtlULongLongToUShort

#define RtlDWordLongToUInt16   RtlULongLongToUShort

#define RtlDWordLongToWord RtlULongLongToUShort

#define RtlDWordLongToInt  RtlULongLongToInt

#define RtlDWordLongToInt32    RtlULongLongToInt

#define RtlDWordLongToIntPtr   RtlULongLongToIntPtr

#define RtlDWordLongToUInt RtlULongLongToUInt

#define RtlDWordLongToUInt32   RtlULongLongToUInt

#define RtlDWordLongToUIntPtr  RtlULongLongToUIntPtr

#define RtlDWordLongToLong RtlULongLongToLong

#define RtlDWordLongToLongPtr  RtlULongLongToLongPtr

#define RtlDWordLongToULong    RtlULongLongToULong

#define RtlDWordLongToULongPtr RtlULongLongToULongPtr

#define RtlDWordLongToDWord    RtlULongLongToULong

#define RtlDWordLongToDWordPtr RtlULongLongToULongPtr

#define RtlDWordLongToLongLong RtlULongLongToLongLong

#define RtlDWordLongToLong64   RtlULongLongToLongLong

#define RtlDWordLongToInt64    RtlULongLongToLongLong

#define RtlDWordLongToPtrdiffT RtlULongLongToIntPtr

#define RtlDWordLongToSizeT    RtlULongLongToUIntPtr

#define RtlDWordLongToSSIZET   RtlULongLongToLongPtr

#define RtlDWordLongToSIZET    RtlULongLongToULongPtr

#define RtlULong64ToChar   RtlULongLongToChar

#define RtlULong64ToInt8   RtlULongLongToInt8

#define RtlULong64ToUChar  RtlULongLongToUChar

#define RtlULong64ToUInt8  RtlULongLongToUInt8

#define RtlULong64ToByte   RtlULongLongToUInt8

#define RtlULong64ToShort  RtlULongLongToShort

#define RtlULong64ToInt16  RtlULongLongToShort

#define RtlULong64ToUShort RtlULongLongToUShort

#define RtlULong64ToUInt16 RtlULongLongToUShort

#define RtlULong64ToWord   RtlULongLongToUShort

#define RtlULong64ToInt    RtlULongLongToInt

#define RtlULong64ToInt32  RtlULongLongToInt

#define RtlULong64ToIntPtr RtlULongLongToIntPtr

#define RtlULong64ToUInt   RtlULongLongToUInt

#define RtlULong64ToUInt32 RtlULongLongToUInt

#define RtlULong64ToUIntPtr    RtlULongLongToUIntPtr

#define RtlULong64ToLong   RtlULongLongToLong

#define RtlULong64ToLongPtr    RtlULongLongToLongPtr

#define RtlULong64ToULong  RtlULongLongToULong

#define RtlULong64ToULongPtr   RtlULongLongToULongPtr

#define RtlULong64ToDWord  RtlULongLongToULong

#define RtlULong64ToDWordPtr   RtlULongLongToULongPtr

#define RtlULong64ToLongLong   RtlULongLongToLongLong

#define RtlULong64ToLong64 RtlULongLongToLongLong

#define RtlULong64ToInt64  RtlULongLongToLongLong

#define RtlULong64ToPtrdiffT   RtlULongLongToIntPtr

#define RtlULong64ToSizeT  RtlULongLongToUIntPtr

#define RtlULong64ToSSIZET RtlULongLongToLongPtr

#define RtlULong64ToSIZET  RtlULongLongToULongPtr

#define RtlDWord64ToChar   RtlULongLongToChar

#define RtlDWord64ToInt8   RtlULongLongToInt8

#define RtlDWord64ToUChar  RtlULongLongToUChar

#define RtlDWord64ToUInt8  RtlULongLongToUInt8

#define RtlDWord64ToByte   RtlULongLongToUInt8

#define RtlDWord64ToShort  RtlULongLongToShort

#define RtlDWord64ToInt16  RtlULongLongToShort

#define RtlDWord64ToUShort RtlULongLongToUShort

#define RtlDWord64ToUInt16 RtlULongLongToUShort

#define RtlDWord64ToWord   RtlULongLongToUShort

#define RtlDWord64ToInt    RtlULongLongToInt

#define RtlDWord64ToInt32  RtlULongLongToInt

#define RtlDWord64ToIntPtr RtlULongLongToIntPtr

#define RtlDWord64ToUInt   RtlULongLongToUInt

#define RtlDWord64ToUInt32 RtlULongLongToUInt

#define RtlDWord64ToUIntPtr    RtlULongLongToUIntPtr

#define RtlDWord64ToLong   RtlULongLongToLong

#define RtlDWord64ToLongPtr    RtlULongLongToLongPtr

#define RtlDWord64ToULong  RtlULongLongToULong

#define RtlDWord64ToULongPtr   RtlULongLongToULongPtr

#define RtlDWord64ToDWord  RtlULongLongToULong

#define RtlDWord64ToDWordPtr   RtlULongLongToULongPtr

#define RtlDWord64ToLongLong   RtlULongLongToLongLong

#define RtlDWord64ToLong64 RtlULongLongToLongLong

#define RtlDWord64ToInt64  RtlULongLongToLongLong

#define RtlDWord64ToPtrdiffT   RtlULongLongToIntPtr

#define RtlDWord64ToSizeT  RtlULongLongToUIntPtr

#define RtlDWord64ToSSIZET RtlULongLongToLongPtr

#define RtlDWord64ToSIZET  RtlULongLongToULongPtr

#define RtlUInt64ToChar    RtlULongLongToChar

#define RtlUInt64ToInt8    RtlULongLongToInt8

#define RtlUInt64ToUChar   RtlULongLongToUChar

#define RtlUInt64ToUInt8   RtlULongLongToUInt8

#define RtlUInt64ToByte    RtlULongLongToUInt8

#define RtlUInt64ToShort   RtlULongLongToShort

#define RtlUInt64ToInt16   RtlULongLongToShort

#define RtlUInt64ToUShort  RtlULongLongToUShort

#define RtlUInt64ToUInt16  RtlULongLongToUShort

#define RtlUInt64ToWord    RtlULongLongToUShort

#define RtlUInt64ToInt RtlULongLongToInt

#define RtlUInt64ToInt32   RtlULongLongToInt

#define RtlUInt64ToIntPtr  RtlULongLongToIntPtr

#define RtlUInt64ToUInt    RtlULongLongToUInt

#define RtlUInt64ToUInt32  RtlULongLongToUInt

#define RtlUInt64ToUIntPtr RtlULongLongToUIntPtr

#define RtlUInt64ToLong    RtlULongLongToLong

#define RtlUInt64ToLongPtr RtlULongLongToLongPtr

#define RtlUInt64ToULong   RtlULongLongToULong

#define RtlUInt64ToULongPtr    RtlULongLongToULongPtr

#define RtlUInt64ToDWord   RtlULongLongToULong

#define RtlUInt64ToDWordPtr    RtlULongLongToULongPtr

#define RtlUInt64ToLongLong    RtlULongLongToLongLong

#define RtlUInt64ToLong64  RtlULongLongToLongLong

#define RtlUInt64ToInt64   RtlULongLongToLongLong

#define RtlUInt64ToPtrdiffT    RtlULongLongToIntPtr

#define RtlUInt64ToSizeT   RtlULongLongToUIntPtr

#define RtlUInt64ToSSIZET  RtlULongLongToLongPtr

#define RtlUInt64ToSIZET  RtlULongLongToULongPtr

#define RtlPtrdiffTToChar  RtlIntPtrToChar

#define RtlPtrdiffTToInt8  RtlIntPtrToInt8

#define RtlPtrdiffTToUChar RtlIntPtrToUChar

#define RtlPtrdiffTToUInt8 RtlIntPtrToUInt8

#define RtlPtrdiffTToByte  RtlIntPtrToUInt8

#define RtlPtrdiffTToShort RtlIntPtrToShort

#define RtlPtrdiffTToInt16 RtlIntPtrToShort

#define RtlPtrdiffTToUShort    RtlIntPtrToUShort

#define RtlPtrdiffTToUInt16    RtlIntPtrToUShort

#define RtlPtrdiffTToWord  RtlIntPtrToUShort

#define RtlPtrdiffTToInt   RtlIntPtrToInt

#define RtlPtrdiffTToInt32 RtlIntPtrToInt

#define RtlPtrdiffTToUInt  RtlIntPtrToUInt

#define RtlPtrdiffTToUInt32    RtlIntPtrToUInt

#define RtlPtrdiffTToUIntPtr   RtlIntPtrToUIntPtr

#define RtlPtrdiffTToLong  RtlIntPtrToLong

#define RtlPtrdiffTToLongPtr   RtlIntPtrToLongPtr

#define RtlPtrdiffTToULong RtlIntPtrToULong

#define RtlPtrdiffTToULongPtr  RtlIntPtrToULongPtr

#define RtlPtrdiffTToDWord RtlIntPtrToULong

#define RtlPtrdiffTToDWordPtr  RtlIntPtrToULongPtr

#define RtlPtrdiffTToULongLong RtlIntPtrToULongLong

#define RtlPtrdiffTToDWordLong RtlIntPtrToULongLong

#define RtlPtrdiffTToULong64   RtlIntPtrToULongLong

#define RtlPtrdiffTToDWord64   RtlIntPtrToULongLong

#define RtlPtrdiffTToUInt64    RtlIntPtrToULongLong

#define RtlPtrdiffTToSizeT RtlIntPtrToUIntPtr

#define RtlPtrdiffTToSIZET RtlIntPtrToULongPtr

#define RtlSizeTToInt8 RtlUIntPtrToInt8

#define RtlSizeTToUChar    RtlUIntPtrToUChar

#define RtlSizeTToChar RtlUIntPtrToChar

#define RtlSizeTToUInt8    RtlUIntPtrToUInt8

#define RtlSizeTToByte RtlUIntPtrToUInt8

#define RtlSizeTToShort    RtlUIntPtrToShort

#define RtlSizeTToInt16    RtlUIntPtrToShort

#define RtlSizeTToUShort   RtlUIntPtrToUShort

#define RtlSizeTToUInt16   RtlUIntPtrToUShort

#define RtlSizeTToWord RtlUIntPtrToUShort

#define RtlSizeTToInt  RtlUIntPtrToInt

#define RtlSizeTToInt32    RtlUIntPtrToInt

#define RtlSizeTToIntPtr   RtlUIntPtrToIntPtr

#define RtlSizeTToUInt RtlUIntPtrToUInt

#define RtlSizeTToUInt32   RtlUIntPtrToUInt

#define RtlSizeTToLong RtlUIntPtrToLong

#define RtlSizeTToLongPtr  RtlUIntPtrToLongPtr

#define RtlSizeTToULong    RtlUIntPtrToULong

#define RtlSizeTToDWord    RtlUIntPtrToULong

#define RtlSizeTToLongLong RtlUIntPtrToLongLong

#define RtlSizeTToLong64   RtlUIntPtrToLongLong

#define RtlSizeTToInt64    RtlUIntPtrToLongLong

#define RtlSizeTToPtrdiffT RtlUIntPtrToIntPtr

#define RtlSizeTToSSIZET   RtlUIntPtrToLongPtr

#define RtlSSIZETToInt8    RtlLongPtrToInt8

#define RtlSSIZETToUChar   RtlLongPtrToUChar

#define RtlSSIZETToChar    RtlLongPtrToChar

#define RtlSSIZETToUInt8   RtlLongPtrToUInt8

#define RtlSSIZETToByte    RtlLongPtrToUInt8

#define RtlSSIZETToShort   RtlLongPtrToShort

#define RtlSSIZETToInt16   RtlLongPtrToShort

#define RtlSSIZETToUShort  RtlLongPtrToUShort

#define RtlSSIZETToUInt16  RtlLongPtrToUShort

#define RtlSSIZETToWord    RtlLongPtrToUShort

#define RtlSSIZETToInt RtlLongPtrToInt

#define RtlSSIZETToInt32   RtlLongPtrToInt

#define RtlSSIZETToIntPtr  RtlLongPtrToIntPtr

#define RtlSSIZETToUInt    RtlLongPtrToUInt

#define RtlSSIZETToUInt32  RtlLongPtrToUInt

#define RtlSSIZETToUIntPtr RtlLongPtrToUIntPtr

#define RtlSSIZETToLong    RtlLongPtrToLong

#define RtlSSIZETToULong   RtlLongPtrToULong

#define RtlSSIZETToULongPtr    RtlLongPtrToULongPtr

#define RtlSSIZETToDWord   RtlLongPtrToULong

#define RtlSSIZETToDWordPtr    RtlLongPtrToULongPtr

#define RtlSSIZETToULongLong   RtlLongPtrToULongLong

#define RtlSSIZETToDWordLong   RtlLongPtrToULongLong

#define RtlSSIZETToULong64 RtlLongPtrToULongLong

#define RtlSSIZETToDWord64 RtlLongPtrToULongLong

#define RtlSSIZETToUInt64  RtlLongPtrToULongLong

#define RtlSSIZETToSizeT   RtlLongPtrToUIntPtr

#define RtlSSIZETToSIZET   RtlLongPtrToULongPtr

#define RtlSIZETToInt8 RtlULongPtrToInt8

#define RtlSIZETToUChar    RtlULongPtrToUChar

#define RtlSIZETToChar RtlULongPtrToChar

#define RtlSIZETToUInt8    RtlULongPtrToUInt8

#define RtlSIZETToByte RtlULongPtrToUInt8

#define RtlSIZETToShort    RtlULongPtrToShort

#define RtlSIZETToInt16    RtlULongPtrToShort

#define RtlSIZETToUShort   RtlULongPtrToUShort

#define RtlSIZETToUInt16   RtlULongPtrToUShort

#define RtlSIZETToWord RtlULongPtrToUShort

#define RtlSIZETToInt  RtlULongPtrToInt

#define RtlSIZETToInt32    RtlULongPtrToInt

#define RtlSIZETToIntPtr   RtlULongPtrToIntPtr

#define RtlSIZETToUInt RtlULongPtrToUInt

#define RtlSIZETToUInt32   RtlULongPtrToUInt

#define RtlSIZETToUIntPtr  RtlULongPtrToUIntPtr

#define RtlSIZETToLong RtlULongPtrToLong

#define RtlSIZETToLongPtr  RtlULongPtrToLongPtr

#define RtlSIZETToULong    RtlULongPtrToULong

#define RtlSIZETToDWord    RtlULongPtrToULong

#define RtlSIZETToLongLong RtlULongPtrToLongLong

#define RtlSIZETToLong64   RtlULongPtrToLongLong

#define RtlSIZETToInt64    RtlULongPtrToLongLong

#define RtlSIZETToPtrdiffT RtlULongPtrToIntPtr

#define RtlSIZETToSSIZET   RtlULongPtrToLongPtr



_Must_inspect_result_
__inline
NTSTATUS
RtlUInt8Add(
    _In_ UINT8 u8Augend,
    _In_ UINT8 u8Addend,
    _Out_ _Deref_out_range_(==, u8Augend + u8Addend) UINT8* pu8Result)
{
    NTSTATUS status;

    if (((UINT8)(u8Augend + u8Addend)) >= u8Augend)
    {
        *pu8Result = (UINT8)(u8Augend + u8Addend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortAdd(
    _In_ USHORT usAugend,
    _In_ USHORT usAddend,
    _Out_ _Deref_out_range_(==, usAugend + usAddend) USHORT* pusResult)
{
    NTSTATUS status;

    if (((USHORT)(usAugend + usAddend)) >= usAugend)
    {
        *pusResult = (USHORT)(usAugend + usAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUInt16Add   RtlUShortAdd

#define RtlWordAdd     RtlUShortAdd

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntAdd(
    _In_ UINT uAugend,
    _In_ UINT uAddend,
    _Out_ _Deref_out_range_(==, uAugend + uAddend) UINT* puResult)
{
    NTSTATUS status;

    if ((uAugend + uAddend) >= uAugend)
    {
        *puResult = (uAugend + uAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUInt32Add   RtlUIntAdd

#ifdef _WIN64
#define RtlUIntPtrAdd      RtlULongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrAdd(
    _In_ UINT_PTR uAugend,
    _In_ UINT_PTR uAddend,
    _Out_ _Deref_out_range_(==, uAugend + uAddend) UINT_PTR* puResult)
{
    NTSTATUS status;

    if ((uAugend + uAddend) >= uAugend)
    {
        *puResult = (uAugend + uAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongAdd(
    _In_ ULONG ulAugend,
    _In_ ULONG ulAddend,
    _Out_ _Deref_out_range_(==, ulAugend + ulAddend) ULONG* pulResult)
{
    NTSTATUS status;

    if ((ulAugend + ulAddend) >= ulAugend)
    {
        *pulResult = (ulAugend + ulAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlULongPtrAdd     RtlULongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrAdd(
    _In_ ULONG_PTR ulAugend,
    _In_ ULONG_PTR ulAddend,
    _Out_ _Deref_out_range_(==, ulAugend + ulAddend) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if ((ulAugend + ulAddend) >= ulAugend)
    {
        *pulResult = (ulAugend + ulAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

#define RtlDWordAdd        RtlULongAdd

#ifdef _WIN64
#define RtlDWordPtrAdd     RtlULongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlDWordPtrAdd(
    _In_ DWORD_PTR dwAugend,
    _In_ DWORD_PTR dwAddend,
    _Out_ _Deref_out_range_(==, dwAugend + dwAddend) DWORD_PTR* pdwResult)
{
    NTSTATUS status;

    if ((dwAugend + dwAddend) >= dwAugend)
    {
        *pdwResult = (dwAugend + dwAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pdwResult = DWORD_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlSizeTAdd(
    _In_ size_t Augend,
    _In_ size_t Addend,
    _Out_ _Deref_out_range_(==, Augend + Addend) size_t* pResult)
{
    NTSTATUS status;

    if ((Augend + Addend) >= Augend)
    {
        *pResult = (Augend + Addend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pResult = SIZE_T_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlSIZETAdd      RtlULongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSIZETAdd(
    _In_ SIZE_T Augend,
    _In_ SIZE_T Addend,
    _Out_ _Deref_out_range_(==, Augend + Addend) SIZE_T* pResult)
{
    NTSTATUS status;

    if ((Augend + Addend) >= Augend)
    {
        *pResult = (Augend + Addend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pResult = _SIZE_T_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongAdd(
    _In_ ULONGLONG ullAugend,
    _In_ ULONGLONG ullAddend,
    _Out_ _Deref_out_range_(==, ullAugend + ullAddend) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if ((ullAugend + ullAddend) >= ullAugend)
    {
        *pullResult = (ullAugend + ullAddend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlDWordLongAdd    RtlULongLongAdd

#define RtlULong64Add  RtlULongLongAdd

#define RtlDWord64Add  RtlULongLongAdd

#define RtlUInt64Add   RtlULongLongAdd



_Must_inspect_result_
__inline
NTSTATUS
RtlUInt8Sub(
    _In_ UINT8 u8Minuend,
    _In_ UINT8 u8Subtrahend,
    _Out_ _Deref_out_range_(==, u8Minuend - u8Subtrahend) UINT8* pu8Result)
{
    NTSTATUS status;

    if (u8Minuend >= u8Subtrahend)
    {
        *pu8Result = (UINT8)(u8Minuend - u8Subtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pu8Result = UINT8_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortSub(
    _In_ USHORT usMinuend,
    _In_ USHORT usSubtrahend,
    _Out_ _Deref_out_range_(==, usMinuend - usSubtrahend) USHORT* pusResult)
{
    NTSTATUS status;

    if (usMinuend >= usSubtrahend)
    {
        *pusResult = (USHORT)(usMinuend - usSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pusResult = USHORT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUInt16Sub  RtlUShortSub

#define RtlWordSub    RtlUShortSub


_Must_inspect_result_
__inline
NTSTATUS
RtlUIntSub(
    _In_ UINT uMinuend,
    _In_ UINT uSubtrahend,
    _Out_ _Deref_out_range_(==, uMinuend - uSubtrahend) UINT* puResult)
{
    NTSTATUS status;

    if (uMinuend >= uSubtrahend)
    {
        *puResult = (uMinuend - uSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlUInt32Sub  RtlUIntSub

#ifdef _WIN64
#define RtlUIntPtrSub RtlULongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrSub(
    _In_ UINT_PTR uMinuend,
    _In_ UINT_PTR uSubtrahend,
    _Out_ _Deref_out_range_(==, uMinuend - uSubtrahend) UINT_PTR* puResult)
{
    NTSTATUS status;

    if (uMinuend >= uSubtrahend)
    {
        *puResult = (uMinuend - uSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *puResult = UINT_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongSub(
    _In_ ULONG ulMinuend,
    _In_ ULONG ulSubtrahend,
    _Out_ _Deref_out_range_(==, ulMinuend - ulSubtrahend) ULONG* pulResult)
{
    NTSTATUS status;

    if (ulMinuend >= ulSubtrahend)
    {
        *pulResult = (ulMinuend - ulSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlULongPtrSub RtlULongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrSub(
    _In_ ULONG_PTR ulMinuend,
    _In_ ULONG_PTR ulSubtrahend,
    _Out_ _Deref_out_range_(==, ulMinuend - ulSubtrahend) ULONG_PTR* pulResult)
{
    NTSTATUS status;

    if (ulMinuend >= ulSubtrahend)
    {
        *pulResult = (ulMinuend - ulSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pulResult = ULONG_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif


#define RtlDWordSub       RtlULongSub

#ifdef _WIN64
#define RtlDWordPtrSub    RtlULongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlDWordPtrSub(
    _In_ DWORD_PTR dwMinuend,
    _In_ DWORD_PTR dwSubtrahend,
    _Out_ _Deref_out_range_(==, dwMinuend - dwSubtrahend) DWORD_PTR* pdwResult)
{
    NTSTATUS status;

    if (dwMinuend >= dwSubtrahend)
    {
        *pdwResult = (dwMinuend - dwSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pdwResult = DWORD_PTR_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlSizeTSub(
    _In_ size_t Minuend,
    _In_ size_t Subtrahend,
    _Out_ _Deref_out_range_(==, Minuend - Subtrahend) size_t* pResult)
{
    NTSTATUS status;

    if (Minuend >= Subtrahend)
    {
        *pResult = (Minuend - Subtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pResult = SIZE_T_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#ifdef _WIN64
#define RtlSIZETSub   RtlULongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSIZETSub(
    _In_ SIZE_T Minuend,
    _In_ SIZE_T Subtrahend,
    _Out_ _Deref_out_range_(==, Minuend - Subtrahend) SIZE_T* pResult)
{
    NTSTATUS status;

    if (Minuend >= Subtrahend)
    {
        *pResult = (Minuend - Subtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pResult = _SIZE_T_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongSub(
    _In_ ULONGLONG ullMinuend,
    _In_ ULONGLONG ullSubtrahend,
    _Out_ _Deref_out_range_(==, ullMinuend - ullSubtrahend) ULONGLONG* pullResult)
{
    NTSTATUS status;

    if (ullMinuend >= ullSubtrahend)
    {
        *pullResult = (ullMinuend - ullSubtrahend);
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }

    return status;
}

#define RtlDWordLongSub   RtlULongLongSub

#define RtlULong64Sub RtlULongLongSub

#define RtlDWord64Sub RtlULongLongSub

#define RtlUInt64Sub  RtlULongLongSub



_Must_inspect_result_
__inline
NTSTATUS
RtlUInt8Mult(
    _In_ UINT8 u8Multiplicand,
    _In_ UINT8 u8Multiplier,
    _Out_ _Deref_out_range_(==, u8Multiplicand * u8Multiplier) UINT8* pu8Result)
{
    UINT uResult = ((UINT)u8Multiplicand) * ((UINT)u8Multiplier);

    return RtlUIntToUInt8(uResult, pu8Result);
}

_Must_inspect_result_
__inline
NTSTATUS
RtlUShortMult(
    _In_ USHORT usMultiplicand,
    _In_ USHORT usMultiplier,
    _Out_ _Deref_out_range_(==, usMultiplicand * usMultiplier) USHORT* pusResult)
{
    ULONG ulResult = ((ULONG)usMultiplicand) * ((ULONG)usMultiplier);

    return RtlULongToUShort(ulResult, pusResult);
}

#define RtlUInt16Mult  RtlUShortMult

#define RtlWordMult    RtlUShortMult

_Must_inspect_result_
__inline
NTSTATUS
RtlUIntMult(
    _In_ UINT uMultiplicand,
    _In_ UINT uMultiplier,
    _Out_ _Deref_out_range_(==, uMultiplicand * uMultiplier) UINT* puResult)
{
    ULONGLONG ull64Result = UInt32x32To64(uMultiplicand, uMultiplier);

    return RtlULongLongToUInt(ull64Result, puResult);
}

#define RtlUInt32Mult  RtlUIntMult

#ifdef _WIN64
#define RtlUIntPtrMult     RtlULongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlUIntPtrMult(
    _In_ UINT_PTR uMultiplicand,
    _In_ UINT_PTR uMultiplier,
    _Out_ _Deref_out_range_(==, uMultiplicand * uMultiplier) UINT_PTR* puResult)
{
    ULONGLONG ull64Result = UInt32x32To64(uMultiplicand, uMultiplier);

    return RtlULongLongToUIntPtr(ull64Result, puResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongMult(
    _In_ ULONG ulMultiplicand,
    _In_ ULONG ulMultiplier,
    _Out_ _Deref_out_range_(==, ulMultiplicand * ulMultiplier) ULONG* pulResult)
{
    ULONGLONG ull64Result = UInt32x32To64(ulMultiplicand, ulMultiplier);

    return RtlULongLongToULong(ull64Result, pulResult);
}

#ifdef _WIN64
#define RtlULongPtrMult    RtlULongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlULongPtrMult(
    _In_ ULONG_PTR ulMultiplicand,
    _In_ ULONG_PTR ulMultiplier,
    _Out_ _Deref_out_range_(==, ulMultiplicand * ulMultiplier) ULONG_PTR* pulResult)
{
    ULONGLONG ull64Result = UInt32x32To64(ulMultiplicand, ulMultiplier);

    return RtlULongLongToULongPtr(ull64Result, pulResult);
}
#endif

#define RtlDWordMult       RtlULongMult

#ifdef _WIN64
#define RtlDWordPtrMult    RtlULongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlDWordPtrMult(
    _In_ DWORD_PTR dwMultiplicand,
    _In_ DWORD_PTR dwMultiplier,
    _Out_ _Deref_out_range_(==, dwMultiplicand * dwMultiplier) DWORD_PTR* pdwResult)
{
    ULONGLONG ull64Result = UInt32x32To64(dwMultiplicand, dwMultiplier);

    return RtlULongLongToDWordPtr(ull64Result, pdwResult);
}
#endif


#ifdef _WIN64
#define RtlSizeTMult       RtlULongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSizeTMult(
    _In_ size_t Multiplicand,
    _In_ size_t Multiplier,
    _Out_ _Deref_out_range_(==, Multiplicand * Multiplier) size_t* pResult)
{
    ULONGLONG ull64Result = UInt32x32To64(Multiplicand, Multiplier);

    return RtlULongLongToSizeT(ull64Result, pResult);
}
#endif

#ifdef _WIN64
#define RtlSIZETMult       RtlULongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSIZETMult(
    _In_ SIZE_T Multiplicand,
    _In_ SIZE_T Multiplier,
    _Out_ _Deref_out_range_(==, Multiplicand * Multiplier) SIZE_T* pResult)
{
    ULONGLONG ull64Result = UInt32x32To64(Multiplicand, Multiplier);

    return RtlULongLongToSIZET(ull64Result, pResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlULongLongMult(
    _In_ ULONGLONG ullMultiplicand,
    _In_ ULONGLONG ullMultiplier,
    _Out_ _Deref_out_range_(==, ullMultiplicand * ullMultiplier) ULONGLONG* pullResult)
{
    NTSTATUS status;
#if defined(_USE_INTRINSIC_MULTIPLY128)
    ULONGLONG ullResultHigh;
    ULONGLONG ullResultLow;

    ullResultLow = UnsignedMultiply128(ullMultiplicand, ullMultiplier, &ullResultHigh);
    if (ullResultHigh == 0)
    {
        _Analysis_assume_(ullMultiplicand * ullMultiplier == ullResultLow);
        *pullResult = ullResultLow;
        status = STATUS_SUCCESS;
    }
    else
    {
        *pullResult = ULONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }
#else

    ULONG dw_a;
    ULONG dw_b;
    ULONG dw_c;
    ULONG dw_d;
    ULONGLONG ad = 0;
    ULONGLONG bc = 0;
    ULONGLONG bd = 0;
    ULONGLONG ullResult = 0;

    status = STATUS_INTEGER_OVERFLOW;

    dw_a = (ULONG)(ullMultiplicand >> 32);
    dw_c = (ULONG)(ullMultiplier >> 32);

    if ((dw_a == 0) && (dw_c == 0))
    {
        dw_b = (DWORD)ullMultiplicand;
        dw_d = (DWORD)ullMultiplier;

        *pullResult = (((ULONGLONG)dw_b) * (ULONGLONG)dw_d);
        status = STATUS_SUCCESS;
    }
    else
    {
        if ((dw_a == 0) ||
            (dw_c == 0))
        {
            dw_d = (DWORD)ullMultiplier;

            ad = (((ULONGLONG)dw_a) * (ULONGLONG)dw_d);
            if ((ad & 0xffffffff00000000) == 0)
            {
                dw_b = (DWORD)ullMultiplicand;

                bc = (((ULONGLONG)dw_b) * (ULONGLONG)dw_c);
                if ((bc & 0xffffffff00000000) == 0)
                {
                    if (NT_SUCCESS(RtlULongLongAdd(bc << 32, ad << 32, &ullResult)))
                    {
                        bd = (((ULONGLONG)dw_b) * (ULONGLONG)dw_d);

                        if (NT_SUCCESS(RtlULongLongAdd(ullResult, bd, &ullResult)))
                        {
                            *pullResult = ullResult;
                            status = STATUS_SUCCESS;
                        }
                    }
                }
            }
        }
    }

    if (!NT_SUCCESS(status))
    {
        *pullResult = ULONGLONG_ERROR;
    }
#pragma warning(suppress:26071)
#endif
    return status;
}

#define RtlDWordLongMult   RtlULongLongMult

#define RtlULong64Mult RtlULongLongMult

#define RtlDWord64Mult RtlULongLongMult

#define RtlUInt64Mult  RtlULongLongMult


#if defined(ENABLE_INTSAFE_SIGNED_FUNCTIONS)

#if defined(_USE_INTRINSIC_MULTIPLY128)
#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_ARM64EC_)

#define Multiply128 _mul128

#else

#define _mul128 Multiply128

#endif

LONG64
Multiply128(
    _In_ LONG64 Multiplier,
    _In_ LONG64  Multiplicand,
    _Out_ LONG64 *HighProduct
    );

#if !defined(_ARM64EC_)

#pragma intrinsic(_mul128)

#endif

#ifdef __cplusplus
}
#endif
#endif



_Must_inspect_result_
__inline
NTSTATUS
RtlInt8Add(
    _In_ INT8 i8Augend,
    _In_ INT8 i8Addend,
    _Out_ _Deref_out_range_(==, i8Augend + i8Addend) INT8* pi8Result
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(INT8));
    return RtlLongToInt8(((LONG)i8Augend) + ((LONG)i8Addend), pi8Result);
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortAdd(
    _In_ SHORT sAugend,
    _In_ SHORT sAddend,
    _Out_ _Deref_out_range_(==, sAugend + sAddend) SHORT* psResult
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(SHORT));
    return RtlLongToShort(((LONG)sAugend) + ((LONG)sAddend), psResult);
}

#define RtlInt16Add    RtlShortAdd

_Must_inspect_result_
__inline
NTSTATUS
RtlIntAdd(
    _In_ INT iAugend,
    _In_ INT iAddend,
    _Out_ _Deref_out_range_(==, iAugend + iAddend) INT* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT));
    return RtlLongLongToInt(((LONGLONG)iAugend) + ((LONGLONG)iAddend), piResult);
}

#define RtlInt32Add    RtlIntAdd

#ifdef _WIN64
#define RtlIntPtrAdd   RtlLongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrAdd(
    _In_ INT_PTR iAugend,
    _In_ INT_PTR iAddend,
    _Out_ _Deref_out_range_(==, iAugend + iAddend) INT_PTR* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT_PTR));
    return RtlLongLongToIntPtr(((LONGLONG)iAugend) + ((LONGLONG)iAddend), piResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongAdd(
    _In_ LONG lAugend,
    _In_ LONG lAddend,
    _Out_ _Deref_out_range_(==, lAugend + lAddend) LONG* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG));
    return RtlLongLongToLong(((LONGLONG)lAugend) + ((LONGLONG)lAddend), plResult);
}

#define RtlLong32Add   RtlIntAdd

#ifdef _WIN64
#define RtlLongPtrAdd   RtlLongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrAdd(
    _In_ LONG_PTR lAugend,
    _In_ LONG_PTR lAddend,
    _Out_ _Deref_out_range_(==, lAugend + lAddend) LONG_PTR* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG_PTR));
    return RtlLongLongToLongPtr(((LONGLONG)lAugend) + ((LONGLONG)lAddend), plResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongAdd(
    _In_ LONGLONG llAugend,
    _In_ LONGLONG llAddend,
    _Out_ _Deref_out_range_(==, llAugend + llAddend) LONGLONG* pllResult
    )
{
    NTSTATUS status;
    LONGLONG llResult = llAugend + llAddend;

    if (((llAugend < 0) == (llAddend < 0))  &&
        ((llAugend < 0) != (llResult < 0)))
    {
        *pllResult = LONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }
    else
    {
        *pllResult = llResult;
        status = STATUS_SUCCESS;
    }

    return status;
}

#define RtlLong64Add   RtlLongLongAdd

#define RtlInt64Add    RtlLongLongAdd

#ifdef _WIN64
#define RtlPtrdiffTAdd RtlLongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlPtrdiffTAdd(
    _In_ ptrdiff_t Augend,
    _In_ ptrdiff_t Addend,
    _Out_ _Deref_out_range_(==, Augend + Addend) ptrdiff_t* pResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(ptrdiff_t));
    return RtlLongLongToPtrdiffT(((LONGLONG)Augend) + ((LONGLONG)Addend), pResult);
}
#endif

#ifdef _WIN64
#define RtlSSIZETAdd   RtlLongLongAdd
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSSIZETAdd(
    _In_ SSIZE_T Augend,
    _In_ SSIZE_T Addend,
    _Out_ _Deref_out_range_(==, Augend + Addend) SSIZE_T* pResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(SSIZE_T));
    return RtlLongLongToSSIZET(((LONGLONG)Augend) + ((LONGLONG)Addend), pResult);
}
#endif



_Must_inspect_result_
__inline
NTSTATUS
RtlInt8Sub(
    _In_ INT8 i8Minuend,
    _In_ INT8 i8Subtrahend,
    _Out_ _Deref_out_range_(==, i8Minuend - i8Subtrahend) INT8* pi8Result
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(INT8));
    return RtlLongToInt8(((LONG)i8Minuend) - ((LONG)i8Subtrahend), pi8Result);
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortSub(
    _In_ SHORT sMinuend,
    _In_ SHORT sSubtrahend,
    _Out_ _Deref_out_range_(==, sMinuend - sSubtrahend) SHORT* psResult
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(SHORT));
    return RtlLongToShort(((LONG)sMinuend) - ((LONG)sSubtrahend), psResult);
}

#define RtlInt16Sub   RtlShortSub

_Must_inspect_result_
__inline
NTSTATUS
RtlIntSub(
    _In_ INT iMinuend,
    _In_ INT iSubtrahend,
    _Out_ _Deref_out_range_(==, iMinuend - iSubtrahend) INT* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT));
    return RtlLongLongToInt(((LONGLONG)iMinuend) - ((LONGLONG)iSubtrahend), piResult);
}

#define RtlInt32Sub   RtlIntSub

#ifdef _WIN64
#define RtlIntPtrSub  RtlLongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrSub(
    _In_ INT_PTR iMinuend,
    _In_ INT_PTR iSubtrahend,
    _Out_ _Deref_out_range_(==, iMinuend - iSubtrahend) INT_PTR* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT_PTR));
    return RtlLongLongToIntPtr(((LONGLONG)iMinuend) - ((LONGLONG)iSubtrahend), piResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongSub(
    _In_ LONG lMinuend,
    _In_ LONG lSubtrahend,
    _Out_ _Deref_out_range_(==, lMinuend - lSubtrahend) LONG* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG));
    return RtlLongLongToLong(((LONGLONG)lMinuend) - ((LONGLONG)lSubtrahend), plResult);
}

#define RtlLong32Sub  RtlIntSub

#ifdef _WIN64
#define RtlLongPtrSub  RtlLongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrSub(
    _In_ LONG_PTR lMinuend,
    _In_ LONG_PTR lSubtrahend,
    _Out_ _Deref_out_range_(==, lMinuend - lSubtrahend) LONG_PTR* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG_PTR));
    return RtlLongLongToLongPtr(((LONGLONG)lMinuend) - ((LONGLONG)lSubtrahend), plResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongSub(
    _In_ LONGLONG llMinuend,
    _In_ LONGLONG llSubtrahend,
    _Out_ _Deref_out_range_(==, llMinuend - llSubtrahend) LONGLONG* pllResult
    )
{
    NTSTATUS status;
    LONGLONG llResult = llMinuend - llSubtrahend;

    if (((llMinuend < 0) != (llSubtrahend < 0)) &&
        ((llMinuend < 0) != (llResult < 0)))
    {
        *pllResult = LONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }
    else
    {
        *pllResult = llResult;
        status = STATUS_SUCCESS;
    }

    return status;
}

#define RtlLong64Sub  RtlLongLongSub

#define RtlInt64Sub   RtlLongLongSub

#ifdef _WIN64
#define RtlPtrdiffTSub RtlLongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlPtrdiffTSub(
    _In_ ptrdiff_t Minuend,
    _In_ ptrdiff_t Subtrahend,
    _Out_ _Deref_out_range_(==, Minuend - Subtrahend) ptrdiff_t* pResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(ptrdiff_t));
    return RtlLongLongToPtrdiffT(((LONGLONG)Minuend) - ((LONGLONG)Subtrahend), pResult);
}
#endif

#ifdef _WIN64
#define RtlSSIZETSub  RtlLongLongSub
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSSIZETSub(
    _In_ SSIZE_T Minuend,
    _In_ SSIZE_T Subtrahend,
    _Out_ _Deref_out_range_(==, Minuend - Subtrahend) SSIZE_T* pResult)
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(SSIZE_T));
    return RtlLongLongToSSIZET(((LONGLONG)Minuend) - ((LONGLONG)Subtrahend), pResult);
}
#endif



_Must_inspect_result_
__inline
NTSTATUS
RtlInt8Mult(
    _In_ INT8 i8Multiplicand,
    _In_ INT8 i8Multiplier,
    _Out_ _Deref_out_range_(==, i8Multiplicand * i8Multiplier) INT8* pi8Result
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(INT8));
    return RtlLongToInt8(((LONG)i8Multiplier) * ((LONG)i8Multiplicand), pi8Result);
}

_Must_inspect_result_
__inline
NTSTATUS
RtlShortMult(
    _In_ SHORT sMultiplicand,
    _In_ SHORT sMultiplier,
    _Out_ _Deref_out_range_(==, sMultiplicand * sMultiplier) SHORT* psResult
    )
{
    C_ASSERT(sizeof(LONG) > sizeof(SHORT));
    return RtlLongToShort(((LONG)sMultiplicand) * ((LONG)sMultiplier), psResult);
}

#define RtlInt16Mult   RtlShortMult

_Must_inspect_result_
__inline
NTSTATUS
RtlIntMult(
    _In_ INT iMultiplicand,
    _In_ INT iMultiplier,
    _Out_ _Deref_out_range_(==, iMultiplicand * iMultiplier) INT* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT));
    return RtlLongLongToInt(((LONGLONG)iMultiplicand) * ((LONGLONG)iMultiplier), piResult);
}

#define RtlInt32Mult   RtlIntMult

#ifdef _WIN64
#define RtlIntPtrMult   RtlLongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlIntPtrMult(
    _In_ INT_PTR iMultiplicand,
    _In_ INT_PTR iMultiplier,
    _Out_ _Deref_out_range_(==, iMultiplicand * iMultiplier) INT_PTR* piResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(INT_PTR));
    return RtlLongLongToIntPtr(((LONGLONG)iMultiplicand) * ((LONGLONG)iMultiplier), piResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongMult(
    _In_ LONG lMultiplicand,
    _In_ LONG lMultiplier,
    _Out_ _Deref_out_range_(==, lMultiplicand * lMultiplier) LONG* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG));
    return RtlLongLongToLong(((LONGLONG)lMultiplicand) * ((LONGLONG)lMultiplier), plResult);
}

#define RtlLong32Mult  RtlIntMult

#ifdef _WIN64
#define RtlLongPtrMult RtlLongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlLongPtrMult(
    _In_ LONG_PTR lMultiplicand,
    _In_ LONG_PTR lMultiplier,
    _Out_ _Deref_out_range_(==, lMultiplicand * lMultiplier) LONG_PTR* plResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(LONG_PTR));
    return RtlLongLongToLongPtr(((LONGLONG)lMultiplicand) * ((LONGLONG)lMultiplier), plResult);
}
#endif

_Must_inspect_result_
__inline
NTSTATUS
RtlLongLongMult(
    _In_ LONGLONG llMultiplicand,
    _In_ LONGLONG llMultiplier,
    _Out_ _Deref_out_range_(==, llMultiplicand * llMultiplier) LONGLONG* pllResult
    )
{
    NTSTATUS status;

#if defined(_USE_INTRINSIC_MULTIPLY128)
    LONGLONG llResultHigh;
    LONGLONG llResultLow;

    llResultLow = Multiply128(llMultiplicand, llMultiplier, &llResultHigh);

    if (((llResultLow < 0) && (llResultHigh != -1))    ||
        ((llResultLow >= 0) && (llResultHigh != 0)))
    {
        *pllResult = LONGLONG_ERROR;
        status = STATUS_INTEGER_OVERFLOW;
    }
    else
    {
        *pllResult = llResultLow;
        status = STATUS_SUCCESS;
    }
#else

    ULONGLONG ullMultiplicand;
    ULONGLONG ullMultiplier;
    ULONGLONG ullResult;
    const ULONGLONG LONGLONG_MIN_MAGNITUDE = ((((ULONGLONG) - (LONGLONG_MIN + 1))) + 1);

    if (llMultiplicand < 0)
    {
        ullMultiplicand = ((ULONGLONG)(- (llMultiplicand + 1))) + 1;
    }
    else
    {
        ullMultiplicand = (ULONGLONG)llMultiplicand;
    }

    if (llMultiplier < 0)
    {
        ullMultiplier = ((ULONGLONG)(- (llMultiplier + 1))) + 1;
    }
    else
    {
        ullMultiplier = (ULONGLONG)llMultiplier;
    }

    status = RtlULongLongMult(ullMultiplicand, ullMultiplier, &ullResult);
    if (NT_SUCCESS(status))
    {
        if ((llMultiplicand < 0) != (llMultiplier < 0))
        {
            if (ullResult > LONGLONG_MIN_MAGNITUDE)
            {
                *pllResult = LONGLONG_ERROR;
                status = STATUS_INTEGER_OVERFLOW;
            }
            else
            {
                *pllResult = - ((LONGLONG)ullResult);
            }
        }
        else
        {
            if (ullResult > LONGLONG_MAX)
            {
                *pllResult = LONGLONG_ERROR;
                status = STATUS_INTEGER_OVERFLOW;
            }
            else
            {
                *pllResult = (LONGLONG)ullResult;
            }
        }
    }
    else
    {
        *pllResult = LONGLONG_ERROR;
    }
#endif

    return status;
}

#define RtlLong64Mult  RtlLongLongMult

#define RtlInt64Mult   RtlLongLongMult

#ifdef _WIN64
#define RtlPtrdiffTMult    RtlLongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlPtrdiffTMult(
    _In_ ptrdiff_t Multiplicand,
    _In_ ptrdiff_t Multiplier,
    _Out_ _Deref_out_range_(==, Multiplicand * Multiplier) ptrdiff_t* pResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(ptrdiff_t));
    return RtlLongLongToPtrdiffT(((LONGLONG)Multiplicand) * ((LONGLONG)Multiplier), pResult);
}
#endif

#ifdef _WIN64
#define RtlSSIZETMult  RtlLongLongMult
#else
_Must_inspect_result_
__inline
NTSTATUS
RtlSSIZETMult(
    _In_ SSIZE_T Multiplicand,
    _In_ SSIZE_T Multiplier,
    _Out_ _Deref_out_range_(==, Multiplicand * Multiplier) SSIZE_T* pResult
    )
{
    C_ASSERT(sizeof(LONGLONG) > sizeof(SSIZE_T));
    return RtlLongLongToSSIZET(((LONGLONG)Multiplicand) * ((LONGLONG)Multiplier), pResult);
}
#endif

#endif

#ifndef LOWORD
#define LOWORD(l)     ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(l)     ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#endif
#ifndef LODWORD
#define LODWORD(_qw)    ((DWORD)(_qw))
#endif
#ifndef HIDWORD
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))
#endif

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP | WINAPI_PARTITION_SYSTEM | WINAPI_PARTITION_GAMES) */
#pragma endregion

#endif
