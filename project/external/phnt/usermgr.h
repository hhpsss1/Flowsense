/*
 * User Manager service API definitions.
 *
 * This file is part of System Informer.
 */

#ifndef _USERMGR_H
#define _USERMGR_H

typedef struct _SESSION_USER_CONTEXT
{
    ULONGLONG ContextToken;
    ULONG SessionId;
    ULONG Reserved;
} SESSION_USER_CONTEXT, *PSESSION_USER_CONTEXT;

typedef struct _CRED_PROV_CREDENTIAL
{
    ULONG Flags;
    ULONG AuthenticationPackage;
    ULONG Size;
    PVOID Information;
} CRED_PROV_CREDENTIAL, *PCRED_PROV_CREDENTIAL;

#define USERMGRAPI DECLSPEC_IMPORT


#if (PHNT_VERSION >= PHNT_WINDOWS_10)

USERMGRAPI
VOID
NTAPI
UMgrFreeSessionUsers(
    _In_ _Post_invalid_ PSESSION_USER_CONTEXT SessionUsers
    );

USERMGRAPI
HRESULT
NTAPI
UMgrEnumerateSessionUsers(
    _Out_ PULONG Count,
    _Outptr_ PSESSION_USER_CONTEXT *SessionUsers
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserContext(
    _In_ HANDLE TokenHandle,
    _Out_ PULONGLONG ContextToken
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserContextFromSid(
    _In_ PCWSTR SidString,
    _Out_ PULONGLONG ContextToken
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserContextFromName(
    _In_ PCWSTR UserName,
    _Out_ PULONGLONG ContextToken
    );

#endif


#if (PHNT_VERSION >= PHNT_WINDOWS_10)

USERMGRAPI
HRESULT
NTAPI
UMgrQueryDefaultAccountToken(
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQuerySessionUserToken(
    _In_ ULONG SessionId,
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserToken(
    _In_ ULONGLONG Context,
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserTokenFromSid(
    _In_ PCWSTR SidString,
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrQueryUserTokenFromName(
    _In_ PCWSTR UserName,
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrGetConstrainedUserToken(
    _In_opt_ HANDLE InputTokenHandle,
    _In_ ULONGLONG Context,
    _In_opt_ PSECURITY_CAPABILITIES Capabilities,
    _Out_ _Ret_maybenull_ PHANDLE OutputTokenHandle
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_TH2)

USERMGRAPI
HRESULT
NTAPI
UMgrChangeSessionUserToken(
    _In_ HANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrGetImpersonationTokenForContext(
    _In_ HANDLE InputTokenHandle,
    _In_ ULONGLONG Context,
    _Out_ PHANDLE OutputTokenHandle
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS1)

USERMGRAPI
HRESULT
NTAPI
UMgrGetSessionActiveShellUserToken(
    _In_ ULONG SessionId,
    _Out_ PHANDLE TokenHandle
    );

#endif


#if (PHNT_VERSION >= PHNT_WINDOWS_10)

USERMGRAPI
HRESULT
NTAPI
UMgrOpenProcessTokenForQuery(
    _In_ ULONG ProcessId,
    _Out_ PHANDLE TokenHandle
    );

USERMGRAPI
HRESULT
NTAPI
UMgrOpenProcessHandleForAccess(
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG ProcessId,
    _Out_ PHANDLE ProcessHandle
    );

#endif


#if (PHNT_VERSION >= PHNT_WINDOWS_10)

USERMGRAPI
HRESULT
NTAPI
UMgrFreeUserCredentials(
    _In_ PCRED_PROV_CREDENTIAL Credentials
    );

USERMGRAPI
HRESULT
NTAPI
UMgrGetCachedCredentials(
    _In_ PSID Sid,
    _Outptr_ PCRED_PROV_CREDENTIAL *Credentials
    );

#endif

#endif
