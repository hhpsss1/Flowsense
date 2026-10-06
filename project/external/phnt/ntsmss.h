/*
 * Windows Session Manager support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTSMSS_H
#define _NTSMSS_H


typedef enum _SMAPINUMBER
{
    SmNotImplementedApi = 0,
    SmSessionCompleteApi = 1,
    SmNotImplemented2Api = 2,
    SmExecPgmApi = 3,
    SmLoadDeferedSubsystemApi = 4,
    SmStartCsrApi = 5,
    SmStopCsrApi = 6,
    SmStartServerSiloApi = 7,
    SmMaxApiNumber = 8,
} SMAPINUMBER, *PSMAPINUMBER;

typedef struct _SMSESSIONCOMPLETE
{
    _In_ ULONG SessionId;
    _In_ NTSTATUS CompletionStatus;
} SMSESSIONCOMPLETE, *PSMSESSIONCOMPLETE;

typedef struct _SMEXECPGM
{
    _In_ RTL_USER_PROCESS_INFORMATION ProcessInformation;
    _In_ BOOLEAN DebugFlag;
} SMEXECPGM, *PSMEXECPGM;

typedef struct _SMLOADDEFERED
{
    _In_ ULONG SubsystemNameLength;
    _In_ _Field_size_bytes_(SubsystemNameLength) WCHAR SubsystemName[32];
} SMLOADDEFERED, *PSMLOADDEFERED;

typedef struct _SMSTARTCSR
{
    _Inout_ ULONG MuSessionId;
    _In_ ULONG InitialCommandLength;
    _In_ _Field_size_bytes_(InitialCommandLength) WCHAR InitialCommand[128];
    _Out_ HANDLE InitialCommandProcessId;
    _Out_ HANDLE WindowsSubSysProcessId;
} SMSTARTCSR, *PSMSTARTCSR;

typedef struct _SMSTOPCSR
{
    _In_ ULONG MuSessionId;
} SMSTOPCSR, *PSMSTOPCSR;

typedef struct _SMSTARTSERVERSILO
{
    _In_ HANDLE JobHandle;
    _In_ BOOLEAN CreateSuspended;
} SMSTARTSERVERSILO, *PSMSTARTSERVERSILO;

typedef struct _SMAPIMSG
{
    PORT_MESSAGE h;
    SMAPINUMBER ApiNumber;
    NTSTATUS ReturnedStatus;
    union
    {
        union
        {
            SMSESSIONCOMPLETE SessionComplete;
            SMEXECPGM ExecPgm;
            SMLOADDEFERED LoadDefered;
            SMSTARTCSR StartCsr;
            SMSTOPCSR StopCsr;
            SMSTARTSERVERSILO StartServerSilo;
        };
    } u;
} SMAPIMSG, *PSMAPIMSG;


typedef enum _SBAPINUMBER
{
    SbCreateSessionApi = 0,
    SbTerminateSessionApi = 1,
    SbForeignSessionCompleteApi = 2,
    SbCreateProcessApi = 3,
    SbMaxApiNumber = 4,
} SBAPINUMBER, *PSBAPINUMBER;

typedef struct _SBCONNECTINFO
{
    _In_ ULONG SubsystemImageType;
    _In_ WCHAR EmulationSubSystemPortName[120];
} SBCONNECTINFO, *PSBCONNECTINFO;

typedef struct _SBCREATESESSION
{
    _In_ ULONG SessionId;
    _In_ RTL_USER_PROCESS_INFORMATION ProcessInformation;
    _In_opt_ PVOID UserProfile;
    _In_ ULONG DebugSession;
    _In_ CLIENT_ID DebugUiClientId;
} SBCREATESESSION, *PSBCREATESESSION;

typedef struct _SBTERMINATESESSION
{
    _In_ ULONG SessionId;
    _In_ NTSTATUS TerminationStatus;
} SBTERMINATESESSION, *PSBTERMINATESESSION;

typedef struct _SBFOREIGNSESSIONCOMPLETE
{
    _In_ ULONG SessionId;
    _In_ NTSTATUS TerminationStatus;
} SBFOREIGNSESSIONCOMPLETE, *PSBFOREIGNSESSIONCOMPLETE;

#define SMP_DEBUG_FLAG 0x00000001
#define SMP_ASYNC_FLAG 0x00000002
#define SMP_DONT_START 0x00000004

typedef struct _SBCREATEPROCESSIN
{
    _In_ PCUNICODE_STRING ImageFileName;
    _In_ PCUNICODE_STRING CurrentDirectory;
    _In_ PCUNICODE_STRING CommandLine;
    _In_opt_ PCUNICODE_STRING DefaultLibPath;
    _In_ ULONG Flags;
    _In_ ULONG DefaultDebugFlags;
} SBCREATEPROCESSIN, *PSBCREATEPROCESSIN;

typedef struct _SBCREATEPROCESSOUT
{
    _Out_ HANDLE Process;
    _Out_ HANDLE Thread;
    _Out_ ULONG SubSystemType;
    _Out_ CLIENT_ID ClientId;
} SBCREATEPROCESSOUT, *PSBCREATEPROCESSOUT;

typedef struct _SBCREATEPROCESS
{
    union
    {
        SBCREATEPROCESSIN i;
        SBCREATEPROCESSOUT o;
    };
} SBCREATEPROCESS, *PSBCREATEPROCESS;

typedef struct _SBAPIMSG
{
    PORT_MESSAGE h;
    union
    {
        SBCONNECTINFO ConnectionRequest;
        struct
        {
            SBAPINUMBER ApiNumber;
            NTSTATUS ReturnedStatus;
            union
            {
                SBCREATESESSION CreateSession;
                SBTERMINATESESSION TerminateSession;
                SBFOREIGNSESSIONCOMPLETE ForeignSessionComplete;
                SBCREATEPROCESS CreateProcessA;
            };
        };
    } u;
} SBAPIMSG, *PSBAPIMSG;


NTSYSAPI
NTSTATUS
NTAPI
RtlConnectToSm(
    _In_opt_ PCUNICODE_STRING ApiPortName,
    _In_opt_ HANDLE ApiPortHandle,
    _In_ ULONG ProcessImageType,
    _Out_ PHANDLE SmssConnection
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSendMsgToSm(
    _In_ HANDLE ApiPortHandle,
    _Inout_updates_(MessageData->u1.s1.TotalLength) PPORT_MESSAGE MessageData
    );

#endif
