/*
 * Power Management support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTPOAPI_H
#define _NTPOAPI_H

#if (PHNT_MODE != PHNT_MODE_KERNEL)
#define POWER_INFORMATION_LEVEL ULONG
#define SystemPowerPolicyAc 0
#define SystemPowerPolicyDc 1
#define VerifySystemPolicyAc 2
#define VerifySystemPolicyDc 3
#define SystemPowerCapabilities 4
#define SystemBatteryState 5
#define SystemPowerStateHandler 6
#define ProcessorStateHandler 7
#define SystemPowerPolicyCurrent 8
#define AdministratorPowerPolicy 9
#define SystemReserveHiberFile 10
#define ProcessorInformation 11
#define SystemPowerInformation 12
#define ProcessorStateHandler2 13
#define LastWakeTime 14
#define LastSleepTime 15
#define SystemExecutionState 16
#define SystemPowerStateNotifyHandler 17
#define ProcessorPowerPolicyAc 18
#define ProcessorPowerPolicyDc 19
#define VerifyProcessorPowerPolicyAc 20
#define VerifyProcessorPowerPolicyDc 21
#define ProcessorPowerPolicyCurrent 22
#define SystemPowerStateLogging 23
#define SystemPowerLoggingEntry 24
#define SetPowerSettingValue 25
#define NotifyUserPowerSetting 26
#define PowerInformationLevelUnused0 27
#define SystemMonitorHiberBootPowerOff 28
#define SystemVideoState 29
#define TraceApplicationPowerMessage 30
#define TraceApplicationPowerMessageEnd 31
#define ProcessorPerfStates 32
#define ProcessorIdleStates 33
#define ProcessorCap 34
#define SystemWakeSource 35
#define SystemHiberFileInformation 36
#define TraceServicePowerMessage 37
#define ProcessorLoad 38
#define PowerShutdownNotification 39
#define MonitorCapabilities 40
#define SessionPowerInit 41
#define SessionDisplayState 42
#define PowerRequestCreate 43
#define PowerRequestAction 44
#define GetPowerRequestList 45
#define ProcessorInformationEx 46
#define NotifyUserModeLegacyPowerEvent 47
#define GroupPark 48
#define ProcessorIdleDomains 49
#define WakeTimerList 50
#define SystemHiberFileSize 51
#define ProcessorIdleStatesHv 52
#define ProcessorPerfStatesHv 53
#define ProcessorPerfCapHv 54
#define ProcessorSetIdle 55
#define LogicalProcessorIdling 56
#define UserPresence 57
#define PowerSettingNotificationName 58
#define GetPowerSettingValue 59
#define IdleResiliency 60
#define SessionRITState 61
#define SessionConnectNotification 62
#define SessionPowerCleanup 63
#define SessionLockState 64
#define SystemHiberbootState 65
#define PlatformInformation 66
#define PdcInvocation 67
#define MonitorInvocation 68
#define FirmwareTableInformationRegistered 69
#define SetShutdownSelectedTime 70
#define SuspendResumeInvocation 71
#define PlmPowerRequestCreate 72
#define ScreenOff 73
#define CsDeviceNotification 74
#define PlatformRole 75
#define LastResumePerformance 76
#define DisplayBurst 77
#define ExitLatencySamplingPercentage 78
#define RegisterSpmPowerSettings 79
#define PlatformIdleStates 80
#define ProcessorIdleVeto 81
#define PlatformIdleVeto 82
#define SystemBatteryStatePrecise 83
#define ThermalEvent 84
#define PowerRequestActionInternal 85
#define BatteryDeviceState 86
#define PowerInformationInternal 87
#define ThermalStandby 88
#define SystemHiberFileType 89
#define PhysicalPowerButtonPress 90
#define QueryPotentialDripsConstraint 91
#define EnergyTrackerCreate 92
#define EnergyTrackerQuery 93
#define UpdateBlackBoxRecorder 94
#define SessionAllowExternalDmaDevices 95
#define SendSuspendResumeNotification 96
#define BlackBoxRecorderDirectAccessBuffer 97
#define SystemPowerSourceState 98
#define PowerInformationLevelMaximum 99
#endif

/**
 * The PROCESSOR_POWER_INFORMATION structure contains information about the power characteristics of a processor.
 * \sa https://learn.microsoft.com/en-us/windows/win32/power/processor-power-information-str
 */
typedef struct _PROCESSOR_POWER_INFORMATION
{
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

#define PO_TZ_ACTIVE 0
#define PO_TZ_PASSIVE 1
#define PO_TZ_INVALID_MODE 2

/**
 * The SYSTEM_POWER_INFORMATION structure contains information about the idleness of the system.
 * \sa https://learn.microsoft.com/en-us/windows/win32/power/system-power-information-str
 */
typedef struct _SYSTEM_POWER_INFORMATION
{
    ULONG MaxIdlenessAllowed;
    ULONG Idleness;
    ULONG TimeRemaining;
    UCHAR CoolingMode;
} SYSTEM_POWER_INFORMATION, *PSYSTEM_POWER_INFORMATION;

typedef struct _SYSTEM_HIBERFILE_INFORMATION
{
    ULONG NumberOfMcbPairs;
    LARGE_INTEGER Mcb[1];
} SYSTEM_HIBERFILE_INFORMATION, *PSYSTEM_HIBERFILE_INFORMATION;

typedef struct _SYSTEM_SERVICE_POWER_MESSAGE
{
    ULONG MessageId;
    ULONG SessionId;
    ULONG Flags;
} SYSTEM_SERVICE_POWER_MESSAGE, *PSYSTEM_SERVICE_POWER_MESSAGE;










#define POWER_PERF_SCALE    100
#define PERF_LEVEL_TO_PERCENT(_x_) ((_x_ * 1000) / (POWER_PERF_SCALE * 10))
#define PERCENT_TO_PERF_LEVEL(_x_) ((_x_ * POWER_PERF_SCALE * 10) / 1000)
#define PO_REASON_STATE_STANDBY (PO_REASON_STATE_S1 | \
                                 PO_REASON_STATE_S2 | \
                                 PO_REASON_STATE_S3)

#define PO_REASON_STATE_ALL     (PO_REASON_STATE_STANDBY | \
                                 PO_REASON_STATE_S4 | \
                                 PO_REASON_STATE_S4FIRM)

typedef struct _SYSTEM_POWER_LOGGING_ENTRY
{
    ULONG Reason;
    ULONG States;
} SYSTEM_POWER_LOGGING_ENTRY, *PSYSTEM_POWER_LOGGING_ENTRY;

typedef struct _SYSTEM_POWER_SETTING_VALUE
{
    GUID SettingGuid;
    ULONG ValueLength;
    UCHAR Value[1];
} SYSTEM_POWER_SETTING_VALUE, *PSYSTEM_POWER_SETTING_VALUE;

typedef enum _POWER_STATE_DISABLED_TYPE
{
    PoDisabledStateSleeping1 = 0,
    PoDisabledStateSleeping2 = 1,
    PoDisabledStateSleeping3 = 2,
    PoDisabledStateSleeping4 = 3,
    PoDisabledStateSleeping0Idle = 4,
    PoDisabledStateReserved5 = 5,
    PoDisabledStateSleeping4Firmware = 6,
    PoDisabledStateMaximum = 7
} POWER_STATE_DISABLED_TYPE, *PPOWER_STATE_DISABLED_TYPE;

#define POWER_STATE_DISABLED_TYPE_MAX  8

_Struct_size_bytes_(sizeof(SYSTEM_POWER_STATE_DISABLE_REASON) + PowerReasonLength)
typedef struct _SYSTEM_POWER_STATE_DISABLE_REASON
{
    BOOLEAN AffectedState[POWER_STATE_DISABLED_TYPE_MAX];
    ULONG PowerReasonCode;
    ULONG PowerReasonLength;
} SYSTEM_POWER_STATE_DISABLE_REASON, *PSYSTEM_POWER_STATE_DISABLE_REASON;

#define POWER_REQUEST_CONTEXT_NOT_SPECIFIED DIAGNOSTIC_REASON_NOT_SPECIFIED

typedef struct _COUNTED_REASON_CONTEXT
{
    ULONG Version;
    ULONG Flags;
    union
    {
        struct
        {
            UNICODE_STRING ResourceFileName;
            USHORT ResourceReasonId;
            ULONG StringCount;
            _Field_size_(StringCount) PUNICODE_STRING ReasonStrings;
        };
        UNICODE_STRING SimpleString;
    };
} COUNTED_REASON_CONTEXT, *PCOUNTED_REASON_CONTEXT;

typedef enum _POWER_REQUEST_TYPE_INTERNAL
{
    PowerRequestDisplayRequiredInternal,
    PowerRequestSystemRequiredInternal,
    PowerRequestAwayModeRequiredInternal,
    PowerRequestExecutionRequiredInternal,
    PowerRequestPerfBoostRequiredInternal,
    PowerRequestActiveLockScreenInternal,
    PowerRequestInternalInvalid,
    PowerRequestInternalUnknown,
    PowerRequestFullScreenVideoRequired
} POWER_REQUEST_TYPE_INTERNAL;

typedef struct _POWER_REQUEST_ACTION
{
    HANDLE PowerRequestHandle;
    POWER_REQUEST_TYPE_INTERNAL RequestType;
    BOOLEAN SetAction;
    HANDLE ProcessHandle;
} POWER_REQUEST_ACTION, *PPOWER_REQUEST_ACTION;

typedef union _POWER_STATE
{
    SYSTEM_POWER_STATE SystemState;
    DEVICE_POWER_STATE DeviceState;
} POWER_STATE, *PPOWER_STATE;

typedef enum _POWER_STATE_TYPE
{
    SystemPowerState = 0,
    DevicePowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;

typedef struct _SYSTEM_POWER_STATE_CONTEXT
{
    union
    {
        struct
        {
            ULONG Reserved1 : 8;
            ULONG TargetSystemState : 4;
            ULONG EffectiveSystemState : 4;
            ULONG CurrentSystemState : 4;
            ULONG IgnoreHibernationPath : 1;
            ULONG PseudoTransition : 1;
            ULONG KernelSoftReboot : 1;
            ULONG DirectedDripsTransition : 1;
            ULONG Reserved2 : 8;
        };
        ULONG ContextAsUlong;
    };
} SYSTEM_POWER_STATE_CONTEXT, *PSYSTEM_POWER_STATE_CONTEXT;

typedef enum _REQUESTER_TYPE
{
    KernelRequester = 0,
    UserProcessRequester = 1,
    UserSharedServiceRequester = 2
} REQUESTER_TYPE;

typedef struct _COUNTED_REASON_CONTEXT_RELATIVE
{
    ULONG Flags;
    union
    {
        struct
        {
            SIZE_T ResourceFileNameOffset;
            USHORT ResourceReasonId;
            ULONG StringCount;
            SIZE_T SubstitutionStringsOffset;
        } DUMMYSTRUCTNAME;
        SIZE_T SimpleStringOffset;
    } DUMMYUNIONNAME;
} COUNTED_REASON_CONTEXT_RELATIVE, *PCOUNTED_REASON_CONTEXT_RELATIVE;

typedef struct _DIAGNOSTIC_BUFFER
{
    SIZE_T Size;
    REQUESTER_TYPE CallerType;
    union
    {
        struct
        {
            SIZE_T ProcessImageNameOffset;
            ULONG ProcessId;
            ULONG ServiceTag;
        } DUMMYSTRUCTNAME;
        struct
        {
            SIZE_T DeviceDescriptionOffset;
            SIZE_T DevicePathOffset;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    SIZE_T ReasonOffset;
} DIAGNOSTIC_BUFFER, *PDIAGNOSTIC_BUFFER;

typedef struct _WAKE_TIMER_INFO
{
    SIZE_T OffsetToNext;
    LARGE_INTEGER DueTime;
    ULONG Period;
    DIAGNOSTIC_BUFFER ReasonContext;
} WAKE_TIMER_INFO, *PWAKE_TIMER_INFO;

typedef struct _PROCESSOR_PERF_CAP_HV
{
    ULONG Version;
    ULONG InitialApicId;
    ULONG Ppc;
    ULONG Tpc;
    ULONG ThermalCap;
} PROCESSOR_PERF_CAP_HV, *PPROCESSOR_PERF_CAP_HV;

typedef struct PROCESSOR_IDLE_TIMES
{
    ULONG64 StartTime;
    ULONG64 EndTime;
    ULONG Reserved[4];
} PROCESSOR_IDLE_TIMES, *PPROCESSOR_IDLE_TIMES;

typedef _Function_class_(PROCESSOR_IDLE_HANDLER)
NTSTATUS FASTCALL PROCESSOR_IDLE_HANDLER(
    _In_ ULONG_PTR Context,
    _Inout_ PPROCESSOR_IDLE_TIMES IdleTimes
    );
typedef PROCESSOR_IDLE_HANDLER *PPROCESSOR_IDLE_HANDLER;

#define PROCESSOR_STATE_TYPE_PERFORMANCE    0x1
#define PROCESSOR_STATE_TYPE_THROTTLE       0x2

#define IDLE_STATE_FLAGS_C1_HLT     0x01
#define IDLE_STATE_FLAGS_C1_IO_HLT  0x02
#define IDLE_STATE_FLAGS_IO         0x04
#define IDLE_STATE_FLAGS_MWAIT      0x08

typedef struct _PROCESSOR_IDLE_STATE
{
    UCHAR StateType;
    ULONG StateFlags;
    ULONG HardwareLatency;
    ULONG Power;
    ULONG_PTR Context;
    PPROCESSOR_IDLE_HANDLER Handler;
} PROCESSOR_IDLE_STATE, *PPROCESSOR_IDLE_STATE;

typedef struct _PROCESSOR_IDLE_STATES
{
    ULONG Size;
    ULONG Revision;
    ULONG Count;
    ULONG Type;
    KAFFINITY TargetProcessors;
    PROCESSOR_IDLE_STATE State[ANYSIZE_ARRAY];
} PROCESSOR_IDLE_STATES, *PPROCESSOR_IDLE_STATES;

typedef struct _PROCESSOR_LOAD
{
    PROCESSOR_NUMBER ProcessorNumber;
    UCHAR BusyPercentage;
    UCHAR FrequencyPercentage;
    USHORT Padding;
} PROCESSOR_LOAD, *PPROCESSOR_LOAD;

typedef struct _POWER_SHUTDOWN_NOTIFICATION
{
    PVOID CallbackRoutine;
    PVOID Context;
} POWER_SHUTDOWN_NOTIFICATION, *PPOWER_SHUTDOWN_NOTIFICATION;

typedef struct _POWER_MONITOR_CAPABILITIES
{
    ULONG Size;
    ULONG Flags;
    ULONG Reserved;
} POWER_MONITOR_CAPABILITIES, *PPOWER_MONITOR_CAPABILITIES;

typedef struct _POWER_SESSION_POWER_INIT
{
    ULONG SessionId;
    ULONG Flags;
    ULONG Reserved;
} POWER_SESSION_POWER_INIT, *PPOWER_SESSION_POWER_INIT;

typedef struct _POWER_SESSION_DISPLAY_STATE
{
    ULONG SessionId;
    ULONG DisplayState;
} POWER_SESSION_DISPLAY_STATE, *PPOWER_SESSION_DISPLAY_STATE;

typedef struct _PROCESSOR_CAP
{
    ULONG Version;
    PROCESSOR_NUMBER ProcessorNumber;
    ULONG PlatformCap;
    ULONG ThermalCap;
    ULONG LimitReasons;
} PROCESSOR_CAP, *PPROCESSOR_CAP;

typedef struct _PO_WAKE_SOURCE_INFO
{
    ULONG Count;
    ULONG Offsets[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_INFO, *PPO_WAKE_SOURCE_INFO;

typedef struct _PO_WAKE_SOURCE_HISTORY
{
    ULONG Count;
    ULONG Offsets[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_HISTORY, *PPO_WAKE_SOURCE_HISTORY;

typedef enum _PO_WAKE_SOURCE_TYPE
{
    DeviceWakeSourceType = 0,
    FixedWakeSourceType = 1,
    TimerWakeSourceType = 2,
    TimerPresumedWakeSourceType = 3,
    InternalWakeSourceType = 4
} PO_WAKE_SOURCE_TYPE, *PPO_WAKE_SOURCE_TYPE;

typedef enum _PO_INTERNAL_WAKE_SOURCE_TYPE
{
    InternalWakeSourceDozeToHibernate = 0,
    InternalWakeSourcePredictedUserPresence = 1
} PO_INTERNAL_WAKE_SOURCE_TYPE;

typedef enum _PO_FIXED_WAKE_SOURCE_TYPE
{
    FixedWakeSourcePowerButton = 0,
    FixedWakeSourceSleepButton = 1,
    FixedWakeSourceRtc = 2,
    FixedWakeSourceDozeToHibernate = 3
} PO_FIXED_WAKE_SOURCE_TYPE, *PPO_FIXED_WAKE_SOURCE_TYPE;

typedef struct _PO_WAKE_SOURCE_HEADER
{
    PO_WAKE_SOURCE_TYPE Type;
    ULONG Size;
} PO_WAKE_SOURCE_HEADER, *PPO_WAKE_SOURCE_HEADER;

typedef struct _PO_WAKE_SOURCE_DEVICE
{
    PO_WAKE_SOURCE_HEADER Header;
    WCHAR InstancePath[ANYSIZE_ARRAY];
} PO_WAKE_SOURCE_DEVICE, *PPO_WAKE_SOURCE_DEVICE;

typedef struct _PO_WAKE_SOURCE_FIXED
{
    PO_WAKE_SOURCE_HEADER Header;
    PO_FIXED_WAKE_SOURCE_TYPE FixedWakeSourceType;
} PO_WAKE_SOURCE_FIXED, *PPO_WAKE_SOURCE_FIXED;

typedef struct _PO_WAKE_SOURCE_INTERNAL
{
    PO_WAKE_SOURCE_HEADER Header;
    PO_INTERNAL_WAKE_SOURCE_TYPE InternalWakeSourceType;
} PO_WAKE_SOURCE_INTERNAL, *PPO_WAKE_SOURCE_INTERNAL;

typedef struct _PO_WAKE_SOURCE_TIMER
{
    PO_WAKE_SOURCE_HEADER Header;
    DIAGNOSTIC_BUFFER Reason;
} PO_WAKE_SOURCE_TIMER, *PPO_WAKE_SOURCE_TIMER;

#define POWER_REQUEST_SUPPORTED_TYPES_V1 3
#define POWER_REQUEST_SUPPORTED_TYPES_V2 9
#define POWER_REQUEST_SUPPORTED_TYPES_V3 5
#define POWER_REQUEST_SUPPORTED_TYPES_V4 6

typedef struct _POWER_REQUEST
{
    union
    {
        struct
        {
            ULONG SupportedRequestMask;
            ULONG PowerRequestCount[POWER_REQUEST_SUPPORTED_TYPES_V1];
            DIAGNOSTIC_BUFFER DiagnosticBuffer;
        } V1;
#if (PHNT_VERSION >= PHNT_WINDOWS_8)
        struct
        {
            ULONG SupportedRequestMask;
            ULONG PowerRequestCount[POWER_REQUEST_SUPPORTED_TYPES_V2];
            DIAGNOSTIC_BUFFER DiagnosticBuffer;
        } V2;
#endif
#if (PHNT_VERSION >= PHNT_WINDOWS_8_1)
        struct
        {
            ULONG SupportedRequestMask;
            ULONG PowerRequestCount[POWER_REQUEST_SUPPORTED_TYPES_V3];
            DIAGNOSTIC_BUFFER DiagnosticBuffer;
        } V3;
#endif
#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS1)
        struct
        {
            ULONG SupportedRequestMask;
            ULONG PowerRequestCount[POWER_REQUEST_SUPPORTED_TYPES_V4];
            DIAGNOSTIC_BUFFER DiagnosticBuffer;
        } V4;
#endif
    };
} POWER_REQUEST, *PPOWER_REQUEST;

typedef struct _POWER_REQUEST_LIST
{
    ULONG_PTR Count;
    ULONG_PTR PowerRequestOffsets[ANYSIZE_ARRAY];
} POWER_REQUEST_LIST, *PPOWER_REQUEST_LIST;

typedef enum _POWER_STATE_HANDLER_TYPE
{
    PowerStateSleeping1 = 0,
    PowerStateSleeping2 = 1,
    PowerStateSleeping3 = 2,
    PowerStateSleeping4 = 3,
    PowerStateShutdownOff = 4,
    PowerStateShutdownReset = 5,
    PowerStateSleeping4Firmware = 6,
    PowerStateMaximum = 7
} POWER_STATE_HANDLER_TYPE, *PPOWER_STATE_HANDLER_TYPE;

typedef _Function_class_(ENTER_STATE_SYSTEM_HANDLER)
NTSTATUS NTAPI ENTER_STATE_SYSTEM_HANDLER(
    _In_ PVOID SystemContext
    );
typedef ENTER_STATE_SYSTEM_HANDLER* PENTER_STATE_SYSTEM_HANDLER;

typedef _Function_class_(ENTER_STATE_HANDLER)
NTSTATUS NTAPI ENTER_STATE_HANDLER(
    _In_ PVOID Context,
    _In_opt_ PENTER_STATE_SYSTEM_HANDLER SystemHandler,
    _In_ PVOID SystemContext,
    _In_ LONG NumberProcessors,
    _In_ LONG volatile* Number
    );
typedef ENTER_STATE_HANDLER* PENTER_STATE_HANDLER;

typedef struct _POWER_STATE_HANDLER
{
    POWER_STATE_HANDLER_TYPE Type;
    BOOLEAN RtcWake;
    UCHAR Spare[3];
    PENTER_STATE_HANDLER Handler;
    PVOID Context;
} POWER_STATE_HANDLER, *PPOWER_STATE_HANDLER;

typedef _Function_class_(ENTER_STATE_NOTIFY_HANDLER)
NTSTATUS NTAPI ENTER_STATE_NOTIFY_HANDLER(
    _In_ POWER_STATE_HANDLER_TYPE State,
    _In_ PVOID Context,
    _In_ BOOLEAN Entering
    );
typedef ENTER_STATE_NOTIFY_HANDLER* PENTER_STATE_NOTIFY_HANDLER;

typedef struct _POWER_STATE_NOTIFY_HANDLER
{
    PENTER_STATE_NOTIFY_HANDLER Handler;
    PVOID Context;
} POWER_STATE_NOTIFY_HANDLER, *PPOWER_STATE_NOTIFY_HANDLER;

typedef struct _POWER_REQUEST_ACTION_INTERNAL
{
    PVOID PowerRequestPointer;
    POWER_REQUEST_TYPE_INTERNAL RequestType;
    BOOLEAN SetAction;
} POWER_REQUEST_ACTION_INTERNAL, *PPOWER_REQUEST_ACTION_INTERNAL;

typedef enum _POWER_INFORMATION_LEVEL_INTERNAL
{
    PowerInternalAcpiInterfaceRegister,
    PowerInternalS0LowPowerIdleInfo,
    PowerInternalReapplyBrightnessSettings,
    PowerInternalUserAbsencePrediction,
    PowerInternalUserAbsencePredictionCapability,
    PowerInternalPoProcessorLatencyHint,
    PowerInternalStandbyNetworkRequest,
    PowerInternalDirtyTransitionInformation,
    PowerInternalSetBackgroundTaskState,
    PowerInternalTtmOpenTerminal,
    PowerInternalTtmCreateTerminal,
    PowerInternalTtmEvacuateDevices,
    PowerInternalTtmCreateTerminalEventQueue,
    PowerInternalTtmGetTerminalEvent,
    PowerInternalTtmSetDefaultDeviceAssignment,
    PowerInternalTtmAssignDevice,
    PowerInternalTtmSetDisplayState,
    PowerInternalTtmSetDisplayTimeouts,
    PowerInternalBootSessionStandbyActivationInformation,
    PowerInternalSessionPowerState,
    PowerInternalSessionTerminalInput,
    PowerInternalSetWatchdog,
    PowerInternalPhysicalPowerButtonPressInfoAtBoot,
    PowerInternalExternalMonitorConnected,
    PowerInternalHighPrecisionBrightnessSettings,
    PowerInternalWinrtScreenToggle,
    PowerInternalPpmQosDisable,
    PowerInternalTransitionCheckpoint,
    PowerInternalInputControllerState,
    PowerInternalFirmwareResetReason,
    PowerInternalPpmSchedulerQosSupport,
    PowerInternalBootStatGet,
    PowerInternalBootStatSet,
    PowerInternalCallHasNotReturnedWatchdog,
    PowerInternalBootStatCheckIntegrity,
    PowerInternalBootStatRestoreDefaults,
    PowerInternalHostEsStateUpdate,
    PowerInternalGetPowerActionState,
    PowerInternalBootStatUnlock,
    PowerInternalWakeOnVoiceState,
    PowerInternalDeepSleepBlock,
    PowerInternalIsPoFxDevice,
    PowerInternalPowerTransitionExtensionAtBoot,
    PowerInternalProcessorBrandedFrequency,
    PowerInternalTimeBrokerExpirationReason,
    PowerInternalNotifyUserShutdownStatus,
    PowerInternalPowerRequestTerminalCoreWindow,
    PowerInternalProcessorIdleVeto,
    PowerInternalPlatformIdleVeto,
    PowerInternalIsLongPowerButtonBugcheckEnabled,
    PowerInternalAutoChkCausedReboot,
    PowerInternalSetWakeAlarmOverride,

    PowerInternalDirectedFxAddTestDevice = 53,
    PowerInternalDirectedFxRemoveTestDevice,

    PowerInternalDirectedFxSetMode = 56,
    PowerInternalRegisterPowerPlane,
    PowerInternalSetDirectedDripsFlags,
    PowerInternalClearDirectedDripsFlags,
    PowerInternalRetrieveHiberFileResumeContext,
    PowerInternalReadHiberFilePage,
    PowerInternalLastBootSucceeded,
    PowerInternalQuerySleepStudyHelperRoutineBlock,
    PowerInternalDirectedDripsQueryCapabilities,
    PowerInternalClearConstraints,
    PowerInternalSoftParkVelocityEnabled,
    PowerInternalQueryIntelPepCapabilities,
    PowerInternalGetSystemIdleLoopEnablement,
    PowerInternalGetVmPerfControlSupport,
    PowerInternalGetVmPerfControlConfig,
    PowerInternalSleepDetailedDiagUpdate,
    PowerInternalProcessorClassFrequencyBandsStats,
    PowerInternalHostGlobalUserPresenceStateUpdate,
    PowerInternalCpuNodeIdleIntervalStats,
    PowerInternalClassIdleIntervalStats,
    PowerInternalCpuNodeConcurrencyStats,
    PowerInternalClassConcurrencyStats,
    PowerInternalQueryProcMeasurementCapabilities,
    PowerInternalQueryProcMeasurementValues,
    PowerInternalPrepareForSystemInitiatedReboot,
    PowerInternalGetAdaptiveSessionState,
    PowerInternalSetConsoleLockedState,
    PowerInternalOverrideSystemInitiatedRebootState,
    PowerInternalFanImpactStats,
    PowerInternalFanRpmBuckets,
    PowerInternalPowerBootAppDiagInfo,
    PowerInternalUnregisterShutdownNotification,
    PowerInternalManageTransitionStateRecord,
    PowerInternalGetAcpiTimeAndAlarmCapabilities,
    PowerInternalSuspendResumeRequest,
    PowerInternalEnergyEstimationInfo,
    PowerInternalProvSocIdentifierOperation,
    PowerInternalGetVmPerfPrioritySupport,
    PowerInternalGetVmPerfPriorityConfig,
    PowerInternalNotifyWin32kPowerRequestQueued,
    PowerInternalNotifyWin32kPowerRequestCompleted,
    PowerInternalPdcAgentSessionQuery,
    PowerInternalSessionConnectionChangeV2,
    PowerInformationInternalMaximum
} POWER_INFORMATION_LEVEL_INTERNAL;

typedef enum _POWER_S0_DISCONNECTED_REASON
{
    PoS0DisconnectedReasonNone,
    PoS0DisconnectedReasonNonCompliantNic,
    PoS0DisconnectedReasonSettingPolicy,
    PoS0DisconnectedReasonEnforceDsPolicy,
    PoS0DisconnectedReasonCsChecksFailed,
    PoS0DisconnectedReasonSmartStandby,
    PoS0DisconnectedReasonMaximum
} POWER_S0_DISCONNECTED_REASON;

typedef struct _POWER_S0_LOW_POWER_IDLE_INFO
{
    POWER_S0_DISCONNECTED_REASON DisconnectedReason;
    union
    {
        BOOLEAN Storage : 1;
        BOOLEAN WiFi : 1;
        BOOLEAN Mbn : 1;
        BOOLEAN Ethernet : 1;
        BOOLEAN Reserved : 4;
        UCHAR AsUCHAR;
    } CsDeviceCompliance;
    union
    {
        BOOLEAN DisconnectInStandby : 1;
        BOOLEAN EnforceDs : 1;
        BOOLEAN Reserved : 6;
        UCHAR AsUCHAR;
    } Policy;
} POWER_S0_LOW_POWER_IDLE_INFO, *PPOWER_S0_LOW_POWER_IDLE_INFO;

typedef struct _POWER_INFORMATION_INTERNAL_HEADER
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INFORMATION_INTERNAL_HEADER, *PPOWER_INFORMATION_INTERNAL_HEADER;

typedef struct _POWER_INTERNAL_ACPI_INTERFACE_REGISTER_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    UCHAR Data[24];
} POWER_INTERNAL_ACPI_INTERFACE_REGISTER_INPUT, *PPOWER_INTERNAL_ACPI_INTERFACE_REGISTER_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_ACPI_INTERFACE_REGISTER_INPUT) == 0x20);

typedef struct _POWER_INTERNAL_ACPI_INTERFACE_REGISTER_OUTPUT
{
    ULONG_PTR RegistrationHandle;
    ULONG_PTR Reserved;
} POWER_INTERNAL_ACPI_INTERFACE_REGISTER_OUTPUT, *PPOWER_INTERNAL_ACPI_INTERFACE_REGISTER_OUTPUT;

typedef struct _POWER_USER_ABSENCE_PREDICTION
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    LARGE_INTEGER ReturnTime;
} POWER_USER_ABSENCE_PREDICTION, *PPOWER_USER_ABSENCE_PREDICTION;

typedef struct _POWER_USER_ABSENCE_PREDICTION_CAPABILITY
{
    BOOLEAN AbsencePredictionCapability;
} POWER_USER_ABSENCE_PREDICTION_CAPABILITY, *PPOWER_USER_ABSENCE_PREDICTION_CAPABILITY;

typedef struct _POWER_PROCESSOR_LATENCY_HINT
{
    POWER_INFORMATION_INTERNAL_HEADER PowerInformationInternalHeader;
    ULONG Type;
} POWER_PROCESSOR_LATENCY_HINT, *PPOWER_PROCESSOR_LATENCY_HINT;

typedef struct _POWER_STANDBY_NETWORK_REQUEST
{
    POWER_INFORMATION_INTERNAL_HEADER PowerInformationInternalHeader;
    BOOLEAN Active;
} POWER_STANDBY_NETWORK_REQUEST, *PPOWER_STANDBY_NETWORK_REQUEST;

typedef struct _POWER_SET_BACKGROUND_TASK_STATE
{
    POWER_INFORMATION_INTERNAL_HEADER PowerInformationInternalHeader;
    BOOLEAN Engaged;
} POWER_SET_BACKGROUND_TASK_STATE, *PPOWER_SET_BACKGROUND_TASK_STATE;

typedef struct _POWER_BOOT_SESSION_STANDBY_ACTIVATION_INFO
{
    ULONG StandbyTotalTime;
    ULONG DripsTotalTime;
    ULONG ActivatorClientTotalActiveTime;
    ULONG PerActivatorClientTotalActiveTime[98];
} POWER_BOOT_SESSION_STANDBY_ACTIVATION_INFO, *PPOWER_BOOT_SESSION_STANDBY_ACTIVATION_INFO;

typedef struct _POWER_SESSION_POWER_STATE
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG SessionId;
    BOOLEAN On;
    BOOLEAN IsConsole;
    POWER_MONITOR_REQUEST_REASON RequestReason;
} POWER_SESSION_POWER_STATE, *PPOWER_SESSION_POWER_STATE;

typedef struct _POWER_INTERNAL_SET_WATCHDOG
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    PVOID WatchdogHandle;
    UCHAR Parameters[0x48];
    BOOLEAN DeleteWatchdog;
    UCHAR Reserved[7];
} POWER_INTERNAL_SET_WATCHDOG, *PPOWER_INTERNAL_SET_WATCHDOG;


typedef struct _POWER_INTERNAL_TERMINAL_CORE_WINDOW_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG SessionId;
    ULONG TerminalId;
    UCHAR InputType;
} POWER_INTERNAL_TERMINAL_CORE_WINDOW_INPUT, *PPOWER_INTERNAL_TERMINAL_CORE_WINDOW_INPUT;

typedef struct _POWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_INPUT, *PPOWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_INPUT;

typedef struct _POWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_OUTPUT
{
    UCHAR Buffer[64];
} POWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_OUTPUT, *PPOWER_INTERNAL_PHYSICAL_POWER_BUTTON_AT_BOOT_OUTPUT;

typedef struct _POWER_INTERNAL_EXTERNAL_MONITOR_CONNECTED_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Connected;
} POWER_INTERNAL_EXTERNAL_MONITOR_CONNECTED_INPUT, *PPOWER_INTERNAL_EXTERNAL_MONITOR_CONNECTED_INPUT;

typedef struct _POWER_INTERNAL_HIGH_PRECISION_BRIGHTNESS_SETTINGS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG SessionId;
    ULONG BrightnessLevel;
    ULONG Flags;
    ULONG Reserved[5];
} POWER_INTERNAL_HIGH_PRECISION_BRIGHTNESS_SETTINGS_INPUT, *PPOWER_INTERNAL_HIGH_PRECISION_BRIGHTNESS_SETTINGS_INPUT;

typedef struct _POWER_INTERNAL_WINRT_SCREEN_TOGGLE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Toggle;
} POWER_INTERNAL_WINRT_SCREEN_TOGGLE_INPUT, *PPOWER_INTERNAL_WINRT_SCREEN_TOGGLE_INPUT;

typedef struct _POWER_INTERNAL_PPM_QOS_DISABLE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN EnableDisable;
} POWER_INTERNAL_PPM_QOS_DISABLE_INPUT, *PPOWER_INTERNAL_PPM_QOS_DISABLE_INPUT;

typedef struct _POWER_INTERNAL_TRANSITION_CHECKPOINT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG CheckpointId;
    ULONG CheckpointType;
} POWER_INTERNAL_TRANSITION_CHECKPOINT_INPUT, *PPOWER_INTERNAL_TRANSITION_CHECKPOINT_INPUT;

typedef struct _POWER_INTERNAL_INPUT_CONTROLLER_STATE
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG InputControllerState;
} POWER_INTERNAL_INPUT_CONTROLLER_STATE, *PPOWER_INTERNAL_INPUT_CONTROLLER_STATE;

typedef struct _POWER_INTERNAL_FIRMWARE_RESET_REASON_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_FIRMWARE_RESET_REASON_INPUT, *PPOWER_INTERNAL_FIRMWARE_RESET_REASON_INPUT;

typedef struct _POWER_INTERNAL_FIRMWARE_RESET_REASON_OUTPUT
{
    ULONG ResetReasonCode;
    UCHAR DiagnosticData1[16];
    UCHAR DiagnosticData2[16];
    UCHAR Reserved[12];
} POWER_INTERNAL_FIRMWARE_RESET_REASON_OUTPUT, *PPOWER_INTERNAL_FIRMWARE_RESET_REASON_OUTPUT;

typedef struct _POWER_INTERNAL_PROCESSOR_QOS_SUPPORT
{
    BOOLEAN QosSupportedAndConfigured;
    BOOLEAN SchedulerDirectedPerfStatesSupported;
    BOOLEAN QosGroupPolicyDisable;
} POWER_INTERNAL_PROCESSOR_QOS_SUPPORT, *PPOWER_INTERNAL_PROCESSOR_QOS_SUPPORT;

typedef struct _RTL_BSD_ITEM RTL_BSD_ITEM, *PRTL_BSD_ITEM;

typedef struct _POWER_INTERNAL_BOOTSTAT_GET_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG EntryCount;
    ULONG Reserved;
    PRTL_BSD_ITEM Entries;
} POWER_INTERNAL_BOOTSTAT_GET_INPUT, *PPOWER_INTERNAL_BOOTSTAT_GET_INPUT;

typedef struct _POWER_INTERNAL_BOOTSTAT_GET_OUTPUT
{
    ULONG Sizes[ANYSIZE_ARRAY];
} POWER_INTERNAL_BOOTSTAT_GET_OUTPUT, *PPOWER_INTERNAL_BOOTSTAT_GET_OUTPUT;

typedef struct _POWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG EntryCount;
    ULONG Reserved;
    PRTL_BSD_ITEM Entries;
} POWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_INPUT, *PPOWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_INPUT) == sizeof(POWER_INTERNAL_BOOTSTAT_GET_INPUT));

typedef struct _POWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_OUTPUT
{
    BOOLEAN IntegrityOk;
} POWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_OUTPUT, *PPOWER_INTERNAL_BOOTSTAT_CHECK_INTEGRITY_OUTPUT;

typedef struct _POWER_INTERNAL_HOST_ENERGY_SAVER_STATE
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    BOOLEAN EsEnabledOnHost;
} POWER_INTERNAL_HOST_ENERGY_SAVER_STATE, *PPOWER_INTERNAL_HOST_ENERGY_SAVER_STATE;

typedef struct _POWER_INTERNAL_IS_POFX_DEVICE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    PVOID DeviceObject;
} POWER_INTERNAL_IS_POFX_DEVICE_INPUT, *PPOWER_INTERNAL_IS_POFX_DEVICE_INPUT;

typedef struct _POWER_INTERNAL_POWER_TRANSITION_EXTENSION_AT_BOOT_OUTPUT
{
    UCHAR Data[32];
} POWER_INTERNAL_POWER_TRANSITION_EXTENSION_AT_BOOT_OUTPUT, *PPOWER_INTERNAL_POWER_TRANSITION_EXTENSION_AT_BOOT_OUTPUT;

typedef struct _POWER_INTERNAL_NOTIFY_USER_SHUTDOWN_STATUS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN ShutdownInitiated;
} POWER_INTERNAL_NOTIFY_USER_SHUTDOWN_STATUS_INPUT, *PPOWER_INTERNAL_NOTIFY_USER_SHUTDOWN_STATUS_INPUT;

typedef struct _POWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_INPUT, *PPOWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_INPUT;

typedef struct _POWER_INTERNAL_READ_HIBERFILE_PAGE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG PageNumber;
} POWER_INTERNAL_READ_HIBERFILE_PAGE_INPUT, *PPOWER_INTERNAL_READ_HIBERFILE_PAGE_INPUT;

typedef struct _POWER_INTERNAL_READ_HIBERFILE_PAGE_OUTPUT
{
    UCHAR PageData[PAGE_SIZE];
} POWER_INTERNAL_READ_HIBERFILE_PAGE_OUTPUT, *PPOWER_INTERNAL_READ_HIBERFILE_PAGE_OUTPUT;

typedef struct _POWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_INPUT, *PPOWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_INPUT;

typedef struct _POWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_OUTPUT
{
    ULONG Capabilities[4];
} POWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_OUTPUT, *PPOWER_INTERNAL_QUERY_INTEL_PEP_CAPABILITIES_OUTPUT;

typedef struct _POWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_OUTPUT
{
    BOOLEAN CausedReboot;
} POWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_OUTPUT, *PPOWER_INTERNAL_AUTOCHK_CAUASED_REBOOT_OUTPUT;

typedef struct _POWER_INTERNAL_TIME_BROKER_EXPIRATION_REASON_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    WCHAR Reason[64];
    ULONGLONG DueTime;
} POWER_INTERNAL_TIME_BROKER_EXPIRATION_REASON_INPUT, *PPOWER_INTERNAL_TIME_BROKER_EXPIRATION_REASON_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_TIME_BROKER_EXPIRATION_REASON_INPUT) == 0x90);

typedef struct _POWER_INTERNAL_POWER_REQUEST_TERMINAL_CORE_WINDOW_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    PVOID ProcessHandle;
    PVOID PowerRequestPointer;
    ULONG ReasonCode;
    ULONG Reserved;
} POWER_INTERNAL_POWER_REQUEST_TERMINAL_CORE_WINDOW_INPUT, *PPOWER_INTERNAL_POWER_REQUEST_TERMINAL_CORE_WINDOW_INPUT;

typedef struct _POWER_INTERNAL_WAKE_ON_VOICE_STATE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Enabled;
} POWER_INTERNAL_WAKE_ON_VOICE_STATE_INPUT, *PPOWER_INTERNAL_WAKE_ON_VOICE_STATE_INPUT;

typedef struct _POWER_INTERNAL_DEEP_SLEEP_BLOCK_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Block;
} POWER_INTERNAL_DEEP_SLEEP_BLOCK_INPUT, *PPOWER_INTERNAL_DEEP_SLEEP_BLOCK_INPUT;

typedef struct _POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    PROCESSOR_NUMBER ProcessorNumber;
} POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_INPUT, *PPOWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_INPUT;

#define POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_VERSION 1

C_ASSERT(sizeof(POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_INPUT) == 0x0C);

typedef struct _POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_OUTPUT
{
    ULONG Version;
    ULONG NominalFrequency;
} POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_OUTPUT, *PPOWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_OUTPUT;

C_ASSERT(sizeof(POWER_INTERNAL_PROCESSOR_BRANDED_FREQUENCY_OUTPUT) == 0x08);

typedef struct _POWER_INTERNAL_SET_WAKE_ALARM_OVERRIDE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONGLONG WakeAlarmOverrideAc;
    ULONGLONG WakeAlarmOverrideDc;
} POWER_INTERNAL_SET_WAKE_ALARM_OVERRIDE_INPUT, *PPOWER_INTERNAL_SET_WAKE_ALARM_OVERRIDE_INPUT;

typedef struct _PROCESSOR_IDLE_VETO
{
    ULONG Version;
    PROCESSOR_NUMBER ProcessorNumber;
    ULONG StateIndex;
    ULONG VetoReason;
    UCHAR Increment;
} PROCESSOR_IDLE_VETO, *PPROCESSOR_IDLE_VETO;

typedef struct _PLATFORM_IDLE_VETO
{
    ULONG Version;
    ULONG StateIndex;
    ULONG VetoReason;
    UCHAR Increment;
} PLATFORM_IDLE_VETO, *PPLATFORM_IDLE_VETO;

typedef struct _POWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_INPUT, *PPOWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_INPUT;

typedef struct _POWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_OUTPUT
{
    ULONG IdleLoopEnabled;
} POWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_OUTPUT, *PPOWER_INTERNAL_SYSTEM_IDLE_LOOP_ENABLEMENT_OUTPUT;

typedef struct _POWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG Reserved1;
} POWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_INPUT, *PPOWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_INPUT;

#define PPM_VMPCS_SUPPORTS_PERF_SET        0x00000001
#define PPM_VMPCS_SUPPORTS_AUTONOMOUS      0x00000002
#define PPM_VMPCS_SUPPORTS_EPP             0x00000004
#define PPM_VMPCS_SUPPORTS_BOOST           0x00000008
#define PPM_VMPCS_SUPPORTS_TIME_WINDOW     0x00000010

typedef struct _POWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_OUTPUT
{
    UCHAR Supported;
    UCHAR Reserved0;
    UCHAR Reserved1;
    UCHAR Reserved2;
    ULONG MinPerfPercent;
    ULONG MaxPerfPercent;
    ULONG StepPerfPercent;
    ULONG Capabilities;
} POWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_OUTPUT, *PPOWER_INTERNAL_VM_PERF_CONTROL_SUPPORT_OUTPUT;

typedef struct _POWER_INTERNAL_SLEEP_DETAILED_DIAG_UPDATE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Enable;
} POWER_INTERNAL_SLEEP_DETAILED_DIAG_UPDATE_INPUT, *PPOWER_INTERNAL_SLEEP_DETAILED_DIAG_UPDATE_INPUT;

typedef struct _POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_INPUT, *PPOWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_INPUT;

typedef struct _POWER_INTERNAL_HOST_GLOBAL_USER_PRESENCE_STATE_UPDATE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN UserPresent;
} POWER_INTERNAL_HOST_GLOBAL_USER_PRESENCE_STATE_UPDATE_INPUT, *PPOWER_INTERNAL_HOST_GLOBAL_USER_PRESENCE_STATE_UPDATE_INPUT;

/**
 * The POWER_INTERNAL_IDLE_INTERVAL_STATS_INPUT structure is passed to internal power management routines
 * to request idle interval statistics for a given processor package or node.
 */
typedef struct _POWER_INTERNAL_IDLE_INTERVAL_STATS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG Node;
} POWER_INTERNAL_IDLE_INTERVAL_STATS_INPUT, *PPOWER_INTERNAL_IDLE_INTERVAL_STATS_INPUT;

/**
 * The POWER_INTERNAL_IDLE_INTERVAL_PACKAGE structure contains a histogram of idle intervals,
 * each entry representing the total time spent in a given duration bucket.
 * The 37 buckets are logarithmically spaced to capture idle durations from ms up to seconds.
 * Approximate bucket ranges:
 * - Indices [0-2] = Very long idle (10-50 seconds)
 * - Indices [3-5] = Long idle (2-10 seconds)
 * - Indices [6-10] = Medium idle (100 ms-1 second)
 * - Indices [11-15] = Short idle (10-100 ms)
 * - Indices [16-36] = Very short idle (<10 ms, down to microseconds)
 */
typedef struct _POWER_INTERNAL_IDLE_INTERVAL_PACKAGE
{
    /**
     * Idle interval histogram buckets.
     * Each entry is a ULONGLONG value in 100-nanosecond units.
     * There are 37 buckets, covering idle durations from microseconds
     * up to tens of seconds.
     */
    ULONGLONG IdleIntervals[37];
} POWER_INTERNAL_IDLE_INTERVAL_PACKAGE, *PPOWER_INTERNAL_IDLE_INTERVAL_PACKAGE;

/**
 * The POWER_INTERNAL_IDLE_INTERVAL_STATS_OUTPUT structure contains the idle interval statistics.
 */
typedef struct _POWER_INTERNAL_IDLE_INTERVAL_STATS_OUTPUT
{
    POWER_INTERNAL_IDLE_INTERVAL_PACKAGE Package[2];
} POWER_INTERNAL_IDLE_INTERVAL_STATS_OUTPUT, *PPOWER_INTERNAL_IDLE_INTERVAL_STATS_OUTPUT;

#define PPM_PERF_BANKS_COUNT 2
#define PPM_PERF_BANDS_COUNT 48
#define PPM_PERF_METRICS_COUNT 3
#define PPM_PERF_BANDS_SIZE sizeof(PPM_PERF_BAND_ENTRY)
#define PPM_PERF_STATS_SIZE (PPM_PERF_BANDS_COUNT * PPM_PERF_BANDS_SIZE)
#define PPM_PERF_DELTA_OFFSET 0xF8

typedef struct _POWER_INTERNAL_PPM_PERF_FREQUENCY_BAND_STATS_BANK
{
    ULONGLONG Metric[PPM_PERF_METRICS_COUNT][PPM_PERF_BANDS_COUNT];
} POWER_INTERNAL_PPM_PERF_FREQUENCY_BAND_STATS_BANK, PPOWER_INTERNAL_PM_PERF_FREQUENCY_BAND_STATS_BANK;

typedef struct _POWER_INTERNAL_PPM_PERF_FREQUENCY_BAND_STATS_OUT
{
    POWER_INTERNAL_PPM_PERF_FREQUENCY_BAND_STATS_BANK Bank[PPM_PERF_BANKS_COUNT];
} POWER_INTERNAL_PPM_PERF_FREQUENCY_BAND_STATS_OUT;

typedef struct _POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS
{
    ULONGLONG Counter[PPM_PERF_METRICS_COUNT];
} POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS, *PPOWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS;

typedef struct _POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_OUTPUT
{
    POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS Band[PPM_PERF_BANDS_COUNT];
} POWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_OUTPUT, *PPOWER_INTERNAL_PROCESSOR_CLASS_BAND_STATS_OUTPUT;

typedef struct _POWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG SessionStateId;
    UCHAR Reserved[28];
} POWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_INPUT, *PPOWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_INPUT;

typedef struct _POWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_OUTPUT
{
    UCHAR Reserved[16];
} POWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_OUTPUT, *PPOWER_INTERNAL_GET_ADAPTIVE_SESSION_STATE_OUTPUT;

typedef struct _POWER_INTERNAL_SET_CONSOLE_LOCKED_STATE_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    BOOLEAN Locked;
} POWER_INTERNAL_SET_CONSOLE_LOCKED_STATE_INPUT, *PPOWER_INTERNAL_SET_CONSOLE_LOCKED_STATE_INPUT;

typedef struct _POWER_INTERNAL_FAN_IMPACT_STATS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_FAN_IMPACT_STATS_INPUT, *PPOWER_INTERNAL_FAN_IMPACT_STATS_INPUT;

typedef struct _POWER_INTERNAL_FAN_IMPACT_STATS_OUTPUT
{
    ULONG BucketCountPlusTwo;
    ULONGLONG Buckets[19];
} POWER_INTERNAL_FAN_IMPACT_STATS_OUTPUT, *PPOWER_INTERNAL_FAN_IMPACT_STATS_OUTPUT;

typedef struct _POWER_INTERNAL_FAN_RPM_BUCKETS_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_FAN_RPM_BUCKETS_INPUT, *PPOWER_INTERNAL_FAN_RPM_BUCKETS_INPUT;

typedef struct _POWER_INTERNAL_FAN_RPM_OUTPUT
{
    ULONG NumberOfFanRpmBuckets;
    ULONG BucketMaxRpm[17];
    ULONG NoiseZoneMaxRpm[4];
} POWER_INTERNAL_FAN_RPM_OUTPUT, *PPOWER_INTERNAL_FAN_RPM_OUTPUT;

C_ASSERT(sizeof(POWER_INTERNAL_FAN_RPM_OUTPUT) == 0x58);

typedef struct _POWER_INTERNAL_BOOTAPP_DIAGNOSTIC
{
    ULONG BootAppErrorDiagCode;
    ULONG BootAppFailureStatus;
} POWER_INTERNAL_BOOTAPP_DIAGNOSTIC, *PPOWER_INTERNAL_BOOTAPP_DIAGNOSTIC;

typedef struct _POWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    UCHAR Reserved[12];
} POWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_INPUT, *PPOWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_INPUT;

typedef struct  _POWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_OUTPUT
{
    UCHAR Capabilities[20];
} POWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_OUTPUT, *PPOWER_INTERNAL_GET_ACPI_TIME_AND_ALARM_CAPABILITIES_OUTPUT;

typedef struct _POWER_INTERNAL_SOC_IDENTIFIER_OPERATION_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG Action;
    ULONG Domain;
    ULONG Reserved[2];
} POWER_INTERNAL_SOC_IDENTIFIER_OPERATION_INPUT, *PPOWER_INTERNAL_SOC_IDENTIFIER_OPERATION_INPUT;

typedef struct _POWER_INTERNAL_SOC_IDENTIFIER_OPERATION_OUTPUT
{
    UCHAR Data[ANYSIZE_ARRAY];
} POWER_INTERNAL_SOC_IDENTIFIER_OPERATION_OUTPUT, *PPOWER_INTERNAL_SOC_IDENTIFIER_OPERATION_OUTPUT;

typedef struct _POWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
} POWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_INPUT, *PPOWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_INPUT;

typedef struct _POWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_OUTPUT
{
    BOOLEAN VmThrottleSupportedAndConfigured;
    ULONG VmThrottlePriorityCount;
} POWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_OUTPUT, *PPOWER_INTERNAL_VMPERF_PRIORITY_SUPPORT_OUTPUT;

typedef struct _POWER_INTERNAL_VMPERF_PRIORITY_CONFIG_INPUT
{
    POWER_INFORMATION_LEVEL_INTERNAL InternalType;
    ULONG Version;
    ULONG Action;
    ULONG Domain;
} POWER_INTERNAL_VMPERF_PRIORITY_CONFIG_INPUT, *PPOWER_INTERNAL_VMPERF_PRIORITY_CONFIG_INPUT;

typedef struct _POWER_INTERNAL_VMPERF_PRIORITY_CONFIG_OUTPUT
{
    BOOLEAN VmThrottleSupportedAndConfigured;
    ULONG VmThrottlePriorityCount;
} POWER_INTERNAL_VMPERF_PRIORITY_CONFIG_OUTPUT, *PPOWER_INTERNAL_VMPERF_PRIORITY_CONFIG_OUTPUT;

typedef struct _POWER_INTERNAL_DIRECTED_FX_ADD_TEST_DEVICE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG Flags;
    ULONG DeviceNameLength;
    WCHAR DeviceName[ANYSIZE_ARRAY];
} POWER_INTERNAL_DIRECTED_FX_ADD_TEST_DEVICE_INPUT, *PPOWER_INTERNAL_DIRECTED_FX_ADD_TEST_DEVICE_INPUT;

typedef struct _POWER_INTERNAL_DIRECTED_FX_REMOVE_TEST_DEVICE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG DeviceNameLength;
    WCHAR DeviceName[ANYSIZE_ARRAY];
} POWER_INTERNAL_DIRECTED_FX_REMOVE_TEST_DEVICE_INPUT, *PPOWER_INTERNAL_DIRECTED_FX_REMOVE_TEST_DEVICE_INPUT;

typedef struct _POWER_INTERNAL_DIRECTED_FX_SET_MODE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    BOOLEAN PermissiveMode;
} POWER_INTERNAL_DIRECTED_FX_SET_MODE_INPUT, *PPOWER_INTERNAL_DIRECTED_FX_SET_MODE_INPUT;

typedef struct _POWER_INTERNAL_DIRECTED_DRIPS_QUERY_CAPABILITIES_OUTPUT
{
    BOOLEAN SupportsDirectedFxTestDevice;
    BOOLEAN SupportsDirectedFxModeControl;
} POWER_INTERNAL_DIRECTED_DRIPS_QUERY_CAPABILITIES_OUTPUT, *PPOWER_INTERNAL_DIRECTED_DRIPS_QUERY_CAPABILITIES_OUTPUT;

typedef struct _POWER_INTERNAL_REGISTER_POWER_PLANE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG_PTR ClientHandle;
    ULONG_PTR DataSize;
    UCHAR Data[ANYSIZE_ARRAY];
} POWER_INTERNAL_REGISTER_POWER_PLANE_INPUT, *PPOWER_INTERNAL_REGISTER_POWER_PLANE_INPUT;

typedef struct _POWER_INTERNAL_DIRECTED_DRIPS_DEVICE_FLAGS_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    PVOID DeviceObject;
    ULONG Flags;
    ULONG Reserved;
} POWER_INTERNAL_DIRECTED_DRIPS_DEVICE_FLAGS_INPUT, *PPOWER_INTERNAL_DIRECTED_DRIPS_DEVICE_FLAGS_INPUT;

typedef struct _POWER_INTERNAL_RETRIEVE_HIBERFILE_RESUME_CONTEXT_OUTPUT
{
    ULONG Version;
    ULONG DataSize;
    ULONG EntryCount;
    UCHAR Data[ANYSIZE_ARRAY];
} POWER_INTERNAL_RETRIEVE_HIBERFILE_RESUME_CONTEXT_OUTPUT, *PPOWER_INTERNAL_RETRIEVE_HIBERFILE_RESUME_CONTEXT_OUTPUT;

typedef struct _POWER_INTERNAL_QUERY_SLEEPSTUDY_HELPER_ROUTINE_BLOCK_OUTPUT
{
    PVOID HelperRoutineBlock;
} POWER_INTERNAL_QUERY_SLEEPSTUDY_HELPER_ROUTINE_BLOCK_OUTPUT, *PPOWER_INTERNAL_QUERY_SLEEPSTUDY_HELPER_ROUTINE_BLOCK_OUTPUT;

typedef struct _POWER_INTERNAL_CLEAR_CONSTRAINTS_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    PVOID DeviceObject;
} POWER_INTERNAL_CLEAR_CONSTRAINTS_INPUT, *PPOWER_INTERNAL_CLEAR_CONSTRAINTS_INPUT;

typedef struct _POWER_INTERNAL_VM_PERF_CONTROL_CONFIG_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG Parameter0;
    ULONG Parameter1;
    ULONG Parameter2;
    ULONG Parameter3;
    ULONG Parameter4;
    BOOLEAN Parameter5;
    UCHAR Reserved[3];
} POWER_INTERNAL_VM_PERF_CONTROL_CONFIG_INPUT, *PPOWER_INTERNAL_VM_PERF_CONTROL_CONFIG_INPUT;

typedef struct _POWER_INTERNAL_VM_PERF_CONTROL_CONFIG_OUTPUT
{
    ULONG Value0;
    ULONG Value1;
} POWER_INTERNAL_VM_PERF_CONTROL_CONFIG_OUTPUT, *PPOWER_INTERNAL_VM_PERF_CONTROL_CONFIG_OUTPUT;

typedef struct _POWER_INTERNAL_CONCURRENCY_STATS_OUTPUT
{
    ULONG Data[ANYSIZE_ARRAY];
} POWER_INTERNAL_CONCURRENCY_STATS_OUTPUT, *PPOWER_INTERNAL_CONCURRENCY_STATS_OUTPUT;

typedef struct _PROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
} PROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES, *PPROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES;

typedef struct _PROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES_OUTPUT
{
    ULONG Capabilities;
} PROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES_OUTPUT, *PPROCESSOR_INTERNAL_QUERY_MEASUREMENT_CAPABILITIES_OUTPUT;

typedef struct _PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG Processor;
} PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES, *PPROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES;

typedef struct _PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUE_ENTRY
{
    ULONGLONG Value[3];
} PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUE_ENTRY, *PPROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUE_ENTRY;

typedef struct _PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES_OUTPUT
{
    ULONG EntryCount;
    ULONG Reserved;
    PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUE_ENTRY Entries[ANYSIZE_ARRAY];
} PROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES_OUTPUT, *PPROCESSOR_INTERNAL_QUERY_MEASUREMENT_VALUES_OUTPUT;

typedef struct _POWER_INTERNAL_PREPARE_FOR_SYSTEM_INITIATED_REBOOT_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
} POWER_INTERNAL_PREPARE_FOR_SYSTEM_INITIATED_REBOOT_INPUT, *PPOWER_INTERNAL_PREPARE_FOR_SYSTEM_INITIATED_REBOOT_INPUT;

typedef struct _POWER_INTERNAL_OVERRIDE_SYSTEM_INITIATED_REBOOT_STATE_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG State;
    ULONG Reserved;
} POWER_INTERNAL_OVERRIDE_SYSTEM_INITIATED_REBOOT_STATE_INPUT, *PPOWER_INTERNAL_OVERRIDE_SYSTEM_INITIATED_REBOOT_STATE_INPUT;

typedef struct _POWER_INTERNAL_UNREGISTER_SHUTDOWN_NOTIFICATION_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    HANDLE ProcessId;
    HANDLE ThreadId;
} POWER_INTERNAL_UNREGISTER_SHUTDOWN_NOTIFICATION_INPUT, *PPOWER_INTERNAL_UNREGISTER_SHUTDOWN_NOTIFICATION_INPUT;

typedef struct _POWER_INTERNAL_MANAGE_TRANSITION_STATE_RECORD_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG Action;
    ULONG Reserved0;
    HANDLE ProcessId;
    HANDLE ThreadId;
    ULONG Reason;
    ULONG CallbackType;
    PVOID CallbackContext;
} POWER_INTERNAL_MANAGE_TRANSITION_STATE_RECORD_INPUT, *PPOWER_INTERNAL_MANAGE_TRANSITION_STATE_RECORD_INPUT;

typedef struct _POWER_INTERNAL_SUSPEND_RESUME_REQUEST_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    UCHAR Data[12];
} POWER_INTERNAL_SUSPEND_RESUME_REQUEST_INPUT, *PPOWER_INTERNAL_SUSPEND_RESUME_REQUEST_INPUT;

typedef struct _POWER_INTERNAL_ENERGY_ESTIMATION_ENTRY
{
    ULONG Data[4];
} POWER_INTERNAL_ENERGY_ESTIMATION_ENTRY, *PPOWER_INTERNAL_ENERGY_ESTIMATION_ENTRY;

typedef struct _POWER_INTERNAL_ENERGY_ESTIMATION_INFO_OUTPUT
{
    ULONG EntryCount;
    POWER_INTERNAL_ENERGY_ESTIMATION_ENTRY Entries[2];
} POWER_INTERNAL_ENERGY_ESTIMATION_INFO_OUTPUT, *PPOWER_INTERNAL_ENERGY_ESTIMATION_INFO_OUTPUT;

typedef struct _POWER_INTERNAL_NOTIFY_WIN32K_POWER_REQUEST_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG RequestIndex;
} POWER_INTERNAL_NOTIFY_WIN32K_POWER_REQUEST_INPUT, *PPOWER_INTERNAL_NOTIFY_WIN32K_POWER_REQUEST_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_NOTIFY_WIN32K_POWER_REQUEST_INPUT) == 0x0C);

typedef struct _POWER_INTERNAL_PDC_AGENT_SESSION_QUERY_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    ULONG QueryKey;
} POWER_INTERNAL_PDC_AGENT_SESSION_QUERY_INPUT, *PPOWER_INTERNAL_PDC_AGENT_SESSION_QUERY_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_PDC_AGENT_SESSION_QUERY_INPUT) == 0x0C);

typedef struct _POWER_INTERNAL_SESSION_CONNECTION_INFO_V2
{
    BOOLEAN Connected;
    UCHAR Reserved0[3];
    ULONG ConnectionType;
} POWER_INTERNAL_SESSION_CONNECTION_INFO_V2, *PPOWER_INTERNAL_SESSION_CONNECTION_INFO_V2;

typedef struct _POWER_INTERNAL_ADAPTIVE_SESSION_STATE_REQUEST
{
    ULONGLONG Field0;
    ULONGLONG Field1;
    ULONGLONG Field2;
    ULONG Field3;
    ULONG Reserved;
} POWER_INTERNAL_ADAPTIVE_SESSION_STATE_REQUEST, *PPOWER_INTERNAL_ADAPTIVE_SESSION_STATE_REQUEST;

typedef struct _POWER_INTERNAL_SESSION_CONNECTION_CHANGE_V2_INPUT
{
    POWER_INFORMATION_INTERNAL_HEADER Header;
    POWER_INTERNAL_SESSION_CONNECTION_INFO_V2 ConnectionInfo;
    POWER_INTERNAL_ADAPTIVE_SESSION_STATE_REQUEST SessionStateRequest;
} POWER_INTERNAL_SESSION_CONNECTION_CHANGE_V2_INPUT, *PPOWER_INTERNAL_SESSION_CONNECTION_CHANGE_V2_INPUT;

C_ASSERT(sizeof(POWER_INTERNAL_SESSION_CONNECTION_CHANGE_V2_INPUT) == 0x30);

typedef struct _POWER_INFORMATION_ENERGY_TRACKER_CREATE_INPUT
{
    ULONG Version;
    ULONG Flags;
    ULONG Reserved;
} POWER_INFORMATION_ENERGY_TRACKER_CREATE_INPUT, *PPOWER_INFORMATION_ENERGY_TRACKER_CREATE_INPUT;

typedef struct _POWER_INFORMATION_ENERGY_TRACKER_CREATE_OUTPUT
{
    HANDLE QueryHandle;
} POWER_INFORMATION_ENERGY_TRACKER_CREATE_OUTPUT, *PPOWER_INFORMATION_ENERGY_TRACKER_CREATE_OUTPUT;

typedef struct _POWER_INFORMATION_ENERGY_TRACKER_QUERY_INPUT
{
    HANDLE QueryHandle;
} POWER_INFORMATION_ENERGY_TRACKER_QUERY_INPUT, *PPOWER_INFORMATION_ENERGY_TRACKER_QUERY_INPUT;

typedef struct _POWER_INFORMATION_ENERGY_TRACKER_QUERY_OUTPUT
{
    ULONG Version;
    ULONG DataType;
    ULONG DataSize;
} POWER_INFORMATION_ENERGY_TRACKER_QUERY_OUTPUT, *PPOWER_INFORMATION_ENERGY_TRACKER_QUERY_OUTPUT;

DEFINE_GUID(PopBlackBoxScmGuid, 0x45F9D5A3, 0xE1D0, 0x8891, 0x07, 0x26, 0xFB, 0x1D, 0x71, 0xAD, 0x11, 0xB8);
DEFINE_GUID(PopBlackBoxBsdGuid, 0x4E01CC45, 0xF573, 0x08DF, 0x0E, 0xC1, 0x0B, 0x0E, 0xBA, 0x42, 0x97, 0x6A);
DEFINE_GUID(PopBlackBoxPnpGuid, 0x4CD6532A, 0xB763, 0x1941, 0x57, 0x45, 0x7D, 0x91, 0xB5, 0xED, 0xB1, 0xB1);
DEFINE_GUID(PopBlackBoxAcpiGuid, 0x429FF755, 0x3B2E, 0xA98B, 0x8C, 0x52, 0x06, 0x81, 0xA1, 0x31, 0xC1, 0x80);
DEFINE_GUID(PopBlackBoxPoIrpGuid, 0x4A654DDB, 0x2523, 0xDB46, 0x0C, 0x65, 0xC9, 0x83, 0xF0, 0xE9, 0x13, 0x9A);
DEFINE_GUID(PopBlackBoxWinLogonNotifyGuid, 0x4E3EAA07, 0x6B2D, 0x3E93, 0x3B, 0xC6, 0x3C, 0x0E, 0x6D, 0x91, 0x1A, 0xA4);
DEFINE_GUID(PopBlackBoxPdcLockGuid, 0x4E912A6E, 0x33DB, 0xDDBB, 0x84, 0x8A, 0x7B, 0x99, 0xE1, 0x5D, 0x42, 0x9E);
DEFINE_GUID(PopBlackBoxPoPepWorkOrderGuid, 0x42750E88, 0xE0E8, 0x5A55, 0x0D, 0x03, 0x45, 0xAF, 0xB3, 0xF1, 0x33, 0xF9);
DEFINE_GUID(PopBlackBoxPoPowerWatchdogGuid, 0x44675326, 0x5545, 0xF79E, 0x55, 0x31, 0xE3, 0x3A, 0x63, 0x81, 0x69, 0xAE);
DEFINE_GUID(PopBlackBoxPnpEventWorkerGuid, 0x4131386C, 0x8BEF, 0xF310, 0x4A, 0x68, 0x1E, 0xFA, 0x44, 0x0A, 0xB1, 0xB1);
DEFINE_GUID(PopBlackBoxPnpDeviceCompletionQueueGuid, 0x452E8590, 0xC129, 0x4D5E, 0x68, 0xCA, 0x00, 0xF7, 0x45, 0x7F, 0x71, 0xBC);
DEFINE_GUID(PopBlackBoxPnpDelayedRemoveWorkerGuid, 0x4D9CFF3A, 0x7392, 0xA43B, 0x08, 0xED, 0xCD, 0x65, 0xAA, 0x50, 0x31, 0xBA);
DEFINE_GUID(PopBlackBoxDxgDisplayGuid, 0x44D6ED00, 0xB3BE, 0xB7EE, 0x1C, 0x0F, 0xF8, 0x2D, 0xA9, 0xC1, 0x60, 0xAD);
DEFINE_GUID(PopBlackBoxCrashedProcessGuid, 0x4367A550, 0xBE84, 0xD651, 0x94, 0x1F, 0x63, 0x2B, 0x79, 0xAB, 0x63, 0x8E);
DEFINE_GUID(PopBlackBoxUsoCommitGuid, 0x4CDA57F3, 0x6DE4, 0xC85A, 0x0E, 0x4F, 0x85, 0x85, 0xA8, 0x5C, 0x8F, 0xE8);
DEFINE_GUID(PopBlackBoxWheaGuid, 0x457D912A, 0x32D3, 0xEA49, 0x0F, 0xC5, 0x3E, 0xF2, 0x92, 0x9D, 0xDE, 0x6B);
DEFINE_GUID(PopBlackBoxNtfsGuid, 0x4213940D, 0x00AF, 0xE9C4, 0x20, 0xBC, 0xB5, 0x19, 0x37, 0xCD, 0x16, 0x80);
DEFINE_GUID(PopBlackBoxWinLogonGuid, 0x4AF1A719, 0x80CC, 0x79CF, 0x0C, 0x1E, 0xB7, 0x6F, 0xF2, 0x9F, 0xE9, 0x7B);
DEFINE_GUID(PopBlackBoxExplorerLogonTasksGuid, 0x4D93B9AC, 0xAA9A, 0x6517, 0x0A, 0x03, 0x93, 0x6E, 0x66, 0x62, 0x6D, 0x08);
DEFINE_GUID(PopBlackBoxExplorerCoreStartupGuid, 0x4E121623, 0xF5A6, 0xB2E1, 0x0F, 0xA8, 0x08, 0x29, 0xBA, 0x84, 0x83, 0x98);
DEFINE_GUID(PopBlackBoxUserModeLKDReasonGuid, 0x44BEB1A5, 0xC1B9, 0x41DF, 0x22, 0x5E, 0xBC, 0x66, 0xF1, 0xDA, 0x5C, 0x9D);
DEFINE_GUID(PopBlackBoxCodeIntegrityGuid, 0x44A03CF4, 0x4EE7, 0x6BD8, 0x0A, 0x33, 0x73, 0xE6, 0x43, 0x73, 0x9A, 0x0C);
DEFINE_GUID(PoBlackBoxIdCsrGuid, 0x470BC061, 0x42C1, 0xADD0, 0x0C, 0xB1, 0x8E, 0x99, 0xF2, 0xEF, 0x68, 0xFB);
DEFINE_GUID(PoBlackBoxIdSmGuid, 0x42D2AC4A, 0xD368, 0xF58F, 0x25, 0xA7, 0x6A, 0xC2, 0xDB, 0x76, 0x97, 0x8F);

typedef struct _POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY
{
    ULONG Index;
    PCSTR Name;
    GUID Guid;
} POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY, *PPOWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY;


typedef struct _POWER_INFORMATION_BBR_UPDATE_REQUEST_INPUT
{
    ULONG Version;
    ULONG Flags;
    ULONG_PTR Reserved0;
    ULONG_PTR Reserved1;
    ULONG_PTR Reserved2;
} POWER_INFORMATION_BBR_UPDATE_REQUEST_INPUT, *PPOWER_INFORMATION_BBR_UPDATE_REQUEST_INPUT;

#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_MAX_CATEGORY RTL_NUMBER_OF(BlackBoxCategories)
#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY_SHIFT 0
#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY_MASK_RAW  0x0000FFFF
#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY_MASK_DIRECT 0x0000001F
#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_MODIFIERS_MASK 0xFFFF0000

#define POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_FLAGS(category, modifiers) \
    ((ULONG)(((category) & POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY_MASK_RAW << POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_CATEGORY_SHIFT) | \
    ((ULONG)(modifiers) & POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_MODIFIERS_MASK)))

typedef struct _POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_INPUT
{
    ULONG Version;
    union
    {
        ULONG Flags;
        struct
        {
            ULONG Category : 5;
            ULONG Reserved : 11;
            ULONG Modifiers : 16;
        };
    };
    ULONG_PTR Offset;
    ULONG_PTR Length;
} POWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_INPUT, *PPOWER_INFORMATION_BBR_DIRECT_ACCESS_REQUEST_INPUT;

typedef struct _POWER_INFORMATION_BBR_DIRECT_ACCESS_RESPONSE_OUTPUT
{
    PVOID UserMappingBase;
    SIZE_T UserMappingSize;
} POWER_INFORMATION_BBR_DIRECT_ACCESS_RESPONSE_OUTPUT, *PPOWER_INFORMATION_BBR_DIRECT_ACCESS_RESPONSE_OUTPUT;

#if (PHNT_MODE != PHNT_MODE_KERNEL)
/**
 * The NtPowerInformation routine sets or retrieves system power information.
 *
 * \param InformationLevel Specifies the requested information level, which indicates the specific power information to be set or retrieved.
 * \param InputBuffer Optional pointer to a caller-allocated input buffer.
 * \param InputBufferLength Size, in bytes, of the buffer at InputBuffer.
 * \param OutputBuffer Optional pointer to an output buffer. The type depends on the InformationLevel requested.
 * \param OutputBufferLength Size, in bytes, of the output buffer.
 * \return Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-ntpowerinformation
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtPowerInformation(
    _In_ POWER_INFORMATION_LEVEL InformationLevel,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength
    );

/**
 * The NtSetThreadExecutionState routine informs the system of execution requirements,
 *
 * in order to prevent the system from entering sleep or turning off the display while the application is running.
 * \param NewFlags New execution state flags.
 * \param PreviousFlags Pointer to receive the previous execution state flags.
 * \return Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setthreadexecutionstate
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetThreadExecutionState(
    _In_ EXECUTION_STATE NewFlags,
    _Out_ EXECUTION_STATE *PreviousFlags
    );
#endif

#if (PHNT_VERSION < PHNT_WINDOWS_7)
/**
 * The NtRequestWakeupLatency routine requests the system resume latency.
 *
 * \param latency The desired latency time.
 * \return Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtRequestWakeupLatency(
    _In_ LATENCY_TIME latency
    );
#endif

/**
 * The NtInitiatePowerAction routine initiates a shutdown and optional restart of the specified computer.
 *
 * \param SystemAction The system power action.
 * \param LightestSystemState The lightest system power state.
 * \param Flags Flags for the power action.
 * \param Asynchronous Whether the action is asynchronous.
 * \return Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winreg/nf-winreg-initiatesystemshutdownw
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitiatePowerAction(
    _In_ POWER_ACTION SystemAction,
    _In_ SYSTEM_POWER_STATE LightestSystemState,
    _In_ ULONG Flags,
    _In_ BOOLEAN Asynchronous
    );

/**
 * The NtSetSystemPowerState routine initiates a suspension and optional forced shutdown of the specified computer.
 *
 * \param SystemAction The system power action.
 * \param LightestSystemState The lightest system power state.
 * \param Flags Flags for the power action.
 * \return Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setsystempowerstate
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetSystemPowerState(
    _In_ POWER_ACTION SystemAction,
    _In_ SYSTEM_POWER_STATE LightestSystemState,
    _In_ ULONG Flags
    );

/**
 * The NtGetDevicePowerState routine retrieves the current power state of the specified device.
 *
 * \param Device A handle to an object on the device, such as a file or socket, or a handle to the device itself.
 * \param State A pointer to the variable that receives the power state.
 * \return Successful or errant status.
 * \remarks An application can use NtGetDevicePowerState to determine whether a device is in the working state or a low-power state.
 * If the device is in a low-power state, accessing the device may cause it to either queue or fail any I/O requests, or transition the device into the working state.
 * The exact behavior depends on the implementation of the device.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtGetDevicePowerState(
    _In_ HANDLE Device,
    _Out_ PDEVICE_POWER_STATE State
    );

/**
 * The NtIsSystemResumeAutomatic routine checks if the system resume is automatic.

 * \return BOOLEAN TRUE if the system resume is automatic, FALSE otherwise.
 */
NTSYSCALLAPI
BOOLEAN
NTAPI
NtIsSystemResumeAutomatic(
    VOID
    );

#endif
