/*
 * Trace Control support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTMISC_H
#define _NTMISC_H


_Enum_is_bitflag_
typedef enum _AHC_INFO_CLASS
{
    AhcInfoClassSdbQueryResult          = 0x00000001,
    AhcInfoClassSdbSxsOverrideManifest  = 0x00000002,
    AhcInfoClassSdbRunlevelFlags        = 0x00000004,
    AhcInfoClassSdbFusionFlags          = 0x00000008,
    AhcInfoClassSdbInstallerFlags       = 0x00000010,
    AhcInfoClassFusionFlags             = 0x00000020,
    AhcInfoClassTelemetryFlags          = 0x00000040,
    AhcInfoClassInstallDetect           = 0x00000080,
    AhcInfoClassRacEventSent            = 0x00000100,
    AhcInfoClassIsSystemFile            = 0x00000200,
    AhcInfoClassMonitoringFlags         = 0x00000400,
    AhcInfoClassExeType                 = 0x00000800,
} AHC_INFO_CLASS, *PAHC_INFO_CLASS;

#define AHC_INFO_CLASS_FILTER_ON_FILETIME_CHANGE            \
    (AHC_INFO_CLASS)(AhcInfoClassSdbQueryResult |           \
                     AhcInfoClassSdbSxsOverrideManifest |   \
                     AhcInfoClassSdbRunlevelFlags |         \
                     AhcInfoClassSdbFusionFlags |           \
                     AhcInfoClassSdbInstallerFlags |        \
                     AhcInfoClassFusionFlags |              \
                     AhcInfoClassRacEventSent)

#define AHC_INFO_CLASS_FILTER_ON_SDB_CHANGE                 \
    (AHC_INFO_CLASS)(AhcInfoClassSdbQueryResult |           \
                     AhcInfoClassSdbSxsOverrideManifest |   \
                     AhcInfoClassSdbRunlevelFlags |         \
                     AhcInfoClassSdbFusionFlags |           \
                     AhcInfoClassSdbInstallerFlags |        \
                     AhcInfoClassInstallDetect)

#define AHC_INFO_CLASS_ALL                                  \
    (AHC_INFO_CLASS)(AhcInfoClassSdbQueryResult |           \
                     AhcInfoClassSdbSxsOverrideManifest |   \
                     AhcInfoClassSdbRunlevelFlags |         \
                     AhcInfoClassSdbFusionFlags |           \
                     AhcInfoClassSdbInstallerFlags |        \
                     AhcInfoClassFusionFlags |              \
                     AhcInfoClassTelemetryFlags |           \
                     AhcInfoClassInstallDetect |            \
                     AhcInfoClassRacEventSent |             \
                     AhcInfoClassIsSystemFile |             \
                     AhcInfoClassMonitoringFlags |          \
                     AhcInfoClassExeType)

#define AHC_INFO_CLASS_INTERNALLY_COMPUTED                  \
    (AHC_INFO_CLASS)(AhcInfoClassSdbQueryResult |           \
                     AhcInfoClassSdbSxsOverrideManifest |   \
                     AhcInfoClassSdbRunlevelFlags |         \
                     AhcInfoClassSdbFusionFlags |           \
                     AhcInfoClassSdbInstallerFlags |        \
                     AhcInfoClassTelemetryFlags |           \
                     AhcInfoClassIsSystemFile |             \
                     AhcInfoClassMonitoringFlags |          \
                     AhcInfoClassExeType)

#define AHC_INFO_CLASS_SAFE_FOR_UNPRIVILEGED_UPDATE         \
    (AHC_INFO_CLASS)(AhcInfoClassInstallDetect |            \
                     AhcInfoClassRacEventSent |             \
                     AhcInfoClassTelemetryFlags |           \
                     AhcInfoClassMonitoringFlags)


typedef enum _AHC_SERVICE_CLASS
{
    ApphelpCacheServiceLookup = 0,
    ApphelpCacheServiceRemove = 1,
    ApphelpCacheServiceUpdate = 2,
    ApphelpCacheServiceClear = 3,
    ApphelpCacheServiceSnapStatistics = 4,
    ApphelpCacheServiceSnapCache = 5,
    ApphelpCacheServiceLookupCdb = 6,
    ApphelpCacheServiceRefreshCdb = 7,
    ApphelpCacheServiceMapQuirks = 8,
    ApphelpCacheServiceHwIdQuery = 9,
    ApphelpCacheServiceInitProcessData = 10,
    ApphelpCacheServiceLookupAndWriteToProcess = 11,
    ApphelpCacheServiceMax
} AHC_SERVICE_CLASS;

typedef struct _AHC_SERVICE_LOOKUP
{
    AHC_INFO_CLASS InfoClass;
    UINT HintFlags;
    UNICODE_STRING PackageAlias;
    HANDLE FileHandle;
    HANDLE ProcessHandle;
    USHORT ExeType;
    USHORT Padding;
    UNICODE_STRING ExeSignature;
    PCZZWSTR Environment;
    UINT EnvironmentSize;
} AHC_SERVICE_LOOKUP, *PAHC_SERVICE_LOOKUP;

typedef struct _AHC_SERVICE_REMOVE
{
    AHC_INFO_CLASS InfoClass;
    UNICODE_STRING PackageAlias;
    HANDLE FileHandle;
    UNICODE_STRING ExeSignature;
} AHC_SERVICE_REMOVE, *PAHC_SERVICE_REMOVE;

typedef struct _AHC_SERVICE_UPDATE
{
    AHC_INFO_CLASS InfoClass;
    UNICODE_STRING PackageAlias;
    HANDLE FileHandle;
    UNICODE_STRING ExeSignature;
    PVOID Data;
    ULONG DataSize;
} AHC_SERVICE_UPDATE, *PAHC_SERVICE_UPDATE;

typedef struct _AHC_SERVICE_CLEAR
{
    AHC_INFO_CLASS InfoClass;
} AHC_SERVICE_CLEAR, *PAHC_SERVICE_CLEAR;

typedef struct _AHC_SERVICE_LOOKUP_CDB
{
    UNICODE_STRING Name;
} AHC_SERVICE_LOOKUP_CDB, *PAHC_SERVICE_LOOKUP_CDB;


#define AHC_HINT_FORCE_BYPASS                           0x00000001
#define AHC_HINT_REMOVABLE_MEDIA                        0x00000002
#define AHC_HINT_TEMPORARY_DIRECTORY                    0x00000004
#define AHC_HINT_USER_PERM_LAYER                        0x00000008
#define AHC_HINT_CREATE_PROCESS                         0x00000010
#define AHC_HINT_NATIVE_EXE                             0x00000020

#define SHIM_CACHE_MAIN_DATABASE_PATH32                 L"\\AppPatch\\sysmain.sdb"
#define SHIM_CACHE_MAIN_DATABASE_PATH64                 L"\\AppPatch\\AppPatch64\\sysmain.sdb"


#define AHC_CACHE_FLAG_MONITORING_IS_CANDIDATE          0x00000001
#define AHC_CACHE_FLAG_MONITORING_IS_COMPLETE           0x00000002
#define AHC_CACHE_FLAG_MONITORING_VALID_MASK            (AHC_CACHE_FLAG_MONITORING_IS_CANDIDATE | \
                                                         AHC_CACHE_FLAG_MONITORING_IS_COMPLETE)

#define AHC_CACHE_FLAG_TELEMETRY_IS_CANDIDATE           0x00000001
#define AHC_CACHE_FLAG_TELEMETRY_HAS_SAMPLED            0x00000002
#define AHC_CACHE_FLAG_TELEMETRY_VALID_MASK             (AHC_CACHE_FLAG_TELEMETRY_IS_CANDIDATE | \
                                                         AHC_CACHE_FLAG_TELEMETRY_HAS_SAMPLED)

#define AHC_CACHE_FLAG_FUSION_HASDOTLOCAL               0x00000001
#define AHC_CACHE_FLAG_FUSION_HASMANIFESTFILE           0x00000002
#define AHC_CACHE_FLAG_FUSION_HASMANIFESTRESOURCE       0x00000004
#define AHC_CACHE_FLAG_FUSION_VALID_MASK                (AHC_CACHE_FLAG_FUSION_HASDOTLOCAL | \
                                                         AHC_CACHE_FLAG_FUSION_HASMANIFESTFILE | \
                                                         AHC_CACHE_FLAG_FUSION_HASMANIFESTRESOURCE)

#define AHC_CACHE_FLAG_RAC_EVENTSENT                    0x00000001
#define AHC_CACHE_FLAG_RAC_VALID_MASK                   (AHC_CACHE_FLAG_RAC_EVENTSENT)

#define AHC_CACHE_FLAG_INSTALLDETECT_CLAIMED            0x00000001
#define AHC_CACHE_FLAG_INSTALLDETECT_VALID_MASK         (AHC_CACHE_FLAG_RAC_EVENTSENT)


typedef struct _AHC_MAIN_STATISTICS
{
    ULONG Lookup;
    ULONG Remove;
    ULONG Update;
    ULONG Clear;
    ULONG SnapStatistics;
    ULONG SnapCache;
} AHC_MAIN_STATISTICS, *PAHC_MAIN_STATISTICS;

typedef struct _AHC_STORE_STATISTICS
{
    ULONG LookupHits;
    ULONG LookupMisses;
    ULONG Inserted;
    ULONG Replaced;
    ULONG Updated;
} AHC_STORE_STATISTICS, *PAHC_STORE_STATISTICS;

typedef struct _AHC_STATISTICS
{
    ULONG Size;
    AHC_MAIN_STATISTICS Main;
    AHC_STORE_STATISTICS Store;
} AHC_STATISTICS, *PAHC_STATISTICS;

typedef struct _AHC_SERVICE_DATAQUERY
{
    AHC_STATISTICS Stats;
    ULONG DataSize;
    PBYTE Data;
} AHC_SERVICE_DATAQUERY, *PAHC_SERVICE_DATAQUERY;

typedef struct _AHC_SERVICE_DATACACHE
{
    HANDLE FileHandle;
    USHORT ExeType;
    USHORT Padding;
    UINT HintFlags;
    HANDLE ProcessHandle;
    UNICODE_STRING FileName;
    UNICODE_STRING Environment;
    UNICODE_STRING PackageAlias;
    ULONG CustomDataSize;
    PBYTE CustomData;
} AHC_SERVICE_DATACACHE, *PAHC_SERVICE_DATACACHE;

typedef struct _AHC_SERVICE_HWID_QUERY
{
    BOOLEAN QueryResult;
    UNICODE_STRING HwId;
} AHC_SERVICE_HWID_QUERY, *PAHC_SERVICE_HWID_QUERY;

typedef struct _AHC_SERVICE_DATA
{
    AHC_SERVICE_LOOKUP Lookup;
    AHC_SERVICE_UPDATE Update;
    AHC_SERVICE_DATACACHE Cache;
    AHC_SERVICE_LOOKUP_CDB LookupCdb;
    AHC_SERVICE_CLEAR Clear;
    AHC_SERVICE_REMOVE Remove;
    AHC_SERVICE_HWID_QUERY HwIdQuery;
    NTSTATUS DriverStatus;
    PVOID ParamsOut;
    ULONG ParamsOutSize;
} AHC_SERVICE_DATA, *PAHC_SERVICE_DATA;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtApphelpCacheControl(
    _In_ AHC_SERVICE_CLASS ServiceClass,
    _Inout_opt_ PVOID ServiceContext
    );


typedef enum _VDMSERVICECLASS
{
    VdmStartExecution,
    VdmQueueInterrupt,
    VdmDelayInterrupt,
    VdmInitialize,
    VdmFeatures,
    VdmSetInt21Handler,
    VdmQueryDir,
    VdmPrinterDirectIoOpen,
    VdmPrinterDirectIoClose,
    VdmPrinterInitialize,
    VdmSetLdtEntries,
    VdmSetProcessLdtInfo,
    VdmAdlibEmulation,
    VdmPMCliControl,
    VdmQueryVdmProcess,
    VdmPreInitialize
} VDMSERVICECLASS, *PVDMSERVICECLASS;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtVdmControl(
    _In_ VDMSERVICECLASS Service,
    _Inout_ PVOID ServiceData
    );


typedef enum _IO_SESSION_EVENT
{
    IoSessionEventIgnore,
    IoSessionEventCreated,
    IoSessionEventTerminated,
    IoSessionEventConnected,
    IoSessionEventDisconnected,
    IoSessionEventLogon,
    IoSessionEventLogoff,
    IoSessionEventMax
} IO_SESSION_EVENT;

typedef enum _IO_SESSION_STATE
{
    IoSessionStateCreated = 1,
    IoSessionStateInitialized = 2,
    IoSessionStateConnected = 3,
    IoSessionStateDisconnected = 4,
    IoSessionStateDisconnectedLoggedOn = 5,
    IoSessionStateLoggedOn = 6,
    IoSessionStateLoggedOff = 7,
    IoSessionStateTerminated = 8,
    IoSessionStateMax
} IO_SESSION_STATE;

#if (PHNT_MODE != PHNT_MODE_KERNEL)

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenSession(
    _Out_ PHANDLE SessionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeSession(
    _In_ HANDLE SessionHandle,
    _In_ ULONG ChangeSequenceNumber,
    _In_ PLARGE_INTEGER ChangeTimeStamp,
    _In_ IO_SESSION_EVENT Event,
    _In_ IO_SESSION_STATE NewState,
    _In_ IO_SESSION_STATE PreviousState,
    _In_reads_bytes_opt_(PayloadSize) PVOID Payload,
    _In_ ULONG PayloadSize
    );

#endif


NTSYSAPI
BOOL
NTAPI
ApiSetQueryApiSetPresence(
    _In_ PCUNICODE_STRING Namespace,
    _Out_ PBOOLEAN Present
    );

NTSYSAPI
BOOL
NTAPI
ApiSetQueryApiSetPresenceEx(
    _In_ PCUNICODE_STRING Namespace,
    _Out_ PBOOLEAN IsInSchema,
    _Out_ PBOOLEAN Present
    );

typedef enum _SECURE_SETTING_VALUE_TYPE
{
    SecureSettingValueTypeBoolean = 0,
    SecureSettingValueTypeUlong = 1,
    SecureSettingValueTypeBinary = 2,
    SecureSettingValueTypeString = 3,
    SecureSettingValueTypeUnknown = 4
} SECURE_SETTING_VALUE_TYPE, *PSECURE_SETTING_VALUE_TYPE;

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS1)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQuerySecurityPolicy(
    _In_ PCUNICODE_STRING Policy,
    _In_ PCUNICODE_STRING KeyName,
    _In_ PCUNICODE_STRING ValueName,
    _In_ SECURE_SETTING_VALUE_TYPE ValueType,
    _Out_writes_bytes_opt_(*ValueSize) PVOID Value,
    _Inout_ PULONG ValueSize
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_20H1)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateCrossVmEvent(
    _Out_ PHANDLE CrossVmEvent,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG CrossVmEventFlags,
    _In_ LPCGUID VMID,
    _In_ LPCGUID ServiceID
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateCrossVmMutant(
    _Out_ PHANDLE EventHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG CrossVmEventFlags,
    _In_ LPCGUID VMID,
    _In_ LPCGUID ServiceID
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAcquireCrossVmMutant(
    _In_ HANDLE CrossVmMutant,
    _In_ PLARGE_INTEGER Timeout
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_20H1)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDirectGraphicsCall(
    _In_ ULONG InputBufferLength,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG OutputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _Out_ PULONG ReturnLength
    );
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_11_22H2)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenCpuPartition(
    _Out_ PHANDLE CpuPartitionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateCpuPartition(
    _Out_ PHANDLE CpuPartitionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationCpuPartition(
    _In_ HANDLE CpuPartitionHandle,
    _In_ ULONG CpuPartitionInformationClass,
    _In_reads_bytes_(CpuPartitionInformationLength) PVOID CpuPartitionInformation,
    _In_ ULONG CpuPartitionInformationLength,
    _Reserved_ PVOID,
    _Reserved_ ULONG,
    _Reserved_ ULONG
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationCpuPartition(
    _In_ HANDLE CpuPartitionHandle,
    _In_ ULONG CpuPartitionInformationClass,
    _Out_writes_bytes_opt_(CpuPartitionInformationLength) PVOID CpuPartitionInformation,
    _In_ ULONG CpuPartitionInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS2)

typedef enum _PROCESS_ACTIVITY_TYPE
{
    ProcessActivityTypeAudio = 0,
    ProcessActivityTypeMax = 1
} PROCESS_ACTIVITY_TYPE;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtAcquireProcessActivityReference(
    _Out_ PHANDLE ActivityReferenceHandle,
    _In_ HANDLE ParentProcessHandle,
    _In_ ULONG ProcessActivityType
    );
#endif


typedef struct _PACKAGE_CONTEXT_REFERENCE
{
    PVOID reserved;
} *PACKAGE_CONTEXT_REFERENCE;

typedef enum PackageProperty
{
    PackageProperty_Name = 1,
    PackageProperty_Version = 2,
    PackageProperty_Architecture = 3,
    PackageProperty_ResourceId = 4,
    PackageProperty_Publisher = 5,
    PackageProperty_PublisherId = 6,
    PackageProperty_FamilyName = 7,
    PackageProperty_FullName = 8,
    PackageProperty_Flags = 9,
    PackageProperty_InstalledLocation = 10,
    PackageProperty_DisplayName = 11,
    PackageProperty_PublisherDisplayName = 12,
    PackageProperty_Description = 13,
    PackageProperty_Logo = 14,
    PackageProperty_PackageOrigin = 15
} PackageProperty;

typedef struct _PACKAGE_APPLICATION_CONTEXT_REFERENCE
{
    PVOID reserved;
} *PACKAGE_APPLICATION_CONTEXT_REFERENCE;

typedef enum PackageApplicationProperty
{
    PackageApplicationProperty_Aumid = 1,
    PackageApplicationProperty_Praid = 2,
    PackageApplicationProperty_DisplayName = 3,
    PackageApplicationProperty_Description = 4,
    PackageApplicationProperty_Logo = 5,
    PackageApplicationProperty_SmallLogo = 6,
    PackageApplicationProperty_ForegroundText = 7,
    PackageApplicationProperty_ForegroundTextString = 8,
    PackageApplicationProperty_BackgroundColor = 9,
    PackageApplicationProperty_StartPage = 10,
    PackageApplicationProperty_ContentURIRulesCount = 11,
    PackageApplicationProperty_ContentURIRules = 12,
    PackageApplicationProperty_StaticContentURIRulesCount = 13,
    PackageApplicationProperty_StaticContentURIRules = 14,
    PackageApplicationProperty_DynamicContentURIRulesCount = 15,
    PackageApplicationProperty_DynamicContentURIRules = 16
} PackageApplicationProperty;

typedef struct _PACKAGE_RESOURCES_CONTEXT_REFERENCE
{
    PVOID reserved;
} *PACKAGE_RESOURCES_CONTEXT_REFERENCE;

typedef enum PackageResourcesProperty
{
    PackageResourcesProperty_DisplayName = 1,
    PackageResourcesProperty_PublisherDisplayName = 2,
    PackageResourcesProperty_Description = 3,
    PackageResourcesProperty_Logo = 4,
    PackageResourcesProperty_SmallLogo = 5,
    PackageResourcesProperty_StartPage = 6
} PackageResourcesProperty;

typedef struct _PACKAGE_SECURITY_CONTEXT_REFERENCE
{
    PVOID reserved;
} *PACKAGE_SECURITY_CONTEXT_REFERENCE;

typedef enum PackageSecurityProperty
{
    PackageSecurityProperty_SecurityFlags = 1,
    PackageSecurityProperty_AppContainerSID = 2,
    PackageSecurityProperty_CapabilitiesCount = 3,
    PackageSecurityProperty_Capabilities = 4
} PackageSecurityProperty;

typedef struct _TARGET_PLATFORM_CONTEXT_REFERENCE
{
    PVOID reserved;
} *TARGET_PLATFORM_CONTEXT_REFERENCE;

typedef enum TargetPlatformProperty
{
    TargetPlatformProperty_Platform = 1,
    TargetPlatformProperty_MinVersion = 2,
    TargetPlatformProperty_MaxVersion = 3
} TargetPlatformProperty;

typedef struct _PACKAGE_GLOBALIZATION_CONTEXT_REFERENCE
{
    PVOID reserved;
} *PACKAGE_GLOBALIZATION_CONTEXT_REFERENCE;

typedef enum PackageGlobalizationProperty
{
    PackageGlobalizationProperty_ForceUtf8 = 1,
    PackageGlobalizationProperty_UseWindowsDisplayLanguage = 2
} PackageGlobalizationProperty;

#if (PHNT_VERSION >= PHNT_WINDOWS_8_1)

WINBASEAPI
ULONG
WINAPI
GetCurrentPackageContext(
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_CONTEXT_REFERENCE *PackageContext
    );

WINBASEAPI
ULONG
WINAPI
GetPackageContext(
    _In_ PVOID PackageInfoReference,
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_CONTEXT_REFERENCE *PackageContext
    );

WINBASEAPI
ULONG
WINAPI
GetPackageProperty(
    _In_ PACKAGE_CONTEXT_REFERENCE PackageContext,
    _In_ PackageProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer
    );

WINBASEAPI
ULONG
WINAPI
GetPackagePropertyString(
    _In_ PACKAGE_CONTEXT_REFERENCE PackageContext,
    _In_ PackageProperty PropertyId,
    _Inout_ PULONG BufferLength,
    _Out_writes_(BufferLength) PWSTR Buffer
    );

WINBASEAPI
ULONG
WINAPI
GetPackageOSMaxVersionTested(
    _In_ PACKAGE_CONTEXT_REFERENCE PackageContext,
    _Out_ ULONGLONG *OSMaxVersionTested
    );


WINBASEAPI
ULONG
WINAPI
GetCurrentPackageApplicationContext(
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_APPLICATION_CONTEXT_REFERENCE *PackageApplicationContext
    );

WINBASEAPI
ULONG
WINAPI
GetPackageApplicationContext(
    _In_ PVOID PackageInfoReference,
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_APPLICATION_CONTEXT_REFERENCE *PackageApplicationContext
    );

WINBASEAPI
ULONG
WINAPI
GetPackageApplicationProperty(
    _In_ PACKAGE_APPLICATION_CONTEXT_REFERENCE PackageApplicationContext,
    _In_ PackageApplicationProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer
    );

WINBASEAPI
ULONG
WINAPI
GetPackageApplicationPropertyString(
    _In_ PACKAGE_APPLICATION_CONTEXT_REFERENCE PackageApplicationContext,
    _In_ PackageApplicationProperty PropertyId,
    _Inout_ PULONG BufferLength,
    _Out_writes_(BufferLength) PWSTR Buffer
    );


WINBASEAPI
ULONG
WINAPI
GetCurrentPackageResourcesContext(
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_RESOURCES_CONTEXT_REFERENCE *PackageResourcesContext
    );

WINBASEAPI
ULONG
WINAPI
GetPackageResourcesContext(
    _In_ PVOID PackageInfoReference,
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_RESOURCES_CONTEXT_REFERENCE *PackageResourcesContext
    );

WINBASEAPI
ULONG
WINAPI
GetCurrentPackageApplicationResourcesContext(
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_RESOURCES_CONTEXT_REFERENCE *PackageResourcesContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageApplicationResourcesContext(
    _In_ PVOID PackageInfoReference,
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_RESOURCES_CONTEXT_REFERENCE *PackageResourcesContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageResourcesProperty(
    _In_ PACKAGE_RESOURCES_CONTEXT_REFERENCE PackageResourcesContext,
    _In_ PackageResourcesProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer,
    _Out_opt_ PULONG Flags
    );


WINBASEAPI
LONG
WINAPI
GetCurrentPackageSecurityContext(
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_SECURITY_CONTEXT_REFERENCE *PackageSecurityContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageSecurityContext(
    _In_ PVOID PackageInfoReference,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_SECURITY_CONTEXT_REFERENCE *PackageSecurityContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageSecurityProperty(
    _In_ PACKAGE_SECURITY_CONTEXT_REFERENCE PackageSecurityContext,
    _In_ PackageSecurityProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10)


WINBASEAPI
LONG
WINAPI
GetCurrentTargetPlatformContext(
    _Reserved_ ULONG_PTR Unused,
    _Out_ TARGET_PLATFORM_CONTEXT_REFERENCE *TargetPlatformContext
    );

WINBASEAPI
LONG
WINAPI
GetTargetPlatformContext(
    _In_ PVOID PackageInfoReference,
    _Reserved_ ULONG_PTR Unused,
    _Out_ TARGET_PLATFORM_CONTEXT_REFERENCE *TargetPlatformContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageTargetPlatformProperty(
    _In_ TARGET_PLATFORM_CONTEXT_REFERENCE TargetPlatformContext,
    _In_ TargetPlatformProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_20H1)

WINBASEAPI
HRESULT
WINAPI
GetCurrentPackageInfo3(
    _In_ ULONG Flags,
    _In_ ULONG PackagePathType,
    _Inout_ PULONG BufferLength,
    _Out_writes_bytes_opt_(*BufferLength) PVOID Buffer,
    _Out_opt_ PULONG ReturnLength
    );


WINBASEAPI
LONG
WINAPI
GetCurrentPackageGlobalizationContext(
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_GLOBALIZATION_CONTEXT_REFERENCE *PackageGlobalizationContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageGlobalizationContext(
    _In_ PVOID PackageInfoReference,
    _In_ ULONG Index,
    _Reserved_ ULONG_PTR Unused,
    _Out_ PACKAGE_GLOBALIZATION_CONTEXT_REFERENCE *PackageGlobalizationContext
    );

WINBASEAPI
LONG
WINAPI
GetPackageGlobalizationProperty(
    _In_ PACKAGE_GLOBALIZATION_CONTEXT_REFERENCE PackageGlobalizationContext,
    _In_ PackageGlobalizationProperty PropertyId,
    _Inout_ PULONG BufferSize,
    _Out_writes_bytes_(BufferSize) PVOID Buffer
    );

#endif


_Enum_is_bitflag_
typedef enum _MTA_HOST_USAGE_FLAGS
{
    MTA_HOST_USAGE_NONE = 0x0,
    MTA_HOST_USAGE_MTAINITIALIZED = 0x1,
    MTA_HOST_USAGE_ACTIVATORINITIALIZED = 0x2,
    MTA_HOST_USAGE_UNLOADCALLED = 0x4,
} MTA_HOST_USAGE_FLAGS, *PMTA_HOST_USAGE_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(MTA_HOST_USAGE_FLAGS);

typedef struct _MTA_USAGE_GLOBALS
{
    _Reserved_ ULONG StackCapture;
    PULONG MTAInits;
    PULONG MTAIncInits;
    PULONG MTAWaiters;
    PULONG MTAIncrementorSize;
    ULONG CompletionTimeOut;
    _Reserved_ PLIST_ENTRY ListEntryHeadMTAUsageIncrementor;
    _Reserved_ PULONG MTAIncrementorCompleted;
    _Reserved_ PVOID* MTAUsageCompletedIncrementorHead;
    PMTA_HOST_USAGE_FLAGS MTAHostUsageFlags;
} MTA_USAGE_GLOBALS, *PMTA_USAGE_GLOBALS;

#if (PHNT_VERSION >= PHNT_WINDOWS_8)
_Success_(return != 0)
_Must_inspect_result_
WINBASEAPI
PMTA_USAGE_GLOBALS
WINAPI
CoGetMTAUsageInfo(
    VOID
    );
#endif


#define OLETLS_LOCALTID 0x01
#define OLETLS_UUIDINITIALIZED 0x02
#define OLETLS_INTHREADDETACH 0x04
#define OLETLS_CHANNELTHREADINITIALZED 0x08
#define OLETLS_WOWTHREAD 0x10
#define OLETLS_THREADUNINITIALIZING 0x20
#define OLETLS_DISABLE_OLE1DDE 0x40
#define OLETLS_APARTMENTTHREADED 0x80
#define OLETLS_MULTITHREADED 0x100
#define OLETLS_IMPERSONATING 0x200
#define OLETLS_DISABLE_EVENTLOGGER 0x400
#define OLETLS_INNEUTRALAPT 0x800
#define OLETLS_DISPATCHTHREAD 0x1000
#define OLETLS_HOSTTHREAD 0x2000
#define OLETLS_ALLOWCOINIT 0x4000
#define OLETLS_PENDINGUNINIT 0x8000
#define OLETLS_FIRSTMTAINIT 0x10000
#define OLETLS_FIRSTNTAINIT 0x20000
#define OLETLS_APTINITIALIZING 0x40000
#define OLETLS_UIMSGSINMODALLOOP 0x80000
#define OLETLS_MARSHALING_ERROR_OBJECT 0x100000
#define OLETLS_WINRT_INITIALIZE 0x200000
#define OLETLS_APPLICATION_STA 0x400000
#define OLETLS_IN_SHUTDOWN_CALLBACKS 0x800000
#define OLETLS_POINTER_INPUT_BLOCKED 0x1000000
#define OLETLS_IN_ACTIVATION_FILTER 0x2000000
#define OLETLS_ASTATOASTAEXEMPT_QUIRK 0x4000000
#define OLETLS_ASTATOASTAEXEMPT_PROXY 0x8000000
#define OLETLS_ASTATOASTAEXEMPT_INDOUBT 0x10000000
#define OLETLS_DETECTED_USER_INITIALIZED 0x20000000
#define OLETLS_BRIDGE_STA 0x40000000
#define OLETLS_NAINITIALIZING 0x80000000UL

typedef struct tagSOleTlsData
{
    PVOID ThreadBase;
    PVOID SmAllocator;
    ULONG ApartmentID;
    ULONG Flags;
    LONG TlsMapIndex;
    PVOID *TlsSlot;
    ULONG ComInits;
    ULONG OleInits;
    ULONG Calls;
    PVOID ServerCall;
    PVOID CallObjectCache;
    PVOID ContextStack;
    PVOID ObjServer;
    ULONG TIDCaller;
} SOleTlsData, *PSOleTlsData;

WINBASEAPI
VOID
WINAPI
UpdateDCOMSettings(
    VOID
    );


typedef struct tagSDBQUERYRESULT
{
    ULONG Exes[16];
    ULONG ExeFlags[16];
    ULONG Layers[8];
    ULONG LayerFlags;
    ULONG AppHelp;
    ULONG ExeCount;
    ULONG LayerCount;
    GUID ID;
    ULONG ExtraFlags;
    ULONG CustomSDBMap;
    GUID DB[16];
} SDBQUERYRESULT, *PSDBQUERYRESULT;

static_assert(sizeof(SDBQUERYRESULT) == 0x1c8, "SDBQUERYRESULT size mismatch");

typedef struct tagSWITCH_CONTEXT_ATTRIBUTE
{
    ULONG_PTR ContextUpdateCounter;
    BOOL AllowContextUpdate;
    BOOL EnableTrace;
    HANDLE EtwHandle;
} SWITCH_CONTEXT_ATTRIBUTE, *PSWITCH_CONTEXT_ATTRIBUTE;

#ifdef _WIN64
static_assert(sizeof(SWITCH_CONTEXT_ATTRIBUTE) == 0x18, "SWITCH_CONTEXT_ATTRIBUTE size mismatch");
#else
static_assert(sizeof(SWITCH_CONTEXT_ATTRIBUTE) == 0x10, "SWITCH_CONTEXT_ATTRIBUTE size mismatch");
#endif

typedef struct tagSWITCH_CONTEXT_DATA
{
    ULONGLONG OsMaxVersionTested;
    ULONG TargetPlatform;
    ULONGLONG ContextMinimum;
    GUID Platform;
    GUID MinPlatform;
    ULONG ContextSource;
    ULONG ElementCount;
    GUID Elements[48];
} SWITCH_CONTEXT_DATA, * PSWITCH_CONTEXT_DATA;

static_assert(sizeof(SWITCH_CONTEXT_DATA) == 0x340, "SWITCH_CONTEXT_DATA size mismatch");

typedef struct tagSWITCH_CONTEXT
{
    SWITCH_CONTEXT_ATTRIBUTE Attribute;
    SWITCH_CONTEXT_DATA Data;
} SWITCH_CONTEXT, *PSWITCH_CONTEXT;

#ifdef _WIN64
static_assert(sizeof(SWITCH_CONTEXT) == 0x358, "SWITCH_CONTEXT size mismatch");
#else
static_assert(sizeof(SWITCH_CONTEXT) == 0x350, "SWITCH_CONTEXT size mismatch");
#endif

typedef struct _SDB_CSTRUCT_COBALT_PROCFLAG
{
    KAFFINITY AffinityMask;
    ULONG CPUIDEcxOverride;
    ULONG CPUIDEdxOverride;
    USHORT ProcessorGroup;
    USHORT FastSelfModThreshold;
    USHORT Reserved1;
    UCHAR Reserved2;
    UCHAR BackgroundWork : 5;
    UCHAR CPUIDBrand : 4;
    UCHAR Reserved3 : 4;
    UCHAR RdtscScaling : 3;
    UCHAR Reserved4 : 2;
    UCHAR UnalignedAtomicApproach : 2;
    UCHAR Win11Atomics : 2;
    UCHAR RunOnSingleCore : 1;
    UCHAR X64CPUID : 1;
    UCHAR PatchUnaligned : 1;
    UCHAR InterpreterOrJitter : 1;
    UCHAR ForceSegmentHeap : 1;
    UCHAR Reserved5 : 1;
    UCHAR Reserved6 : 1;
    union
    {
        ULONGLONG Group1AsUINT64;
        struct _SDB_CSTRUCT_COBALT_PROCFLAG* Specified;
    };
} SDB_CSTRUCT_COBALT_PROCFLAG, *PSDB_CSTRUCT_COBALT_PROCFLAG;

#ifdef _WIN64
static_assert(sizeof(SDB_CSTRUCT_COBALT_PROCFLAG) == 0x28, "SDB_CSTRUCT_COBALT_PROCFLAG size mismatch");
#else
static_assert(sizeof(SDB_CSTRUCT_COBALT_PROCFLAG) == 0x20, "SDB_CSTRUCT_COBALT_PROCFLAG size mismatch");
#endif

typedef struct _APPCOMPAT_EXE_DATA
{
    ULONG_PTR Reserved[65];
    ULONG Size;
    ULONG Magic;
    BOOL LoadShimEngine;
    USHORT ExeType;
    SDBQUERYRESULT SdbQueryResult;
    ULONG_PTR DbgLogChannels[128];
    SWITCH_CONTEXT SwitchContext;
    ULONG ParentProcessId;
    WCHAR ParentImageName[260];
    WCHAR ParentCompatLayers[256];
    WCHAR ActiveCompatLayers[256];
    ULONG ImageFileSize;
    ULONG ImageCheckSum;
    BOOL LatestOs;
    BOOL PackageId;
    BOOL SwitchBackManifest;
    BOOL UacManifest;
    BOOL LegacyInstaller;
    ULONG RunLevel;
    ULONG_PTR WinRTFlags;
    PVOID HookCOM;
    PVOID ComponentOnDemandEvent;
    PVOID Quirks;
    ULONG QuirksSize;
    SDB_CSTRUCT_COBALT_PROCFLAG CobaltProcFlags;
    ULONG FullMatchDbSizeCb;
    ULONG FullMatchDbOffset;
} APPCOMPAT_EXE_DATA;

#ifdef _WIN64
static_assert(sizeof(APPCOMPAT_EXE_DATA) == 0x11C0, "APPCOMPAT_EXE_DATA size mismatch");
#else
static_assert(sizeof(APPCOMPAT_EXE_DATA) == 0xE98, "APPCOMPAT_EXE_DATA size mismatch");
#endif


/**
 * The D3DKMT_GET_PROCESS_LIST structure is used for retrieving a list of process handles using a graphics adapter.
 * \remarks The caller is responsible for closing the returned process handles.
 */
typedef struct _D3DKMT_GET_PROCESS_LIST
{
    LUID AdapterLuid;
    ULONG DesiredAccess;
    ULONG ProcessHandleCount;
    HANDLE ProcessHandle;
} D3DKMT_GET_PROCESS_LIST, *PD3DKMT_GET_PROCESS_LIST;

/**
 * The D3DKMTGetProcessList function retrieves a list of processes that are using a specific graphics adapter.
 *
 * \param[in,out] GetProcessList A pointer to a \ref D3DKMT_GET_PROCESS_LIST structure that contains the processes using the graphics adapter.
 * \return NTSTATUS Successful or errant status.
 */
EXTERN_C
NTSTATUS
NTAPI
D3DKMTGetProcessList(
    _Inout_ PD3DKMT_GET_PROCESS_LIST GetProcessList
    );

/**
 * The D3DKMT_ENUM_PROCESS_LIST structure is used for retrieving a list of process identifiers using a graphics adapter.
 */
typedef struct _D3DKMT_ENUM_PROCESS_LIST
{
    LUID AdapterLuid;
    PULONG ProcessIdBuffer;
    SIZE_T ProcessIdCount;
} D3DKMT_ENUM_PROCESS_LIST, *PD3DKMT_ENUM_PROCESS_LIST;

/**
 * The D3DKMTEnumProcesses function provides a list of process IDs (PIDs) rather than handles that are using a specific graphics adapter, 
 * which can be more efficient for monitoring purposes.
 *
 * \param[in,out] EnumProcessList A pointer to a \ref D3DKMT_ENUM_PROCESS_LIST structure that contains the processes using the graphics adapter.
 * \return NTSTATUS Successful or errant status.
 */
EXTERN_C
NTSTATUS
NTAPI
D3DKMTEnumProcesses(
    _Inout_ PD3DKMT_ENUM_PROCESS_LIST EnumProcessList
    );

#endif
