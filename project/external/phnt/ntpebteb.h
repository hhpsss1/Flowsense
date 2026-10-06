/*
 * Process and Thread Environment Block support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTPEBTEB_H
#define _NTPEBTEB_H

#ifdef __has_include
#if __has_include (<ntgdi.h>)
#include <ntgdi.h>
#endif
#if __has_include (<ntsxs.h>)
#include <ntsxs.h>
#endif
#endif

typedef struct _APPCOMPAT_EXE_DATA APPCOMPAT_EXE_DATA, *PAPPCOMPAT_EXE_DATA;
typedef struct _RTL_USER_PROCESS_PARAMETERS *PRTL_USER_PROCESS_PARAMETERS;
typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION;
typedef struct _SILO_USER_SHARED_DATA *PSILO_USER_SHARED_DATA;
typedef struct _LDR_RESLOADER_RET LDR_RESLOADER_RET, *PLDR_RESLOADER_RET;
typedef struct _LEAP_SECOND_DATA *PLEAP_SECOND_DATA;
typedef struct _PEB_LDR_DATA PEB_LDR_DATA, *PPEB_LDR_DATA;
typedef struct tagSOleTlsData SOleTlsData, *PSOleTlsData;
typedef struct _KERNEL_CALLBACK_TABLE KERNEL_CALLBACK_TABLE, *PKERNEL_CALLBACK_TABLE;
typedef struct _GDI_HANDLE_ENTRY GDI_HANDLE_ENTRY, *PGDI_HANDLE_ENTRY;
typedef struct _SHIM_PROCESS_CONTEXT SHIM_PROCESS_CONTEXT, *PSHIM_PROCESS_CONTEXT;
typedef struct _HEAP HEAP, *PHEAP;

#define KACF_OLDGETSHORTPATHNAME                      0x00000001
#define KACF_VERSIONLIE_NOT_USED                      0x00000002
#define KACF_GETTEMPPATH_NOT_USED                     0x00000004
#define KACF_GETDISKFREESPACE                         0x00000008
#define KACF_APPCOMPATFLAG_BIT4                       0x00000010
#define KACF_FTMFROMCURRENTAPT                        0x00000020
#define KACF_DISALLOWORBINDINGCHANGES                 0x00000040
#define KACF_OLE32VALIDATEPTRS                        0x00000080
#define KACF_DISABLECICERO                            0x00000100
#define KACF_OLE32ENABLEASYNCDOCFILE                  0x00000200
#define KACF_OLE32ENABLELEGACYEXCEPTIONHANDLING       0x00000400
#define KACF_RPCDISABLENDRCLIENTHARDENING             0x00000800
#define KACF_RPCDISABLENDRMAYBENULL_SIZEIS            0x00001000
#define KACF_DISABLEALLDDEHACK_NOT_USED               0x00002000
#define KACF_RPCDISABLENDR61_RANGE                    0x00004000
#define KACF_RPC32ENABLELEGACYEXCEPTIONHANDLING       0x00008000
#define KACF_OLE32DOCFILEUSELEGACYNTFSFLAGS           0x00010000
#define KACF_RPCDISABLENDRCONSTIIDCHECK               0x00020000
#define KACF_USERDISABLEFORWARDERPATCH                0x00040000
#define KACF_APPCOMPATFLAG_BIT19                      0x00080000
#define KACF_OLE32DISABLENEW_WMPAINT_DISPATCH         0x00100000
#define KACF_ADDRESTRICTEDSIDINCOINITIALIZESECURITY   0x00200000
#define KACF_ALLOCDEBUGINFOFORCRITSECTIONS            0x00400000
#define KACF_OLEAUT32ENABLEUNSAFELOADTYPELIBRELATIVE  0x00800000
#define KACF_ALLOWMAXIMIZEDWINDOWGAMMA                0x01000000
#define KACF_DONOTADDTOCACHE                          0x80000000
#define KACF_DISABLEPOSIXDELETEFILE                   0x100000000
#define KACF_ENABLE_PROCESS_SYSTEMDPIAWARENESS        0x20000000000000
#define KACF_DISABLE_PROCESS_SYSTEMDPIAWARENESS       0x40000000000000
#define KACF_ENABLE_GDI_DPI_SCALING                   0x800000000000000
#define KACF_FORCE_DISABLE_GDI_SCALING                0x4000000000000000

#define PEB_FLAG_PROCESS_IN_JOB                       0x00000001
#define PEB_FLAG_PROCESS_INITIALIZING                 0x00000002
#define PEB_FLAG_PROCESS_USING_VEH                    0x00000004
#define PEB_FLAG_PROCESS_USING_VCH                    0x00000008
#define PEB_FLAG_PROCESS_USING_FTH                    0x00000010
#define PEB_FLAG_PROCESS_PREVIOUSLY_THROTTLED         0x00000020
#define PEB_FLAG_PROCESS_CURRENTLY_THROTTLED          0x00000040
#define PEB_FLAG_PROCESS_IMAGES_HOT_PATCHED           0x00000080

#define API_SET_SECTION_NAME ".apiset"

#define API_SET_SCHEMA_VERSION_V2 0x00000002
#define API_SET_SCHEMA_VERSION_V4 0x00000004
#define API_SET_SCHEMA_VERSION_V6 0x00000006
#define API_SET_SCHEMA_VERSION API_SET_SCHEMA_VERSION_V6

#define API_SET_SCHEMA_FLAGS_SEALED 0x00000001
#define API_SET_SCHEMA_FLAGS_HOST_EXTENSION 0x00000002

#define API_SET_SCHEMA_ENTRY_FLAGS_SEALED 0x00000001
#define API_SET_SCHEMA_ENTRY_FLAGS_EXTENSION 0x00000002

typedef struct _API_SET_VALUE_ENTRY_V2
{
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V2, *PAPI_SET_VALUE_ENTRY_V2;

typedef struct _API_SET_VALUE_ARRAY_V2
{
    ULONG Count;
    _Field_size_full_(Count) API_SET_VALUE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V2, *PAPI_SET_VALUE_ARRAY_V2;

typedef struct _API_SET_NAMESPACE_ENTRY_V2
{
    ULONG NameOffset;
    ULONG NameLength;
    ULONG DataOffset;
} API_SET_NAMESPACE_ENTRY_V2, *PAPI_SET_NAMESPACE_ENTRY_V2;

typedef struct _API_SET_NAMESPACE_ARRAY_V2
{
    ULONG Version;
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V2, *PAPI_SET_NAMESPACE_ARRAY_V2;

typedef struct _API_SET_VALUE_ENTRY_V4
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V4, *PAPI_SET_VALUE_ENTRY_V4;

typedef struct _API_SET_VALUE_ARRAY_V4
{
    ULONG Flags;
    ULONG Count;
    _Field_size_full_(Count) API_SET_VALUE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V4, *PAPI_SET_VALUE_ARRAY_V4;

typedef struct _API_SET_NAMESPACE_ENTRY_V4
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG AliasOffset;
    ULONG AliasLength;
    ULONG DataOffset;
} API_SET_NAMESPACE_ENTRY_V4, *PAPI_SET_NAMESPACE_ENTRY_V4;

typedef struct _API_SET_NAMESPACE_ARRAY_V4
{
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V4, *PAPI_SET_NAMESPACE_ARRAY_V4;

typedef struct _API_SET_VALUE_ENTRY
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY, *PAPI_SET_VALUE_ENTRY;

typedef struct _API_SET_NAMESPACE_ENTRY
{
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG HashedLength;
    ULONG ValueOffset;
    ULONG ValueCount;
} API_SET_NAMESPACE_ENTRY, *PAPI_SET_NAMESPACE_ENTRY;

typedef struct _API_SET_HASH_ENTRY
{
    ULONG Hash;
    ULONG Index;
} API_SET_HASH_ENTRY, *PAPI_SET_HASH_ENTRY;

typedef struct _API_SET_NAMESPACE
{
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    ULONG EntryOffset;
    ULONG HashOffset;
    ULONG HashFactor;
} API_SET_NAMESPACE, *PAPI_SET_NAMESPACE;

typedef struct _TELEMETRY_COVERAGE_HEADER
{
    UCHAR MajorVersion;
    UCHAR MinorVersion;
    struct
    {
        USHORT TracingEnabled : 1;
        USHORT Reserved1 : 15;
    };
    ULONG HashTableEntries;
    ULONG HashIndexMask;
    ULONG TableUpdateVersion;
    ULONG TableSizeInBytes;
    ULONG LastResetTick;
    ULONG ResetRound;
    ULONG Reserved2;
    ULONG RecordedCount;
    ULONG Reserved3[4];
    ULONG HashTable[ANYSIZE_ARRAY];
} TELEMETRY_COVERAGE_HEADER, *PTELEMETRY_COVERAGE_HEADER;

typedef struct _WER_RECOVERY_INFO
{
    ULONG Length;
    PVOID Callback;
    PVOID Parameter;
    HANDLE Started;
    HANDLE Finished;
    HANDLE InProgress;
    LONG LastError;
    BOOL Successful;
    ULONG PingInterval;
    ULONG Flags;
} WER_RECOVERY_INFO, *PWER_RECOVERY_INFO;

typedef struct _WER_FILE
{
    USHORT Flags;
    WCHAR Path[MAX_PATH];
} WER_FILE, *PWER_FILE;

typedef struct _WER_MEMORY
{
    PVOID Address;
    ULONG Size;
} WER_MEMORY, *PWER_MEMORY;

typedef struct _WER_GATHER
{
    PVOID Next;
    USHORT Flags;
    union
    {
      WER_FILE File;
      WER_MEMORY Memory;
    } v;
} WER_GATHER, *PWER_GATHER;

typedef struct _WER_METADATA
{
    PVOID Next;
    WCHAR Key[64];
    WCHAR Value[128];
} WER_METADATA, *PWER_METADATA;

typedef struct _WER_RUNTIME_DLL
{
    PVOID Next;
    ULONG Length;
    PVOID Context;
    WCHAR CallbackDllPath[MAX_PATH];
} WER_RUNTIME_DLL, *PWER_RUNTIME_DLL;

typedef struct _WER_DUMP_COLLECTION
{
    PVOID Next;
    ULONG ProcessId;
    ULONG ThreadId;
} WER_DUMP_COLLECTION, *PWER_DUMP_COLLECTION;

typedef struct _WER_HEAP_MAIN_HEADER
{
    WCHAR Signature[16];
    LIST_ENTRY Links;
    HANDLE Mutex;
    PVOID FreeHeap;
    ULONG FreeCount;
} WER_HEAP_MAIN_HEADER, *PWER_HEAP_MAIN_HEADER;

#ifndef RESTART_MAX_CMD_LINE
#define RESTART_MAX_CMD_LINE 1024
#endif

typedef struct _WER_PEB_HEADER_BLOCK
{
    LONG Length;
    WCHAR Signature[16];
    WCHAR AppDataRelativePath[64];
    WCHAR RestartCommandLine[RESTART_MAX_CMD_LINE];
    WER_RECOVERY_INFO RecoveryInfo;
    PWER_GATHER Gather;
    PWER_METADATA MetaData;
    PWER_RUNTIME_DLL RuntimeDll;
    PWER_DUMP_COLLECTION DumpCollection;
    LONG GatherCount;
    LONG MetaDataCount;
    LONG DumpCount;
    LONG Flags;
    WER_HEAP_MAIN_HEADER MainHeader;
    PVOID Reserved;
} WER_PEB_HEADER_BLOCK, *PWER_PEB_HEADER_BLOCK;

#define GDI_HANDLE_BUFFER_SIZE32 34
#define GDI_HANDLE_BUFFER_SIZE64 60

#ifndef _WIN64
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE64
#endif

typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];

typedef _Function_class_(PS_POST_PROCESS_INIT_ROUTINE)
VOID NTAPI PS_POST_PROCESS_INIT_ROUTINE(
    VOID
    );
typedef PS_POST_PROCESS_INIT_ROUTINE* PPS_POST_PROCESS_INIT_ROUTINE;

#ifndef RTL_FLS_MAXIMUM_AVAILABLE
#define RTL_FLS_MAXIMUM_AVAILABLE 128
#endif
#ifndef FLS_MAXIMUM_AVAILABLE
#define FLS_MAXIMUM_AVAILABLE 4080
#endif
#ifndef TLS_MINIMUM_AVAILABLE
#define TLS_MINIMUM_AVAILABLE 64
#endif
#ifndef TLS_EXPANSION_SLOTS
#define TLS_EXPANSION_SLOTS 1024
#endif

/**
 * Process Environment Block (PEB) structure.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-peb
 */
typedef struct _PEB
{
    BOOLEAN InheritedAddressSpace;

    BOOLEAN ReadImageFileExecOptions;

    BOOLEAN BeingDebugged;

    union
    {
        BOOLEAN BitField;
        struct
        {
            BOOLEAN ImageUsesLargePages : 1;
            BOOLEAN IsProtectedProcess : 1;
            BOOLEAN IsImageDynamicallyRelocated : 1;
            BOOLEAN SkipPatchingUser32Forwarders : 1;
            BOOLEAN IsPackagedProcess : 1;
            BOOLEAN IsAppContainerProcess : 1;
            BOOLEAN IsProtectedProcessLight : 1;
            BOOLEAN IsLongPathAwareProcess : 1;
        };
    };

    HANDLE Mutant;

    PVOID ImageBaseAddress;

    PPEB_LDR_DATA Ldr;

    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;

    PVOID SubSystemData;

    PHEAP ProcessHeap;

    PRTL_CRITICAL_SECTION FastPebLock;

    PSLIST_HEADER AtlThunkSListPtr;

    HANDLE IFEOKey;

    union
    {
        ULONG CrossProcessFlags;
        struct
        {
            ULONG ProcessInJob : 1;
            ULONG ProcessInitializing : 1;
            ULONG ProcessUsingVEH : 1;
            ULONG ProcessUsingVCH : 1;
            ULONG ProcessUsingFTH : 1;
            ULONG ProcessPreviouslyThrottled : 1;
            ULONG ProcessCurrentlyThrottled : 1;
            ULONG ProcessImagesHotPatched : 1;
            ULONG ReservedBits0 : 24;
        };
    };

    union
    {
        PKERNEL_CALLBACK_TABLE KernelCallbackTable;
        PVOID UserSharedInfoPtr;
    };

    ULONG SystemReserved;

    ULONG AtlThunkSListPtr32;

    PAPI_SET_NAMESPACE ApiSetMap;

    ULONG TlsExpansionCounter;

    PRTL_BITMAP TlsBitmap;

    ULONG TlsBitmapBits[2];

    PVOID ReadOnlySharedMemoryBase;

    PSILO_USER_SHARED_DATA SharedData;

    PVOID* ReadOnlyStaticServerData;

    PCPTABLEINFO AnsiCodePageData;

    PCPTABLEINFO OemCodePageData;

    PNLSTABLEINFO UnicodeCaseTableData;

    ULONG NumberOfProcessors;

    union
    {
        ULONG NtGlobalFlag;
        struct
        {
            ULONG StopOnException : 1;
            ULONG ShowLoaderSnaps : 1;
            ULONG DebugInitialCommand : 1;
            ULONG StopOnHungGUI : 1;
            ULONG HeapEnableTailCheck : 1;
            ULONG HeapEnableFreeCheck : 1;
            ULONG HeapValidateParameters : 1;
            ULONG HeapValidateAll : 1;
            ULONG ApplicationVerifier : 1;
            ULONG MonitorSilentProcessExit : 1;
            ULONG PoolEnableTagging : 1;
            ULONG HeapEnableTagging : 1;
            ULONG UserStackTraceDb : 1;
            ULONG KernelStackTraceDb : 1;
            ULONG MaintainObjectTypeList : 1;
            ULONG HeapEnableTagByDll : 1;
            ULONG DisableStackExtension : 1;
            ULONG EnableCsrDebug : 1;
            ULONG EnableKDebugSymbolLoad : 1;
            ULONG DisablePageKernelStacks : 1;
            ULONG EnableSystemCritBreaks : 1;
            ULONG HeapDisableCoalescing : 1;
            ULONG EnableCloseExceptions : 1;
            ULONG EnableExceptionLogging : 1;
            ULONG EnableHandleTypeTagging : 1;
            ULONG HeapPageAllocs : 1;
            ULONG DebugInitialCommandEx : 1;
            ULONG DisableDbgPrint : 1;
            ULONG CritSecEventCreation : 1;
            ULONG LdrTopDown : 1;
            ULONG EnableHandleExceptions : 1;
            ULONG DisableProtDlls : 1;
        } NtGlobalFlags;
    };

    LARGE_INTEGER CriticalSectionTimeout;

    SIZE_T HeapSegmentReserve;

    SIZE_T HeapSegmentCommit;

    SIZE_T HeapDeCommitTotalFreeThreshold;

    SIZE_T HeapDeCommitFreeBlockThreshold;

    ULONG NumberOfHeaps;

    ULONG MaximumNumberOfHeaps;

    PVOID* ProcessHeaps;

    PGDI_HANDLE_ENTRY GdiSharedHandleTable;

    PVOID ProcessStarterHelper;

    ULONG GdiDCAttributeList;

    PRTL_CRITICAL_SECTION LoaderLock;

    ULONG OSMajorVersion;

    ULONG OSMinorVersion;

    USHORT OSBuildNumber;

    USHORT OSCSDVersion;

    ULONG OSPlatformId;

    ULONG ImageSubsystem;

    ULONG ImageSubsystemMajorVersion;

    ULONG ImageSubsystemMinorVersion;

    KAFFINITY ActiveProcessAffinityMask;

    GDI_HANDLE_BUFFER GdiHandleBuffer;

    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;

    PRTL_BITMAP TlsExpansionBitmap;

    ULONG TlsExpansionBitmapBits[32];

    ULONG SessionId;

    ULARGE_INTEGER AppCompatFlags;

    ULARGE_INTEGER AppCompatFlagsUser;

    PSHIM_PROCESS_CONTEXT pShimData;

    PAPPCOMPAT_EXE_DATA AppCompatInfo;

    UNICODE_STRING CSDVersion;

    PACTIVATION_CONTEXT_DATA ActivationContextData;

    PASSEMBLY_STORAGE_MAP ProcessAssemblyStorageMap;

    PACTIVATION_CONTEXT_DATA SystemDefaultActivationContextData;

    PASSEMBLY_STORAGE_MAP SystemAssemblyStorageMap;

    SIZE_T MinimumStackCommit;

    PVOID SparePointers[2];

    PVOID PatchLoaderData;

    PVOID ChpeV2ProcessInfo;

    ULONG AppModelFeatureState;

    ULONG SpareUlongs[2];

    USHORT ActiveCodePage;

    USHORT OemCodePage;

    USHORT UseCaseMapping;

    USHORT UnusedNlsField;

    PWER_PEB_HEADER_BLOCK WerRegistrationData;

    PVOID WerShipAssertPtr;

    union
    {
        PVOID pContextData;
        PVOID EcCodeBitMap;
    };

    PVOID ImageHeaderHash;

    union
    {
        ULONG TracingFlags;
        struct
        {
            ULONG HeapTracingEnabled : 1;
            ULONG CritSecTracingEnabled : 1;
            ULONG LibLoaderTracingEnabled : 1;
            ULONG SpareTracingBits : 29;
        };
    };

    ULONGLONG CsrServerReadOnlySharedMemoryBase;

    PRTL_CRITICAL_SECTION TppWorkerpListLock;

    LIST_ENTRY TppWorkerpList;

    PVOID WaitOnAddressHashTable[128];

    PTELEMETRY_COVERAGE_HEADER TelemetryCoverageHeader;

    ULONG CloudFileFlags;

    ULONG CloudFileDiagFlags;

    CHAR PlaceholderCompatibilityMode;

    CHAR PlaceholderCompatibilityModeReserved[7];

    PLEAP_SECOND_DATA LeapSecondData;

    union
    {
        ULONG LeapSecondFlags;
        struct
        {
            ULONG SixtySecondEnabled : 1;
            ULONG Reserved : 31;
        };
    };

    ULONG NtGlobalFlag2;

    ULONGLONG ExtendedFeatureDisableMask;
} PEB, *PPEB;

#ifdef _WIN64
static_assert(FIELD_OFFSET(PEB, SessionId) == 0x2C0, "FIELD_OFFSET(PEB, SessionId) is incorrect");
static_assert(sizeof(PEB) == 0x7d0, "Size of PEB is incorrect");
#else
static_assert(FIELD_OFFSET(PEB, SessionId) == 0x1D4, "FIELD_OFFSET(PEB, SessionId) is incorrect");
static_assert(sizeof(PEB) == 0x488, "Size of PEB is incorrect");
#endif

#define GDI_BATCH_BUFFER_SIZE 310

/**
 * The GDI_TEB_BATCH structure is used to store information about GDI batch operations.
 */
typedef struct _GDI_TEB_BATCH
{
    ULONG Offset;
    ULONG_PTR HDC;
    ULONG Buffer[GDI_BATCH_BUFFER_SIZE];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

#define TEB_ACTIVE_FRAME_CONTEXT_FLAG_EXTENDED (0x00000001)

/**
 * The TEB_ACTIVE_FRAME_CONTEXT structure is used to store information about an active frame context.
 */
typedef struct _TEB_ACTIVE_FRAME_CONTEXT
{
    ULONG Flags;
    PCSTR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

/**
 * The TEB_ACTIVE_FRAME_CONTEXT_EX structure extends TEB_ACTIVE_FRAME_CONTEXT with additional information.
 */
typedef struct _TEB_ACTIVE_FRAME_CONTEXT_EX
{
    TEB_ACTIVE_FRAME_CONTEXT BasicContext;
    PCSTR SourceLocation;
} TEB_ACTIVE_FRAME_CONTEXT_EX, *PTEB_ACTIVE_FRAME_CONTEXT_EX;

#define TEB_ACTIVE_FRAME_FLAG_EXTENDED (0x00000001)

/**
 * The TEB_ACTIVE_FRAME structure is used to store information about an active frame.
 */
typedef struct _TEB_ACTIVE_FRAME
{
    ULONG Flags;
    struct _TEB_ACTIVE_FRAME *Previous;
    PTEB_ACTIVE_FRAME_CONTEXT Context;
} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

/**
 * The TEB_ACTIVE_FRAME_EX structure extends TEB_ACTIVE_FRAME with additional information.
 */
typedef struct _TEB_ACTIVE_FRAME_EX
{
    TEB_ACTIVE_FRAME BasicFrame;
    PVOID ExtensionIdentifier;
} TEB_ACTIVE_FRAME_EX, *PTEB_ACTIVE_FRAME_EX;

#define STATIC_UNICODE_BUFFER_LENGTH 261
#define WIN32_CLIENT_INFO_LENGTH 62

#if (PHNT_MODE != PHNT_MODE_KERNEL)
typedef struct _CALLBACKWND
{
    HWND hwnd;
    ULONG_PTR pwnd;
    PACTIVATION_CONTEXT ActCtx;
} CALLBACKWND, *PCALLBACKWND;

typedef struct tagDPICONTEXTINFO
{
    ULONG dpiContext;
    LOGICAL Dirty;
} DPICONTEXTINFO, *PDPICONTEXTINFO;

typedef struct tagCLIENTINFO
{
    ULONG_PTR CI_flags;
    ULONG_PTR Spins;
    ULONG ExpWinVer;
    ULONG CompatFlags;
    ULONG CompatFlags2;
    ULONG TIFlags;
    struct tagDESKTOPINFO* DeskInfo;
    PVOID DesktopBase;
    HHOOK hkCurrent;
    ULONG Hooks;
    CALLBACKWND CallbackWnd;
    ULONG HookCurrent;
    LONG InDDEMLCallback;
    struct tagCLIENTTHREADINFO* ClientThreadInfo;
    ULONG_PTR HookData;
    ULONG KeyCache;
    UCHAR KeyState[8];
    ULONG AsyncKeyCache;
    UCHAR AsyncKeyState[8];
    UCHAR AsyncKeyStateRecentDown[8];
    HKL hKL;
    USHORT CodePage;
    UCHAR DbcsCFOld[2];
    UCHAR DbcsCFNew[2];
    MSG msgDbcsCB;
    PULONG RegisteredClasses;
    HANDLE mmcssHandle;
    ULONG_PTR CI_exflags;
    DPICONTEXTINFO dci;
} CLIENTINFO, *PCLIENTINFO;
#endif

#ifdef _WIN64
#define RPC_THREAD_POINTER_KEY 0xABABABABDEDEDEDEui64
#else
#define RPC_THREAD_POINTER_KEY 0xABABABAB
#endif

/**
 * Thread Environment Block (TEB) structure.
 *
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-teb
 */
typedef struct _TEB
{
    NT_TIB NtTib;

    PVOID EnvironmentPointer;

    CLIENT_ID ClientId;

    PVOID ActiveRpcHandle;

    PVOID ThreadLocalStoragePointer;

    PPEB ProcessEnvironmentBlock;

    ULONG LastErrorValue;

    ULONG CountOfOwnedCriticalSections;

    PVOID CsrClientThread;

    PVOID Win32ThreadInfo;

    ULONG User32Reserved[26];

    ULONG UserReserved[5];

    PVOID WOW32Reserved;

    LCID CurrentLocale;

    ULONG FpSoftwareStatusRegister;

    PVOID ReservedForDebuggerInstrumentation[16];

#ifdef _WIN64
    PVOID SystemReserved1[25];

    PVOID HeapFlsData;

    ULONG_PTR RngState[4];
#else
    PVOID SystemReserved1[26];
#endif

    CHAR PlaceholderCompatibilityMode;

    BOOLEAN PlaceholderHydrationAlwaysExplicit;

    CHAR PlaceholderReserved[10];

    ULONG ProxiedProcessId;

    ACTIVATION_CONTEXT_STACK ActivationStack;

    UCHAR WorkingOnBehalfTicket[8];

    NTSTATUS ExceptionCode;

    PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;

    ULONG_PTR InstrumentationCallbackSp;

    ULONG_PTR InstrumentationCallbackPreviousPc;

    ULONG_PTR InstrumentationCallbackPreviousSp;

#ifdef _WIN64
    ULONG TxFsContext;
#endif

    BOOLEAN InstrumentationCallbackDisabled;

#ifdef _WIN64
    BOOLEAN UnalignedLoadStoreExceptions;
#endif

#ifndef _WIN64
    UCHAR SpareBytes[23];

    ULONG TxFsContext;
#endif

    GDI_TEB_BATCH GdiTebBatch;
    CLIENT_ID RealClientId;
    HANDLE GdiCachedProcessHandle;
    ULONG GdiClientPID;
    ULONG GdiClientTID;
    PVOID GdiThreadLocalInfo;

#if (PHNT_MODE != PHNT_MODE_KERNEL)
    union
    {
        CLIENTINFO Win32ClientInfo;
        ULONG_PTR Win32ClientInfoArea[WIN32_CLIENT_INFO_LENGTH];
    };
#else
    ULONG_PTR Win32ClientInfo[WIN32_CLIENT_INFO_LENGTH];
#endif

    PVOID glDispatchTable[233];
    ULONG_PTR glReserved1[29];
    PVOID glReserved2;
    PVOID glSectionInfo;
    PVOID glSection;
    PVOID glTable;
    PVOID glCurrentRC;
    PVOID glContext;

    NTSTATUS LastStatusValue;

    UNICODE_STRING StaticUnicodeString;

    WCHAR StaticUnicodeBuffer[STATIC_UNICODE_BUFFER_LENGTH];

    PVOID DeallocationStack;

    PVOID TlsSlots[TLS_MINIMUM_AVAILABLE];

    LIST_ENTRY TlsLinks;

    PVOID Vdm;

    PVOID ReservedForNtRpc;

    PVOID DbgSsReserved[2];

    ULONG HardErrorMode;

#ifdef _WIN64
    PVOID Instrumentation[11];
#else
    PVOID Instrumentation[9];
#endif

    GUID ActivityId;

    PVOID SubProcessTag;

    PVOID PerflibData;

    PVOID EtwTraceData;

    HANDLE WinSockData;

    ULONG GdiBatchCount;

    union
    {
        PROCESSOR_NUMBER CurrentIdealProcessor;
        ULONG IdealProcessorValue;
        struct
        {
            UCHAR ReservedPad0;
            UCHAR ReservedPad1;
            UCHAR ReservedPad2;
            UCHAR IdealProcessor;
        };
    };

    ULONG GuaranteedStackBytes;

    PVOID ReservedForPerf;

    PSOleTlsData ReservedForOle;

    ULONG WaitingOnLoaderLock;

    PVOID SavedPriorityState;

    ULONG_PTR ReservedForCodeCoverage;

    PVOID ThreadPoolData;

    PVOID *TlsExpansionSlots;

#ifdef _WIN64
    PVOID ChpeV2CpuAreaInfo;
    PVOID Unused;
#endif

    ULONG MuiGeneration;

    ULONG IsImpersonating;

    PVOID NlsCache;

    PVOID pShimData;

    ULONG HeapData;

    HANDLE CurrentTransactionHandle;

    PTEB_ACTIVE_FRAME ActiveFrame;

    PVOID FlsData;

    PVOID PreferredLanguages;

    PVOID UserPrefLanguages;

    PVOID MergedPrefLanguages;

    ULONG MuiImpersonation;

    union
    {
        USHORT CrossTebFlags;
        USHORT SpareCrossTebBits : 16;
    };

    union
    {
        USHORT SameTebFlags;
        struct
        {
            USHORT SafeThunkCall : 1;
            USHORT InDebugPrint : 1;
            USHORT HasFiberData : 1;
            USHORT SkipThreadAttach : 1;
            USHORT WerInShipAssertCode : 1;
            USHORT RanProcessInit : 1;
            USHORT ClonedThread : 1;
            USHORT SuppressDebugMsg : 1;
            USHORT DisableUserStackWalk : 1;
            USHORT RtlExceptionAttached : 1;
            USHORT InitialThread : 1;
            USHORT SessionAware : 1;
            USHORT LoadOwner : 1;
            USHORT LoaderWorker : 1;
            USHORT SkipLoaderInit : 1;
            USHORT SkipFileAPIBrokering : 1;
        };
    };

    PVOID TxnScopeEnterCallback;

    PVOID TxnScopeExitCallback;

    PVOID TxnScopeContext;

    ULONG LockCount;

    LONG WowTebOffset;

    PLDR_RESLOADER_RET ResourceRetValue;

    PVOID ReservedForWdf;

    ULONGLONG ReservedForCrt;

    GUID EffectiveContainerId;

    ULONGLONG LastSleepCounter;

    ULONG SpinCallCount;

    ULONGLONG ExtendedFeatureDisableMask;

    PVOID SchedulerSharedDataSlot;

    PVOID HeapWalkContext;

    GROUP_AFFINITY PrimaryGroupAffinity;

    ULONG Rcu[2];
} TEB, *PTEB;

#ifdef _WIN64
static_assert(FIELD_OFFSET(TEB, SchedulerSharedDataSlot) == 0x1850, "Size of TEB is incorrect");
static_assert(sizeof(TEB) == 0x1878, "Size of TEB is incorrect");
#else
static_assert(FIELD_OFFSET(TEB, SchedulerSharedDataSlot) == 0x1018, "Size of TEB is incorrect");
static_assert(sizeof(TEB) == 0x1038, "Size of TEB is incorrect");
#endif

#endif
