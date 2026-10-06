/*
 * Prefetcher (Superfetch) support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTPFAPI_H
#define _NTPFAPI_H



typedef enum _PREFETCHER_INFORMATION_CLASS
{
    PrefetcherRetrieveTrace = 1,
    PrefetcherSystemParameters,
    PrefetcherBootPhase,
    PrefetcherSpare1,
    PrefetcherOperationProcess,
    PrefetcherCacheEntryUpdate,
    PrefetcherSpare2,
    PrefetcherAppLaunchScenarioControl,
    PrefetcherInformationMax
} PREFETCHER_INFORMATION_CLASS;

#define PREFETCHER_INFORMATION_VERSION 23
#define PREFETCHER_INFORMATION_MAGIC ('kuhC')

typedef struct _PREFETCHER_INFORMATION
{
    _In_ ULONG Version;
    _In_ ULONG Magic;
    _In_ PREFETCHER_INFORMATION_CLASS PrefetcherInformationClass;
    _Inout_ PVOID PrefetcherInformation;
    _Inout_ ULONG PrefetcherInformationLength;
} PREFETCHER_INFORMATION, *PPREFETCHER_INFORMATION;

typedef struct _PF_RETRIEVE_TRACE
{
    UCHAR Buffer[1];
} PF_RETRIEVE_TRACE, *PPF_RETRIEVE_TRACE;

typedef enum _PF_ENABLE_STATUS
{
    PfSvNotSpecified,
    PfSvEnabled,
    PfSvDisabled,
    PfSvMaxEnableStatus
} PF_ENABLE_STATUS;

typedef struct _PF_TRACE_LIMITS
{
    ULONG MaxNumPages;
    ULONG MaxNumSections;
    LONGLONG TimerPeriod;
} PF_TRACE_LIMITS, *PPF_TRACE_LIMITS;

typedef struct _PF_SYSTEM_PREFETCH_PARAMETERS
{
    PF_ENABLE_STATUS EnableStatus[2];
    PF_TRACE_LIMITS TraceLimits[2];
    ULONG MaxNumActiveTraces;
    ULONG MaxNumSavedTraces;
    WCHAR RootDirPath[32];
    WCHAR HostingApplicationList[128];
} PF_SYSTEM_PREFETCH_PARAMETERS, *PPF_SYSTEM_PREFETCH_PARAMETERS;

typedef enum _PF_BOOT_PHASE_ID
{
    PfKernelInitPhase = 0,
    PfBootDriverInitPhase = 90,
    PfSystemDriverInitPhase = 120,
    PfSessionManagerInitPhase = 150,
    PfSMRegistryInitPhase = 180,
    PfVideoInitPhase = 210,
    PfPostVideoInitPhase = 240,
    PfBootAcceptedRegistryInitPhase = 270,
    PfUserShellReadyPhase = 300,
    PfMaxBootPhaseId = 900
} PF_BOOT_PHASE_ID;

#define PF_SN_OPERATION_PROCESS_VERSION 1

typedef enum _PF_OPERATION_PROCESS_ACTION
{
    PfSnOpProcessBegin = 0,
    PfSnOpProcessEnd = 1,
    PfSnOpProcessMax = 2
} PF_OPERATION_PROCESS_ACTION;

typedef struct _PF_OPERATION_PROCESS
{
    UCHAR Version;
    UCHAR Action;
    USHORT Reserved;
    ULONG OpFlags;
    ULONG Value;
} PF_OPERATION_PROCESS, *PPF_OPERATION_PROCESS;

#define PF_BOOT_CONTROL_VERSION 1

typedef struct _PF_BOOT_CONTROL
{
    ULONG Version;
    ULONG DisableBootPrefetching;
} PF_BOOT_CONTROL, *PPF_BOOT_CONTROL;

#define PF_CACHE_ENTRY_UPDATE_VERSION 2

typedef struct _PF_CACHE_ENTRY_UPDATE
{
    ULONG Version;
    UCHAR Name[64];
    ULONG NewValue;
} PF_CACHE_ENTRY_UPDATE, *PPF_CACHE_ENTRY_UPDATE;

#define PF_APP_LAUNCH_SCENARIO_CONTROL_VERSION 1

typedef struct _PF_APP_LAUNCH_SCENARIO_CONTROL
{
    ULONG Version;
    ULONG Enable;
    HANDLE ProcessHandle;
} PF_APP_LAUNCH_SCENARIO_CONTROL, *PPF_APP_LAUNCH_SCENARIO_CONTROL;


typedef enum _SUPERFETCH_INFORMATION_CLASS
{
    SuperfetchRetrieveTrace = 1,
    SuperfetchSystemParameters,
    SuperfetchLogEvent,
    SuperfetchGenerateTrace,
    SuperfetchPrefetch,
    SuperfetchPfnQuery,
    SuperfetchPfnSetPriority,
    SuperfetchPrivSourceQuery,
    SuperfetchSequenceNumberQuery,
    SuperfetchScenarioPhase,
    SuperfetchWorkerPriority,
    SuperfetchScenarioQuery,
    SuperfetchScenarioPrefetch,
    SuperfetchRobustnessControl,
    SuperfetchTimeControl,
    SuperfetchMemoryListQuery,
    SuperfetchMemoryRangesQuery,
    SuperfetchTracingControl,
    SuperfetchTrimWhileAgingControl,
    SuperfetchRepurposedByPrefetch,
    SuperfetchChannelPowerRequest,
    SuperfetchMovePages,
    SuperfetchVirtualQuery,
    SuperfetchCombineStatsQuery,
    SuperfetchSetMinWsAgeRate,
    SuperfetchDeprioritizeOldPagesInWs,
    SuperfetchFileExtentsQuery,
    SuperfetchGpuUtilizationQuery,
    SuperfetchPfnSet,
    SuperfetchInformationMax
} SUPERFETCH_INFORMATION_CLASS;

#define SUPERFETCH_INFORMATION_VERSION 45
#define SUPERFETCH_INFORMATION_MAGIC ('kuhC')

typedef struct _SUPERFETCH_INFORMATION
{
    _In_ ULONG Version;
    _In_ ULONG Magic;
    _In_ SUPERFETCH_INFORMATION_CLASS SuperfetchInformationClass;
    _Inout_ PVOID SuperfetchInformation;
    _Inout_ ULONG SuperfetchInformationLength;
} SUPERFETCH_INFORMATION, *PSUPERFETCH_INFORMATION;

typedef struct _PF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE
{
    union
    {
        struct
        {
            ULONGLONG RequestType;
            ULONGLONG Reserved;
            HANDLE    PartitionHandle;
        } Input;
        struct
        {
            ULONGLONG TypeFlags;
            ULONGLONG Timestamp;
            HANDLE    PartitionHandle;
        } Output;

        UCHAR Buffer[ANYSIZE_ARRAY];
    };
} PF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE, *PPF_SYSTEM_SUPERFETCH_RETRIEVE_TRACE;

typedef struct _PF_SYSTEM_SUPERFETCH_PARAMETERS
{
    ULONG EnabledComponents;
    ULONG BootID;
    ULONG SavedSectInfoTracesMax;
    ULONG SavedPageAccessTracesMax;
    ULONG ScenarioPrefetchTimeoutStandby;
    ULONG ScenarioPrefetchTimeoutHibernate;
    ULONG ScenarioPrefetchTimeoutHiberBoot;
} PF_SYSTEM_SUPERFETCH_PARAMETERS, *PPF_SYSTEM_SUPERFETCH_PARAMETERS;

typedef enum _PF_EVENT_TYPE
{
    PfEventTypeImageLoad = 0,
    PfEventTypeAppLaunch = 1,
    PfEventTypeStartTrace = 2,
    PfEventTypeEndTrace = 3,
    PfEventTypeTimestamp = 4,
    PfEventTypeOperation = 5,
    PfEventTypeRepurpose = 6,
    PfEventTypeForegroundProcess = 7,
    PfEventTypeTimeRange = 8,
    PfEventTypeUserInput = 9,
    PfEventTypeFileAccess = 10,
    PfEventTypeUnmap = 11,
    PfEventTypeUtilization = 11,
    PfEventTypeMemInfo = 12,
    PfEventTypeFileDelete = 13,
    PfEventTypeAppExit = 14,
    PfEventTypeSystemTime = 15,
    PfEventTypePower = 16,
    PfEventTypeSessionChange = 17,
    PfEventTypeHardFaultTimeStamp = 18,
    PfEventTypeVirtualFree = 19,
    PfEventTypePerfInfo = 20,
    PfEventTypeProcessSnapshot = 21,
    PfEventTypeUserSnapshot = 22,
    PfEventTypeStreamSequenceNumber = 23,
    PfEventTypeFileTruncate = 24,
    PfEventTypeFileRename = 25,
    PfEventTypeFileCreate = 26,
    PfEventTypeAgCxContext = 27,
    PfEventTypePowerAction = 28,
    PfEventTypeHardFaultTS = 29,
    PfEventTypeRobustInfo = 30,
    PfEventTypeFileDefrag = 31,
    PfEventTypeMax = 32
} PF_EVENT_TYPE;

#define PF_LOG_EVENT_DATA_VERSION 1

typedef struct _PF_LOG_EVENT_DATA
{
    ULONG Version;
    union
    {
        ULONG Packed;
        struct
        {
            ULONG DataSize : 25;
            ULONG Flags    : 2;
            ULONG EventType: 5;
        };
    };
    PVOID EventData;
    HANDLE PartitionHandle;
} PF_LOG_EVENT_DATA , *PPF_LOG_EVENT_DATA ;

typedef struct _PFN_TRIPLET
{
    ULONGLONG MaskOrKey;
    ULONGLONG Pfn;
    ULONGLONG Flags;
} PFN_TRIPLET, *PPFN_TRIPLET;

#define PF_PFN_PRIO_REQUEST_VERSION 1
#define PF_PFN_PRIO_REQUEST_QUERY_MEMORY_LIST 0x1
#define PF_PFN_PRIO_REQUEST_VALID_FLAGS 0x1

typedef struct _PF_PFN_PRIO_REQUEST
{
    ULONG Version;
    ULONG RequestFlags;
    SIZE_T PfnCount;
    SYSTEM_MEMORY_LIST_INFORMATION MemInfo;
    union
    {
        MMPFN_IDENTITY PageIdentities[256];
        PFN_TRIPLET Entries[256];
    };
} PF_PFN_PRIO_REQUEST, *PPF_PFN_PRIO_REQUEST;

typedef enum _PFS_PRIVATE_PAGE_SOURCE_TYPE
{
    PfsPrivateSourceKernel,
    PfsPrivateSourceSession,
    PfsPrivateSourceProcess,
    PfsPrivateSourceMax
} PFS_PRIVATE_PAGE_SOURCE_TYPE;

typedef struct _PFS_PRIVATE_PAGE_SOURCE
{
    PFS_PRIVATE_PAGE_SOURCE_TYPE Type;
    union
    {
        ULONG SessionId;
        ULONG ProcessId;
    };
    ULONG ImagePathHash;
    ULONG_PTR UniqueProcessHash;
} PFS_PRIVATE_PAGE_SOURCE, *PPFS_PRIVATE_PAGE_SOURCE;

typedef struct _PF_PRIVSOURCE_INFO
{
    PFS_PRIVATE_PAGE_SOURCE DbInfo;
    PVOID EProcess;
    SIZE_T WsPrivatePages;
    SIZE_T TotalPrivatePages;
    ULONG SessionID;
    UCHAR ImageName[16];
    union
    {
        SIZE_T WsSwapPages;
        SIZE_T SessionPagedPoolPages;
        SIZE_T StoreSizePages;
    };
    SIZE_T WsTotalPages;
    ULONG DeepFreezeTimeMs;
    ULONG ModernApp : 1;
    ULONG DeepFrozen : 1;
    ULONG Foreground : 1;
    ULONG PerProcessStore : 1;
    ULONG Spare : 28;
} PF_PRIVSOURCE_INFO, *PPF_PRIVSOURCE_INFO;

#define PF_PRIVSOURCE_QUERY_REQUEST_VERSION 8
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYWSPAGES 0x1
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYCOMPRESSEDPAGES 0x2
#define PF_PRIVSOURCE_QUERY_REQUEST_FLAGS_QUERYSKIPPAGES 0x4

typedef struct _PF_PRIVSOURCE_QUERY_REQUEST
{
    ULONG Version;
    ULONG Flags;
    ULONG InfoCount;
    PF_PRIVSOURCE_INFO InfoArray[1];
} PF_PRIVSOURCE_QUERY_REQUEST, *PPF_PRIVSOURCE_QUERY_REQUEST;

typedef enum _PF_PHASED_SCENARIO_TYPE
{
    PfScenarioTypeNone,
    PfScenarioTypeStandby,
    PfScenarioTypeHibernate,
    PfScenarioTypeFUS,
    PfScenarioTypeMax
} PF_PHASED_SCENARIO_TYPE;

#define PF_SCENARIO_PHASE_INFO_VERSION 4

typedef struct _PF_SCENARIO_PHASE_INFO
{
    ULONG Version;
    PF_PHASED_SCENARIO_TYPE ScenType;
    ULONG PhaseId;
    ULONG SequenceNumber;
    ULONG Flags;
    ULONG FUSUserId;
    ULONG Reserved0;
    ULONG Reserved1;
} PF_SCENARIO_PHASE_INFO, *PPF_SCENARIO_PHASE_INFO;

typedef struct _PF_WORKER_PRIORITY_CONTROL
{
    ULONG Version;
    KPRIORITY Priority;
    HANDLE PartitionHandle;
} PF_WORKER_PRIORITY_CONTROL, *PPF_WORKER_PRIORITY_CONTROL;

#define PF_SCENARIO_QUERY_INFO_VERSION 4

typedef struct _PF_SCENARIO_QUERY_INFO 
{
    ULONG_PTR Version;
    ULONG_PTR Field1;
    ULONG_PTR Field2;
    ULONG_PTR Field3;
} PF_SCENARIO_QUERY_INFO , *PPF_SCENARIO_QUERY_INFO;

typedef struct _PF_MEMORY_LIST_NODE
{
    ULONGLONG Node : 8;
    ULONGLONG Spare : 56;
    ULONGLONG StandbyLowPageCount;
    ULONGLONG StandbyMediumPageCount;
    ULONGLONG StandbyHighPageCount;
    ULONGLONG FreePageCount;
    ULONGLONG ModifiedPageCount;
} PF_MEMORY_LIST_NODE, *PPF_MEMORY_LIST_NODE;

typedef struct _PF_ROBUST_PROCESS_ENTRY
{
    ULONG ImagePathHash;
    ULONG Pid;
    ULONG Alignment;
} PF_ROBUST_PROCESS_ENTRY, *PPF_ROBUST_PROCESS_ENTRY;

typedef struct _PF_ROBUST_FILE_ENTRY
{
    ULONG FilePathHash;
} PF_ROBUST_FILE_ENTRY, *PPF_ROBUST_FILE_ENTRY;

typedef enum _PF_ROBUSTNESS_CONTROL_COMMAND
{
    PfRpControlUpdate = 0,
    PfRpControlReset = 1,
    PfRpControlRobustAllStart = 2,
    PfRpControlRobustAllStop = 3,
    PfRpControlCommandMax = 4
} PF_ROBUSTNESS_CONTROL_COMMAND;

#define PF_ROBUSTNESS_CONTROL_VERSION 1

typedef struct _PF_ROBUSTNESS_CONTROL
{
    ULONG Version;
    PF_ROBUSTNESS_CONTROL_COMMAND Command;
    ULONG DeprioProcessCount;
    ULONG ExemptProcessCount;
    ULONG DeprioFileCount;
    ULONG ExemptFileCount;
    PF_ROBUST_PROCESS_ENTRY ProcessEntries[1];
    PF_ROBUST_FILE_ENTRY FileEntries[1];
} PF_ROBUSTNESS_CONTROL, *PPF_ROBUSTNESS_CONTROL;

typedef struct _PF_SCENARIO_PREFETCH_INFO
{
    ULONG Version;
    ULONG State;
} PF_SCENARIO_PREFETCH_INFO, *PPF_SCENARIO_PREFETCH_INFO;

#define PF_TRIM_WHILE_AGING_CONTROL_VERSION_1 1

typedef enum _PF_TRIM_WHILE_AGING_STATE
{
    PfTrimWhileAgingOff = 0,
    PfTrimWhileAgingLowPriority = 1,
    PfTrimWhileAgingPassive = 2,
    PfTrimWhileAgingNormal = 3,
    PfTrimWhileAgingAggressive = 4,
    PfTrimWhileAgingMax = 5,
} PF_TRIM_WHILE_AGING_STATE, *PPF_TRIM_WHILE_AGING_STATE;

typedef struct _PF_TRIM_WHILE_AGING_CONTROL_1
{
    ULONG Version;
    PF_TRIM_WHILE_AGING_STATE TrimWhileAgingState;
    BOOLEAN PrivatePageTrimAge;
    BOOLEAN SharedPageTrimAge;
    USHORT Spare;
} PF_TRIM_WHILE_AGING_CONTROL_1, *PPF_TRIM_WHILE_AGING_CONTROL_1;

#define PF_TRIM_WHILE_AGING_CONTROL_VERSION_2 2

typedef struct _PF_TRIM_WHILE_AGING_CONTROL_2
{
    ULONG Version;
    PF_TRIM_WHILE_AGING_STATE TrimWhileAgingState;
    UCHAR PrivatePageTrimAge;
    UCHAR SharedPageTrimAge;
    USHORT Spare;
} PF_TRIM_WHILE_AGING_CONTROL_2, *PPF_TRIM_WHILE_AGING_CONTROL_2;

typedef struct _PF_TIME_CONTROL
{
    LONG TimeAdjustment;
} PF_TIME_CONTROL, *PPF_TIME_CONTROL;

#define PF_MEMORY_LIST_INFO_VERSION 1

typedef struct _PF_MEMORY_LIST_INFO
{
    ULONG Version;
    ULONG Size;
    ULONG NodeCount;
    PF_MEMORY_LIST_NODE Nodes[1];
} PF_MEMORY_LIST_INFO, *PPF_MEMORY_LIST_INFO;

typedef struct _PF_PHYSICAL_MEMORY_RANGE
{
    ULONG_PTR BasePfn;
    ULONG_PTR PageCount;
} PF_PHYSICAL_MEMORY_RANGE, *PPF_PHYSICAL_MEMORY_RANGE;

#define PF_PHYSICAL_MEMORY_RANGE_INFO_V1_VERSION 1

typedef struct _PF_PHYSICAL_MEMORY_RANGE_INFO_V1
{
    ULONG Version;
    ULONG RangeCount;
    PF_PHYSICAL_MEMORY_RANGE Ranges[1];
} PF_PHYSICAL_MEMORY_RANGE_INFO_V1, *PPF_PHYSICAL_MEMORY_RANGE_INFO_V1;

#define PF_PHYSICAL_MEMORY_RANGE_INFO_V2_VERSION 2

typedef struct _PF_PHYSICAL_MEMORY_RANGE_INFO_V2
{
    ULONG Version;
    ULONG Flags;
    SIZE_T RangeCount;
    PF_PHYSICAL_MEMORY_RANGE Ranges[1];
} PF_PHYSICAL_MEMORY_RANGE_INFO_V2, *PPF_PHYSICAL_MEMORY_RANGE_INFO_V2;

typedef struct _PF_START_TRACE_CONTROL
{
    struct
    {
        ULONG Type;
        ULONG Mode;
        ULONG Flags;
        ULONG Restart;
    };
    struct
    {
        HANDLE PartitionHandle;
        HANDLE TraceHandleOut;
    };
} PF_START_TRACE_CONTROL, *PPF_START_TRACE_CONTROL;

#define PF_ACCESS_TRACING_CONTROL_VERSION 1

typedef struct _PF_ACCESS_TRACING_CONTROL
{
    ULONG Version;
    ULONG Command;
    ULONG ComponentMask;
} PF_ACCESS_TRACING_CONTROL, *PPF_ACCESS_TRACING_CONTROL;

#define PF_REPURPOSED_BY_PREFETCH_INFO_VERSION 1

typedef struct _PF_REPURPOSED_BY_PREFETCH_INFO
{
    ULONG Version;
    ULONG Reserved;
    SIZE_T RepurposedByPrefetch;
} PF_REPURPOSED_BY_PREFETCH_INFO, *PPF_REPURPOSED_BY_PREFETCH_INFO;

#define PF_VIRTUAL_QUERY_VERSION 1

typedef struct _PF_VIRTUAL_QUERY
{
    ULONG Version;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG FaultInPageTables : 1;
            ULONG ReportPageTables : 1;
            ULONG Spare : 30;
        };
    };
    PVOID QueryBuffer;
    SIZE_T QueryBufferSize;
    HANDLE ProcessHandle;
} PF_VIRTUAL_QUERY, *PPF_VIRTUAL_QUERY;

#define PF_PAGECOMBINE_AGGREGATE_STAT_VERSION 1

typedef struct _PF_PAGECOMBINE_AGGREGATE_STAT
{
    ULONG Version;
    ULONG CombineScanCount;
    ULONG CombinedBlocksInUse;
    ULONG SumCombinedBlocksReferenceCount;
} PF_PAGECOMBINE_AGGREGATE_STAT, *PPF_PAGECOMBINE_AGGREGATE_STAT;

#define PF_MIN_WS_AGE_RATE_CONTROL_VERSION 1

typedef struct _PF_MIN_WS_AGE_RATE_CONTROL
{
    ULONG Version;
    ULONG SecondsToOldestAgeRate;
} PF_MIN_WS_AGE_RATE_CONTROL, *PPF_MIN_WS_AGE_RATE_CONTROL;

#define PF_DEPRIORITIZE_OLD_PAGES_VERSION 3

typedef struct _PF_DEPRIORITIZE_OLD_PAGES
{
    ULONG Version;
    HANDLE ProcessHandle;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG TargetPriority : 4;
            ULONG TrimPages : 2;
            ULONG Spare : 26;
        };
    };
} PF_DEPRIORITIZE_OLD_PAGES, *PPF_DEPRIORITIZE_OLD_PAGES;

#define PF_FILE_EXTENTS_INFO_VERSION 1

typedef struct _PF_FILE_EXTENTS_INFO
{
    ULONG Version;
    PWSTR FilePath;
    ULONG FilePathSize;
    ULONG VolumePathSize;
    LARGE_INTEGER FileIndexNumber;
    ULONG VolumeSerialNumber;
    RETRIEVAL_POINTERS_BUFFER ExtentsBuffer;
    ULONGLONG ExtentsBufferSize;
} PF_FILE_EXTENTS_INFO, *PPF_FILE_EXTENTS_INFO;

#define PF_FILE_EXTENTS_INFO_VERSION2 2

typedef struct _PF_FILE_EXTENTS_INFO_V2
{
    ULONG Version;
    ULONG Reserved0;
    PWSTR PathUtf16;

    ULONG PathBytes;
    ULONG PathMeta;
    ULONG ParamA;
    ULONG Reserved1;

    PVOID OutMeta0;
    PVOID OutBuffer;
    ULONG OutBufferBytesRequested;
    ULONG Reserved2;
} PF_FILE_EXTENTS_INFO_V2, *PPF_FILE_EXTENTS_INFO_V2;

#define PF_GPU_UTILIZATION_INFO_VERSION 1

typedef struct _PF_GPU_UTILIZATION_INFO
{
    ULONG Version;
    ULONG SessionId;
    ULONGLONG GpuTime;
} PF_GPU_UTILIZATION_INFO, *PPF_GPU_UTILIZATION_INFO;


#endif
