/*
 * Integer literal/constant support macros for typesafe integer types.
 *
 * This file is part of System Informer.
 */


#ifndef _NTTYPESAFE_H_INCLUDED_
#define _NTTYPESAFE_H_INCLUDED_

#include <stdint.h>


#ifndef BOOLEAN_C
#if defined(_MSC_VER)
#define BOOLEAN_C(x)  (x##ui8)
#else
#define BOOLEAN_C(x)  (x)
#endif
#endif


#ifndef CHAR_C
#if defined(_MSC_VER)
#define CHAR_C(x)    (x##i8)
#else
#define CHAR_C(x)    (x)
#endif
#endif

#ifndef CCHAR_C
#if defined(_MSC_VER)
#define CCHAR_C(x)   (x##i8)
#else
#define CCHAR_C(x)   (x)
#endif
#endif

#ifndef UCHAR_C
#if defined(_MSC_VER)
#define UCHAR_C(x)   (x##ui8)
#else
#define UCHAR_C(x)   (x)
#endif
#endif

#ifndef WCHAR_C
#if defined(_MSC_VER)
#define WCHAR_C(x)   (x##ui16)
#else
#define WCHAR_C(x)   (x)
#endif
#endif


#ifndef SHORT_C
#if defined(_MSC_VER)
#define SHORT_C(x)   (x##i16)
#else
#define SHORT_C(x)   (x)
#endif
#endif

#ifndef USHORT_C
#if defined(_MSC_VER)
#define USHORT_C(x)  (x##ui16)
#else
#define USHORT_C(x)  (x)
#endif
#endif

#ifndef WORD_C
#if defined(_MSC_VER)
#define WORD_C(x)    (x##ui16)
#else
#define WORD_C(x)    (x)
#endif
#endif


#ifndef HALF_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define HALF_PTR_C(x) (x##i32)
#else
#define HALF_PTR_C(x) (x)
#endif
#else
#if defined(_MSC_VER)
#define HALF_PTR_C(x) (x##i16)
#else
#define HALF_PTR_C(x) (x)
#endif
#endif
#endif

#ifndef UHALF_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define UHALF_PTR_C(x) (x##ui32)
#else
#define UHALF_PTR_C(x) (x##U)
#endif
#else
#if defined(_MSC_VER)
#define UHALF_PTR_C(x) (x##ui16)
#else
#define UHALF_PTR_C(x) (x)
#endif
#endif
#endif

#ifndef PHALF_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define PHALF_PTR_C(x) (x##i32)
#else
#define PHALF_PTR_C(x) (x)
#endif
#else
#if defined(_MSC_VER)
#define PHALF_PTR_C(x) (x##i16)
#else
#define PHALF_PTR_C(x) (x)
#endif
#endif
#endif

#ifndef PUHALF_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define PUHALF_PTR_C(x) (x##ui32)
#else
#define PUHALF_PTR_C(x) (x##U)
#endif
#else
#if defined(_MSC_VER)
#define PUHALF_PTR_C(x) (x##ui16)
#else
#define PUHALF_PTR_C(x) (x)
#endif
#endif
#endif


#ifndef INT_C
#if defined(_MSC_VER)
#define INT_C(x)     (x##i32)
#else
#define INT_C(x)     (x)
#endif
#endif

#ifndef UINT_C
#if defined(_MSC_VER)
#define UINT_C(x)    (x##ui32)
#else
#define UINT_C(x)    (x##U)
#endif
#endif


#ifndef LONG_C
#if defined(_MSC_VER)
#define LONG_C(x)    (x##i32)
#else
#define LONG_C(x)    (x)
#endif
#endif

#ifndef LONG32_C
#if defined(_MSC_VER)
#define LONG32_C(x)  (x##i32)
#else
#define LONG32_C(x)  (x)
#endif
#endif

#ifndef LONG64_C
#if defined(_MSC_VER)
#define LONG64_C(x)  (x##i64)
#else
#define LONG64_C(x)  (x##LL)
#endif
#endif

#ifndef LONGLONG_C
#if defined(_MSC_VER)
#define LONGLONG_C(x) (x##i64)
#else
#define LONGLONG_C(x) (x##LL)
#endif
#endif

#ifndef ULONG_C
#if defined(_MSC_VER)
#define ULONG_C(x)   (x##ui32)
#else
#define ULONG_C(x)   (x##U)
#endif
#endif

#ifndef ULONG32_C
#if defined(_MSC_VER)
#define ULONG32_C(x) (x##ui32)
#else
#define ULONG32_C(x) (x##U)
#endif
#endif

#ifndef ULONG64_C
#if defined(_MSC_VER)
#define ULONG64_C(x) (x##ui64)
#else
#define ULONG64_C(x) (x##ULL)
#endif
#endif

#ifndef ULONGLONG_C
#if defined(_MSC_VER)
#define ULONGLONG_C(x) (x##ui64)
#else
#define ULONGLONG_C(x) (x##ULL)
#endif
#endif


#ifndef FLOAT_C
#define FLOAT_C(x)   (x##f)
#endif

#ifndef DOUBLE_C
#if defined(_MSC_VER)
#define DOUBLE_C(x)  (x##.0)
#else
#define DOUBLE_C(x)  (x)
#endif
#endif


#ifndef CSHORT_C
#if defined(_MSC_VER)
#define CSHORT_C(x)  (x##i16)
#else
#define CSHORT_C(x)  (x)
#endif
#endif

#ifndef CLONG_C
#if defined(_MSC_VER)
#define CLONG_C(x)   (x##ui32)
#else
#define CLONG_C(x)   (x##U)
#endif
#endif

#ifndef LOGICAL_C
#if defined(_MSC_VER)
#define LOGICAL_C(x) (x##ui32)
#else
#define LOGICAL_C(x) (x##U)
#endif
#endif


#ifndef INT_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define INT_PTR_C(x) (x##i64)
#else
#define INT_PTR_C(x) (x##LL)
#endif
#else
#if defined(_MSC_VER)
#define INT_PTR_C(x) (x##i32)
#else
#define INT_PTR_C(x) (x)
#endif
#endif
#endif

#ifndef UINT_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define UINT_PTR_C(x) (x##ui64)
#else
#define UINT_PTR_C(x) (x##ULL)
#endif
#else
#if defined(_MSC_VER)
#define UINT_PTR_C(x) (x##ui32)
#else
#define UINT_PTR_C(x) (x##U)
#endif
#endif
#endif

#ifndef LONG_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define LONG_PTR_C(x) (x##i64)
#else
#define LONG_PTR_C(x) (x##LL)
#endif
#else
#if defined(_MSC_VER)
#define LONG_PTR_C(x) (x##i32)
#else
#define LONG_PTR_C(x) (x)
#endif
#endif
#endif

#ifndef ULONG_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define ULONG_PTR_C(x) (x##ui64)
#else
#define ULONG_PTR_C(x) (x##ULL)
#endif
#else
#if defined(_MSC_VER)
#define ULONG_PTR_C(x) (x##ui32)
#else
#define ULONG_PTR_C(x) (x##U)
#endif
#endif
#endif

#ifndef SIZE_T_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define SIZE_T_C(x) (x##ui64)
#else
#define SIZE_T_C(x) (x##ULL)
#endif
#else
#if defined(_MSC_VER)
#define SIZE_T_C(x) (x##ui32)
#else
#define SIZE_T_C(x) (x##U)
#endif
#endif
#endif

#ifndef SSIZE_T_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define SSIZE_T_C(x) (x##i64)
#else
#define SSIZE_T_C(x) (x##LL)
#endif
#else
#if defined(_MSC_VER)
#define SSIZE_T_C(x) (x##i32)
#else
#define SSIZE_T_C(x) (x)
#endif
#endif
#endif

#ifndef PTRDIFF_T_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define PTRDIFF_T_C(x) (x##i64)
#else
#define PTRDIFF_T_C(x) (x##LL)
#endif
#else
#if defined(_MSC_VER)
#define PTRDIFF_T_C(x) (x##i32)
#else
#define PTRDIFF_T_C(x) (x)
#endif
#endif
#endif


#undef INT8_C
#undef UINT8_C
#undef INT16_C
#undef UINT16_C
#undef INT32_C
#undef UINT32_C
#undef INT64_C
#undef UINT64_C
#undef INTMAX_C
#undef UINTMAX_C

#if defined(_MSC_VER)
#define INT8_C(x)    (x##i8)
#define UINT8_C(x)   (x##ui8)
#define INT16_C(x)   (x##i16)
#define UINT16_C(x)  (x##ui16)
#define INT32_C(x)   (x##i32)
#define UINT32_C(x)  (x##ui32)
#define INT64_C(x)   (x##i64)
#define UINT64_C(x)  (x##ui64)
#define INTMAX_C(x)  (x##i64)
#define UINTMAX_C(x) (x##ui64)
#else
#define INT8_C(x)    (x)
#define UINT8_C(x)   (x)
#define INT16_C(x)   (x)
#define UINT16_C(x)  (x)
#define INT32_C(x)   (x)
#define UINT32_C(x)  (x##U)
#define INT64_C(x)   (x##LL)
#define UINT64_C(x)  (x##ULL)
#define INTMAX_C(x)  (x##LL)
#define UINTMAX_C(x) (x##ULL)
#endif


#ifndef BYTE_C
#if defined(_MSC_VER)
#define BYTE_C(x)    (x##ui8)
#else
#define BYTE_C(x)    (x)
#endif
#endif

#ifndef DWORD_C
#if defined(_MSC_VER)
#define DWORD_C(x)   (x##ui32)
#else
#define DWORD_C(x)   (x##U)
#endif
#endif

#ifndef DWORD32_C
#if defined(_MSC_VER)
#define DWORD32_C(x) (x##ui32)
#else
#define DWORD32_C(x) (x##U)
#endif
#endif

#ifndef DWORD64_C
#if defined(_MSC_VER)
#define DWORD64_C(x) (x##ui64)
#else
#define DWORD64_C(x) (x##ULL)
#endif
#endif

#ifndef DWORDLONG_C
#if defined(_MSC_VER)
#define DWORDLONG_C(x) (x##ui64)
#else
#define DWORDLONG_C(x) (x##ULL)
#endif
#endif

#ifndef DWORD_PTR_C
#ifdef _WIN64
#if defined(_MSC_VER)
#define DWORD_PTR_C(x) (x##ui64)
#else
#define DWORD_PTR_C(x) (x##ULL)
#endif
#else
#if defined(_MSC_VER)
#define DWORD_PTR_C(x) (x##ui32)
#else
#define DWORD_PTR_C(x) (x##U)
#endif
#endif
#endif

#ifndef QWORD_C
#if defined(_MSC_VER)
#define QWORD_C(x)   (x##ui64)
#else
#define QWORD_C(x)   (x##ULL)
#endif
#endif


#ifndef INT128_C
#if defined(_MSC_VER)
#define INT128_C(x)  (x##i128)
#else
#define INT128_C(x)  ((__int128)(x##LL))
#endif
#endif

#ifndef UINT128_C
#if defined(_MSC_VER)
#define UINT128_C(x) (x##ui128)
#else
#define UINT128_C(x) ((unsigned __int128)(x##ULL))
#endif
#endif

#endif /* _NTTYPESAFE_H_INCLUDED_ */
