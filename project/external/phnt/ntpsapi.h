/*
 * Process support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTPSAPI_H
#define _NTPSAPI_H

#include <ntpebteb.h>


#ifndef PROCESS_TERMINATE
#define PROCESS_TERMINATE 0x0001
#endif
#ifndef PROCESS_CREATE_THREAD
#define PROCESS_CREATE_THREAD 0x0002
#endif
#ifndef PROCESS_SET_SESSIONID
#define PROCESS_SET_SESSIONID 0x0004
#endif
#ifndef PROCESS_VM_OPERATION
#define PROCESS_VM_OPERATION 0x0008
#endif
#ifndef PROCESS_VM_READ
#define PROCESS_VM_READ 0x0010
#endif
#ifndef PROCESS_VM_WRITE
#define PROCESS_VM_WRITE 0x0020
#endif
#ifndef PROCESS_DUP_HANDLE
#define PROCESS_DUP_HANDLE 0x0040
#endif
#ifndef PROCESS_CREATE_PROCESS
#define PROCESS_CREATE_PROCESS 0x0080
#endif
#ifndef PROCESS_SET_QUOTA
#define PROCESS_SET_QUOTA 0x0100
#endif
#ifndef PROCESS_SET_INFORMATION
#define PROCESS_SET_INFORMATION 0x0200
#endif
#ifndef PROCESS_QUERY_INFORMATION
#define PROCESS_QUERY_INFORMATION 0x0400
#endif
#ifndef PROCESS_SET_PORT
#define PROCESS_SET_PORT 0x0800
#endif
#ifndef PROCESS_SUSPEND_RESUME
#define PROCESS_SUSPEND_RESUME 0x0800
#endif
#ifndef PROCESS_QUERY_LIMITED_INFORMATION
#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#endif
#ifndef PROCESS_SET_LIMITED_INFORMATION
#define PROCESS_SET_LIMITED_INFORMATION 0x2000
#endif
#ifndef PROCESS_ALL_ACCESS
#if (PHNT_VERSION >= PHNT_WINDOWS_VISTA)
#define PROCESS_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | SPECIFIC_RIGHTS_ALL)
#else
#define PROCESS_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF)
#endif
#endif


#ifndef THREAD_TERMINATE
#define THREAD_TERMINATE 0x0001
#endif
#ifndef THREAD_SUSPEND_RESUME
#define THREAD_SUSPEND_RESUME 0x0002
#endif
#ifndef THREAD_ALERT
#define THREAD_ALERT 0x0004
#endif
#ifndef THREAD_GET_CONTEXT
#define THREAD_GET_CONTEXT 0x0008
#endif
#ifndef THREAD_SET_CONTEXT
#define THREAD_SET_CONTEXT 0x0010
#endif
#ifndef THREAD_SET_INFORMATION
#define THREAD_SET_INFORMATION 0x0020
#endif
#ifndef THREAD_QUERY_INFORMATION
#define THREAD_QUERY_INFORMATION 0x0040
#endif
#ifndef THREAD_SET_THREAD_TOKEN
#define THREAD_SET_THREAD_TOKEN 0x0080
#endif
#ifndef THREAD_IMPERSONATE
#define THREAD_IMPERSONATE 0x0100
#endif
#ifndef THREAD_DIRECT_IMPERSONATION
#define THREAD_DIRECT_IMPERSONATION 0x0200
#endif
#ifndef THREAD_SET_LIMITED_INFORMATION
#define THREAD_SET_LIMITED_INFORMATION 0x0400
#endif
#ifndef THREAD_QUERY_LIMITED_INFORMATION
#define THREAD_QUERY_LIMITED_INFORMATION 0x0800
#endif
#ifndef THREAD_RESUME
#define THREAD_RESUME 0x1000
#endif
#ifndef THREAD_ALL_ACCESS
#if (PHNT_VERSION >= PHNT_WINDOWS_VISTA)
#define THREAD_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | SPECIFIC_RIGHTS_ALL)
#else
#define THREAD_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3FF)
#endif
#endif


#ifndef JOB_OBJECT_ASSIGN_PROCESS
#define JOB_OBJECT_ASSIGN_PROCESS 0x0001
#endif
#ifndef JOB_OBJECT_SET_ATTRIBUTES
#define JOB_OBJECT_SET_ATTRIBUTES 0x0002
#endif
#ifndef JOB_OBJECT_QUERY
#define JOB_OBJECT_QUERY 0x0004
#endif
#ifndef JOB_OBJECT_TERMINATE
#define JOB_OBJECT_TERMINATE 0x0008
#endif
#ifndef JOB_OBJECT_SET_SECURITY_ATTRIBUTES
#define JOB_OBJECT_SET_SECURITY_ATTRIBUTES 0x0010
#endif
#ifndef JOB_OBJECT_ALL_ACCESS
#if (PHNT_VERSION >= PHNT_WINDOWS_VISTA)
#define JOB_OBJECT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3F)
#else
#define JOB_OBJECT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1f)
#endif
#endif


/**
 * The PEB_LDR_DATA structure contains information about the loaded modules for the process.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb_ldr_data
 */
typedef struct _PEB_LDR_DATA
{
    ULONG Length;
    BOOLEAN Initialized;
    HANDLE SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID EntryInProgress;
    BOOLEAN ShutdownInProgress;
    HANDLE ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

/**
 * The INITIAL_TEB structure contains information about the initial stack for a thread.
 * This structure is used when creating a new thread to specify the stack boundaries and allocation base.
 * It also contains information about the previous stack if the thread is being recreated.
 */
typedef struct _INITIAL_TEB
{
    struct
    {
        PVOID OldStackBase;
        PVOID OldStackLimit;
    } OldInitialTeb;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID StackAllocationBase;
} INITIAL_TEB, *PINITIAL_TEB;


#if (PHNT_MODE != PHNT_MODE_KERNEL)
typedef enum _PROCESSINFOCLASS
{
    ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    ProcessDeviceMap,
    ProcessSessionInformation,
    ProcessForegroundInformation,
    ProcessWow64Information,
    ProcessImageFileName,
    ProcessLUIDDeviceMapsEnabled,
    ProcessBreakOnTermination,
    ProcessDebugObjectHandle,
    ProcessDebugFlags,
    ProcessHandleTracing,
    ProcessIoPriority,
    ProcessExecuteFlags,
    ProcessTlsInformation,
    ProcessCookie,
    ProcessImageInformation,
    ProcessCycleTime,
    ProcessPagePriority,
    ProcessInstrumentationCallback,
    ProcessThreadStackAllocation,
    ProcessWorkingSetWatchEx,
    ProcessImageFileNameWin32,
    ProcessImageFileMapping,
    ProcessAffinityUpdateMode,
    ProcessMemoryAllocationMode,
    ProcessGroupInformation,
    ProcessTokenVirtualizationEnabled,
    ProcessConsoleHostProcess,
    ProcessWindowInformation,
    ProcessHandleInformation,
    ProcessMitigationPolicy,
    ProcessDynamicFunctionTableInformation,
    ProcessHandleCheckingMode,
    ProcessKeepAliveCount,
    ProcessRevokeFileHandles,
    ProcessWorkingSetControl,
    ProcessHandleTable,
    ProcessCheckStackExtentsMode,
    ProcessCommandLineInformation,
    ProcessProtectionInformation,
    ProcessMemoryExhaustion,
    ProcessFaultInformation,
    ProcessTelemetryIdInformation,
    ProcessCommitReleaseInformation,
    ProcessDefaultCpuSetsInformation,
    ProcessAllowedCpuSetsInformation,
    ProcessSubsystemProcess,
    ProcessJobMemoryInformation,
    ProcessInPrivate,
    ProcessRaiseUMExceptionOnInvalidHandleClose,
    ProcessIumChallengeResponse,
    ProcessChildProcessInformation,
    ProcessHighGraphicsPriorityInformation,
    ProcessSubsystemInformation,
    ProcessEnergyValues,
    ProcessPowerThrottlingState,
    ProcessActivityThrottlePolicy,
    ProcessWin32kSyscallFilterInformation,
    ProcessDisableSystemAllowedCpuSets,
    ProcessWakeInformation,
    ProcessEnergyTrackingState,
    ProcessManageWritesToExecutableMemory,
    ProcessCaptureTrustletLiveDump,
    ProcessTelemetryCoverage,
    ProcessEnclaveInformation,
    ProcessEnableReadWriteVmLogging,
    ProcessUptimeInformation,
    ProcessImageSection,
    ProcessDebugAuthInformation,
    ProcessSystemResourceManagement,
    ProcessSequenceNumber,
    ProcessLoaderDetour,
    ProcessSecurityDomainInformation,
    ProcessCombineSecurityDomainsInformation,
    ProcessEnableLogging,
    ProcessLeapSecondInformation,
    ProcessFiberShadowStackAllocation,
    ProcessFreeFiberShadowStackAllocation,
    ProcessAltSystemCallInformation,
    ProcessDynamicEHContinuationTargets,
    ProcessDynamicEnforcedCetCompatibleRanges,
    ProcessCreateStateChange,
    ProcessApplyStateChange,
    ProcessEnableOptionalXStateFeatures,
    ProcessAltPrefetchParam,
    ProcessAssignCpuPartitions,
    ProcessPriorityClassEx,
    ProcessMembershipInformation,
    ProcessEffectiveIoPriority,
    ProcessEffectivePagePriority,
    ProcessSchedulerSharedData,
    ProcessSlistRollbackInformation,
    ProcessNetworkIoCounters,
    ProcessFindFirstThreadByTebValue,
    ProcessEnclaveAddressSpaceRestriction,
    ProcessAvailableCpus,
    MaxProcessInfoClass
} PROCESSINFOCLASS;
#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)
typedef enum _THREADINFOCLASS
{
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger,
    ThreadBreakOnTermination,
    ThreadSwitchLegacyState,
    ThreadIsTerminated,
    ThreadLastSystemCall,
    ThreadIoPriority,
    ThreadCycleTime,
    ThreadPagePriority,
    ThreadActualBasePriority,
    ThreadTebInformation,
    ThreadCSwitchMon,
    ThreadCSwitchPmu,
    ThreadWow64Context,
    ThreadGroupInformation,
    ThreadUmsInformation,
    ThreadCounterProfiling,
    ThreadIdealProcessorEx,
    ThreadCpuAccountingInformation,
    ThreadSuspendCount,
    ThreadHeterogeneousCpuPolicy,
    ThreadContainerId,
    ThreadNameInformation,
    ThreadSelectedCpuSets,
    ThreadSystemThreadInformation,
    ThreadActualGroupAffinity,
    ThreadDynamicCodePolicyInfo,
    ThreadExplicitCaseSensitivity,
    ThreadWorkOnBehalfTicket,
    ThreadSubsystemInformation,
    ThreadDbgkWerReportActive,
    ThreadAttachContainer,
    ThreadManageWritesToExecutableMemory,
    ThreadPowerThrottlingState,
    ThreadWorkloadClass,
    ThreadCreateStateChange,
    ThreadApplyStateChange,
    ThreadStrongerBadHandleChecks,
    ThreadEffectiveIoPriority,
    ThreadEffectivePagePriority,
    ThreadUpdateLockOwnership,
    ThreadSchedulerSharedDataSlot,
    ThreadTebInformationAtomic,
    ThreadIndexInformation,
    MaxThreadInfoClass
} THREADINFOCLASS;
#endif

#if (PHNT_MODE != PHNT_MODE_KERNEL)

typedef struct _PAGE_PRIORITY_INFORMATION
{
    ULONG PagePriority;
} PAGE_PRIORITY_INFORMATION, *PPAGE_PRIORITY_INFORMATION;


/**
 * The PROCESS_BASIC_INFORMATION structure contains basic information about a process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-ntqueryinformationprocess#process_basic_information
 */
typedef struct _PROCESS_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;
    PPEB PebBaseAddress;
    KAFFINITY AffinityMask;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

/**
 * The PROCESS_EXTENDED_BASIC_INFORMATION structure contains extended basic information about a process.
 */
_Struct_size_bytes_(Size)
typedef struct _PROCESS_EXTENDED_BASIC_INFORMATION
{
    _In_ SIZE_T Size;
    union
    {
        PROCESS_BASIC_INFORMATION BasicInfo;
        struct
        {
            NTSTATUS ExitStatus;
            PPEB PebBaseAddress;
            KAFFINITY AffinityMask;
            KPRIORITY BasePriority;
            HANDLE UniqueProcessId;
            HANDLE InheritedFromUniqueProcessId;
        };
    };
    union
    {
        ULONG Flags;
        struct
        {
            ULONG IsProtectedProcess : 1;
            ULONG IsWow64Process : 1;
            ULONG IsProcessDeleting : 1;
            ULONG IsCrossSessionCreate : 1;
            ULONG IsFrozen : 1;
            ULONG IsBackground : 1;
            ULONG IsStronglyNamed : 1;
            ULONG IsSecureProcess : 1;
            ULONG IsSubsystemProcess : 1;
            ULONG IsTrustedApp : 1;
            ULONG SpareBits : 22;
        };
    };
} PROCESS_EXTENDED_BASIC_INFORMATION, *PPROCESS_EXTENDED_BASIC_INFORMATION;

/**
 * The VM_COUNTERS structure contains various memory usage statistics for a process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters
 */
typedef struct _VM_COUNTERS
{
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} VM_COUNTERS, *PVM_COUNTERS;

/**
 * The VM_COUNTERS_EX structure extends VM_COUNTERS to include private memory usage.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2
 */
typedef struct _VM_COUNTERS_EX
{
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivateUsage;
} VM_COUNTERS_EX, *PVM_COUNTERS_EX;

/**
 * The VM_COUNTERS_EX2 structure extends VM_COUNTERS_EX to include private working set size and shared commit usage.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-process_memory_counters_ex2
 */
typedef struct _VM_COUNTERS_EX2
{
    union
    {
        VM_COUNTERS_EX CountersEx;
        struct
        {
            SIZE_T PeakVirtualSize;
            SIZE_T VirtualSize;
            ULONG PageFaultCount;
            SIZE_T PeakWorkingSetSize;
            SIZE_T WorkingSetSize;
            SIZE_T QuotaPeakPagedPoolUsage;
            SIZE_T QuotaPagedPoolUsage;
            SIZE_T QuotaPeakNonPagedPoolUsage;
            SIZE_T QuotaNonPagedPoolUsage;
            SIZE_T PagefileUsage;
            SIZE_T PeakPagefileUsage;
            SIZE_T PrivateUsage;
        };
    };
    SIZE_T PrivateWorkingSetSize;
    SIZE_T SharedCommitUsage;
} VM_COUNTERS_EX2, *PVM_COUNTERS_EX2;

/**
 * The KERNEL_USER_TIMES structure contains timing information for a process or thread.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getthreadtimes
 */
typedef struct _KERNEL_USER_TIMES
{
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
} KERNEL_USER_TIMES, *PKERNEL_USER_TIMES;

/**
 * The POOLED_USAGE_AND_LIMITS structure contains information about the usage and limits of paged and non-paged pool memory.
 */
typedef struct _POOLED_USAGE_AND_LIMITS
{
    SIZE_T PeakPagedPoolUsage;
    SIZE_T PagedPoolUsage;
    SIZE_T PagedPoolLimit;
    SIZE_T PeakNonPagedPoolUsage;
    SIZE_T NonPagedPoolUsage;
    SIZE_T NonPagedPoolLimit;
    SIZE_T PeakPagefileUsage;
    SIZE_T PagefileUsage;
    SIZE_T PagefileLimit;
} POOLED_USAGE_AND_LIMITS, *PPOOLED_USAGE_AND_LIMITS;

#define PROCESS_EXCEPTION_PORT_ALL_STATE_BITS 0x00000003
#define PROCESS_EXCEPTION_PORT_ALL_STATE_FLAGS ((ULONG_PTR)((1UL << PROCESS_EXCEPTION_PORT_ALL_STATE_BITS) - 1))

/**
 * The PROCESS_EXCEPTION_PORT structure is used to manage exception ports for a process.
 */
typedef struct _PROCESS_EXCEPTION_PORT
{
    _In_ HANDLE ExceptionPortHandle;

    _Inout_ ULONG StateFlags;

} PROCESS_EXCEPTION_PORT, *PPROCESS_EXCEPTION_PORT;

/**
 * The PROCESS_ACCESS_TOKEN structure is used to manage the security context of a process or thread.
 *
 * A process's access token can only be changed if the process has no threads or a single thread that has not yet begun execution.
 */
typedef struct _PROCESS_ACCESS_TOKEN
{
    HANDLE Token;

    HANDLE Thread;

} PROCESS_ACCESS_TOKEN, *PPROCESS_ACCESS_TOKEN;

#ifndef _LDT_ENTRY_DEFINED
#define _LDT_ENTRY_DEFINED
typedef struct _LDT_ENTRY
{
    USHORT LimitLow;
    USHORT BaseLow;
    union
    {
        struct
        {
            UCHAR BaseMid;
            UCHAR Flags1;
            UCHAR Flags2;
            UCHAR BaseHi;
        } Bytes;
        struct
        {
            ULONG BaseMid : 8;
            ULONG Type : 5;
            ULONG Dpl : 2;
            ULONG Pres : 1;
            ULONG LimitHi : 4;
            ULONG Sys : 1;
            ULONG Reserved_0 : 1;
            ULONG Default_Big : 1;
            ULONG Granularity : 1;
            ULONG BaseHi : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;
#endif

/**
 * The PROCESS_LDT_INFORMATION structure is used to manage Local Descriptor Table (LDT) entries for a process.
 */
typedef struct _PROCESS_LDT_INFORMATION
{
    ULONG Start;
    ULONG Length;
    LDT_ENTRY LdtEntries[1];
} PROCESS_LDT_INFORMATION, *PPROCESS_LDT_INFORMATION;

/**
 * The PROCESS_LDT_SIZE structure is used to specify the size of the Local Descriptor Table (LDT) for a process.
 */
typedef struct _PROCESS_LDT_SIZE
{
    ULONG Length;
} PROCESS_LDT_SIZE, *PPROCESS_LDT_SIZE;

/**
 * The PROCESS_WS_WATCH_INFORMATION structure is used to store information about working set watch events for a process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_ws_watch_information
 */
typedef struct _PROCESS_WS_WATCH_INFORMATION
{
    PVOID FaultingPc;
    PVOID FaultingVa;
} PROCESS_WS_WATCH_INFORMATION, *PPROCESS_WS_WATCH_INFORMATION;

#endif

/**
 * The PROCESS_WS_WATCH_INFORMATION_EX structure contains extended information about a page added to a process working set.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/psapi/ns-psapi-psapi_ws_watch_information_ex
 */
typedef struct _PROCESS_WS_WATCH_INFORMATION_EX
{
    union
    {
        PROCESS_WS_WATCH_INFORMATION BasicInfo;
        struct
        {
            PVOID FaultingPc;
            PVOID FaultingVa;
        };
    };
    HANDLE FaultingThreadId;
    ULONG_PTR Flags;
} PROCESS_WS_WATCH_INFORMATION_EX, *PPROCESS_WS_WATCH_INFORMATION_EX;

#define PROCESS_PRIORITY_CLASS_UNKNOWN 0
#define PROCESS_PRIORITY_CLASS_IDLE 1
#define PROCESS_PRIORITY_CLASS_NORMAL 2
#define PROCESS_PRIORITY_CLASS_HIGH 3
#define PROCESS_PRIORITY_CLASS_REALTIME 4
#define PROCESS_PRIORITY_CLASS_BELOW_NORMAL 5
#define PROCESS_PRIORITY_CLASS_ABOVE_NORMAL 6

/**
 * The PROCESS_PRIORITY_CLASS structure is used to manage the priority class of a process.
 */
typedef struct _PROCESS_PRIORITY_CLASS
{
    BOOLEAN Foreground;
    UCHAR PriorityClass;
} PROCESS_PRIORITY_CLASS, *PPROCESS_PRIORITY_CLASS;

/**
 * The PROCESS_PRIORITY_CLASS_EX structure extends PROCESS_PRIORITY_CLASS to include validity flags.
 */
typedef struct _PROCESS_PRIORITY_CLASS_EX
{
    union
    {
        struct
        {
            USHORT ForegroundValid : 1;
            USHORT PriorityClassValid : 1;
        };
        USHORT AllFlags;
    };
    UCHAR PriorityClass;
    BOOLEAN Foreground;
} PROCESS_PRIORITY_CLASS_EX, *PPROCESS_PRIORITY_CLASS_EX;

/**
 * The PROCESS_FOREGROUND_BACKGROUND structure is used to manage the priority class of a process, specifically whether it runs in the foreground or background.
 */
typedef struct _PROCESS_FOREGROUND_BACKGROUND
{
    BOOLEAN Foreground;
} PROCESS_FOREGROUND_BACKGROUND, *PPROCESS_FOREGROUND_BACKGROUND;

#if (PHNT_MODE != PHNT_MODE_KERNEL)

#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6

/**
 * The PROCESS_DEVICEMAP_INFORMATION structure contains information about a process's device map.
 */
typedef struct _PROCESS_DEVICEMAP_INFORMATION
{
    union
    {
        struct
        {
            HANDLE DirectoryHandle;
        } Set;
        struct
        {
            ULONG DriveMap;
            UCHAR DriveType[32];
        } Query;
    };
} PROCESS_DEVICEMAP_INFORMATION, *PPROCESS_DEVICEMAP_INFORMATION;

/**
 * The PROCESS_LUID_DOSDEVICES_ONLY flag limits the device map to only devices from the current process or logon session.
 */
#define PROCESS_LUID_DOSDEVICES_ONLY 0x00000001

/**
 * The PROCESS_DEVICEMAP_INFORMATION_EX structure contains information about a process's device map.
 */
typedef struct _PROCESS_DEVICEMAP_INFORMATION_EX
{
    union
    {
        struct
        {
            HANDLE DirectoryHandle;
        } Set;
        struct
        {
            ULONG DriveMap;
            UCHAR DriveType[32];
        } Query;
    };
    ULONG Flags;
} PROCESS_DEVICEMAP_INFORMATION_EX, *PPROCESS_DEVICEMAP_INFORMATION_EX;

/**
 * The PROCESS_SESSION_INFORMATION structure is used to store information about the session ID of a process.
 */
typedef struct _PROCESS_SESSION_INFORMATION
{
    ULONG SessionId;
} PROCESS_SESSION_INFORMATION, *PPROCESS_SESSION_INFORMATION;

#define PROCESS_HANDLE_EXCEPTIONS_ENABLED 0x00000001
#define PROCESS_HANDLE_RAISE_EXCEPTION_ON_INVALID_HANDLE_CLOSE_DISABLED 0x00000000
#define PROCESS_HANDLE_RAISE_EXCEPTION_ON_INVALID_HANDLE_CLOSE_ENABLED 0x00000001

/**
 * The PROCESS_HANDLE_TRACING_ENABLE structure is used to enable handle tracing for a process.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENABLE
{
    ULONG Flags;
} PROCESS_HANDLE_TRACING_ENABLE, *PPROCESS_HANDLE_TRACING_ENABLE;

/**
 * The PROCESS_HANDLE_TRACING_MAX_SLOTS macro specifies the maximum number of slots.
 */
#define PROCESS_HANDLE_TRACING_MAX_SLOTS 0x20000

/**
 * The PROCESS_HANDLE_TRACING_ENABLE_EX structure extends PROCESS_HANDLE_TRACING_ENABLE to include the total number of slots.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENABLE_EX
{
    ULONG Flags;
    ULONG TotalSlots;
} PROCESS_HANDLE_TRACING_ENABLE_EX, *PPROCESS_HANDLE_TRACING_ENABLE_EX;

#define PROCESS_HANDLE_TRACE_TYPE_OPEN 1
#define PROCESS_HANDLE_TRACE_TYPE_CLOSE 2
#define PROCESS_HANDLE_TRACE_TYPE_BADREF 3

/**
 * The PROCESS_HANDLE_TRACING_ENTRY structure contains information about the handle operation associated with the event.
 */
typedef struct _PROCESS_HANDLE_TRACING_ENTRY
{
    HANDLE Handle;
    CLIENT_ID ClientId;
    ULONG Type;
    PVOID Stacks[16];
} PROCESS_HANDLE_TRACING_ENTRY, *PPROCESS_HANDLE_TRACING_ENTRY;

/**
 * The PROCESS_HANDLE_TRACING_QUERY structure is used to query all handle events or a specific handle event for a process.
 */
typedef struct _PROCESS_HANDLE_TRACING_QUERY
{
    _In_opt_ HANDLE Handle;
    _Out_ ULONG TotalTraces;
    _Out_ _Field_size_(TotalTraces) PROCESS_HANDLE_TRACING_ENTRY HandleTrace[1];
} PROCESS_HANDLE_TRACING_QUERY, *PPROCESS_HANDLE_TRACING_QUERY;

#endif

/**
 * The THREAD_TLS_INFORMATION structure contains information about the Thread Local Storage (TLS) data for a thread.
 */
typedef struct _THREAD_TLS_INFORMATION
{
    ULONG Flags;
    PVOID NewTlsData;
    PVOID OldTlsData;
    HANDLE ThreadId;
} THREAD_TLS_INFORMATION, *PTHREAD_TLS_INFORMATION;

/**
 * The PROCESS_TLS_INFORMATION_TYPE enumeration defines the types of TLS operations that can be performed on a process.
 */
typedef enum _PROCESS_TLS_INFORMATION_TYPE
{
    ProcessTlsReplaceIndex,
    ProcessTlsReplaceVector,
    MaxProcessTlsOperation
} PROCESS_TLS_INFORMATION_TYPE, *PPROCESS_TLS_INFORMATION_TYPE;

/**
 * The PROCESS_TLS_INFORMATION structure contains information about the TLS operations for a process.
 */
typedef struct _PROCESS_TLS_INFORMATION
{
    ULONG Flags;
    ULONG OperationType;
    ULONG ThreadDataCount;
    ULONG TlsIndex;
    ULONG PreviousCount;
    _Field_size_(ThreadDataCount) THREAD_TLS_INFORMATION ThreadData[1];
} PROCESS_TLS_INFORMATION, *PPROCESS_TLS_INFORMATION;

/**
 * The PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION structure contains information about the instrumentation callback for a process.
 */
typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
{
    ULONG Version;
    ULONG Reserved;
    PVOID Callback;
} PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, *PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

/**
 * The PROCESS_STACK_ALLOCATION_INFORMATION structure contains information about the stack allocation for a process.
 */
typedef struct _PROCESS_STACK_ALLOCATION_INFORMATION
{
    SIZE_T ReserveSize;
    SIZE_T ZeroBits;
    PVOID StackBase;
} PROCESS_STACK_ALLOCATION_INFORMATION, *PPROCESS_STACK_ALLOCATION_INFORMATION;

/**
 * The PROCESS_STACK_ALLOCATION_INFORMATION_EX structure extends PROCESS_STACK_ALLOCATION_INFORMATION to include additional fields.
 */
typedef struct _PROCESS_STACK_ALLOCATION_INFORMATION_EX
{
    ULONG PreferredNode;
    ULONG Reserved0;
    ULONG Reserved1;
    ULONG Reserved2;
    PROCESS_STACK_ALLOCATION_INFORMATION AllocInfo;
} PROCESS_STACK_ALLOCATION_INFORMATION_EX, *PPROCESS_STACK_ALLOCATION_INFORMATION_EX;

/**
 * The PROCESS_AFFINITY_UPDATE_MODE union is used to specify the affinity update mode for a process.
 */
typedef struct _PROCESS_AFFINITY_UPDATE_MODE
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG EnableAutoUpdate : 1;
            ULONG Permanent : 1;
            ULONG Reserved : 30;
        };
    };
} PROCESS_AFFINITY_UPDATE_MODE, *PPROCESS_AFFINITY_UPDATE_MODE;

/**
 * The PROCESS_MEMORY_ALLOCATION_MODE union is used to specify the memory allocation mode for a process.
 */
typedef struct _PROCESS_MEMORY_ALLOCATION_MODE
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG TopDown : 1;
            ULONG Reserved : 31;
        };
    };
} PROCESS_MEMORY_ALLOCATION_MODE, *PPROCESS_MEMORY_ALLOCATION_MODE;

/**
 * The PROCESS_HANDLE_INFORMATION structure contains information about the handles of a process.
 */
typedef struct _PROCESS_HANDLE_INFORMATION
{
    ULONG HandleCount;
    ULONG HandleCountHighWatermark;
} PROCESS_HANDLE_INFORMATION, *PPROCESS_HANDLE_INFORMATION;

/**
 * The PROCESS_CYCLE_TIME_INFORMATION structure contains information about the cycle time of a process.
 */
typedef struct _PROCESS_CYCLE_TIME_INFORMATION
{
    ULONGLONG AccumulatedCycles;
    ULONGLONG CurrentCycleCount;
} PROCESS_CYCLE_TIME_INFORMATION, *PPROCESS_CYCLE_TIME_INFORMATION;

/**
 * The PROCESS_WINDOW_INFORMATION structure contains information about the windows of a process.
 */
typedef struct _PROCESS_WINDOW_INFORMATION
{
    ULONG WindowFlags;
    USHORT WindowTitleLength;
    _Field_size_bytes_(WindowTitleLength) WCHAR WindowTitle[1];
} PROCESS_WINDOW_INFORMATION, *PPROCESS_WINDOW_INFORMATION;

/**
 * The PROCESS_HANDLE_TABLE_ENTRY_INFO structure contains information about a handle table entry of a process.
 */
typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO
{
    HANDLE HandleValue;
    SIZE_T HandleCount;
    SIZE_T PointerCount;
    ACCESS_MASK GrantedAccess;
    ULONG ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO, *PPROCESS_HANDLE_TABLE_ENTRY_INFO;

/**
 * The PROCESS_HANDLE_SNAPSHOT_INFORMATION structure contains information about the handle snapshot of a process.
 */
typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    _Field_size_(NumberOfHandles) PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION, *PPROCESS_HANDLE_SNAPSHOT_INFORMATION;

#if (PHNT_MODE != PHNT_MODE_KERNEL)

#if !defined(NTDDI_WIN10_FE) || (NTDDI_VERSION < NTDDI_WIN10_FE)
typedef struct _PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY
{
    union {
        ULONG Flags;
        struct {
            ULONG EnforceRedirectionTrust : 1;
            ULONG AuditRedirectionTrust : 1;
            ULONG ReservedFlags : 30;
        };
    };
} PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY, *PPROCESS_MITIGATION_REDIRECTION_TRUST_POLICY;
#endif

#if !defined(NTDDI_WIN10_NI) || (NTDDI_VERSION < NTDDI_WIN10_NI)
typedef struct _PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG EnablePointerAuthUserIp : 1;
            ULONG ReservedFlags : 31;
        };
    };
} PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY, *PPROCESS_MITIGATION_USER_POINTER_AUTH_POLICY;

typedef struct _PROCESS_MITIGATION_SEHOP_POLICY {
    union {
        ULONG Flags;
        struct {
            ULONG EnableSehop : 1;
            ULONG ReservedFlags : 31;
        };
    };
} PROCESS_MITIGATION_SEHOP_POLICY, *PPROCESS_MITIGATION_SEHOP_POLICY;
#endif

typedef struct _PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG AssemblyManifestRedirectionTrust : 1;
            ULONG ReservedFlags : 31;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2, *PPROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2;

#if defined(_PHLIB_)
#define PROCESS_MITIGATION_POLICY ULONG
#define ProcessDEPPolicy 0
#define ProcessASLRPolicy 1
#define ProcessDynamicCodePolicy 2
#define ProcessStrictHandleCheckPolicy 3
#define ProcessSystemCallDisablePolicy 4
#define ProcessMitigationOptionsMask 5
#define ProcessExtensionPointDisablePolicy 6
#define ProcessControlFlowGuardPolicy 7
#define ProcessSignaturePolicy 8
#define ProcessFontDisablePolicy 9
#define ProcessImageLoadPolicy 10
#define ProcessSystemCallFilterPolicy 11
#define ProcessPayloadRestrictionPolicy 12
#define ProcessChildProcessPolicy 13
#define ProcessSideChannelIsolationPolicy 14
#define ProcessUserShadowStackPolicy 15
#define ProcessRedirectionTrustPolicy 16
#define ProcessUserPointerAuthPolicy 17
#define ProcessSEHOPPolicy 18
#define ProcessActivationContextTrustPolicy 19
#define MaxProcessMitigationPolicy 20
#endif

/**
 * The PROCESS_MITIGATION_POLICY_INFORMATION structure represents the different process mitigation policies.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ne-winnt-process_mitigation_policy
 */
typedef struct _PROCESS_MITIGATION_POLICY_INFORMATION
{
    PROCESS_MITIGATION_POLICY Policy;
    union
    {
        PROCESS_MITIGATION_ASLR_POLICY ASLRPolicy;
        PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY StrictHandleCheckPolicy;
        PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY SystemCallDisablePolicy;
        PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY ExtensionPointDisablePolicy;
        PROCESS_MITIGATION_DYNAMIC_CODE_POLICY DynamicCodePolicy;
        PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY ControlFlowGuardPolicy;
        PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY SignaturePolicy;
        PROCESS_MITIGATION_FONT_DISABLE_POLICY FontDisablePolicy;
        PROCESS_MITIGATION_IMAGE_LOAD_POLICY ImageLoadPolicy;
        PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY SystemCallFilterPolicy;
        PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY PayloadRestrictionPolicy;
        PROCESS_MITIGATION_CHILD_PROCESS_POLICY ChildProcessPolicy;
        PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY SideChannelIsolationPolicy;
        PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY UserShadowStackPolicy;
        PROCESS_MITIGATION_REDIRECTION_TRUST_POLICY RedirectionTrustPolicy;
        PROCESS_MITIGATION_USER_POINTER_AUTH_POLICY UserPointerAuthPolicy;
        PROCESS_MITIGATION_SEHOP_POLICY SEHOPPolicy;
        PROCESS_MITIGATION_ACTIVATION_CONTEXT_TRUST_POLICY2 ActivationContextTrustPolicy;
    };
} PROCESS_MITIGATION_POLICY_INFORMATION, *PPROCESS_MITIGATION_POLICY_INFORMATION;

typedef struct _DYNAMIC_FUNCTION_TABLE DYNAMIC_FUNCTION_TABLE, *PDYNAMIC_FUNCTION_TABLE;

/**
 * The PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION structure is used to manage dynamic function tables for a process.
 */
typedef struct _PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION
{
    PDYNAMIC_FUNCTION_TABLE DynamicFunctionTable;
    BOOLEAN Remove;
} PROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION, *PPROCESS_DYNAMIC_FUNCTION_TABLE_INFORMATION;

/**
 * The PROCESS_KEEPALIVE_COUNT_INFORMATION structure contains information about the wake and no-wake counts for a process.
 */
typedef struct _PROCESS_KEEPALIVE_COUNT_INFORMATION
{
    ULONG WakeCount;
    ULONG NoWakeCount;
} PROCESS_KEEPALIVE_COUNT_INFORMATION, *PPROCESS_KEEPALIVE_COUNT_INFORMATION;

/**
 * The PROCESS_REVOKE_FILE_HANDLES_INFORMATION structure revokes handles to the specified device from a process.
 */
typedef struct _PROCESS_REVOKE_FILE_HANDLES_INFORMATION
{
    UNICODE_STRING TargetDevicePath;
} PROCESS_REVOKE_FILE_HANDLES_INFORMATION, *PPROCESS_REVOKE_FILE_HANDLES_INFORMATION;

#define PROCESS_WORKING_SET_CONTROL_VERSION 3

/**
 * The PROCESS_WORKING_SET_OPERATION enumeration defines the operation to perform on a process's working set.
 */
typedef enum _PROCESS_WORKING_SET_OPERATION
{
    ProcessWorkingSetSwap,
    ProcessWorkingSetEmpty,
    ProcessWorkingSetEmptyPrivatePages,
    ProcessWorkingSetOperationMax
} PROCESS_WORKING_SET_OPERATION;

/**
 * The PROCESS_WORKING_SET_FLAG_EMPTY_PRIVATE_PAGES flag indicates that the operation should target private pages in the working set.
 * Private pages are those that are not shared with other processes.
 */
#define PROCESS_WORKING_SET_FLAG_EMPTY_PRIVATE_PAGES 0x01
/**
 * The PROCESS_WORKING_SET_FLAG_EMPTY_SHARED_PAGES flag indicates that the operation should target shared pages in the working set.
 * Shared pages are those that are shared between multiple processes.
 */
#define PROCESS_WORKING_SET_FLAG_EMPTY_SHARED_PAGES  0x02
 /**
  * The PROCESS_WORKING_SET_FLAG_EMPTY_PAGES flag indicates that the operation should target pages in the working set.
  */
#define PROCESS_WORKING_SET_FLAG_EMPTY_PAGES         0x04
/**
 * The PROCESS_WORKING_SET_FLAG_COMPRESS flag indicates that the operation should compress the pages before they are removed from the working set.
 * Compression is typically used in conjunction with other flags to specify that the pages should be compressed as part of the operation.
 */
#define PROCESS_WORKING_SET_FLAG_COMPRESS            0x08
/**
 * The PROCESS_WORKING_SET_FLAG_STORE flag indicates that the operation should store the compressed pages.
 * This is useful when the compressed data might be needed later, allowing for efficient retrieval and decompression when required.
 * This flag is typically used in conjunction with the PROCESS_WORKING_SET_FLAG_COMPRESS flag to specify that the compressed pages should be stored.
 */
#define PROCESS_WORKING_SET_FLAG_STORE               0x10

/**
 * The PROCESS_WORKING_SET_CONTROL structure is used to control the working set of a process.
 */
typedef struct _PROCESS_WORKING_SET_CONTROL
{
    ULONG Version;
    PROCESS_WORKING_SET_OPERATION Operation;
    ULONG Flags;
} PROCESS_WORKING_SET_CONTROL, *PPROCESS_WORKING_SET_CONTROL;

/**
 * The PS_PROTECTED_TYPE enumeration defines the types of protection that can be applied to a process.
 */
typedef enum _PS_PROTECTED_TYPE
{
    PsProtectedTypeNone,
    PsProtectedTypeProtectedLight,
    PsProtectedTypeProtected,
    PsProtectedTypeMax
} PS_PROTECTED_TYPE;

/**
 * The PS_PROTECTED_SIGNER enumeration defines the types of signers that can be associated with a protected process.
 */
typedef enum _PS_PROTECTED_SIGNER
{
    PsProtectedSignerNone,
    PsProtectedSignerAuthenticode,
    PsProtectedSignerCodeGen,
    PsProtectedSignerAntimalware,
    PsProtectedSignerLsa,
    PsProtectedSignerWindows,
    PsProtectedSignerWinTcb,
    PsProtectedSignerWinSystem,
    PsProtectedSignerApp,
    PsProtectedSignerMax
} PS_PROTECTED_SIGNER;

#define PS_PROTECTED_SIGNER_MASK 0xFF
#define PS_PROTECTED_AUDIT_MASK 0x08
#define PS_PROTECTED_TYPE_MASK 0x07

#define PsProtectedValue(PsSigner, PsAudit, PsType) ( \
    (((PsSigner) & PS_PROTECTED_SIGNER_MASK) << 4) | \
    (((PsAudit) & PS_PROTECTED_AUDIT_MASK) << 3) | \
    (((PsType) & PS_PROTECTED_TYPE_MASK)) \
    )

#define InitializePsProtection(PsProtectionLevel, PsSigner, PsAudit, PsType) { \
    (PsProtectionLevel)->Signer = (PsSigner); \
    (PsProtectionLevel)->Audit = (PsAudit); \
    (PsProtectionLevel)->Type = (PsType); \
    }

/**
 * The PS_PROTECTION structure is used to define the protection level of a process.
 */
typedef struct _PS_PROTECTION
{
    union
    {
        UCHAR Level;
        struct
        {
            UCHAR Type : 3;
            UCHAR Audit : 1;
            UCHAR Signer : 4;
        };
    };
} PS_PROTECTION, *PPS_PROTECTION;

/**
 * The PROCESS_MEMORY_EXHAUSTION_TYPE enumeration defines the different memory exhaustion types.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ne-processthreadsapi-process_memory_exhaustion_type
 */

/**
 * The PROCESS_MEMORY_EXHAUSTION_INFO structure allows applications to configure
 * termination if an allocation fails to commit memory.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_memory_exhaustion_info
 */

#define PROCESS_FAULT_FLAG_MARK_CRASHED 0x00000001
#define PROCESS_FAULT_FLAG_SET_STATE_CRASHED 0x00000002
#define PROCESS_FAULT_FLAG_ESCALATE_SEVERITY 0x00000004
#define PROCESS_FAULT_FLAG_SET_TERMINAL_BIT 0x00000008

/**
 * The PROCESS_FAULT_INFORMATION structure contains information about process faults.
 */
typedef struct _PROCESS_FAULT_INFORMATION
{
    union
    {
        ULONG FaultFlags;
        struct
        {
            ULONG MarkCrashed : 1;
            ULONG SetStateCrashed : 1;
            ULONG EscalateSeverity : 1;
            ULONG SetTerminalBit : 1;
            ULONG Reserved : 28;
        };
    };
    ULONG AdditionalInfo;
} PROCESS_FAULT_INFORMATION, *PPROCESS_FAULT_INFORMATION;

/**
 * The PROCESS_TELEMETRY_ID_INFORMATION structure contains telemetry information about a process.
 */
_Struct_size_bytes_(HeaderSize)
typedef struct _PROCESS_TELEMETRY_ID_INFORMATION
{
    ULONG HeaderSize;
    ULONG ProcessId;
    ULONG64 ProcessStartKey;
    ULONG64 CreateTime;
    ULONG64 CreateInterruptTime;
    ULONG64 CreateUnbiasedInterruptTime;
    ULONG64 ProcessSequenceNumber;
    ULONG64 SessionCreateTime;
    ULONG SessionId;
    ULONG BootId;
    ULONG ImageChecksum;
    ULONG ImageTimeDateStamp;
    ULONG UserSidOffset;
    ULONG ImagePathOffset;
    ULONG PackageNameOffset;
    ULONG RelativeAppNameOffset;
    ULONG CommandLineOffset;
} PROCESS_TELEMETRY_ID_INFORMATION, *PPROCESS_TELEMETRY_ID_INFORMATION;

/**
 * The PROCESS_COMMIT_RELEASE_INFORMATION structure contains information about the commit and release of memory for a process.
 */
typedef struct _PROCESS_COMMIT_RELEASE_INFORMATION
{
    ULONG Version;
    struct
    {
        ULONG Eligible : 1;
        ULONG ReleaseRepurposedMemResetCommit : 1;
        ULONG ForceReleaseMemResetCommit : 1;
        ULONG Spare : 29;
    };
    SIZE_T CommitDebt;
    SIZE_T CommittedMemResetSize;
    SIZE_T RepurposedMemResetSize;
} PROCESS_COMMIT_RELEASE_INFORMATION, *PPROCESS_COMMIT_RELEASE_INFORMATION;

/**
 * The PROCESS_JOB_MEMORY_INFO structure represents app memory usage at a single point in time.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-app_memory_information
 */
typedef struct _PROCESS_JOB_MEMORY_INFO
{
    ULONG64 SharedCommitUsage;
    ULONG64 PrivateCommitUsage;
    ULONG64 PeakPrivateCommitUsage;
    ULONG64 PrivateCommitLimit;
    ULONG64 TotalCommitLimit;
} PROCESS_JOB_MEMORY_INFO, *PPROCESS_JOB_MEMORY_INFO;

typedef struct _PROCESS_IUM_CHALLENGE_RESPONSE
{
    BYTE Buffer[0x1000];
} PROCESS_IUM_CHALLENGE_RESPONSE, *PPROCESS_IUM_CHALLENGE_RESPONSE;

/**
 * The PROCESS_CHILD_PROCESS_INFORMATION structure contains information about child process policies.
 */
typedef struct _PROCESS_CHILD_PROCESS_INFORMATION
{
    BOOLEAN ProhibitChildProcesses;
    BOOLEAN AlwaysAllowSecureChildProcess;
    BOOLEAN AuditProhibitChildProcesses;
} PROCESS_CHILD_PROCESS_INFORMATION, *PPROCESS_CHILD_PROCESS_INFORMATION;

/**
 * Defines the current version of the power throttling structure.
 */
#define POWER_THROTTLING_PROCESS_CURRENT_VERSION 1
/**
 * Limits the CPU execution speed of the process to reduce power consumption.
 */
#define POWER_THROTTLING_PROCESS_EXECUTION_SPEED 0x1
/**
 * The POWER_THROTTLING_PROCESS_DELAYTIMERS flag delays the expiration of waits and timers for the process.
 * When this flag is set, the process's wait and timer expiration events may be postponed,
 * which can help reduce power consumption by allowing the system to remain in low-power states longer.
 */
#define POWER_THROTTLING_PROCESS_DELAYTIMERS 0x2
/**
 * The POWER_THROTTLING_PROCESS_IGNORE_TIMER_RESOLUTION flag controls whether calls made by the
 * process to adjust the system timer resolution (such as timeBeginPeriod or NtSetTimerResolution)
 * are honored. When this flag is enabled, such requests are ignored.
 *
 * This behavior is part of Windows power-throttling mechanism introduced in Windows 11 and is
 * enabled by default for all processes. Changes to the system timer resolution can alter the
 * behavior of system timers, wait timeouts, and sleep durations, often causing unintended
 * side effects in applications. Higher-precision timer resolutions also negatively impact
 * battery life and overall system performance.
 *
 * \note Enabled by default since Windows 11. This may cause performance issues for legacy
 *       applications and games that rely on modifying the system tick resolution.
 */
#define POWER_THROTTLING_PROCESS_IGNORE_TIMER_RESOLUTION 0x4
/**
 * Valid flags for power throttling process control and state masks.
 */
#define POWER_THROTTLING_PROCESS_VALID_FLAGS \
    ((POWER_THROTTLING_PROCESS_EXECUTION_SPEED | POWER_THROTTLING_PROCESS_DELAYTIMERS | POWER_THROTTLING_PROCESS_IGNORE_TIMER_RESOLUTION))

/**
 * The POWER_THROTTLING_PROCESS_STATE structure is used to manage the power throttling state of a process.
 */
typedef struct _POWER_THROTTLING_PROCESS_STATE
{
    ULONG Version;
    ULONG ControlMask;
    ULONG StateMask;
} POWER_THROTTLING_PROCESS_STATE, *PPOWER_THROTTLING_PROCESS_STATE;

#ifndef PROCESS_POWER_THROTTLING_CURRENT_VERSION
#define PROCESS_POWER_THROTTLING_CURRENT_VERSION 1
#endif
#ifndef PROCESS_POWER_THROTTLING_EXECUTION_SPEED
#define PROCESS_POWER_THROTTLING_EXECUTION_SPEED 0x1
#endif
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif
#ifndef PROCESS_POWER_THROTTLING_VALID_FLAGS
#define PROCESS_POWER_THROTTLING_VALID_FLAGS \
    ((PROCESS_POWER_THROTTLING_EXECUTION_SPEED | PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION))
#endif


typedef enum _PROCESS_ACTIVITY_THROTTLE_POLICY_OP
{
    ProcessActivityThrottlePolicyDisable = 0,
    ProcessActivityThrottlePolicyEnable = 1,
    ProcessActivityThrottlePolicyDefault = 2,
    MaxProcessActivityThrottlePolicy
} PROCESS_ACTIVITY_THROTTLE_POLICY_OP;

#define PROCESS_ACTIVITY_THROTTLE_EXECUTIONSPEED 0x1
#define PROCESS_ACTIVITY_THROTTLE_DELAYTIMERS 0x2
#define PROCESS_ACTIVITY_THROTTLE_ALL \
    ((PROCESS_ACTIVITY_THROTTLE_EXECUTIONSPEED | PROCESS_ACTIVITY_THROTTLE_DELAYTIMERS))

/**
 * The PROCESS_ACTIVITY_THROTTLE_POLICY structure is used to manage the activity throttle of a process.
 */
typedef struct _PROCESS_ACTIVITY_THROTTLE_POLICY
{
    PROCESS_ACTIVITY_THROTTLE_POLICY_OP Operation;
    ULONG PolicyFlags;
} PROCESS_ACTIVITY_THROTTLE_POLICY, *PPROCESS_ACTIVITY_THROTTLE_POLICY;

#define WIN32K_SYSCALL_FILTER_STATE_ENABLE 0x1
#define WIN32K_SYSCALL_FILTER_STATE_AUDIT 0x2

/**
 * The WIN32K_SYSCALL_FILTER structure is used to specify filtering options for Win32k system calls.
 */
typedef struct _WIN32K_SYSCALL_FILTER
{
    ULONG FilterState;
    ULONG FilterSet;
} WIN32K_SYSCALL_FILTER, *PWIN32K_SYSCALL_FILTER;

typedef struct _JOBOBJECT_WAKE_FILTER
{
    ULONG HighEdgeFilter;
    ULONG LowEdgeFilter;
} JOBOBJECT_WAKE_FILTER, *PJOBOBJECT_WAKE_FILTER;

typedef enum _PS_WAKE_REASON
{
    PsWakeReasonUser = 0,
    PsWakeReasonExecutionRequired = 1,
    PsWakeReasonKernel = 2,
    PsWakeReasonInstrumentation = 3,
    PsWakeReasonPreserveProcess = 4,
    PsWakeReasonActivityReference = 5,
    PsWakeReasonWorkOnBehalf = 6,
    PsMaxWakeReasons = 7,
} PS_WAKE_REASON, *PPS_WAKE_REASON;

typedef struct _PROCESS_WAKE_INFORMATION
{
    ULONG64 NotificationChannel;
    ULONG WakeCounters[PsMaxWakeReasons];
    JOBOBJECT_WAKE_FILTER WakeFilter;
} PROCESS_WAKE_INFORMATION, *PPROCESS_WAKE_INFORMATION;

typedef struct _PROCESS_ENERGY_TRACKING_STATE
{
    ULONG StateUpdateMask;
    ULONG StateDesiredValue;
    ULONG StateSequence;
    ULONG UpdateTag : 1;
    WCHAR Tag[64];
} PROCESS_ENERGY_TRACKING_STATE, *PPROCESS_ENERGY_TRACKING_STATE;

/**
 * The MANAGE_WRITES_TO_EXECUTABLE_MEMORY structure controls write permissions to executable memory
 * for processes and threads, and provides a signal for kernel write events.
 */
typedef struct _MANAGE_WRITES_TO_EXECUTABLE_MEMORY
{
    ULONG Version : 8;
    ULONG ProcessEnableWriteExceptions : 1;
    ULONG ThreadAllowWrites : 1;
    ULONG Spare : 22;
    HANDLE KernelWriteToExecutableSignal;
} MANAGE_WRITES_TO_EXECUTABLE_MEMORY, *PMANAGE_WRITES_TO_EXECUTABLE_MEMORY;

#define POWER_THROTTLING_THREAD_CURRENT_VERSION 1
#define POWER_THROTTLING_THREAD_EXECUTION_SPEED 0x1
#define POWER_THROTTLING_THREAD_VALID_FLAGS (POWER_THROTTLING_THREAD_EXECUTION_SPEED)

/**
 * The POWER_THROTTLING_THREAD_STATE structure contains the throttling policies of a thread subject to power management.
 *
 * \remarks https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_power_throttling_thread_state
 */
typedef struct _POWER_THROTTLING_THREAD_STATE
{
    ULONG Version;
    ULONG ControlMask;
    ULONG StateMask;
} POWER_THROTTLING_THREAD_STATE, *PPOWER_THROTTLING_THREAD_STATE;

#define PROCESS_READWRITEVM_LOGGING_ENABLE_READVM 1
#define PROCESS_READWRITEVM_LOGGING_ENABLE_WRITEVM 2
#define PROCESS_READWRITEVM_LOGGING_ENABLE_READVM_V 1UL
#define PROCESS_READWRITEVM_LOGGING_ENABLE_WRITEVM_V 2UL

/**
 * The PROCESS_READWRITEVM_LOGGING_INFORMATION structure provides flags to enable logging
 * of read and write operations to a process's virtual memory.
 */
typedef struct _PROCESS_READWRITEVM_LOGGING_INFORMATION
{
    union
    {
        UCHAR Flags;
        struct
        {
            UCHAR EnableReadVmLogging : 1;
            UCHAR EnableWriteVmLogging : 1;
            UCHAR Unused : 6;
        };
    };
} PROCESS_READWRITEVM_LOGGING_INFORMATION, *PPROCESS_READWRITEVM_LOGGING_INFORMATION;

/**
 * The PROCESS_UPTIME_INFORMATION structure contains information about the uptime of a process and diagnostic information.
 */
typedef struct _PROCESS_UPTIME_INFORMATION
{
    ULONGLONG QueryInterruptTime;
    ULONGLONG QueryUnbiasedTime;
    ULONGLONG EndInterruptTime;
    ULONGLONG TimeSinceCreation;
    ULONGLONG Uptime;
    ULONGLONG SuspendedTime;
    struct
    {
        ULONG HangCount : 4;
        ULONG GhostCount : 4;
        ULONG Crashed : 1;
        ULONG Terminated : 1;
    };
} PROCESS_UPTIME_INFORMATION, *PPROCESS_UPTIME_INFORMATION;

/**
 * The PROCESS_SYSTEM_RESOURCE_MANAGEMENT union is used to specify system resource management flags for a process.
 */
typedef struct _PROCESS_SYSTEM_RESOURCE_MANAGEMENT
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG Foreground : 1;
            ULONG Reserved : 31;
        };
    };
} PROCESS_SYSTEM_RESOURCE_MANAGEMENT, *PPROCESS_SYSTEM_RESOURCE_MANAGEMENT;

/**
 * The PROCESS_SECURITY_DOMAIN_INFORMATION structure contains the security domain identifier for a process.
 *
 * This structure is used to query or set the security domain of a process, which can be used for isolation
 * and security boundary purposes in Windows. The SecurityDomain field is a 64-bit value that uniquely
 * identifies the security domain associated with the process.
 */
typedef struct _PROCESS_SECURITY_DOMAIN_INFORMATION
{
    ULONGLONG SecurityDomain;
} PROCESS_SECURITY_DOMAIN_INFORMATION, *PPROCESS_SECURITY_DOMAIN_INFORMATION;

/**
 * The PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION structure combines the security domain of a process.
 *
 * This structure contains information required to combine the security domains
 * of a specified process. It is typically used in system-level or security-related
 * operations where process security contexts need to be merged or managed.
 */
typedef struct _PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION
{
    HANDLE ProcessHandle;
} PROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION, *PPROCESS_COMBINE_SECURITY_DOMAINS_INFORMATION;

/**
 * The PROCESS_LOGGING_INFORMATION structure provides flags to enable or disable logging
 * for specific process and thread events, such as virtual memory access, suspend/resume,
 * execution protection, and impersonation.
 */
typedef struct _PROCESS_LOGGING_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG EnableReadVmLogging : 1;
            ULONG EnableWriteVmLogging : 1;
            ULONG EnableProcessSuspendResumeLogging : 1;
            ULONG EnableThreadSuspendResumeLogging : 1;
            ULONG EnableLocalExecProtectVmLogging : 1;
            ULONG EnableRemoteExecProtectVmLogging : 1;
            ULONG EnableImpersonationLogging : 1;
            ULONG Reserved : 25;
        };
    };
} PROCESS_LOGGING_INFORMATION, *PPROCESS_LOGGING_INFORMATION;

/**
 * This value changes the seconds field during a positive leap second adjustment by the system.
 * If enabled, then the seconds field returns any positive leap second (For example: 23:59:59 -> 23:59:60 -> 00:00:00).
 * If not enabled, then the 59th second preceding a positive leap second will be shown for 2 seconds with the milliseconds
 * value ticking twice as slow. (For example: 23:59:59 -> 23:59:59.500 -> 00:00:00, which takes 2 seconds in wall clock time).
 * Note: Leap second adjustments are disabled by default for each process, this flag also does not persist if the process is restarted.
 */
#define PROCESS_LEAP_SECOND_FLAG_ENABLE_SIXTY_SECOND 0x1
#define PROCESS_LEAP_SECOND_VALID_FLAGS (PROCESS_LEAP_SECOND_INFO_FLAG_ENABLE_SIXTY_SECOND)

/**
 * The PROCESS_LEAP_SECOND_INFORMATION structure contains information about leap second adjustments for a process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-process_leap_second_info
 */
typedef struct _PROCESS_LEAP_SECOND_INFORMATION
{
    ULONG Flags;
    ULONG Reserved;
} PROCESS_LEAP_SECOND_INFORMATION, *PPROCESS_LEAP_SECOND_INFORMATION;

typedef struct _PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
{
    ULONGLONG ReserveSize;
    ULONGLONG CommitSize;
    ULONG PreferredNode;
    ULONG Reserved;
    PVOID Ssp;
} PROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION, *PPROCESS_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION;

typedef struct _PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION
{
    PVOID Ssp;
} PROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION, *PPROCESS_FREE_FIBER_SHADOW_STACK_ALLOCATION_INFORMATION;

/**
 * The PROCESS_SYSCALL_PROVIDER_INFORMATION structure contains information about a system call provider
 * and level for system call filtering or instrumentation.
 */
typedef struct _PROCESS_SYSCALL_PROVIDER_INFORMATION
{
    GUID ProviderId;
    UCHAR Level;
} PROCESS_SYSCALL_PROVIDER_INFORMATION, *PPROCESS_SYSCALL_PROVIDER_INFORMATION;

/**
 * Contains dynamic enforced address ranges used by various features related to user-mode Hardware-enforced Stack Protection (HSP).
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-process_dynamic_enforced_address_range
 */

/**
 * The PROCESS_MEMBERSHIP_INFORMATION structure contains the Silo identifier of the process.
 *
 * \remarks https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-process_membership_information
 */
typedef struct _PROCESS_MEMBERSHIP_INFORMATION
{
    ULONG ServerSiloId;
} PROCESS_MEMBERSHIP_INFORMATION, *PPROCESS_MEMBERSHIP_INFORMATION;

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
/**
 * The PROCESS_NETWORK_COUNTERS structure contains network usage statistics for a process.
 */
typedef struct _PROCESS_NETWORK_COUNTERS
{
    ULONG64 BytesIn;
    ULONG64 BytesOut;
} PROCESS_NETWORK_COUNTERS, *PPROCESS_NETWORK_COUNTERS;
#endif

/**
 * The PROCESS_TEB_VALUE_INFORMATION structure contains information from the Thread Environment Block (TEB) for a specific thread.
 */
typedef struct _PROCESS_TEB_VALUE_INFORMATION
{
    ULONG ThreadId;
    ULONG TebOffset;
    ULONG_PTR Value;
} PROCESS_TEB_VALUE_INFORMATION, *PPROCESS_TEB_VALUE_INFORMATION;

typedef struct _PROCESS_AVAILABLE_CPUS_INFORMATION
{
    ULONG64 ObservedSequenceNumber;
    ULONG64 SequenceNumber;
    ULONG AvailableCpusCount;
    PKAFFINITY_EX Affinity;
} PROCESS_AVAILABLE_CPUS_INFORMATION, *PPROCESS_AVAILABLE_CPUS_INFORMATION;

/**
 * The NtQueryPortInformationProcess function retrieves the status of the current process exception port.
 *
 * \return LOGICAL If TRUE, the process exception port is valid.
 */
NTSYSCALLAPI
LOGICAL
NTAPI
NtQueryPortInformationProcess(
    VOID
    );

#endif


/**
 * The THREAD_BASIC_INFORMATION structure contains basic information about the thread.
 */
typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;
    PTEB TebBaseAddress;
    CLIENT_ID ClientId;
    KAFFINITY AffinityMask;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

/**
 * The THREAD_LAST_SYSCALL_INFORMATION structure contains information about the last system call made by a thread.
 */
typedef struct _THREAD_LAST_SYSCALL_INFORMATION
{
    PVOID FirstArgument;
    USHORT SystemCallNumber;
    ULONG64 WaitTime;
} THREAD_LAST_SYSCALL_INFORMATION, *PTHREAD_LAST_SYSCALL_INFORMATION;

/**
 * The THREAD_CYCLE_TIME_INFORMATION structure contains information about the cycle time of a thread.
 */
typedef struct _THREAD_CYCLE_TIME_INFORMATION
{
    ULONG64 AccumulatedCycles;
    ULONG64 CurrentCycleCount;
} THREAD_CYCLE_TIME_INFORMATION, *PTHREAD_CYCLE_TIME_INFORMATION;

typedef struct _THREAD_LOCK_OWNERSHIP
{
    ULONG SrwLock[1];
} THREAD_LOCK_OWNERSHIP, *PTHREAD_LOCK_OWNERSHIP;

typedef enum _SCHEDULER_SHARED_DATA_SLOT_ACTION
{
    SchedulerSharedSlotAssign,
    SchedulerSharedSlotFree,
    SchedulerSharedSlotQuery
} SCHEDULER_SHARED_DATA_SLOT_ACTION;

typedef struct _SCHEDULER_SHARED_DATA_SLOT_INFORMATION
{
    SCHEDULER_SHARED_DATA_SLOT_ACTION Action;
    PVOID SchedulerSharedDataHandle;
    PVOID Slot;
} SCHEDULER_SHARED_DATA_SLOT_INFORMATION, *PSCHEDULER_SHARED_DATA_SLOT_INFORMATION;

typedef struct _THREAD_TEB_INFORMATION
{
    _Inout_bytecount_(BytesToRead) PVOID TebInformation;
    _In_ ULONG TebOffset;
    _In_ ULONG BytesToRead;
} THREAD_TEB_INFORMATION, *PTHREAD_TEB_INFORMATION;

/**
 * The COUNTER_READING structure is used to store individual counter data from a hardware counter.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-hardware_counter_data
 */
typedef struct _COUNTER_READING
{
    HARDWARE_COUNTER_TYPE Type;
    ULONG Index;
    ULONG64 Start;
    ULONG64 Total;
} COUNTER_READING, *PCOUNTER_READING;

#ifndef THREAD_PERFORMANCE_DATA_VERSION
#define THREAD_PERFORMANCE_DATA_VERSION 1
#endif

/**
 * The THREAD_PERFORMANCE_DATA structure aggregates various performance metrics for a thread.
 *
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-performance_data
 */
_Struct_size_bytes_(Size)
typedef struct _THREAD_PERFORMANCE_DATA
{
    USHORT Size;
    USHORT Version;
    PROCESSOR_NUMBER ProcessorNumber;
    ULONG ContextSwitches;
    ULONG HwCountersCount;
    ULONG64 UpdateCount;
    ULONG64 WaitReasonBitMap;
    ULONG64 HardwareCounters;
    COUNTER_READING CycleTime;
    COUNTER_READING HwCounters[MAX_HW_COUNTERS];
} THREAD_PERFORMANCE_DATA, *PTHREAD_PERFORMANCE_DATA;

#ifndef THREAD_PROFILING_FLAG_DISPATCH
#define THREAD_PROFILING_FLAG_DISPATCH 0x00000001
#endif

#ifndef THREAD_PROFILING_FLAG_HARDWARE_COUNTERS
#define THREAD_PROFILING_FLAG_HARDWARE_COUNTERS 0x00000002
#endif

/**
 * The THREAD_PROFILING_INFORMATION structure contains profiling information and references to performance data.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-readthreadprofilingdata
 */
typedef struct _THREAD_PROFILING_INFORMATION
{
    ULONG64 HardwareCounters;
    ULONG Flags;
    ULONG Enable;
    PTHREAD_PERFORMANCE_DATA PerformanceData;
} THREAD_PROFILING_INFORMATION, *PTHREAD_PROFILING_INFORMATION;

typedef struct _RTL_UMS_CONTEXT
{
    SINGLE_LIST_ENTRY Link;
    CONTEXT Context;
    PVOID Teb;
    PVOID UserContext;
    volatile ULONG ScheduledThread : 1;
    volatile ULONG Suspended : 1;
    volatile ULONG VolatileContext : 1;
    volatile ULONG Terminated : 1;
    volatile ULONG DebugActive : 1;
    volatile ULONG RunningOnSelfThread : 1;
    volatile ULONG DenyRunningOnSelfThread : 1;
    volatile LONG Flags;
    volatile ULONG64 KernelUpdateLock : 2;
    volatile ULONG64 PrimaryClientID : 62;
    volatile ULONG64 ContextLock;
    struct _RTL_UMS_CONTEXT* PrimaryUmsContext;
    ULONG SwitchCount;
    ULONG KernelYieldCount;
    ULONG MixedYieldCount;
    ULONG YieldCount;
} RTL_UMS_CONTEXT, *PRTL_UMS_CONTEXT;

typedef enum _THREAD_UMS_INFORMATION_COMMAND
{
    UmsInformationCommandInvalid,
    UmsInformationCommandAttach,
    UmsInformationCommandDetach,
    UmsInformationCommandQuery
} THREAD_UMS_INFORMATION_COMMAND;

typedef struct _RTL_UMS_COMPLETION_LIST
{
    PSINGLE_LIST_ENTRY ThreadListHead;
    PVOID CompletionEvent;
    ULONG CompletionFlags;
    SINGLE_LIST_ENTRY InternalListHead;
} RTL_UMS_COMPLETION_LIST, *PRTL_UMS_COMPLETION_LIST;

typedef struct _THREAD_UMS_INFORMATION
{
    THREAD_UMS_INFORMATION_COMMAND Command;
    PRTL_UMS_COMPLETION_LIST CompletionList;
    PRTL_UMS_CONTEXT UmsContext;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG IsUmsSchedulerThread : 1;
            ULONG IsUmsWorkerThread : 1;
            ULONG SpareBits : 30;
        };
    };
} THREAD_UMS_INFORMATION, *PTHREAD_UMS_INFORMATION;

/**
 * The THREAD_NAME_INFORMATION structure assigns a description to a thread.
 *
 * \remarks The handle must have THREAD_SET_LIMITED_INFORMATION access.
 * \remarks https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreaddescription
 */
typedef struct _THREAD_NAME_INFORMATION
{
    UNICODE_STRING ThreadName;
} THREAD_NAME_INFORMATION, *PTHREAD_NAME_INFORMATION;

typedef struct _ALPC_WORK_ON_BEHALF_TICKET
{
    ULONG ThreadId;
    ULONG ThreadCreationTimeLow;
} ALPC_WORK_ON_BEHALF_TICKET, *PALPC_WORK_ON_BEHALF_TICKET;

typedef struct _RTL_WORK_ON_BEHALF_TICKET_EX
{
    ALPC_WORK_ON_BEHALF_TICKET Ticket;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG CurrentThread : 1;
            ULONG Reserved1 : 31;
        };
    };
    ULONG Reserved2;
} RTL_WORK_ON_BEHALF_TICKET_EX, *PRTL_WORK_ON_BEHALF_TICKET_EX;

#if (PHNT_MODE != PHNT_MODE_KERNEL)
typedef enum _SUBSYSTEM_INFORMATION_TYPE
{
    SubsystemInformationTypeWin32,
    SubsystemInformationTypeWSL,
    MaxSubsystemInformationType
} SUBSYSTEM_INFORMATION_TYPE;
#endif

typedef enum _THREAD_WORKLOAD_CLASS
{
    ThreadWorkloadClassDefault,
    ThreadWorkloadClassGraphics,
    MaxThreadWorkloadClass
} THREAD_WORKLOAD_CLASS;

#if defined(_ARM64_)

#define CONTEXT_ARM   0x00200000L

#define CONTEXT_ARM_CONTROL (CONTEXT_ARM | 0x1L)
#define CONTEXT_ARM_INTEGER (CONTEXT_ARM | 0x2L)
#define CONTEXT_ARM_FLOATING_POINT  (CONTEXT_ARM | 0x4L)
#define CONTEXT_ARM_DEBUG_REGISTERS (CONTEXT_ARM | 0x8L)
#define CONTEXT_ARM_FULL (CONTEXT_ARM_CONTROL | CONTEXT_ARM_INTEGER | CONTEXT_ARM_FLOATING_POINT)
#define CONTEXT_ARM_ALL (CONTEXT_ARM_CONTROL | CONTEXT_ARM_INTEGER | CONTEXT_ARM_FLOATING_POINT | CONTEXT_ARM_DEBUG_REGISTERS)

#define ARM_MAX_BREAKPOINTS     8
#define ARM_MAX_WATCHPOINTS     1

typedef struct _ARM_NT_NEON128 {
    ULONGLONG Low;
    LONGLONG High;
} ARM_NT_NEON128, *PARM_NT_NEON128;

typedef struct DECLSPEC_ALIGN(8) DECLSPEC_NOINITALL _ARM_NT_CONTEXT {


    ULONG ContextFlags;


    ULONG R0;
    ULONG R1;
    ULONG R2;
    ULONG R3;
    ULONG R4;
    ULONG R5;
    ULONG R6;
    ULONG R7;
    ULONG R8;
    ULONG R9;
    ULONG R10;
    ULONG R11;
    ULONG R12;


    ULONG Sp;
    ULONG Lr;
    ULONG Pc;
    ULONG Cpsr;


    ULONG Fpscr;
    ULONG Padding;
    union {
        ARM_NT_NEON128 Q[16];
        ULONGLONG D[32];
        ULONG S[32];
    } DUMMYUNIONNAME;


    ULONG Bvr[ARM_MAX_BREAKPOINTS];
    ULONG Bcr[ARM_MAX_BREAKPOINTS];
    ULONG Wvr[ARM_MAX_WATCHPOINTS];
    ULONG Wcr[ARM_MAX_WATCHPOINTS];

    ULONG Padding2[2];

} ARM_NT_CONTEXT, *PARM_NT_CONTEXT;

#endif

typedef struct _THREAD_INDEX_INFORMATION
{
    ULONG Index;
    ULONG Sequence;
} THREAD_INDEX_INFORMATION, *PTHREAD_INDEX_INFORMATION;


#if (PHNT_MODE != PHNT_MODE_KERNEL)

/**
 * Creates a new process.
 *
 * \param ProcessHandle A pointer to a handle that receives the process object handle.
 * \param DesiredAccess The access rights desired for the process object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * \param ParentProcess A handle to the parent process.
 * \param InheritObjectTable If TRUE, the new process inherits the object table of the parent process.
 * \param SectionHandle Optional. A handle to a section object to be used for the new process.
 * \param DebugPort Optional. A handle to a debug port to be used for the new process.
 * \param TokenHandle Optional. A handle to an access token to be used for the new process.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ParentProcess,
    _In_ BOOLEAN InheritObjectTable,
    _In_opt_ HANDLE SectionHandle,
    _In_opt_ HANDLE DebugPort,
    _In_opt_ HANDLE TokenHandle
    );

#define PROCESS_CREATE_FLAGS_NONE 0x00000000
#define PROCESS_CREATE_FLAGS_BREAKAWAY 0x00000001
#define PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT 0x00000002
#define PROCESS_CREATE_FLAGS_INHERIT_HANDLES 0x00000004
#define PROCESS_CREATE_FLAGS_OVERRIDE_ADDRESS_SPACE 0x00000008
#define PROCESS_CREATE_FLAGS_LARGE_PAGES 0x00000010
#define PROCESS_CREATE_FLAGS_LARGE_PAGE_SYSTEM_DLL 0x00000020
#define PROCESS_CREATE_FLAGS_PROTECTED_PROCESS 0x00000040
#define PROCESS_CREATE_FLAGS_CREATE_SESSION 0x00000080
#define PROCESS_CREATE_FLAGS_INHERIT_FROM_PARENT 0x00000100
#define PROCESS_CREATE_FLAGS_CREATE_SUSPENDED 0x00000200
#define PROCESS_CREATE_FLAGS_FORCE_BREAKAWAY 0x00000400
#define PROCESS_CREATE_FLAGS_MINIMAL_PROCESS 0x00000800
#define PROCESS_CREATE_FLAGS_RELEASE_SECTION 0x00001000
#define PROCESS_CREATE_FLAGS_CLONE_MINIMAL 0x00002000
#define PROCESS_CREATE_FLAGS_CLONE_MINIMAL_REDUCED_COMMIT 0x00004000
#define PROCESS_CREATE_FLAGS_AUXILIARY_PROCESS 0x00008000
#define PROCESS_CREATE_FLAGS_CREATE_STORE 0x00020000
#define PROCESS_CREATE_FLAGS_USE_PROTECTED_ENVIRONMENT 0x00040000
#define PROCESS_CREATE_FLAGS_IMAGE_EXPANSION_MITIGATION_DISABLE 0x00080000
#define PROCESS_CREATE_FLAGS_PARTITION_CREATE_SLAB_IDENTITY 0x00400000

/**
 * Creates a new process with extended options.
 *
 * \param ProcessHandle A pointer to a handle that receives the process object handle.
 * \param DesiredAccess The access rights desired for the process object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * \param ParentProcess A handle to the parent process.
 * \param Flags Flags that control the creation of the process. These flags are defined as PROCESS_CREATE_FLAGS_*.
 * \param SectionHandle Optional. A handle to a section object to be used for the new process.
 * \param DebugPort Optional. A handle to a debug port to be used for the new process.
 * \param TokenHandle Optional. A handle to an access token to be used for the new process.
 * \param Reserved Reserved for future use. Must be zero.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcessEx(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ParentProcess,
    _In_ ULONG Flags,
    _In_opt_ HANDLE SectionHandle,
    _In_opt_ HANDLE DebugPort,
    _In_opt_ HANDLE TokenHandle,
    _Reserved_ ULONG Reserved
    );

/**
 * Opens an existing process object.
 *
 * \param ProcessHandle A pointer to a handle that receives the process object handle.
 * \param DesiredAccess The access rights desired for the process object.
 * \param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * \param ClientId Optional. A pointer to a CLIENT_ID structure that specifies the client ID of the process to be opened.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-ntopenprocess
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenProcess(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
    );

/**
 * Terminates the specified process.
 *
 * \param ProcessHandle Optional. A handle to the process to be terminated. If this parameter is NULL, the calling process is terminated.
 * \param ExitStatus The exit status to be used by the process and the process's termination status.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-zwterminateprocess
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Suspends the specified process.
 *
 * \param ProcessHandle A handle to the process to be suspended.
 * \return NTSTATUS Successful or errant status.
 * \remarks Use NtCreateProcessStateChange instead.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSuspendProcess(
    _In_ HANDLE ProcessHandle
    );

/**
 * Resumes the specified process.
 *
 * \param ProcessHandle A handle to the process to be resumed.
 * \return NTSTATUS Successful or errant status.
 * \remarks Use NtCreateProcessStateChange instead.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtResumeProcess(
    _In_ HANDLE ProcessHandle
    );


#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#define ZwCurrentProcess() NtCurrentProcess()
#define NtCurrentThread() ((HANDLE)(LONG_PTR)-2)
#define ZwCurrentThread() NtCurrentThread()
#define NtCurrentSession() ((HANDLE)(LONG_PTR)-3)
#define ZwCurrentSession() NtCurrentSession()

#define NtCurrentPeb() (NtCurrentTeb()->ProcessEnvironmentBlock)

#define NtCurrentProcessId() (NtCurrentTeb()->ClientId.UniqueProcess)
#define NtCurrentThreadId() (NtCurrentTeb()->ClientId.UniqueThread)

#define NtCurrentProcessToken() ((HANDLE)(LONG_PTR)-4)
#define NtCurrentThreadToken() ((HANDLE)(LONG_PTR)-5)
#define NtCurrentThreadEffectiveToken() ((HANDLE)(LONG_PTR)-6)
#define NtCurrentSilo() ((HANDLE)(LONG_PTR)-1)

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define NtCurrentImageBase() ((PVOID)((PIMAGE_DOS_HEADER)&__ImageBase))

#define NtCurrentSessionId() (RtlGetActiveConsoleId())

/**
 * The NtQueryInformationProcess routine retrieves information about the specified process.
 *
 * \param ProcessHandle A handle to the process.
 * \param ProcessInformationClass The type of process information to be retrieved.
 * \param ProcessInformation A pointer to a buffer that receives the process information.
 * \param ProcessInformationLength The size of the buffer pointed to by the ProcessInformation parameter.
 * \param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationProcess(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * The NtWow64QueryInformationProcess64 routine retrieves information about the specified process.
 *
 * \param ProcessHandle A handle to the process.
 * \param ProcessInformationClass The type of process information to be retrieved.
 * \param ProcessInformation A pointer to a buffer that receives the process information.
 * \param ProcessInformationLength The size of the buffer pointed to by the ProcessInformation parameter.
 * \param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
NtWow64QueryInformationProcess64(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _Out_writes_bytes_(ProcessInformationLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * The NtSetInformationProcess routine sets information for the specified process.
 *
 * \param ProcessHandle A handle to the process.
 * \param ProcessInformationClass The type of process information to be set.
 * \param ProcessInformation A pointer to a buffer that contains the process information.
 * \param ProcessInformationLength The size of the buffer pointed to by the ProcessInformation parameter.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationProcess(
    _In_ HANDLE ProcessHandle,
    _In_ PROCESSINFOCLASS ProcessInformationClass,
    _In_reads_bytes_(ProcessInformationLength) PVOID ProcessInformation,
    _In_ ULONG ProcessInformationLength
    );

/**
 * The PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS flag retrieves the previous process in the system.
 *
 * When calling NtGetNextProcess, this flag can be specified in the Flags parameter to indicate
 * that the function should return the previous process in the system enumeration order,
 * rather than the next process. This can be useful for iterating through processes in reverse order.
 */
#ifndef PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS
#define PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS 0x00000001
#endif

/**
 * Retrieves a handle to the next process in the system.
 *
 * \param ProcessHandle An optional handle to a process. If this parameter is NULL, the function retrieves the first process in the system.
 * \param DesiredAccess The access rights desired for the new process handle.
 * \param HandleAttributes The attributes for the new process handle.
 * \param Flags Flags that modify the behavior of the function. This can be a combination of the following flags:
 * - \ref PROCESS_GET_NEXT_FLAGS_PREVIOUS_PROCESS (0x00000001): Retrieve the previous process in the system.
 * \param NewProcessHandle A pointer to a variable that receives the handle to the next process.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNextProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_ ULONG Flags,
    _Out_ PHANDLE NewProcessHandle
    );

/**
 * Retrieves a handle to the next thread in the system.
 *
 * \param ProcessHandle A handle to the process for enumeration of threads.
 * \param ThreadHandle An optional handle to a thread. If this parameter is NULL, the function retrieves the first thread in the process.
 * \param DesiredAccess The access rights desired for the new thread handle.
 * \param HandleAttributes The attributes for the new thread handle.
 * \param Flags Flags that modify the behavior of the function. Unused and should be zero.
 * \param NewThreadHandle A pointer to a variable that receives the handle to the next thread.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetNextThread(
    _In_ HANDLE ProcessHandle,
    _In_opt_ HANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG HandleAttributes,
    _In_opt_ _Reserved_ ULONG Flags,
    _Out_ PHANDLE NewThreadHandle
    );

#endif

#define STATECHANGE_SET_ATTRIBUTES 0x0001

typedef enum _PROCESS_STATE_CHANGE_TYPE
{
    ProcessStateChangeSuspend,
    ProcessStateChangeResume,
    ProcessStateChangeMax,
} PROCESS_STATE_CHANGE_TYPE, *PPROCESS_STATE_CHANGE_TYPE;

#if (PHNT_VERSION >= PHNT_WINDOWS_11)
/**
 * Creates a state change handle for changing the suspension state of a process.
 *
 * \param ProcessStateChangeHandle A pointer to a variable that receives the handle.
 * \param DesiredAccess The access rights desired for the handle.
 * \param ObjectAttributes Optional attributes for the handle.
 * \param ProcessHandle A handle to the process.
 * \param Reserved Reserved for future use.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateProcessStateChange(
    _Out_ PHANDLE ProcessStateChangeHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_opt_ _Reserved_ ULONG Reserved
    );

/**
 * Changes the suspension state of a process.
 *
 * \param ProcessStateChangeHandle A handle to the process state change object.
 * \param ProcessHandle A handle to the process.
 * \param StateChangeType The type of state change.
 * \param ExtendedInformation Optional extended information.
 * \param ExtendedInformationLength The length of the extended information.
 * \param Reserved Reserved for future use.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtChangeProcessState(
    _In_ HANDLE ProcessStateChangeHandle,
    _In_ HANDLE ProcessHandle,
    _In_ PROCESS_STATE_CHANGE_TYPE StateChangeType,
    _In_opt_ _Reserved_ PVOID ExtendedInformation,
    _In_opt_ _Reserved_ SIZE_T ExtendedInformationLength,
    _In_opt_ _Reserved_ ULONG Reserved
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_11)
/**
 * Creates a state change handle for changing the suspension state of a thread.
 *
 * \param ThreadStateChangeHandle A pointer to a variable that receives the handle.
 * \param DesiredAccess The access rights desired for the handle.
 * \param ObjectAttributes Optional attributes for the handle.
 * \param ThreadHandle A handle to the thread.
 * \param Reserved Reserved for future use.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThreadStateChange(
    _Out_ PHANDLE ThreadStateChangeHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ThreadHandle,
    _In_opt_ _Reserved_ ULONG Reserved
    );

typedef enum _THREAD_STATE_CHANGE_TYPE
{
    ThreadStateChangeSuspend,
    ThreadStateChangeResume,
    ThreadStateChangeMax,
} THREAD_STATE_CHANGE_TYPE, *PTHREAD_STATE_CHANGE_TYPE;

/**
 * Changes the suspension state of a thread.
 *
 * \param ThreadStateChangeHandle A handle to the thread state change object.
 * \param ThreadHandle A handle to the thread.
 * \param StateChangeType The type of state change.
 * \param ExtendedInformation Optional extended information.
 * \param ExtendedInformationLength The length of the extended information.
 * \param Reserved Reserved for future use.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtChangeThreadState(
    _In_ HANDLE ThreadStateChangeHandle,
    _In_ HANDLE ThreadHandle,
    _In_ THREAD_STATE_CHANGE_TYPE StateChangeType,
    _In_opt_ PVOID ExtendedInformation,
    _In_opt_ SIZE_T ExtendedInformationLength,
    _In_opt_ ULONG Reserved
    );
#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)
/**
 * Creates a new thread in the specified process.
 *
 * \param ThreadHandle A pointer to a handle that receives the thread object handle.
 * \param DesiredAccess The access rights desired for the thread object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \param ProcessHandle A handle to the process in which the thread is to be created.
 * \param ClientId A pointer to a CLIENT_ID structure that receives the client ID of the new thread.
 * \param ThreadContext A pointer to a CONTEXT structure that specifies the initial context of the new thread.
 * \param InitialTeb A pointer to an INITIAL_TEB structure that specifies the initial stack limits of the new thread.
 * \param CreateSuspended If TRUE, the thread is created in a suspended state.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThread(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _Out_ PCLIENT_ID ClientId,
    _In_ PCONTEXT ThreadContext,
    _In_ PINITIAL_TEB InitialTeb,
    _In_ BOOLEAN CreateSuspended
    );

/**
 * Opens an existing thread object.
 *
 * \param ThreadHandle A pointer to a handle that receives the thread object handle.
 * \param DesiredAccess The access rights desired for the thread object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \param ClientId Optional. A pointer to a CLIENT_ID structure that specifies the client ID of the thread to be opened.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenThread(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
    );

/**
 * Terminates the specified thread.
 *
 * \param ThreadHandle Optional. A handle to the thread to be terminated. If this parameter is NULL, the calling thread is terminated.
 * \param ExitStatus The exit status to be used by the thread and the thread's termination status.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateThread(
    _In_opt_ HANDLE ThreadHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Suspends the specified thread.
 *
 * \param ThreadHandle A handle to the thread to be suspended.
 * \param PreviousSuspendCount Optional. A pointer to a variable that receives the thread's previous suspend count.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSuspendThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * Resumes the specified thread.
 *
 * \param ThreadHandle A handle to the thread to be resumed.
 * \param PreviousSuspendCount Optional. A pointer to a variable that receives the thread's previous suspend count.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtResumeThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * Retrieves the number of the current processor.
 *
 * \return ULONG The number of the current processor.
 * \sa https://learn.microsoft.com/en-us/windows/win32/procthread/ntgetcurrentprocessornumber
 */
NTSYSCALLAPI
ULONG
NTAPI
NtGetCurrentProcessorNumber(
    VOID
    );

/**
 * Retrieves the number of the current processor.
 *
 * \param ProcessorNumber An optional pointer to a PROCESSOR_NUMBER structure that receives the processor number.
 * \return ULONG The number of the current processor.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nf-ntddk-kegetcurrentprocessornumberex
 */
NTSYSCALLAPI
ULONG
NTAPI
NtGetCurrentProcessorNumberEx(
    _Out_opt_ PPROCESSOR_NUMBER ProcessorNumber
    );

/**
 * Retrieves the context of the specified thread.
 *
 * \param ThreadHandle A handle to the thread.
 * \param ThreadContext A pointer to a CONTEXT structure that receives the thread context.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetContextThread(
    _In_ HANDLE ThreadHandle,
    _Inout_ PCONTEXT ThreadContext
    );

/**
 * Sets the context of the specified thread.
 *
 * \param ThreadHandle A handle to the thread.
 * \param ThreadContext A pointer to a CONTEXT structure that specifies the thread context.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetContextThread(
    _In_ HANDLE ThreadHandle,
    _In_ PCONTEXT ThreadContext
    );

/**
 * Retrieves information about the specified thread.
 *
 * \param ThreadHandle A handle to the thread.
 * \param ThreadInformationClass The type of thread information to be retrieved.
 * \param ThreadInformation A pointer to a buffer that receives the thread information.
 * \param ThreadInformationLength The size of the buffer pointed to by the ThreadInformation parameter.
 * \param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationThread(
    _In_ HANDLE ThreadHandle,
    _In_ THREADINFOCLASS ThreadInformationClass,
    _Out_writes_bytes_(ThreadInformationLength) PVOID ThreadInformation,
    _In_ ULONG ThreadInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * Sets information for the specified thread.
 *
 * \param ThreadHandle A handle to the thread.
 * \param ThreadInformationClass The type of thread information to be set.
 * \param ThreadInformation A pointer to a buffer that contains the thread information.
 * \param ThreadInformationLength The size of the buffer pointed to by the ThreadInformation parameter.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationThread(
    _In_ HANDLE ThreadHandle,
    _In_ THREADINFOCLASS ThreadInformationClass,
    _In_reads_bytes_(ThreadInformationLength) PVOID ThreadInformation,
    _In_ ULONG ThreadInformationLength
    );

/**
 * The NtAlertThread routine alerts the specified thread.
 *
 * \param[in] ThreadHandle A handle to the thread to be alerted.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThread(
    _In_ HANDLE ThreadHandle
    );

/**
 * The NtAlertResumeThread routine resumes a specified thread that was previously suspended and alerts the thread.
 *
 * \param[in] ThreadHandle A handle to the thread to be resumed and alerted.
 * \param[out, optional] PreviousSuspendCount An optional pointer to a variable that receives the thread's previous suspend count.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertResumeThread(
    _In_ HANDLE ThreadHandle,
    _Out_opt_ PULONG PreviousSuspendCount
    );

/**
 * The NtTestAlert routine indicates whether the current thread has an alert pending
 * and executes asynchronous procedure calls (APCs) queued to the current thread.
 *
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTestAlert(
    VOID
    );

#if (PHNT_VERSION >= PHNT_WINDOWS_8)
/**
 * The NtAlertThreadByThreadId routine sends an alert to the specified thread.
 *
 * \param ThreadId The thread ID of the thread to be alerted.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThreadByThreadId(
    _In_ HANDLE ThreadId
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_11)
/**
 * The NtAlertThreadByThreadIdEx routine sends an alert to the specified thread by its thread ID, with an optional lock.
 *
 * \param ThreadId The thread ID of the thread to be alerted.
 * \param Lock An optional pointer to an SRW lock to be used during the alert.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertThreadByThreadIdEx(
    _In_ HANDLE ThreadId,
    _In_opt_ PRTL_SRWLOCK Lock
    );

/**
 * Identifies the interpretation of a PS_ALERT_THREAD_EXTENDED_PARAMETER record.
 *
 * The current kernel implementation for NtAlertMultipleThreadByThreadId accepts only
 * PsAlertMultipleExtendedParameterAutoBoostContext (0). Other values are rejected with
 * STATUS_INVALID_PARAMETER.
 */
typedef enum _PS_ALERT_THREAD_EXTENDED_PARAMETER_TYPE
{
    /**
     * AutoBoost context token passed into the kernel's AutoBoost (Ab) bookkeeping.
     *
     * The payload (ULong64/Pointer/etc.) is forwarded to internal routines and may be used to
     * associate the wakeup with a particular synchronization/ownership handoff context.
     */
    PsAlertMultipleExtendedParameterAutoBoostContext = 0,
    /**
     * Sentinel / upper bound (not a usable type).
     */
    PsAlertMultipleExtendedParameterMax = 1,
} PS_ALERT_THREAD_EXTENDED_PARAMETER_TYPE, *PPS_ALERT_THREAD_EXTENDED_PARAMETER_TYPE;

/**
 * Extended parameter record for NtAlertMultipleThreadByThreadId.
 *
 * In the analyzed NtAlertMultipleThreadByThreadId implementation:
 * - Type must be 0 (PsAlertMultipleExtendedParameterAutoBoostContext).
 * - The union payload at offset +0x8 is forwarded as an opaque 64-bit context value.
 */
typedef struct _PS_ALERT_THREAD_EXTENDED_PARAMETER
{
    /**
     * Parameter type (8-bit)
     */
    ULONGLONG Type : 8;
    /**
     * Reserved bits; should be zero.
     */
    ULONGLONG Reserved : 56;
    /**
     * Payload value whose meaning depends on Type.
     *
     * For Type == PsAlertMultipleExtendedParameterAutoBoostContext, this is treated as an
     * opaque "AutoBoostContext" token. It may be pointer-like and may use low-bit tagging.
     */
    union
    {
        ULONGLONG ULong64;
        PVOID Pointer;
        SIZE_T Size;
        HANDLE Handle;
        ULONG ULong;
        UCHAR Boolean;
    };
} PS_ALERT_THREAD_EXTENDED_PARAMETER, *PPS_ALERT_THREAD_EXTENDED_PARAMETER;

/**
 * The NtAlertMultipleThreadByThreadId routine alerts each target thread specified by thread ID.
 * Target threads must belong to the caller's current process (cross-process targets fail STATUS_ACCESS_DENIED).
 *
 * \param MultipleThreadId A pointer to an array of thread IDs to be alerted.
 * \param Count The number of thread IDs in the array.
 * \param ExtendedParameters An optional pointer to one or more extended parameters of type PS_ALERT_THREAD_EXTENDED_PARAMETER.
 * \param ExtendedParameterCount Specifies the number of elements in the ExtendedParameters array.
 * \return NTSTATUS Successful or errant status.
 * \remarks STATUS_ACCESS_DENIED may be returned when the thread belongs to another process.
 * \note
 * - Only PS_ALERT_THREAD_EXTENDED_PARAMETER.Type == 0 is currently accepted.
 * - If multiple extended parameters are provided, the implementation consumes them in order and
 *   the *last* parameter's payload is the one forwarded to the internal alert logic.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAlertMultipleThreadByThreadId(
    _In_ PHANDLE MultipleThreadId,
    _In_ ULONG Count,
    _Inout_updates_opt_(ExtendedParameterCount) PPS_ALERT_THREAD_EXTENDED_PARAMETER ExtendedParameters,
    _In_ ULONG ExtendedParameterCount
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_8)
/**
 * The NtWaitForAlertByThreadId routine blocks the calling thread until another thread calls NtAlertThreadByThreadId
 * with a matching address, or until the timeout expires.
 *
 * \param Address A unique address used to identify this wait operation. Other threads call
 *                NtAlertThreadByThreadId with this same address to wake the waiting thread.
 *                Can be NULL to wait on the thread ID itself.
 * \param Timeout Optional timeout value. If NULL, waits indefinitely. If present, specifies
 *                the absolute or relative time to wait before returning STATUS_TIMEOUT.
 * \return STATUS_SUCCESS if alerted successfully.
 * \return STATUS_TIMEOUT if the timeout expired before being alerted.
 * \return STATUS_ALERTED if woken by NtAlertThreadByThreadId.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWaitForAlertByThreadId(
    _In_opt_ PVOID Address,
    _In_opt_ PLARGE_INTEGER Timeout
    );
#endif

/**
 * Impersonates a client thread.
 *
 * \param ServerThreadHandle A handle to the server thread.
 * \param ClientThreadHandle A handle to the client thread.
 * \param SecurityQos A pointer to a SECURITY_QUALITY_OF_SERVICE structure that specifies the impersonation level and context tracking mode.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtImpersonateThread(
    _In_ HANDLE ServerThreadHandle,
    _In_ HANDLE ClientThreadHandle,
    _In_ PSECURITY_QUALITY_OF_SERVICE SecurityQos
    );

/**
 * Registers a thread termination port.
 *
 * \param PortHandle A handle to the port to be registered.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRegisterThreadTerminatePort(
    _In_ HANDLE PortHandle
    );

/**
 * Sets LDT (Local Descriptor Table) entries.
 *
 * \param Selector0 The first selector.
 * \param Entry0Low The low part of the first entry.
 * \param Entry0Hi The high part of the first entry.
 * \param Selector1 The second selector.
 * \param Entry1Low The low part of the second entry.
 * \param Entry1Hi The high part of the second entry.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetLdtEntries(
    _In_ ULONG Selector0,
    _In_ ULONG Entry0Low,
    _In_ ULONG Entry0Hi,
    _In_ ULONG Selector1,
    _In_ ULONG Entry1Low,
    _In_ ULONG Entry1Hi
    );

/**
 * Dispatches the Asynchronous Procedure Call (APC) from the NtQueueApc* functions to the specified routine.
 *
 * \param ApcRoutine A pointer to the APC routine to be executed.
 * \param Parameter Optional. A pointer to a parameter to be passed to the APC routine.
 * \param ActxContext Optional. A handle to an activation context.
 */
NTSYSAPI
VOID
NTAPI
RtlDispatchAPC(
    _In_ PAPCFUNC ApcRoutine,
    _In_opt_ PVOID Parameter,
    _In_opt_ HANDLE ActxContext
    );

/**
 * A pointer to a function that serves as an APC routine.
 *
 * \param ApcArgument1 Optional. A pointer to the first argument to be passed to the APC routine.
 * \param ApcArgument2 Optional. A pointer to the second argument to be passed to the APC routine.
 * \param ApcArgument3 Optional. A pointer to the third argument to be passed to the APC routine.
 */
typedef _Function_class_(PS_APC_ROUTINE)
VOID NTAPI PS_APC_ROUTINE(
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );
typedef PS_APC_ROUTINE* PPS_APC_ROUTINE;

/**
 * Encodes an APC routine pointer for use in a WOW64 environment.
 *
 * \param ApcRoutine The APC routine pointer to be encoded.
 * \return PVOID The encoded APC routine pointer.
 */
#define Wow64EncodeApcRoutine(ApcRoutine) \
    ((PVOID)((0 - ((LONG_PTR)(ApcRoutine))) << 2))

/**
 * Decodes an APC routine pointer that was encoded for use in a WOW64 environment.
 *
 * \param ApcRoutine The encoded APC routine pointer to be decoded.
 * \return PVOID The decoded APC routine pointer.
 */
#define Wow64DecodeApcRoutine(ApcRoutine) \
    ((PVOID)(0 - (((LONG_PTR)(ApcRoutine)) >> 2)))

/**
 * Queues an APC (Asynchronous Procedure Call) to a thread.
 *
 * \param ThreadHandle Handle to the thread to which the APC is to be queued.
 * \param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * \param ApcArgument1 Optional first argument to be passed to the APC routine.
 * \param ApcArgument2 Optional second argument to be passed to the APC routine.
 * \param ApcArgument3 Optional third argument to be passed to the APC routine.
 * \return NTSTATUS Successful or errant status.
 * \remarks The APC will be executed in the context of the specified thread when the thread enters an alertable wait state or when any
 * process calls the NtTestAlert, NtAlertThread, NtAlertResumeThread or NtAlertThreadByThreadId functions.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThread(
    _In_ HANDLE ThreadHandle,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

/**
 * A special handle value used to queue a user APC (Asynchronous Procedure Call).
 */
#define QUEUE_USER_APC_SPECIAL_USER_APC ((HANDLE)0x1)

/**
 * Queues an APC (Asynchronous Procedure Call) to a thread.
 *
 * \param ThreadHandle Handle to the thread to which the APC is to be queued.
 * \param ReserveHandle Optional handle to a reserve object. This can be QUEUE_USER_APC_SPECIAL_USER_APC or a handle returned by NtAllocateReserveObject.
 * \param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * \param ApcArgument1 Optional first argument to be passed to the APC routine.
 * \param ApcArgument2 Optional second argument to be passed to the APC routine.
 * \param ApcArgument3 Optional third argument to be passed to the APC routine.
 * \return NTSTATUS Successful or errant status.
 * \remarks The APC will be executed in the context of the specified thread after the thread enters an alertable wait state or immediately
 * when QUEUE_USER_APC_SPECIAL_USER_APC is used or NtTestAlert, NtAlertThread, NtAlertResumeThread or NtAlertThreadByThreadId are called.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThreadEx(
    _In_ HANDLE ThreadHandle,
    _In_opt_ HANDLE ReserveHandle,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

/**
 * The APC_CALLBACK_DATA_CONTEXT structure is used to pass information to the APC callback routine.
 */
typedef struct _APC_CALLBACK_DATA_CONTEXT
{
    ULONG_PTR Parameter;
    PCONTEXT ContextRecord;
    ULONG_PTR Reserved0;
    ULONG_PTR Reserved1;
} APC_CALLBACK_DATA_CONTEXT, *PAPC_CALLBACK_DATA_CONTEXT;

#define QUEUE_USER_APC_FLAGS_NONE 0x00000000
#define QUEUE_USER_APC_FLAGS_SPECIAL_USER_APC 0x00000001
#define QUEUE_USER_APC_FLAGS_CALLBACK_DATA_CONTEXT 0x00010000

#if (PHNT_VERSION >= PHNT_WINDOWS_11)
/**
 * Queues an Asynchronous Procedure Call (APC) to a specified thread.
 *
 * \param ThreadHandle A handle to the thread to which the APC is to be queued.
 * \param ReserveHandle An optional handle to a reserve object. This can be obtained using NtAllocateReserveObject.
 * \param ApcFlags Flags that control the behavior of the APC. These flags are defined in QUEUE_USER_APC_FLAGS.
 * \param ApcRoutine A pointer to the RtlDispatchAPC function or custom APC routine to be executed.
 * \param ApcArgument1 An optional argument to be passed to the APC routine.
 * \param ApcArgument2 An optional argument to be passed to the APC routine.
 * \param ApcArgument3 An optional argument to be passed to the APC routine.
 * \return NTSTATUS Successful or errant status.
 * \remarks The APC will be executed in the context of the specified thread when the thread enters an alertable wait state or immediately
 * when QUEUE_USER_APC_SPECIAL_USER_APC is used or any process calls the NtTestAlert, NtAlertThread,
 * NtAlertResumeThread or NtAlertThreadByThreadId functions.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueueApcThreadEx2(
    _In_ HANDLE ThreadHandle,
    _In_opt_ HANDLE ReserveHandle,
    _In_ ULONG ApcFlags,
    _In_ PPS_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcArgument1,
    _In_opt_ PVOID ApcArgument2,
    _In_opt_ PVOID ApcArgument3
    );

#endif

#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)


#define ProcThreadAttributeParentProcess 0
#define ProcThreadAttributeExtendedFlags 1
#define ProcThreadAttributeHandleList 2
#define ProcThreadAttributeGroupAffinity 3
#define ProcThreadAttributePreferredNode 4
#define ProcThreadAttributeIdealProcessor 5
#define ProcThreadAttributeUmsThread 6
#define ProcThreadAttributeMitigationPolicy 7
#define ProcThreadAttributePackageFullName 8
#define ProcThreadAttributeSecurityCapabilities 9
#define ProcThreadAttributeConsoleReference 10
#define ProcThreadAttributeProtectionLevel 11
#define ProcThreadAttributeOsMaxVersionTested 12
#define ProcThreadAttributeJobList 13
#define ProcThreadAttributeChildProcessPolicy 14
#define ProcThreadAttributeAllApplicationPackagesPolicy 15
#define ProcThreadAttributeWin32kFilter 16
#define ProcThreadAttributeSafeOpenPromptOriginClaim 17
#define ProcThreadAttributeDesktopAppPolicy 18
#define ProcThreadAttributeBnoIsolation 19
#define ProcThreadAttributePseudoConsole 22
#define ProcThreadAttributeIsolationManifest 23
#define ProcThreadAttributeMitigationAuditPolicy 24
#define ProcThreadAttributeMachineType 25
#define ProcThreadAttributeComponentFilter 26
#define ProcThreadAttributeEnableOptionalXStateFeatures 27
#define ProcThreadAttributeCreateStore 28
#define ProcThreadAttributeTrustedApp 29
#define ProcThreadAttributeSveVectorLength 30
#define ProcThreadAttributeSmeVectorLength 31

#ifndef PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS
#define PROC_THREAD_ATTRIBUTE_EXTENDED_FLAGS \
    ProcThreadAttributeValue(ProcThreadAttributeExtendedFlags, FALSE, TRUE, TRUE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_PACKAGE_FULL_NAME
#define PROC_THREAD_ATTRIBUTE_PACKAGE_FULL_NAME \
    ProcThreadAttributeValue(ProcThreadAttributePackageFullName, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_CONSOLE_REFERENCE
#define PROC_THREAD_ATTRIBUTE_CONSOLE_REFERENCE \
    ProcThreadAttributeValue(ProcThreadAttributeConsoleReference, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_OSMAXVERSIONTESTED
#define PROC_THREAD_ATTRIBUTE_OSMAXVERSIONTESTED \
    ProcThreadAttributeValue(ProcThreadAttributeOsMaxVersionTested, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM
#define PROC_THREAD_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM \
    ProcThreadAttributeValue(ProcThreadAttributeSafeOpenPromptOriginClaim, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_BNO_ISOLATION
#define PROC_THREAD_ATTRIBUTE_BNO_ISOLATION \
    ProcThreadAttributeValue(ProcThreadAttributeBnoIsolation, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_ISOLATION_MANIFEST
#define PROC_THREAD_ATTRIBUTE_ISOLATION_MANIFEST \
    ProcThreadAttributeValue(ProcThreadAttributeIsolationManifest, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_CREATE_STORE
#define PROC_THREAD_ATTRIBUTE_CREATE_STORE \
    ProcThreadAttributeValue(ProcThreadAttributeCreateStore, FALSE, TRUE, FALSE)
#endif
#ifndef PROC_THREAD_ATTRIBUTE_TRUSTED_APP
#define PROC_THREAD_ATTRIBUTE_TRUSTED_APP \
    ProcThreadAttributeValue(ProcThreadAttributeTrustedApp, FALSE, TRUE, FALSE)
#endif

typedef struct _PROC_THREAD_ATTRIBUTE
{
    ULONG_PTR Attribute;
    SIZE_T Size;
    ULONG_PTR Value;
} PROC_THREAD_ATTRIBUTE, *PPROC_THREAD_ATTRIBUTE;

/**
 * The PROC_THREAD_ATTRIBUTE_LIST structure contains the list of attributes for process and thread creation.
 */
typedef struct _PROC_THREAD_ATTRIBUTE_LIST
{
    ULONG PresentFlags;
    ULONG AttributeCount;
    ULONG LastAttribute;
    ULONG SpareUlong0;
    PPROC_THREAD_ATTRIBUTE ExtendedFlagsAttribute;
    _Field_size_(AttributeCount) PROC_THREAD_ATTRIBUTE Attributes[1];
} PROC_THREAD_ATTRIBUTE_LIST, *PPROC_THREAD_ATTRIBUTE_LIST;

#define EXTENDED_PROCESS_CREATION_FLAG_ELEVATION_HANDLED 0x00000001
#define EXTENDED_PROCESS_CREATION_FLAG_FORCELUA 0x00000002
#define EXTENDED_PROCESS_CREATION_FLAG_FORCE_BREAKAWAY 0x00000004

#define PROTECTION_LEVEL_WINTCB_LIGHT       0x00000000
#define PROTECTION_LEVEL_WINDOWS            0x00000001
#define PROTECTION_LEVEL_WINDOWS_LIGHT      0x00000002
#define PROTECTION_LEVEL_ANTIMALWARE_LIGHT  0x00000003
#define PROTECTION_LEVEL_LSA_LIGHT          0x00000004
#define PROTECTION_LEVEL_WINTCB             0x00000005
#define PROTECTION_LEVEL_CODEGEN_LIGHT      0x00000006
#define PROTECTION_LEVEL_AUTHENTICODE       0x00000007
#define PROTECTION_LEVEL_PPL_APP            0x00000008

#define PROTECTION_LEVEL_SAME               0xFFFFFFFF
#define PROTECTION_LEVEL_NONE               0xFFFFFFFE

typedef enum _SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS
{
    SeSafeOpenExperienceNone = 0x00,
    SeSafeOpenExperienceCalled = 0x01,
    SeSafeOpenExperienceAppRepCalled = 0x02,
    SeSafeOpenExperiencePromptDisplayed = 0x04,
    SeSafeOpenExperienceUAC = 0x08,
    SeSafeOpenExperienceUninstaller = 0x10,
    SeSafeOpenExperienceIgnoreUnknownOrBad = 0x20,
    SeSafeOpenExperienceDefenderTrustedInstaller = 0x40,
    SeSafeOpenExperienceMOTWPresent = 0x80,
    SeSafeOpenExperienceElevatedNoPropagation = 0x100
} SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS;

typedef struct _SE_SAFE_OPEN_PROMPT_RESULTS
{
    SE_SAFE_OPEN_PROMPT_EXPERIENCE_RESULTS Results;
    WCHAR Path[MAX_PATH];
} SE_SAFE_OPEN_PROMPT_RESULTS, *PSE_SAFE_OPEN_PROMPT_RESULTS;

typedef struct _PROC_THREAD_BNOISOLATION_ATTRIBUTE
{
    BOOL IsolationEnabled;
    WCHAR IsolationPrefix[0x88];
} PROC_THREAD_BNOISOLATION_ATTRIBUTE, *PPROC_THREAD_BNOISOLATION_ATTRIBUTE;

typedef struct _ISOLATION_MANIFEST_PROPERTIES
{
    UNICODE_STRING InstancePath;
    UNICODE_STRING FriendlyName;
    UNICODE_STRING Description;
    ULONG_PTR Level;
} ISOLATION_MANIFEST_PROPERTIES, *PISOLATION_MANIFEST_PROPERTIES;


typedef enum _PS_ATTRIBUTE_NUM
{
    PsAttributeParentProcess,
    PsAttributeDebugObject,
    PsAttributeToken,
    PsAttributeClientId,
    PsAttributeTebAddress,
    PsAttributeImageName,
    PsAttributeImageInfo,
    PsAttributeMemoryReserve,
    PsAttributePriorityClass,
    PsAttributeErrorMode,
    PsAttributeStdHandleInfo,
    PsAttributeHandleList,
    PsAttributeGroupAffinity,
    PsAttributePreferredNode,
    PsAttributeIdealProcessor,
    PsAttributeUmsThread,
    PsAttributeMitigationOptions,
    PsAttributeProtectionLevel,
    PsAttributeSecureProcess,
    PsAttributeJobList,
    PsAttributeChildProcessPolicy,
    PsAttributeAllApplicationPackagesPolicy,
    PsAttributeWin32kFilter,
    PsAttributeSafeOpenPromptOriginClaim,
    PsAttributeBnoIsolation,
    PsAttributeDesktopAppPolicy,
    PsAttributeChpe,
    PsAttributeMitigationAuditOptions,
    PsAttributeMachineType,
    PsAttributeComponentFilter,
    PsAttributeEnableOptionalXStateFeatures,
    PsAttributeSupportedMachines,
    PsAttributeSveVectorLength,
    PsAttributeMax
} PS_ATTRIBUTE_NUM;

#define PS_ATTRIBUTE_NUMBER_MASK 0x0000ffff
#define PS_ATTRIBUTE_THREAD 0x00010000
#define PS_ATTRIBUTE_INPUT 0x00020000
#define PS_ATTRIBUTE_ADDITIVE 0x00040000


#define PsAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | \
    ((Input) ? PS_ATTRIBUTE_INPUT : 0) | \
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))

#define PS_ATTRIBUTE_PARENT_PROCESS \
    PsAttributeValue(PsAttributeParentProcess, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_DEBUG_OBJECT \
    PsAttributeValue(PsAttributeDebugObject, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_TOKEN \
    PsAttributeValue(PsAttributeToken, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_CLIENT_ID \
    PsAttributeValue(PsAttributeClientId, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_TEB_ADDRESS \
    PsAttributeValue(PsAttributeTebAddress, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_IMAGE_NAME \
    PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IMAGE_INFO \
    PsAttributeValue(PsAttributeImageInfo, FALSE, FALSE, FALSE)
#define PS_ATTRIBUTE_MEMORY_RESERVE \
    PsAttributeValue(PsAttributeMemoryReserve, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_PRIORITY_CLASS \
    PsAttributeValue(PsAttributePriorityClass, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ERROR_MODE \
    PsAttributeValue(PsAttributeErrorMode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_STD_HANDLE_INFO \
    PsAttributeValue(PsAttributeStdHandleInfo, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_HANDLE_LIST \
    PsAttributeValue(PsAttributeHandleList, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_GROUP_AFFINITY \
    PsAttributeValue(PsAttributeGroupAffinity, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_PREFERRED_NODE \
    PsAttributeValue(PsAttributePreferredNode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IDEAL_PROCESSOR \
    PsAttributeValue(PsAttributeIdealProcessor, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_UMS_THREAD \
    PsAttributeValue(PsAttributeUmsThread, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_MITIGATION_OPTIONS \
    PsAttributeValue(PsAttributeMitigationOptions, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_PROTECTION_LEVEL \
    PsAttributeValue(PsAttributeProtectionLevel, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_SECURE_PROCESS \
    PsAttributeValue(PsAttributeSecureProcess, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_JOB_LIST \
    PsAttributeValue(PsAttributeJobList, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_CHILD_PROCESS_POLICY \
    PsAttributeValue(PsAttributeChildProcessPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ALL_APPLICATION_PACKAGES_POLICY \
    PsAttributeValue(PsAttributeAllApplicationPackagesPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_WIN32K_FILTER \
    PsAttributeValue(PsAttributeWin32kFilter, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_SAFE_OPEN_PROMPT_ORIGIN_CLAIM \
    PsAttributeValue(PsAttributeSafeOpenPromptOriginClaim, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_BNO_ISOLATION \
    PsAttributeValue(PsAttributeBnoIsolation, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_DESKTOP_APP_POLICY \
    PsAttributeValue(PsAttributeDesktopAppPolicy, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_CHPE \
    PsAttributeValue(PsAttributeChpe, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_MITIGATION_AUDIT_OPTIONS \
    PsAttributeValue(PsAttributeMitigationAuditOptions, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_MACHINE_TYPE \
    PsAttributeValue(PsAttributeMachineType, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_COMPONENT_FILTER \
    PsAttributeValue(PsAttributeComponentFilter, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ENABLE_OPTIONAL_XSTATE_FEATURES \
    PsAttributeValue(PsAttributeEnableOptionalXStateFeatures, TRUE, TRUE, FALSE)



typedef struct _PS_ATTRIBUTE
{
    ULONG_PTR Attribute;
    SIZE_T Size;
    union
    {
        ULONG_PTR Value;
        PVOID ValuePtr;
    };
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

_Struct_size_bytes_(TotalLength)
typedef struct _PS_ATTRIBUTE_LIST
{
    SIZE_T TotalLength;
    PS_ATTRIBUTE Attributes[1];
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

typedef struct _PS_MEMORY_RESERVE
{
    PVOID ReserveAddress;
    SIZE_T ReserveSize;
} PS_MEMORY_RESERVE, *PPS_MEMORY_RESERVE;

typedef enum _PS_STD_HANDLE_STATE
{
    PsNeverDuplicate,
    PsRequestDuplicate,
    PsAlwaysDuplicate,
    PsMaxStdHandleStates
} PS_STD_HANDLE_STATE;

#define PS_STD_INPUT_HANDLE 0x1
#define PS_STD_OUTPUT_HANDLE 0x2
#define PS_STD_ERROR_HANDLE 0x4

typedef struct _PS_STD_HANDLE_INFO
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG StdHandleState : 2;
            ULONG PseudoHandleMask : 3;
        };
    };
    ULONG StdHandleSubsystemType;
} PS_STD_HANDLE_INFO, *PPS_STD_HANDLE_INFO;

typedef union _PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS
{
    UCHAR Trustlet : 1;
    UCHAR Ntos : 1;
    UCHAR WriteHandle : 1;
    UCHAR ReadHandle : 1;
    UCHAR Reserved : 4;
    UCHAR AccessRights;
} PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS, *PPS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS;

typedef union _PS_TRUSTLET_ATTRIBUTE_TYPE
{
    ULONG AttributeType;
    struct
    {
        UCHAR Version;
        UCHAR DataCount;
        UCHAR SemanticType;
        PS_TRUSTLET_ATTRIBUTE_ACCESSRIGHTS AccessRights;
    } DUMMYSTRUCTNAME;
} PS_TRUSTLET_ATTRIBUTE_TYPE, *PPS_TRUSTLET_ATTRIBUTE_TYPE;

typedef struct _PS_TRUSTLET_ATTRIBUTE_HEADER
{
    PS_TRUSTLET_ATTRIBUTE_TYPE AttributeType;
    ULONG InstanceNumber : 8;
    ULONG Reserved : 24;
} PS_TRUSTLET_ATTRIBUTE_HEADER, *PPS_TRUSTLET_ATTRIBUTE_HEADER;

typedef struct _PS_TRUSTLET_ATTRIBUTE_DATA
{
    PS_TRUSTLET_ATTRIBUTE_HEADER Header;
    ULONGLONG Data[1];
} PS_TRUSTLET_ATTRIBUTE_DATA, *PPS_TRUSTLET_ATTRIBUTE_DATA;

typedef struct _PS_TRUSTLET_CREATE_ATTRIBUTES
{
    ULONGLONG TrustletIdentity;
    PS_TRUSTLET_ATTRIBUTE_DATA Attributes[1];
} PS_TRUSTLET_CREATE_ATTRIBUTES, *PPS_TRUSTLET_CREATE_ATTRIBUTES;

typedef struct _PS_BNO_ISOLATION_PARAMETERS
{
    UNICODE_STRING IsolationPrefix;
    ULONG HandleCount;
    PVOID *Handles;
    BOOLEAN IsolationEnabled;
} PS_BNO_ISOLATION_PARAMETERS, *PPS_BNO_ISOLATION_PARAMETERS;

typedef union _PS_PROCESS_CREATION_SVE_VECTOR_LENGTH
{
    ULONG VectorLength : 24;
    ULONG FlagsReserved : 8;
} PS_PROCESS_CREATION_SVE_VECTOR_LENGTH, *PPS_PROCESS_CREATION_SVE_VECTOR_LENGTH;

typedef enum _PS_MITIGATION_OPTION
{
    PS_MITIGATION_OPTION_NX,
    PS_MITIGATION_OPTION_SEHOP,
    PS_MITIGATION_OPTION_FORCE_RELOCATE_IMAGES,
    PS_MITIGATION_OPTION_HEAP_TERMINATE,
    PS_MITIGATION_OPTION_BOTTOM_UP_ASLR,
    PS_MITIGATION_OPTION_HIGH_ENTROPY_ASLR,
    PS_MITIGATION_OPTION_STRICT_HANDLE_CHECKS,
    PS_MITIGATION_OPTION_WIN32K_SYSTEM_CALL_DISABLE,
    PS_MITIGATION_OPTION_EXTENSION_POINT_DISABLE,
    PS_MITIGATION_OPTION_PROHIBIT_DYNAMIC_CODE,
    PS_MITIGATION_OPTION_CONTROL_FLOW_GUARD,
    PS_MITIGATION_OPTION_BLOCK_NON_MICROSOFT_BINARIES,
    PS_MITIGATION_OPTION_FONT_DISABLE,
    PS_MITIGATION_OPTION_IMAGE_LOAD_NO_REMOTE,
    PS_MITIGATION_OPTION_IMAGE_LOAD_NO_LOW_LABEL,
    PS_MITIGATION_OPTION_IMAGE_LOAD_PREFER_SYSTEM32,
    PS_MITIGATION_OPTION_RETURN_FLOW_GUARD,
    PS_MITIGATION_OPTION_LOADER_INTEGRITY_CONTINUITY,
    PS_MITIGATION_OPTION_STRICT_CONTROL_FLOW_GUARD,
    PS_MITIGATION_OPTION_RESTRICT_SET_THREAD_CONTEXT,
    PS_MITIGATION_OPTION_ROP_STACKPIVOT,
    PS_MITIGATION_OPTION_ROP_CALLER_CHECK,
    PS_MITIGATION_OPTION_ROP_SIMEXEC,
    PS_MITIGATION_OPTION_EXPORT_ADDRESS_FILTER,
    PS_MITIGATION_OPTION_EXPORT_ADDRESS_FILTER_PLUS,
    PS_MITIGATION_OPTION_RESTRICT_CHILD_PROCESS_CREATION,
    PS_MITIGATION_OPTION_IMPORT_ADDRESS_FILTER,
    PS_MITIGATION_OPTION_MODULE_TAMPERING_PROTECTION,
    PS_MITIGATION_OPTION_RESTRICT_INDIRECT_BRANCH_PREDICTION,
    PS_MITIGATION_OPTION_SPECULATIVE_STORE_BYPASS_DISABLE,
    PS_MITIGATION_OPTION_ALLOW_DOWNGRADE_DYNAMIC_CODE_POLICY,
    PS_MITIGATION_OPTION_CET_USER_SHADOW_STACKS,
    PS_MITIGATION_OPTION_USER_CET_SET_CONTEXT_IP_VALIDATION,
    PS_MITIGATION_OPTION_BLOCK_NON_CET_BINARIES,
    PS_MITIGATION_OPTION_CET_DYNAMIC_APIS_OUT_OF_PROC_ONLY,
    PS_MITIGATION_OPTION_REDIRECTION_TRUST,
    PS_MITIGATION_OPTION_RESTRICT_CORE_SHARING,
    PS_MITIGATION_OPTION_FSCTL_SYSTEM_CALL_DISABLE,
} PS_MITIGATION_OPTION;

typedef enum _PS_CREATE_STATE
{
    PsCreateInitialState,
    PsCreateFailOnFileOpen,
    PsCreateFailOnSectionCreate,
    PsCreateFailExeFormat,
    PsCreateFailMachineMismatch,
    PsCreateFailExeName,
    PsCreateSuccess,
    PsCreateMaximumStates
} PS_CREATE_STATE;

_Struct_size_bytes_(Size)
typedef struct _PS_CREATE_INFO
{
    SIZE_T Size;
    PS_CREATE_STATE State;
    union
    {
        struct
        {
            union
            {
                ULONG InitFlags;
                struct
                {
                    UCHAR WriteOutputOnExit : 1;
                    UCHAR DetectManifest : 1;
                    UCHAR IFEOSkipDebugger : 1;
                    UCHAR IFEODoNotPropagateKeyState : 1;
                    UCHAR SpareBits1 : 4;
                    UCHAR SpareBits2 : 8;
                    USHORT ProhibitedImageCharacteristics : 16;
                };
            };
            ACCESS_MASK AdditionalFileAccess;
        } InitState;

        struct
        {
            HANDLE FileHandle;
        } FailSection;

        struct
        {
            USHORT DllCharacteristics;
        } ExeFormat;

        struct
        {
            HANDLE IFEOKey;
        } ExeName;

        struct
        {
            union
            {
                ULONG OutputFlags;
                struct
                {
                    UCHAR ProtectedProcess : 1;
                    UCHAR AddressSpaceOverride : 1;
                    UCHAR DevOverrideEnabled : 1;
                    UCHAR ManifestDetected : 1;
                    UCHAR ProtectedProcessLight : 1;
                    UCHAR SpareBits1 : 3;
                    UCHAR SpareBits2 : 8;
                    USHORT SpareBits3 : 16;
                };
            };
            HANDLE FileHandle;
            HANDLE SectionHandle;
            ULONGLONG UserProcessParametersNative;
            ULONG UserProcessParametersWow64;
            ULONG CurrentParameterFlags;
            ULONGLONG PebAddressNative;
            ULONG PebAddressWow64;
            ULONGLONG ManifestAddress;
            ULONG ManifestSize;
        } SuccessState;
    };
} PS_CREATE_INFO, *PPS_CREATE_INFO;


/**
 * Creates a new process and primary thread.
 *
 * \param ProcessHandle A pointer to a handle that receives the process object handle.
 * \param ThreadHandle A pointer to a handle that receives the thread object handle.
 * \param ProcessDesiredAccess The access rights desired for the process object.
 * \param ThreadDesiredAccess The access rights desired for the thread object.
 * \param ProcessObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new process.
 * \param ThreadObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \param ProcessFlags Flags that control the creation of the process. These flags are defined as PROCESS_CREATE_FLAGS_*.
 * \param ThreadFlags Flags that control the creation of the thread. These flags are defined as THREAD_CREATE_FLAGS_*.
 * \param ProcessParameters Optional. A pointer to a RTL_USER_PROCESS_PARAMETERS structure that specifies the parameters for the new process.
 * \param CreateInfo A pointer to a PS_CREATE_INFO structure that specifies additional information for the process creation.
 * \param AttributeList Optional. A pointer to a list of attributes for the process and thread.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateUserProcess(
    _Out_ PHANDLE ProcessHandle,
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK ProcessDesiredAccess,
    _In_ ACCESS_MASK ThreadDesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ProcessObjectAttributes,
    _In_opt_ PCOBJECT_ATTRIBUTES ThreadObjectAttributes,
    _In_ ULONG ProcessFlags,
    _In_ ULONG ThreadFlags,
    _In_opt_ PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    _Inout_ PPS_CREATE_INFO CreateInfo,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );

#define THREAD_CREATE_FLAGS_NONE 0x00000000
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_LOADER_WORKER 0x00000010
#define THREAD_CREATE_FLAGS_SKIP_LOADER_INIT 0x00000020
#define THREAD_CREATE_FLAGS_BYPASS_PROCESS_FREEZE 0x00000040

/**
 * A pointer to a user-defined function that serves as the starting routine for a new thread.
 *
 * \param ThreadParameter A pointer to a variable that is passed to the thread.
 * \return NTSTATUS Successful or errant status.
 */
typedef _Function_class_(USER_THREAD_START_ROUTINE)
NTSTATUS NTAPI USER_THREAD_START_ROUTINE(
    _In_ PVOID ThreadParameter
    );
typedef USER_THREAD_START_ROUTINE* PUSER_THREAD_START_ROUTINE;

/**
 * Creates a new thread in the specified process.
 *
 * \param ThreadHandle A pointer to a handle that receives the thread object handle.
 * \param DesiredAccess The access rights desired for the thread object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \param ProcessHandle A handle to the process in which the thread is to be created.
 * \param StartRoutine A pointer to the application-defined function to be executed by the thread.
 * \param Argument Optional. A pointer to a variable that is passed to the thread.
 * \param CreateFlags Flags that control the creation of the thread. These flags are defined as THREAD_CREATE_FLAGS_*.
 * \param ZeroBits The number of zero bits in the starting address of the thread's stack.
 * \param StackSize The initial size of the thread's stack, in bytes.
 * \param MaximumStackSize The maximum size of the thread's stack, in bytes.
 * \param AttributeList Optional. A pointer to a list of attributes for the thread.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateThreadEx(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_ PUSER_THREAD_START_ROUTINE StartRoutine,
    _In_opt_ PVOID Argument,
    _In_ ULONG CreateFlags,
    _In_ SIZE_T ZeroBits,
    _In_ SIZE_T StackSize,
    _In_ SIZE_T MaximumStackSize,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );

#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)

#define JOBOBJECTINFOCLASS ULONG
#define JobObjectBasicAccountingInformation 1
#define JobObjectBasicLimitInformation 2
#define JobObjectBasicProcessIdList 3
#define JobObjectBasicUIRestrictions 4
#define JobObjectSecurityLimitInformation 5
#define JobObjectEndOfJobTimeInformation 6
#define JobObjectAssociateCompletionPortInformation 7
#define JobObjectBasicAndIoAccountingInformation 8
#define JobObjectExtendedLimitInformation 9
#define JobObjectJobSetInformation 10
#define JobObjectGroupInformation 11
#define JobObjectNotificationLimitInformation 12
#define JobObjectLimitViolationInformation 13
#define JobObjectGroupInformationEx 14
#define JobObjectCpuRateControlInformation 15
#define JobObjectCompletionFilter 16
#define JobObjectCompletionCounter 17
#define JobObjectFreezeInformation 18
#define JobObjectExtendedAccountingInformation 19
#define JobObjectWakeInformation 20
#define JobObjectBackgroundInformation 21
#define JobObjectSchedulingRankBiasInformation 22
#define JobObjectTimerVirtualizationInformation 23
#define JobObjectCycleTimeNotification 24
#define JobObjectClearEvent 25
#define JobObjectInterferenceInformation 26
#define JobObjectClearPeakJobMemoryUsed 27
#define JobObjectMemoryUsageInformation 28
#define JobObjectSharedCommit 29
#define JobObjectContainerId 30
#define JobObjectIoRateControlInformation 31
#define JobObjectNetRateControlInformation 32
#define JobObjectNotificationLimitInformation2 33
#define JobObjectLimitViolationInformation2 34
#define JobObjectCreateSilo 35
#define JobObjectSiloBasicInformation 36
#define JobObjectSiloRootDirectory 37
#define JobObjectServerSiloBasicInformation 38
#define JobObjectServerSiloUserSharedData 39
#define JobObjectServerSiloInitialize 40
#define JobObjectServerSiloRunningState 41
#define JobObjectIoAttribution 42
#define JobObjectMemoryPartitionInformation 43
#define JobObjectContainerTelemetryId 44
#define JobObjectSiloSystemRoot 45
#define JobObjectEnergyTrackingState 46
#define JobObjectThreadImpersonationInformation 47
#define JobObjectIoPriorityLimit 48
#define JobObjectPagePriorityLimit 49
#define JobObjectServerSiloDiagnosticInformation 50
#define JobObjectNetworkAccountingInformation 51
#define JobObjectCpuPartition 52
#define MaxJobObjectInfoClass 53

#define JOB_OBJECT_LIMIT_SILO_READY 0x00400000

/**
 * The JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2 structure contains basic and extended limit information for a job object.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_extended_limit_information
 */
typedef struct _JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2
{
    JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
    IO_COUNTERS IoInfo;
    SIZE_T ProcessMemoryLimit;
    SIZE_T JobMemoryLimit;
    SIZE_T PeakProcessMemoryUsed;
    SIZE_T PeakJobMemoryUsed;
    SIZE_T JobTotalMemoryLimit;
} JOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2, *PJOBOBJECT_EXTENDED_LIMIT_INFORMATION_V2;

typedef struct _JOBOBJECT_SCHEDULING_RANK_BIAS_INFORMATION
{
    ULONG SchedulingRankBias;
} JOBOBJECT_SCHEDULING_RANK_BIAS_INFORMATION, *PJOBOBJECT_SCHEDULING_RANK_BIAS_INFORMATION;

typedef struct _JOBOBJECT_TIMER_VIRTUALIZATION_INFORMATION
{
    ULONG TimerVirtualizationEnabled;
} JOBOBJECT_TIMER_VIRTUALIZATION_INFORMATION, *PJOBOBJECT_TIMER_VIRTUALIZATION_INFORMATION;

typedef struct _JOBOBJECT_CYCLE_TIME_NOTIFICATION
{
    HANDLE NotificationChannel;
    ULONG64 CycleTime;
} JOBOBJECT_CYCLE_TIME_NOTIFICATION, *PJOBOBJECT_CYCLE_TIME_NOTIFICATION;

typedef struct _JOBOBJECT_SHARED_COMMIT
{
    ULONG64 SharedCommit;
} JOBOBJECT_SHARED_COMMIT, *PJOBOBJECT_SHARED_COMMIT;

typedef struct _JOBOBJECT_MEMORY_PARTITION_INFORMATION
{
    ULONG_PTR PartitionId;
} JOBOBJECT_MEMORY_PARTITION_INFORMATION, *PJOBOBJECT_MEMORY_PARTITION_INFORMATION;

typedef struct _JOBOBJECT_CPU_PARTITION_INFORMATION
{
    ULONG_PTR PartitionId;
} JOBOBJECT_CPU_PARTITION_INFORMATION, *PJOBOBJECT_CPU_PARTITION_INFORMATION;

typedef struct _JOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION
{
    JOBOBJECT_BASIC_ACCOUNTING_INFORMATION BasicInfo;
    IO_COUNTERS IoInfo;
    PROCESS_DISK_COUNTERS DiskIoInfo;
    ULONG64 ContextSwitches;
    LARGE_INTEGER TotalCycleTime;
    ULONG64 ReadyTime;
    PROCESS_ENERGY_VALUES EnergyValues;
} JOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION, *PJOBOBJECT_EXTENDED_ACCOUNTING_INFORMATION;

typedef struct _JOBOBJECT_WAKE_INFORMATION
{
    HANDLE NotificationChannel;
    ULONG64 WakeCounters[PsMaxWakeReasons];
} JOBOBJECT_WAKE_INFORMATION, *PJOBOBJECT_WAKE_INFORMATION;

typedef struct _JOBOBJECT_WAKE_INFORMATION_V1
{
    HANDLE NotificationChannel;
    ULONG64 WakeCounters[4];
} JOBOBJECT_WAKE_INFORMATION_V1, *PJOBOBJECT_WAKE_INFORMATION_V1;

typedef struct _JOBOBJECT_INTERFERENCE_INFORMATION
{
    ULONG64 Count;
} JOBOBJECT_INTERFERENCE_INFORMATION, *PJOBOBJECT_INTERFERENCE_INFORMATION;

typedef struct _JOBOBJECT_FREEZE_INFORMATION
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG FreezeOperation : 1;
            ULONG FilterOperation : 1;
            ULONG SwapOperation : 1;
            ULONG Reserved : 29;
        };
    };
    BOOLEAN Freeze;
    BOOLEAN Swap;
    UCHAR Reserved0[2];
    JOBOBJECT_WAKE_FILTER WakeFilter;
} JOBOBJECT_FREEZE_INFORMATION, *PJOBOBJECT_FREEZE_INFORMATION;

typedef struct _JOBOBJECT_CONTAINER_IDENTIFIER_V2
{
    GUID ContainerId;
    GUID ContainerTelemetryId;
    ULONG JobId;
} JOBOBJECT_CONTAINER_IDENTIFIER_V2, *PJOBOBJECT_CONTAINER_IDENTIFIER_V2;

typedef struct _JOBOBJECT_MEMORY_USAGE_INFORMATION
{
    ULONG64 JobMemory;
    ULONG64 PeakJobMemoryUsed;
} JOBOBJECT_MEMORY_USAGE_INFORMATION, *PJOBOBJECT_MEMORY_USAGE_INFORMATION;

typedef struct _JOBOBJECT_MEMORY_USAGE_INFORMATION_V2
{
    JOBOBJECT_MEMORY_USAGE_INFORMATION BasicInfo;
    ULONG64 JobSharedMemory;
    ULONG64 Reserved[2];
} JOBOBJECT_MEMORY_USAGE_INFORMATION_V2, *PJOBOBJECT_MEMORY_USAGE_INFORMATION_V2;

typedef struct _SILO_USER_SHARED_DATA
{
    ULONG ServiceSessionId;
    ULONG ActiveConsoleId;
    LONGLONG ConsoleSessionForegroundProcessId;
    NT_PRODUCT_TYPE NtProductType;
    ULONG SuiteMask;
    ULONG SharedUserSessionId;
    BOOLEAN IsMultiSessionSku;
    BOOLEAN IsStateSeparationEnabled;
    WCHAR NtSystemRoot[260];
    USHORT UserModeGlobalLogger[16];
    ULONG TimeZoneId;
    LONG TimeZoneBiasStamp;
    KSYSTEM_TIME TimeZoneBias;
    LARGE_INTEGER TimeZoneBiasEffectiveStart;
    LARGE_INTEGER TimeZoneBiasEffectiveEnd;
} SILO_USER_SHARED_DATA, *PSILO_USER_SHARED_DATA;

#define SILO_OBJECT_ROOT_DIRECTORY_SHADOW_ROOT 0x00000001
#define SILO_OBJECT_ROOT_DIRECTORY_INITIALIZE 0x00000002
#define SILO_OBJECT_ROOT_DIRECTORY_SHADOW_DOS_DEVICES 0x00000004

typedef struct _SILOOBJECT_ROOT_DIRECTORY
{
    union
    {
        ULONG ControlFlags;
        UNICODE_STRING Path;
    };
} SILOOBJECT_ROOT_DIRECTORY, *PSILOOBJECT_ROOT_DIRECTORY;

typedef struct _SERVERSILO_INIT_INFORMATION
{
    HANDLE DeleteEvent;
    BOOLEAN IsDownlevelContainer;
} SERVERSILO_INIT_INFORMATION, *PSERVERSILO_INIT_INFORMATION;

typedef struct _JOBOBJECT_ENERGY_TRACKING_STATE
{
    ULONG64 Value;
    ULONG UpdateMask;
    ULONG DesiredState;
} JOBOBJECT_ENERGY_TRACKING_STATE, *PJOBOBJECT_ENERGY_TRACKING_STATE;

_Enum_is_bitflag_
typedef enum _JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS
{
    JOBOBJECT_IO_PRIORITY_LIMIT_ENABLE = 0x1,
    JOBOBJECT_IO_PRIORITY_LIMIT_VALID_FLAGS = 0x1,
} JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS);

typedef struct _JOBOBJECT_IO_PRIORITY_LIMIT
{
    JOBOBJECT_IO_PRIORITY_LIMIT_FLAGS Flags;
    ULONG Priority;
} JOBOBJECT_IO_PRIORITY_LIMIT, *PJOBOBJECT_IO_PRIORITY_LIMIT;

_Enum_is_bitflag_
typedef enum _JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS
{
    JOBOBJECT_PAGE_PRIORITY_LIMIT_ENABLE = 0x1,
    JOBOBJECT_PAGE_PRIORITY_LIMIT_VALID_FLAGS = 0x1,
} JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS);

typedef struct _JOBOBJECT_PAGE_PRIORITY_LIMIT
{
    JOBOBJECT_PAGE_PRIORITY_LIMIT_FLAGS Flags;
    ULONG Priority;
} JOBOBJECT_PAGE_PRIORITY_LIMIT, *PJOBOBJECT_PAGE_PRIORITY_LIMIT;

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _SERVERSILO_DIAGNOSTIC_INFORMATION
{
    NTSTATUS ExitStatus;
    WCHAR CriticalProcessName[15];
} SERVERSILO_DIAGNOSTIC_INFORMATION, *PSERVERSILO_DIAGNOSTIC_INFORMATION;

typedef struct _JOBOBJECT_NETWORK_ACCOUNTING_INFORMATION
{
    ULONG64 DataBytesIn;
    ULONG64 DataBytesOut;
} JOBOBJECT_NETWORK_ACCOUNTING_INFORMATION, *PJOBOBJECT_NETWORK_ACCOUNTING_INFORMATION;
#endif

/**
 * Creates or opens a job object.
 *
 * \param JobHandle A handle to the job object.
 * \param DesiredAccess The access rights desired for the thread object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateJobObject(
    _Out_ PHANDLE JobHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * Opens an existing job object.
 *
 * \param JobHandle A handle to the job object.
 * \param DesiredAccess The access rights desired for the thread object.
 * \param ObjectAttributes Optional. A pointer to an OBJECT_ATTRIBUTES structure that specifies the attributes of the new thread.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenJobObject(
    _Out_ PHANDLE JobHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * Assigns a process to an existing job object.
 *
 * \param JobHandle A handle to the job object to which the process will be associated. The handle must have the JOB_OBJECT_ASSIGN_PROCESS access right.
 * \param ProcessHandle A handle to the process to associate with the job object. The handle must have the PROCESS_SET_QUOTA and PROCESS_TERMINATE access rights.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAssignProcessToJobObject(
    _In_ HANDLE JobHandle,
    _In_ HANDLE ProcessHandle
    );

/**
 * Terminates all processes associated with the job object. If the job is nested, all processes currently associated with the job and all child jobs in the hierarchy are terminated.
 *
 * \param JobHandle A handle to the job whose processes will be terminated. The handle must have the JOB_OBJECT_TERMINATE access right.
 * \param ExitStatus The exit status to be used by all processes and threads in the job object.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateJobObject(
    _In_ HANDLE JobHandle,
    _In_ NTSTATUS ExitStatus
    );

/**
 * Checks if a process is associated with a job object.
 *
 * \param ProcessHandle A handle to the process to be checked.
 * \param JobHandle An optional handle to the job object. If this parameter is NULL, the function checks if the process is associated with any job object.
 * \return NTSTATUS Successful or errant status.
 * \remarks This function can be used to determine if a process is running within a job object, which can be useful for managing process resources and constraints.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtIsProcessInJob(
    _In_ HANDLE ProcessHandle,
    _In_opt_ HANDLE JobHandle
    );

/**
 * Retrieves information about a job object.
 *
 * \param JobHandle An optional handle to the job object. If this parameter is NULL, the function retrieves information about the job object associated with the calling process.
 * \param JobObjectInformationClass The type of job object information to be retrieved.
 * \param JobObjectInformation A pointer to a buffer that receives the job object information.
 * \param JobObjectInformationLength The size of the buffer pointed to by the JobObjectInformation parameter.
 * \param ReturnLength An optional pointer to a variable that receives the size of the data returned.
 * \return NTSTATUS Successful or errant status.
 * \remarks This function can be used to query various types of information about a job object, such as accounting information, limit information, and process ID list.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationJobObject(
    _In_opt_ HANDLE JobHandle,
    _In_ JOBOBJECTINFOCLASS JobObjectInformationClass,
    _Out_writes_bytes_(JobObjectInformationLength) PVOID JobObjectInformation,
    _In_ ULONG JobObjectInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

/**
 * Sets information for a job object.
 *
 * \param JobHandle A handle to the job object.
 * \param JobObjectInformationClass The type of job object information to be set.
 * \param JobObjectInformation A pointer to a buffer that contains the job object information.
 * \param JobObjectInformationLength The size of the buffer pointed to by the JobObjectInformation parameter.
 * \return NTSTATUS Successful or errant status.
 * \remarks This function can be used to set various types of information for a job object, such as limit information, UI restrictions, and security limit information.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationJobObject(
    _In_ HANDLE JobHandle,
    _In_ JOBOBJECTINFOCLASS JobObjectInformationClass,
    _In_reads_bytes_(JobObjectInformationLength) PVOID JobObjectInformation,
    _In_ ULONG JobObjectInformationLength
    );

/**
 * Creates a set of job objects.
 *
 * \param NumJob The number of job objects in the set.
 * \param UserJobSet A pointer to an array of JOB_SET_ARRAY structures that specify the job objects in the set.
 * \param Flags Reserved for future use. Must be zero.
 * \return NTSTATUS Successful or errant status.
 * \remarks This function can be used to create a set of job objects, which can be useful for managing groups of related processes.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateJobSet(
    _In_ ULONG NumJob,
    _In_reads_(NumJob) PJOB_SET_ARRAY UserJobSet,
    _In_ ULONG Flags
    );

#if (PHNT_VERSION >= PHNT_WINDOWS_10)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRevertContainerImpersonation(
    VOID
    );
#endif

#endif


#if (PHNT_MODE != PHNT_MODE_KERNEL)

typedef enum _MEMORY_RESERVE_TYPE
{
    MemoryReserveUserApc,
    MemoryReserveIoCompletion,
    MemoryReserveTypeMax
} MEMORY_RESERVE_TYPE;

/**
 * Allocates a memory reserve object.
 *
 * \param MemoryReserveHandle Pointer to a variable that receives the memory reserve object handle.
 * \param ObjectAttributes Pointer to an object attributes structure.
 * \param Type The type of memory reserve.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAllocateReserveObject(
    _Out_ PHANDLE MemoryReserveHandle,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_ MEMORY_RESERVE_TYPE Type
    );


_Enum_is_bitflag_
typedef enum _PSSNT_CAPTURE_FLAGS
{
    PSSNT_CAPTURE_NONE                                = 0x00000000,
    PSSNT_CAPTURE_VA_CLONE                            = 0x00000001,
    PSSNT_CAPTURE_RESERVED_00000002                   = 0x00000002,
    PSSNT_CAPTURE_HANDLES                             = 0x00000004,
    PSSNT_CAPTURE_HANDLE_NAME_INFORMATION             = 0x00000008,
    PSSNT_CAPTURE_HANDLE_BASIC_INFORMATION            = 0x00000010,
    PSSNT_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION    = 0x00000020,
    PSSNT_CAPTURE_HANDLE_TRACE                        = 0x00000040,
    PSSNT_CAPTURE_THREADS                             = 0x00000080,
    PSSNT_CAPTURE_THREAD_CONTEXT                      = 0x00000100,
    PSSNT_CAPTURE_THREAD_CONTEXT_EXTENDED             = 0x00000200,
    PSSNT_CAPTURE_RESERVED_00000400                   = 0x00000400,
    PSSNT_CAPTURE_VA_SPACE                            = 0x00000800,
    PSSNT_CAPTURE_VA_SPACE_SECTION_INFORMATION        = 0x00001000,
    PSSNT_CAPTURE_IPT_TRACE                           = 0x00002000,
    PSSNT_CAPTURE_RESERVED_00004000                   = 0x00004000,

    PSSNT_CREATE_BREAKAWAY_OPTIONAL                   = 0x04000000,
    PSSNT_CREATE_BREAKAWAY                            = 0x08000000,
    PSSNT_CREATE_FORCE_BREAKAWAY                      = 0x10000000,
    PSSNT_CREATE_USE_VM_ALLOCATIONS                   = 0x20000000,
    PSSNT_CREATE_MEASURE_PERFORMANCE                  = 0x40000000,
    PSSNT_CREATE_RELEASE_SECTION                      = 0x80000000
} PSSNT_CAPTURE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(PSSNT_CAPTURE_FLAGS);

_Enum_is_bitflag_
typedef enum _PSSNT_DUPLICATE_FLAGS
{
    PSSNT_DUPLICATE_NONE         = 0x00,
    PSSNT_DUPLICATE_CLOSE_SOURCE = 0x01
} PSSNT_DUPLICATE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(PSSNT_DUPLICATE_FLAGS);

typedef enum _PSSNT_QUERY_INFORMATION_CLASS
{
    PSSNT_QUERY_PROCESS_INFORMATION = 0,
    PSSNT_QUERY_VA_CLONE_INFORMATION = 1,
    PSSNT_QUERY_AUXILIARY_PAGES_INFORMATION = 2,
    PSSNT_QUERY_VA_SPACE_INFORMATION = 3,
    PSSNT_QUERY_HANDLE_INFORMATION = 4,
    PSSNT_QUERY_THREAD_INFORMATION = 5,
    PSSNT_QUERY_HANDLE_TRACE_INFORMATION = 6,
    PSSNT_QUERY_PERFORMANCE_COUNTERS = 7
} PSSNT_QUERY_INFORMATION_CLASS;

#define PSSNT_SIGNATURE_PSSD 'PSSD'

#if (PHNT_VERSION >= PHNT_WINDOWS_8_1)
/**
 * The PssNtCaptureSnapshot routine captures a snapshot of the specified process.
 *
 * \param SnapshotHandle Pointer to a variable that receives the snapshot handle.
 * \param ProcessHandle Handle to the process.
 * \param CaptureFlags Flags indicating what to capture.
 * \param ThreadContextFlags Optional flags for capturing thread context.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtCaptureSnapshot(
    _Out_ PHANDLE SnapshotHandle,
    _In_ HANDLE ProcessHandle,
    _In_ PSSNT_CAPTURE_FLAGS CaptureFlags,
    _In_opt_ ULONG ThreadContextFlags
    );

/**
 * The PssNtDuplicateSnapshot routine duplicates a process snapshot from one process to another.
 *
 * \param SourceProcessHandle Handle to the source process.
 * \param SnapshotHandle Handle to the snapshot to duplicate.
 * \param TargetProcessHandle Handle to the target process.
 * \param TargetSnapshotHandle Pointer to a variable that receives the duplicated snapshot handle.
 * \param Flags Optional flags for duplication.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtDuplicateSnapshot(
    _In_ HANDLE SourceProcessHandle,
    _In_ HANDLE SnapshotHandle,
    _In_ HANDLE TargetProcessHandle,
    _Out_ PHANDLE TargetSnapshotHandle,
    _In_opt_ PSSNT_DUPLICATE_FLAGS Flags
    );

/**
 * The PssNtFreeSnapshot routine frees a snapshot.
 *
 * \param SnapshotHandle Handle to the snapshot to free.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeSnapshot(
    _In_ HANDLE SnapshotHandle
    );

/**
 * The PssNtFreeRemoteSnapshot routine frees a remote process snapshot.
 *
 * \param ProcessHandle A handle to the process that contains the snapshot. The handle must have PROCESS_VM_READ, PROCESS_VM_OPERATION, and PROCESS_DUP_HANDLE rights.
 * \param SnapshotHandle Handle to the snapshot to free.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeRemoteSnapshot(
    _In_ HANDLE ProcessHandle,
    _In_ HANDLE SnapshotHandle
    );

/**
 * The PssNtQuerySnapshot routine queries information from the specified snapshot.
 *
 * \param SnapshotHandle Handle to the snapshot.
 * \param InformationClass The information class to query.
 * \param Buffer Pointer to a buffer that receives the queried information.
 * \param BufferLength Length of the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
PssNtQuerySnapshot(
    _In_ HANDLE SnapshotHandle,
    _In_ PSSNT_QUERY_INFORMATION_CLASS InformationClass,
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength
    );

typedef struct _PSSNT_WALK_MARKER_INFO
{
    ULONG Signature;
    HANDLE SectionHandle;
} PSSNT_WALK_MARKER_INFO, *PPSSNT_WALK_MARKER_INFO;

NTSYSAPI
NTSTATUS
NTAPI
PssNtWalkSnapshot(
    _In_ HANDLE SnapshotHandle,
    _In_ ULONG InformationClass,
    _In_ HANDLE WalkMarkerHandle,
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
PssNtFreeWalkMarker(
    _Inout_ PHANDLE WalkMarkerHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
PssNtValidateDescriptor(
    _In_ HANDLE SnapshotHandle,
    _In_opt_ PVOID ExceptionAddress
    );

#endif

/**
 * Flag indicating the type of bulk information to query.
 */
#define MEMORY_BULK_INFORMATION_FLAG_BASIC 0x00000001

/**
 * The NTPSS_MEMORY_BULK_INFORMATION structure is used to query basic memory information in bulk for a process.
 */
typedef struct _NTPSS_MEMORY_BULK_INFORMATION
{
    ULONG QueryFlags;
    ULONG NumberOfEntries;
    PVOID NextValidAddress;
} NTPSS_MEMORY_BULK_INFORMATION, *PNTPSS_MEMORY_BULK_INFORMATION;

#if (PHNT_VERSION >= PHNT_WINDOWS_10_20H1)
/**
 * Captures virtual address space bulk information for a process.
 *
 * \param ProcessHandle Handle to the process.
 * \param BaseAddress Optional base address to start the capture.
 * \param BulkInformation Pointer to the memory bulk information structure.
 * \param BulkInformationLength Length of the memory bulk information structure.
 * \param ReturnLength Optional pointer to a variable that receives the length of the captured information.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtPssCaptureVaSpaceBulk(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ PNTPSS_MEMORY_BULK_INFORMATION BulkInformation,
    _In_ SIZE_T BulkInformationLength,
    _Out_opt_ PSIZE_T ReturnLength
    );
#endif

#endif

#endif
