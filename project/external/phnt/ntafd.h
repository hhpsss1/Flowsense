/*
 * Ancillary Function Driver definitions
 *
 * This file is part of System Informer.
 */

#ifndef _NTAFD_H
#define _NTAFD_H

#include <WinSock2.h>
#include <MSWSock.h>
#include <tdi.h>

#define AFD_DEVICE_NAME L"\\Device\\Afd"

#define AfdOpenPacket      "AfdOpenPacketXX"
#define AfdSwOpenPacket    "AfdSwOpenPacket"
#define AfdRioRDOpenPacket "AfdRioRDOpenPacket"

typedef struct _AFD_ENDPOINT_FLAGS
{
    union
    {
        struct
        {
            UCHAR ConnectionLess : 1;
            UCHAR : 3;
            UCHAR MessageMode : 1;
            UCHAR : 3;
            UCHAR Raw : 1;
            UCHAR : 3;
            UCHAR Multipoint : 1;
            UCHAR : 3;
            UCHAR C_Root : 1;
            UCHAR : 3;
            UCHAR D_Root : 1;
            UCHAR : 3;
            UCHAR IgnoreTDI : 1;
            UCHAR : 3;
            UCHAR RioSocket : 1;
            UCHAR : 3;
        };
        ULONG EndpointFlags;
    };
} AFD_ENDPOINT_FLAGS, *PAFD_ENDPOINT_FLAGS;

#define DD_TCP_DEVICE_NAME      L"\\Device\\Tcp"
#define DD_TCPV6_DEVICE_NAME    L"\\Device\\Tcp6"
#define DD_UDP_DEVICE_NAME      L"\\Device\\Udp"
#define DD_UDPV6_DEVICE_NAME    L"\\Device\\Udp6"
#define DD_RAW_IP_DEVICE_NAME   L"\\Device\\RawIp"
#define DD_RAW_IPV6_DEVICE_NAME L"\\Device\\RawIp6"

typedef struct _AFD_OPEN_PACKET
{
    _In_ AFD_ENDPOINT_FLAGS __f;
    _In_opt_ GROUP GroupID;
    _In_ LONG AddressFamily;
    _In_ LONG SocketType;
    _In_ LONG Protocol;
    _In_opt_ ULONG TransportDeviceNameLength;
    _Field_size_bytes_opt_(TransportDeviceNameLength) WCHAR TransportDeviceName[ANYSIZE_ARRAY];
} AFD_OPEN_PACKET, *PAFD_OPEN_PACKET;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _AFD_OPEN_PACKET_FULL_EA
{
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[sizeof(AfdOpenPacket)];
    AFD_OPEN_PACKET OpenPacket;
} AFD_OPEN_PACKET_FULL_EA, *PAFD_OPEN_PACKET_FULL_EA;

typedef struct _AFD_SWITCH_OPEN_PACKET
{
    HANDLE CompletionPort;
    HANDLE CompletionEvent;
} AFD_SWITCH_OPEN_PACKET, *PAFD_SWITCH_OPEN_PACKET;


#define AFD_BIND                        0
#define AFD_CONNECT                     1
#define AFD_START_LISTEN                2
#define AFD_WAIT_FOR_LISTEN             3
#define AFD_ACCEPT                      4
#define AFD_RECEIVE                     5
#define AFD_RECEIVE_DATAGRAM            6
#define AFD_SEND                        7
#define AFD_SEND_DATAGRAM               8
#define AFD_POLL                        9
#define AFD_PARTIAL_DISCONNECT          10
#define AFD_GET_ADDRESS                 11
#define AFD_QUERY_RECEIVE_INFO          12
#define AFD_QUERY_HANDLES               13
#define AFD_SET_INFORMATION             14
#define AFD_GET_REMOTE_ADDRESS          15
#define AFD_GET_CONTEXT                 16
#define AFD_SET_CONTEXT                 17
#define AFD_SET_CONNECT_DATA            18
#define AFD_SET_CONNECT_OPTIONS         19
#define AFD_SET_DISCONNECT_DATA         20
#define AFD_SET_DISCONNECT_OPTIONS      21
#define AFD_GET_CONNECT_DATA            22
#define AFD_GET_CONNECT_OPTIONS         23
#define AFD_GET_DISCONNECT_DATA         24
#define AFD_GET_DISCONNECT_OPTIONS      25
#define AFD_SIZE_CONNECT_DATA           26
#define AFD_SIZE_CONNECT_OPTIONS        27
#define AFD_SIZE_DISCONNECT_DATA        28
#define AFD_SIZE_DISCONNECT_OPTIONS     29
#define AFD_GET_INFORMATION             30
#define AFD_TRANSMIT_FILE               31
#define AFD_SUPER_ACCEPT                32
#define AFD_EVENT_SELECT                33
#define AFD_ENUM_NETWORK_EVENTS         34
#define AFD_DEFER_ACCEPT                35
#define AFD_WAIT_FOR_LISTEN_LIFO        36
#define AFD_SET_QOS                     37
#define AFD_GET_QOS                     38
#define AFD_NO_OPERATION                39
#define AFD_VALIDATE_GROUP              40
#define AFD_GET_UNACCEPTED_CONNECT_DATA 41
#define AFD_ROUTING_INTERFACE_QUERY     42
#define AFD_ROUTING_INTERFACE_CHANGE    43
#define AFD_ADDRESS_LIST_QUERY          44
#define AFD_ADDRESS_LIST_CHANGE         45
#define AFD_JOIN_LEAF                   46
#define AFD_TRANSPORT_IOCTL             47
#define AFD_TRANSMIT_PACKETS            48
#define AFD_SUPER_CONNECT               49
#define AFD_SUPER_DISCONNECT            50
#define AFD_RECEIVE_MESSAGE             51
#define AFD_SEND_MESSAGE                52
#define AFD_SWITCH_CEMENT_SAN           53
#define AFD_SWITCH_SET_EVENTS           54
#define AFD_SWITCH_RESET_EVENTS         55
#define AFD_SWITCH_CONNECT_IND          56
#define AFD_SWITCH_CMPL_ACCEPT          57
#define AFD_SWITCH_CMPL_REQUEST         58
#define AFD_SWITCH_CMPL_IO              59
#define AFD_SWITCH_REFRESH_ENDP         60
#define AFD_SWITCH_GET_PHYSICAL_ADDR    61
#define AFD_SWITCH_ACQUIRE_CTX          62
#define AFD_SWITCH_TRANSFER_CTX         63
#define AFD_SWITCH_GET_SERVICE_PID      64
#define AFD_SWITCH_SET_SERVICE_PROCESS  65
#define AFD_SWITCH_PROVIDER_CHANGE      66
#define AFD_SWITCH_ADDRLIST_CHANGE      67
#define AFD_UNBIND                      68
#define AFD_SQM                         69
#define AFD_RIO                         70
#define AFD_TRANSFER_BEGIN              71
#define AFD_TRANSFER_END                72
#define AFD_NOTIFY                      73

#define FSCTL_AFD_BASE  FILE_DEVICE_NETWORK
#define _AFD_CONTROL_CODE(Request, Method) (FSCTL_AFD_BASE << 12 | (Request) << 2 | (Method))

#define IOCTL_AFD_BIND                        _AFD_CONTROL_CODE(AFD_BIND, METHOD_NEITHER)
#define IOCTL_AFD_CONNECT                     _AFD_CONTROL_CODE(AFD_CONNECT, METHOD_NEITHER)
#define IOCTL_AFD_START_LISTEN                _AFD_CONTROL_CODE(AFD_START_LISTEN, METHOD_NEITHER)
#define IOCTL_AFD_WAIT_FOR_LISTEN             _AFD_CONTROL_CODE(AFD_WAIT_FOR_LISTEN, METHOD_BUFFERED)
#define IOCTL_AFD_ACCEPT                      _AFD_CONTROL_CODE(AFD_ACCEPT, METHOD_BUFFERED)
#define IOCTL_AFD_RECEIVE                     _AFD_CONTROL_CODE(AFD_RECEIVE, METHOD_NEITHER)
#define IOCTL_AFD_RECEIVE_DATAGRAM            _AFD_CONTROL_CODE(AFD_RECEIVE_DATAGRAM, METHOD_NEITHER)
#define IOCTL_AFD_SEND                        _AFD_CONTROL_CODE(AFD_SEND, METHOD_NEITHER)
#define IOCTL_AFD_SEND_DATAGRAM               _AFD_CONTROL_CODE(AFD_SEND_DATAGRAM, METHOD_NEITHER)
#define IOCTL_AFD_POLL                        _AFD_CONTROL_CODE(AFD_POLL, METHOD_BUFFERED)
#define IOCTL_AFD_PARTIAL_DISCONNECT          _AFD_CONTROL_CODE(AFD_PARTIAL_DISCONNECT, METHOD_NEITHER)
#define IOCTL_AFD_GET_ADDRESS                 _AFD_CONTROL_CODE(AFD_GET_ADDRESS, METHOD_NEITHER)
#define IOCTL_AFD_QUERY_RECEIVE_INFO          _AFD_CONTROL_CODE(AFD_QUERY_RECEIVE_INFO, METHOD_NEITHER)
#define IOCTL_AFD_QUERY_HANDLES               _AFD_CONTROL_CODE(AFD_QUERY_HANDLES, METHOD_NEITHER)
#define IOCTL_AFD_SET_INFORMATION             _AFD_CONTROL_CODE(AFD_SET_INFORMATION, METHOD_NEITHER)
#define IOCTL_AFD_GET_REMOTE_ADDRESS          _AFD_CONTROL_CODE(AFD_GET_REMOTE_ADDRESS, METHOD_NEITHER)
#define IOCTL_AFD_GET_CONTEXT                 _AFD_CONTROL_CODE(AFD_GET_CONTEXT, METHOD_NEITHER)
#define IOCTL_AFD_SET_CONTEXT                 _AFD_CONTROL_CODE(AFD_SET_CONTEXT, METHOD_NEITHER)
#define IOCTL_AFD_SET_CONNECT_DATA            _AFD_CONTROL_CODE(AFD_SET_CONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_SET_CONNECT_OPTIONS         _AFD_CONTROL_CODE(AFD_SET_CONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_SET_DISCONNECT_DATA         _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_SET_DISCONNECT_OPTIONS      _AFD_CONTROL_CODE(AFD_SET_DISCONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_GET_CONNECT_DATA            _AFD_CONTROL_CODE(AFD_GET_CONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_GET_CONNECT_OPTIONS         _AFD_CONTROL_CODE(AFD_GET_CONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_GET_DISCONNECT_DATA         _AFD_CONTROL_CODE(AFD_GET_DISCONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_GET_DISCONNECT_OPTIONS      _AFD_CONTROL_CODE(AFD_GET_DISCONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_SIZE_CONNECT_DATA           _AFD_CONTROL_CODE(AFD_SIZE_CONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_SIZE_CONNECT_OPTIONS        _AFD_CONTROL_CODE(AFD_SIZE_CONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_SIZE_DISCONNECT_DATA        _AFD_CONTROL_CODE(AFD_SIZE_DISCONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_SIZE_DISCONNECT_OPTIONS     _AFD_CONTROL_CODE(AFD_SIZE_DISCONNECT_OPTIONS, METHOD_NEITHER)
#define IOCTL_AFD_GET_INFORMATION             _AFD_CONTROL_CODE(AFD_GET_INFORMATION, METHOD_NEITHER)
#define IOCTL_AFD_TRANSMIT_FILE               _AFD_CONTROL_CODE(AFD_TRANSMIT_FILE, METHOD_NEITHER)
#define IOCTL_AFD_SUPER_ACCEPT                _AFD_CONTROL_CODE(AFD_SUPER_ACCEPT, METHOD_NEITHER)
#define IOCTL_AFD_EVENT_SELECT                _AFD_CONTROL_CODE(AFD_EVENT_SELECT, METHOD_NEITHER)
#define IOCTL_AFD_ENUM_NETWORK_EVENTS         _AFD_CONTROL_CODE(AFD_ENUM_NETWORK_EVENTS, METHOD_NEITHER)
#define IOCTL_AFD_DEFER_ACCEPT                _AFD_CONTROL_CODE(AFD_DEFER_ACCEPT, METHOD_BUFFERED)
#define IOCTL_AFD_WAIT_FOR_LISTEN_LIFO        _AFD_CONTROL_CODE(AFD_WAIT_FOR_LISTEN_LIFO, METHOD_BUFFERED)
#define IOCTL_AFD_SET_QOS                     _AFD_CONTROL_CODE(AFD_SET_QOS, METHOD_BUFFERED)
#define IOCTL_AFD_GET_QOS                     _AFD_CONTROL_CODE(AFD_GET_QOS, METHOD_BUFFERED)
#define IOCTL_AFD_NO_OPERATION                _AFD_CONTROL_CODE(AFD_NO_OPERATION, METHOD_NEITHER)
#define IOCTL_AFD_VALIDATE_GROUP              _AFD_CONTROL_CODE(AFD_VALIDATE_GROUP, METHOD_BUFFERED)
#define IOCTL_AFD_GET_UNACCEPTED_CONNECT_DATA _AFD_CONTROL_CODE(AFD_GET_UNACCEPTED_CONNECT_DATA, METHOD_NEITHER)
#define IOCTL_AFD_ROUTING_INTERFACE_QUERY     _AFD_CONTROL_CODE(AFD_ROUTING_INTERFACE_QUERY, METHOD_NEITHER)
#define IOCTL_AFD_ROUTING_INTERFACE_CHANGE    _AFD_CONTROL_CODE(AFD_ROUTING_INTERFACE_CHANGE, METHOD_BUFFERED)
#define IOCTL_AFD_ADDRESS_LIST_QUERY          _AFD_CONTROL_CODE(AFD_ADDRESS_LIST_QUERY, METHOD_NEITHER)
#define IOCTL_AFD_ADDRESS_LIST_CHANGE         _AFD_CONTROL_CODE(AFD_ADDRESS_LIST_CHANGE, METHOD_BUFFERED)
#define IOCTL_AFD_JOIN_LEAF                   _AFD_CONTROL_CODE(AFD_JOIN_LEAF, METHOD_NEITHER)
#define IOCTL_AFD_TRANSPORT_IOCTL             _AFD_CONTROL_CODE(AFD_TRANSPORT_IOCTL, METHOD_NEITHER)
#define IOCTL_AFD_TRANSMIT_PACKETS            _AFD_CONTROL_CODE(AFD_TRANSMIT_PACKETS, METHOD_NEITHER)
#define IOCTL_AFD_SUPER_CONNECT               _AFD_CONTROL_CODE(AFD_SUPER_CONNECT, METHOD_NEITHER)
#define IOCTL_AFD_SUPER_DISCONNECT            _AFD_CONTROL_CODE(AFD_SUPER_DISCONNECT, METHOD_NEITHER)
#define IOCTL_AFD_RECEIVE_MESSAGE             _AFD_CONTROL_CODE(AFD_RECEIVE_MESSAGE, METHOD_NEITHER)
#define IOCTL_AFD_SEND_MESSAGE                _AFD_CONTROL_CODE(AFD_SEND_MESSAGE, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_CEMENT_SAN           _AFD_CONTROL_CODE(AFD_SWITCH_CEMENT_SAN, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_SET_EVENTS           _AFD_CONTROL_CODE(AFD_SWITCH_SET_EVENTS, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_RESET_EVENTS         _AFD_CONTROL_CODE(AFD_SWITCH_RESET_EVENTS, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_CONNECT_IND          _AFD_CONTROL_CODE(AFD_SWITCH_CONNECT_IND, METHOD_OUT_DIRECT)
#define IOCTL_AFD_SWITCH_CMPL_ACCEPT          _AFD_CONTROL_CODE(AFD_SWITCH_CMPL_ACCEPT, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_CMPL_REQUEST         _AFD_CONTROL_CODE(AFD_SWITCH_CMPL_REQUEST, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_CMPL_IO              _AFD_CONTROL_CODE(AFD_SWITCH_CMPL_IO, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_REFRESH_ENDP         _AFD_CONTROL_CODE(AFD_SWITCH_REFRESH_ENDP, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_GET_PHYSICAL_ADDR    _AFD_CONTROL_CODE(AFD_SWITCH_GET_PHYSICAL_ADDR, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_ACQUIRE_CTX          _AFD_CONTROL_CODE(AFD_SWITCH_ACQUIRE_CTX, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_TRANSFER_CTX         _AFD_CONTROL_CODE(AFD_SWITCH_TRANSFER_CTX, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_GET_SERVICE_PID      _AFD_CONTROL_CODE(AFD_SWITCH_GET_SERVICE_PID, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_SET_SERVICE_PROCESS  _AFD_CONTROL_CODE(AFD_SWITCH_SET_SERVICE_PROCESS, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_PROVIDER_CHANGE      _AFD_CONTROL_CODE(AFD_SWITCH_PROVIDER_CHANGE, METHOD_NEITHER)
#define IOCTL_AFD_SWITCH_ADDRLIST_CHANGE      _AFD_CONTROL_CODE(AFD_SWITCH_ADDRLIST_CHANGE, METHOD_BUFFERED)
#define IOCTL_AFD_UNBIND                      _AFD_CONTROL_CODE(AFD_UNBIND, METHOD_NEITHER)
#define IOCTL_AFD_SQM                         _AFD_CONTROL_CODE(AFD_SQM, METHOD_NEITHER)
#define IOCTL_AFD_RIO                         _AFD_CONTROL_CODE(AFD_RIO, METHOD_NEITHER)
#define IOCTL_AFD_TRANSFER_BEGIN              _AFD_CONTROL_CODE(AFD_TRANSFER_BEGIN, METHOD_NEITHER)
#define IOCTL_AFD_TRANSFER_END                _AFD_CONTROL_CODE(AFD_TRANSFER_END, METHOD_NEITHER)
#define IOCTL_AFD_NOTIFY                      _AFD_CONTROL_CODE(AFD_NOTIFY, METHOD_NEITHER)

#include <pshpack1.h>

typedef union _AFD_ADDRESS
{
    SOCKADDR_STORAGE TliAddress;
    TDI_ADDRESS_INFO TdiAddress;

    struct
    {

        UCHAR Padding[10];
        SOCKADDR_STORAGE EmbeddedAddress;
    } TdiAddressUnpacked;
} AFD_ADDRESS, *PAFD_ADDRESS;

#include <poppack.h>

#define AFD_NORMALADDRUSE    0
#define AFD_REUSEADDRESS     1
#define AFD_WILDCARDADDRESS  2
#define AFD_EXCLUSIVEADDRUSE 3

typedef struct _AFD_BIND_INFO
{
    ULONG ShareAccess;
    TRANSPORT_ADDRESS Address;
} AFD_BIND_INFO, *PAFD_BIND_INFO;

typedef struct _AFD_BIND_INFO_TL
{
    ULONG ShareAccess;
    SOCKADDR Address;
} AFD_BIND_INFO_TL, *PAFD_BIND_INFO_TL;

typedef struct _AFD_CONNECT_JOIN_INFO
{
    BOOLEAN SanActive;
    HANDLE RootEndpoint;
    HANDLE ConnectEndpoint;
    TRANSPORT_ADDRESS RemoteAddress;
} AFD_CONNECT_JOIN_INFO, *PAFD_CONNECT_JOIN_INFO;

typedef struct _AFD_CONNECT_JOIN_INFO_TL
{
    BOOLEAN SanActive;
    HANDLE RootEndpoint;
    HANDLE ConnectEndpoint;
    SOCKADDR RemoteAddress;
} AFD_CONNECT_JOIN_INFO_TL, *PAFD_CONNECT_JOIN_INFO_TL;

typedef struct _AFD_LISTEN_INFO
{
    BOOLEAN SanActive;
    ULONG MaximumConnectionQueue;
    BOOLEAN UseDelayedAcceptance;
} AFD_LISTEN_INFO, *PAFD_LISTEN_INFO;

typedef struct _AFD_LISTEN_RESPONSE_INFO
{
    LONG Sequence;
    TRANSPORT_ADDRESS RemoteAddress;
} AFD_LISTEN_RESPONSE_INFO, *PAFD_LISTEN_RESPONSE_INFO;

typedef struct _AFD_LISTEN_RESPONSE_INFO_TL
{
    LONG Sequence;
    SOCKADDR RemoteAddress;
} AFD_LISTEN_RESPONSE_INFO_TL, *PAFD_LISTEN_RESPONSE_INFO_TL;

typedef struct _AFD_ACCEPT_INFO
{
    BOOLEAN SanActive;
    LONG Sequence;
    HANDLE AcceptHandle;
} AFD_ACCEPT_INFO, *PAFD_ACCEPT_INFO;

#define AFD_NO_FAST_IO 0x0001
#define AFD_OVERLAPPED 0x0002

typedef struct _AFD_RECV_INFO
{
    _Field_size_(BufferCount) LPWSABUF BufferArray;
    ULONG BufferCount;
    ULONG AfdFlags;
    ULONG TdiFlags;
} AFD_RECV_INFO, *PAFD_RECV_INFO;

typedef struct _AFD_DATAGRAM_INFO
{
    _Field_size_(BufferCount) LPWSABUF BufferArray;
    ULONG BufferCount;
    ULONG AfdFlags;
    ULONG TdiFlags;
    PVOID Address;
    PULONG AddressLength;
} AFD_DATAGRAM_INFO, *PAFD_DATAGRAM_INFO;

typedef struct _AFD_SEND_INFO
{
    _Field_size_(BufferCount) LPWSABUF BufferArray;
    ULONG BufferCount;
    ULONG AfdFlags;
    ULONG TdiFlags;
} AFD_SEND_INFO, *PAFD_SEND_INFO;

typedef struct _AFD_SEND_DATAGRAM_INFO
{
    _Field_size_(BufferCount) LPWSABUF BufferArray;
    ULONG BufferCount;
    ULONG AfdFlags;
    TDI_REQUEST_SEND_DATAGRAM TdiRequest;
    TDI_CONNECTION_INFORMATION TdiConnInfo;
} AFD_SEND_DATAGRAM_INFO, *PAFD_SEND_DATAGRAM_INFO;

#define AFD_POLL_RECEIVE_BIT             0
#define AFD_POLL_RECEIVE_EXPEDITED_BIT   1
#define AFD_POLL_SEND_BIT                2
#define AFD_POLL_DISCONNECT_BIT          3
#define AFD_POLL_ABORT_BIT               4
#define AFD_POLL_LOCAL_CLOSE_BIT         5
#define AFD_POLL_CONNECT_BIT             6
#define AFD_POLL_ACCEPT_BIT              7
#define AFD_POLL_CONNECT_FAIL_BIT        8
#define AFD_POLL_QOS_BIT                 9
#define AFD_POLL_GROUP_QOS_BIT           10
#define AFD_POLL_ROUTING_IF_CHANGE_BIT   11
#define AFD_POLL_ADDRESS_LIST_CHANGE_BIT 12
#define AFD_NUM_POLL_EVENTS              13

#define AFD_POLL_RECEIVE             (1 << AFD_POLL_RECEIVE_BIT)
#define AFD_POLL_RECEIVE_EXPEDITED   (1 << AFD_POLL_RECEIVE_EXPEDITED_BIT)
#define AFD_POLL_SEND                (1 << AFD_POLL_SEND_BIT)
#define AFD_POLL_DISCONNECT          (1 << AFD_POLL_DISCONNECT_BIT)
#define AFD_POLL_ABORT               (1 << AFD_POLL_ABORT_BIT)
#define AFD_POLL_LOCAL_CLOSE         (1 << AFD_POLL_LOCAL_CLOSE_BIT)
#define AFD_POLL_CONNECT             (1 << AFD_POLL_CONNECT_BIT)
#define AFD_POLL_ACCEPT              (1 << AFD_POLL_ACCEPT_BIT)
#define AFD_POLL_CONNECT_FAIL        (1 << AFD_POLL_CONNECT_FAIL_BIT)
#define AFD_POLL_QOS                 (1 << AFD_POLL_QOS_BIT)
#define AFD_POLL_GROUP_QOS           (1 << AFD_POLL_GROUP_QOS_BIT)
#define AFD_POLL_ROUTING_IF_CHANGE   (1 << AFD_POLL_ROUTING_IF_CHANGE_BIT)
#define AFD_POLL_ADDRESS_LIST_CHANGE (1 << AFD_POLL_ADDRESS_LIST_CHANGE_BIT)
#define AFD_POLL_ALL                 ((1 << AFD_NUM_POLL_EVENTS) - 1)
#define AFD_POLL_SANCOUNTS_UPDATED   0x80000000

typedef struct _AFD_POLL_HANDLE_INFO
{
    HANDLE Handle;
    ULONG PollEvents;
    NTSTATUS Status;
} AFD_POLL_HANDLE_INFO, *PAFD_POLL_HANDLE_INFO;

typedef struct _AFD_POLL_INFO
{
    LARGE_INTEGER Timeout;
    ULONG NumberOfHandles;
    BOOLEAN Unique;
    _Field_size_(NumberOfHandles) AFD_POLL_HANDLE_INFO Handles[ANYSIZE_ARRAY];
} AFD_POLL_INFO, *PAFD_POLL_INFO;

#define AFD_PARTIAL_DISCONNECT_SEND    0x01
#define AFD_PARTIAL_DISCONNECT_RECEIVE 0x02
#define AFD_ABORTIVE_DISCONNECT        0x04
#define AFD_UNCONNECT_DATAGRAM         0x08

typedef struct _AFD_PARTIAL_DISCONNECT_INFO
{
    ULONG DisconnectMode;
    LARGE_INTEGER Timeout;
} AFD_PARTIAL_DISCONNECT_INFO, *PAFD_PARTIAL_DISCONNECT_INFO;

typedef struct _AFD_RECEIVE_INFORMATION
{
    ULONG BytesAvailable;
    ULONG ExpeditedBytesAvailable;
} AFD_RECEIVE_INFORMATION, *PAFD_RECEIVE_INFORMATION;

#define AFD_QUERY_ADDRESS_HANDLE    0x01
#define AFD_QUERY_CONNECTION_HANDLE 0x02

typedef struct _AFD_HANDLE_INFO
{
    HANDLE TdiAddressHandle;
    HANDLE TdiConnectionHandle;
} AFD_HANDLE_INFO, *PAFD_HANDLE_INFO;

#define AFD_INLINE_MODE                1
#define AFD_NONBLOCKING_MODE           2
#define AFD_MAX_SEND_SIZE              3
#define AFD_SENDS_PENDING              4
#define AFD_MAX_PATH_SEND_SIZE         5
#define AFD_RECEIVE_WINDOW_SIZE        6
#define AFD_SEND_WINDOW_SIZE           7
#define AFD_CONNECT_TIME               8
#define AFD_CIRCULAR_QUEUEING          9
#define AFD_GROUP_ID_AND_TYPE          10
#define AFD_REPORT_PORT_UNREACHABLE    11
#define AFD_REPORT_NETWORK_UNREACHABLE 12
#define AFD_DELIVERY_STATUS            14
#define AFD_CANCEL_TL                  15

typedef enum _AFD_GROUP_TYPE
{
    GroupTypeNeither = 0,
    GroupTypeUnconstrained = SG_UNCONSTRAINED_GROUP,
    GroupTypeConstrained = SG_CONSTRAINED_GROUP,
} AFD_GROUP_TYPE, *PAFD_GROUP_TYPE;

typedef struct _AFD_GROUP_INFO
{
    GROUP GroupID;
    AFD_GROUP_TYPE GroupType;
} AFD_GROUP_INFO, *PAFD_GROUP_INFO;

typedef struct _SIO_DELIVERY_STATUS
{
    BOOLEAN DeliveryAvailable;
    ULONG PendedReceiveRequests;
} SIO_DELIVERY_STATUS, *PSIO_DELIVERY_STATUS;

typedef struct _AFD_INFORMATION
{
    ULONG InformationType;
    union
    {
        BOOLEAN Boolean;
        ULONG Ulong;
        LARGE_INTEGER LargeInteger;
        AFD_GROUP_INFO GroupInfo;
        SIO_DELIVERY_STATUS DeliveryStatus;
    } Information;
} AFD_INFORMATION, *PAFD_INFORMATION;

typedef enum _SOCKET_STATE
{
  SocketStateInitializing = -1,
  SocketStateOpen = 0,
  SocketStateBound = 1,
  SocketStateBoundSpecific = 2,
  SocketStateConnected = 3,
  SocketStateClosing = 4,
} SOCKET_STATE, *PSOCKET_STATE;

typedef struct _SOCK_SHARED_INFO
{
    SOCKET_STATE State;
    LONG AddressFamily;
    LONG SocketType;
    LONG Protocol;
    LONG LocalAddressLength;
    LONG RemoteAddressLength;
    LINGER LingerInfo;
    ULONG SendTimeout;
    ULONG ReceiveTimeout;
    ULONG ReceiveBufferSize;
    ULONG SendBufferSize;
    union
    {
        USHORT Flags;
        struct
        {
            USHORT Listening : 1;
            USHORT Broadcast : 1;
            USHORT Debug : 1;
            USHORT OobInline : 1;
            USHORT ReuseAddresses : 1;
            USHORT ExclusiveAddressUse : 1;
            USHORT NonBlocking : 1;
            USHORT DontUseWildcard : 1;
            USHORT ReceiveShutdown : 1;
            USHORT SendShutdown : 1;
            USHORT ConditionalAccept : 1;
            USHORT IsSANSocket : 1;
            USHORT fIsTLI : 1;
            USHORT Rio : 1;
            USHORT ReceiveBufferSizeSet : 1;
            USHORT SendBufferSizeSet : 1;
        };
    };
    ULONG CreationFlags;
    ULONG CatalogEntryId;
    ULONG ServiceFlags1;
    ULONG ProviderFlags;
    GROUP GroupID;
    AFD_GROUP_TYPE GroupType;
    LONG GroupPriority;
    LONG LastError;
    union
    {
        HWND AsyncSelecthWnd;
        ULONGLONG AsyncSelectWnd64;
    };
    ULONG AsyncSelectSerialNumber;
    ULONG AsyncSelectwMsg;
    LONG AsyncSelectlEvent;
    LONG DisabledAsyncSelectEvents;
    GUID ProviderId;
} SOCK_SHARED_INFO, *PSOCK_SHARED_INFO;

typedef struct _AFD_UNACCEPTED_CONNECT_DATA_INFO
{
    LONG Sequence;
    ULONG ConnectDataLength;
    BOOLEAN LengthOnly;
} AFD_UNACCEPTED_CONNECT_DATA_INFO, *PAFD_UNACCEPTED_CONNECT_DATA_INFO;

#define AFD_TF_DISCONNECT         0x01
#define AFD_TF_REUSE_SOCKET       0x02
#define AFD_TF_WRITE_BEHIND       0x04
#define AFD_TF_USE_DEFAULT_WORKER 0x00
#define AFD_TF_USE_SYSTEM_THREAD  0x10
#define AFD_TF_USE_KERNEL_APC     0x20
#define AFD_TF_WORKER_KIND_MASK   0x30

typedef struct _AFD_TRANSMIT_FILE_INFO
{
    LARGE_INTEGER Offset;
    LARGE_INTEGER WriteLength;
    ULONG SendPacketLength;
    HANDLE FileHandle;
    PVOID Head;
    ULONG HeadLength;
    PVOID Tail;
    ULONG TailLength;
    ULONG Flags;
} AFD_TRANSMIT_FILE_INFO, *PAFD_TRANSMIT_FILE_INFO;

typedef struct _AFD_SUPER_ACCEPT_INFO
{
    BOOLEAN SanActive;
    BOOLEAN FixAddressAlignment;
    HANDLE AcceptHandle;
    ULONG ReceiveDataLength;
    ULONG LocalAddressLength;
    ULONG RemoteAddressLength;
} AFD_SUPER_ACCEPT_INFO, *PAFD_SUPER_ACCEPT_INFO;

typedef struct _AFD_EVENT_SELECT_INFO
{
    HANDLE Event;
    ULONG PollEvents;
} AFD_EVENT_SELECT_INFO, *PAFD_EVENT_SELECT_INFO;

typedef struct _AFD_ENUM_NETWORK_EVENTS_INFO
{
    ULONG PollEvents;
    NTSTATUS EventStatus[AFD_NUM_POLL_EVENTS];
} AFD_ENUM_NETWORK_EVENTS_INFO, *PAFD_ENUM_NETWORK_EVENTS_INFO;

typedef struct _AFD_DEFER_ACCEPT_INFO
{
    LONG Sequence;
    BOOLEAN Reject;
} AFD_DEFER_ACCEPT_INFO, *PAFD_DEFER_ACCEPT_INFO;

typedef struct _AFD_QOS_INFO
{
    QOS Qos;
    BOOLEAN GroupQos;
} AFD_QOS_INFO, *PAFD_QOS_INFO;

typedef struct _AFD_VALIDATE_GROUP_INFO
{
    GROUP GroupID;
    TRANSPORT_ADDRESS RemoteAddress;
} AFD_VALIDATE_GROUP_INFO, *PAFD_VALIDATE_GROUP_INFO;

typedef struct _AFD_TRANSPORT_IOCTL_INFO
{
    HANDLE Handle;
    _Field_size_bytes_(InputBufferLength) PVOID InputBuffer;
    ULONG InputBufferLength;
    ULONG IoControlCode;
    ULONG AfdFlags;
    ULONG PollEvent;
} AFD_TRANSPORT_IOCTL_INFO, *PAFD_TRANSPORT_IOCTL_INFO;

#define HVSOCKET_CONTAINER_PASSTHRU     0x02

typedef enum TL_IO_CONTROL_TYPE
{
    TlEndpointIoControlType = 0,
    TlSetSockOptIoControlType = 1,
    TlGetSockOptIoControlType = 2,
    TlSocketIoControlType = 3,
} TL_IO_CONTROL_TYPE, *PTL_IO_CONTROL_TYPE;

typedef struct _AFD_TL_IO_CONTROL_INFO
{
    TL_IO_CONTROL_TYPE Type;
    ULONG Level;
    ULONG IoControlCode;
    BOOLEAN EndpointIoctl;
    _Field_size_bytes_(InputBufferLength) PVOID InputBuffer;
    SIZE_T InputBufferLength;
} AFD_TL_IO_CONTROL_INFO, *PAFD_TL_IO_CONTROL_INFO;

typedef struct _AFD_TPACKETS_INFO
{
    _Field_size_(ElementCount) PTRANSMIT_PACKETS_ELEMENT ElementArray;
    ULONG ElementCount;
    ULONG SendSize;
    ULONG Flags;
} AFD_TPACKETS_INFO, *PAFD_TPACKETS_INFO;

typedef struct _AFD_SUPER_CONNECT_INFO
{
    BOOLEAN SanActive;
    TRANSPORT_ADDRESS RemoteAddress;
} AFD_SUPER_CONNECT_INFO, *PAFD_SUPER_CONNECT_INFO;

typedef struct _AFD_SUPER_CONNECT_INFO_TL
{
    BOOLEAN SanActive;
    SOCKADDR RemoteAddress;
} AFD_SUPER_CONNECT_INFO_TL, *PAFD_SUPER_CONNECT_INFO_TL;

typedef struct _AFD_SUPER_DISCONNECT_INFO
{
    ULONG Flags;
} AFD_SUPER_DISCONNECT_INFO, *PAFD_SUPER_DISCONNECT_INFO;

typedef struct _AFD_MESSAGE_INFO
{
    AFD_DATAGRAM_INFO dgi;
    _Field_size_bytes_(ControlLength) PVOID ControlBuffer;
    PULONG ControlLength;
    PULONG MsgFlags;
} AFD_MESSAGE_INFO, *PAFD_MESSAGE_INFO;

typedef struct _AFD_SWITCH_CONTEXT
{
    LONG EventsActive;
    LONG RcvCount;
    LONG ExpCount;
    LONG SndCount;
    BOOLEAN SelectFlag;
} AFD_SWITCH_CONTEXT, *PAFD_SWITCH_CONTEXT;

typedef struct _AFD_SWITCH_CONTEXT_INFO
{
    HANDLE SocketHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
} AFD_SWITCH_CONTEXT_INFO, *PAFD_SWITCH_CONTEXT_INFO;

typedef struct _AFD_SWITCH_EVENT_INFO
{
    HANDLE SocketHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
    ULONG EventBit;
    NTSTATUS Status;
} AFD_SWITCH_EVENT_INFO, *PAFD_SWITCH_EVENT_INFO;

typedef struct _AFD_SWITCH_CONNECT_INFO
{
    HANDLE ListenHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
    TRANSPORT_ADDRESS RemoteAddress;
} AFD_SWITCH_CONNECT_INFO, *PAFD_SWITCH_CONNECT_INFO;

typedef struct _AFD_SWITCH_ACCEPT_INFO
{
    HANDLE AcceptHandle;
    ULONG ReceiveLength;
} AFD_SWITCH_ACCEPT_INFO, *PAFD_SWITCH_ACCEPT_INFO;

typedef struct _AFD_SWITCH_REQUEST_INFO
{
    HANDLE SocketHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
    PVOID RequestContext;
    NTSTATUS RequestStatus;
    ULONG DataOffset;
} AFD_SWITCH_REQUEST_INFO, *PAFD_SWITCH_REQUEST_INFO;

typedef struct _AFD_SWITCH_ACQUIRE_CTX_INFO
{
    HANDLE SocketHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
    PVOID SocketCtxBuf;
    ULONG SocketCtxBufSize;
} AFD_SWITCH_ACQUIRE_CTX_INFO, *PAFD_SWITCH_ACQUIRE_CTX_INFO;

typedef struct _AFD_SWITCH_TRANSFER_CTX_INFO
{
    HANDLE SocketHandle;
    PAFD_SWITCH_CONTEXT SwitchContext;
    PVOID RequestContext;
    PVOID SocketCtxBuf;
    ULONG SocketCtxBufSize;
    _Field_size_(RcvBufferCount) LPWSABUF RcvBufferArray;
    ULONG RcvBufferCount;
    NTSTATUS Status;
} AFD_SWITCH_TRANSFER_CTX_INFO, *PAFD_SWITCH_TRANSFER_CTX_INFO;

typedef struct _AFD_UNBIND_INFO
{
    LONG AddressFamily;
    LONG Protocol;
} AFD_UNBIND_INFO, *PAFD_UNBIND_INFO;

#define AFD_SQM_CONTROL_CODE_STORE 1
#define AFD_SQM_CONTROL_CODE_SWEEP -1

typedef struct _AFD_SQM_CONTROL
{
    ULONG Code;
} AFD_SQM_CONTROL, *PAFD_SQM_CONTROL;

typedef struct _WINSOCK_SQM_SOCKTYPE_DESC
{
    ULONG StreamTCP : 1;
    ULONG StreamOther : 1;
    ULONG DatagramUDP : 1;
    ULONG DatagramUDPMulticast : 1;
    ULONG DatagramOther : 1;
    ULONG RawTCP : 1;
    ULONG RawUDP : 1;
    ULONG RawOther : 1;
    ULONG OtherSocketTypes : 1;
    ULONG IPv6Socket : 1;
    ULONG ProviderLoadedWSD : 1;
    ULONG ProviderLoadedLSP : 1;
    ULONG ProviderLoadedMultiLSP : 1;
    ULONG NonMswsockBSP : 1;
} WINSOCK_SQM_SOCKTYPE_DESC, *PWINSOCK_SQM_SOCKTYPE_DESC;

typedef struct _WINSOCK_SQM_SOCKTYPE_COUNTS
{
    ULONG MaxStreamSocketsConn;
    ULONG MaxStreamSocketsListen;
    ULONG MaxDatagramSockets;
    ULONG MaxRawSockets;
    ULONG MaxOtherSockets;
} WINSOCK_SQM_SOCKTYPE_COUNTS, *PWINSOCK_SQM_SOCKTYPE_COUNTS;

typedef struct _WINSOCK_SQM_NONCORE_FUNC
{
    ULONG Select : 1;
    ULONG WSAPoll : 1;
    ULONG StreamWSAEventSelect : 1;
    ULONG StreamWSAAsyncSelect : 1;
    ULONG TransmitFile : 1;
    ULONG StreamTransmitPackets : 1;
    ULONG DisconnectEx : 1;
    ULONG SocketReuse : 1;
    ULONG StreamDuplicateSocket : 1;
    ULONG ReadWriteFile : 1;
    ULONG OOBSendRecv : 1;
    ULONG StreamSoSndTimeO : 1;
    ULONG StreamSoRcvTimeO : 1;
    ULONG MsgWaitAll : 1;
    ULONG StreamNonCoreSOLOptions : 1;
    ULONG StreamNonCoreTCPOptions : 1;
    ULONG StreamNonCoreIPOptions : 1;
    ULONG StreamNonCoreOtherOptions : 1;
    ULONG StreamNonCoreIOCTLs : 1;
    ULONG StreamNonCoreWSASocket : 1;
    ULONG NonCoreRecv : 1;
    ULONG NonCoreSend : 1;
    ULONG StreamNonCoreWSAIoctl : 1;
    ULONG NonCoreWSAAccept : 1;
    ULONG StreamNonCoreWSAConnect : 1;
    ULONG StreamWSAJoinLeaf : 1;
    ULONG StreamWSAGetQoSByName : 1;
    ULONG WSASendDisconnect : 1;
    ULONG WSARecvDisconnect : 1;
} WINSOCK_SQM_NONCORE_FUNC, *PWINSOCK_SQM_NONCORE_FUNC;

typedef struct _WINSOCK_SQM_DEPRECATION_LIST
{
    ULONG bEnumProtocols : 1;
    ULONG bGetNameByType : 1;
    ULONG bGetService : 1;
    ULONG bGetTypeByName : 1;
    ULONG bSetService : 1;
    ULONG bWSACancelBlockingCall : 1;
    ULONG bWSAIsBlocking : 1;
    ULONG bWSASetBlockingHook : 1;
    ULONG bWSAUnhookBlockingHook : 1;
    ULONG bGetAddressByName : 1;
    ULONG bGetHostByAddr : 1;
    ULONG bGetHostByName : 1;
    ULONG bGetHostName : 1;
    ULONG bGetProtoByName : 1;
    ULONG bGetProtoByNumber : 1;
    ULONG bGetServByName : 1;
    ULONG bGetServByPort : 1;
    ULONG bInetAddr : 1;
    ULONG bInetNtoa : 1;
    ULONG bWSAAsyncGetHostByAddr : 1;
    ULONG bWSAAsyncGetHostByName : 1;
    ULONG bWSAAsyncGetProtoByName : 1;
    ULONG bWSAAsyncGetProtoByNumber : 1;
    ULONG bWSAAsyncGetServByName : 1;
    ULONG bWSAAsyncGetServByPort : 1;
    ULONG bWSACancelAsyncRequest : 1;
    ULONG bWSARecvEx : 1;
} WINSOCK_SQM_DEPRECATION_LIST, *PWINSOCK_SQM_DEPRECATION_LIST;

typedef struct _WINSOCK_SQM_SOCK_OPTIONS
{
    ULONG SoSndBuf : 1;
    ULONG SoRcvBuf : 1;
    ULONG SoSndBuf0 : 1;
    ULONG SoRcvBuf0 : 1;
    ULONG SoKeepAliveOn : 1;
    ULONG SoReuseAddrOn : 1;
    ULONG SoExclusiveAddrUseOn : 1;
    ULONG SoLingerOn : 1;
    ULONG SoLingerOn0 : 1;
    ULONG SoRandomizePortOn : 1;
    ULONG SoPortScaleOn : 1;
    ULONG SoCondAcceptOn : 1;
    ULONG SoOobInlineOn : 1;
    ULONG SoOther : 1;
    ULONG TCPNoDelayOn : 1;
    ULONG TCPOther : 1;
    ULONG IPV6V6OnlyOff : 1;
    ULONG IPV6ProtectLevel : 1;
    ULONG IPOther : 1;
    ULONG LevelOther : 1;
    ULONG ISBQuery : 1;
    ULONG ISBNotify : 1;
    ULONG AddressQuery : 1;
    ULONG AddressNotify : 1;
    ULONG RouteQuery : 1;
    ULONG RouteNotify : 1;
    ULONG FionbioOn : 1;
    ULONG Fionread : 1;
    ULONG Siocatmark : 1;
    ULONG Siokeepalivevals : 1;
    ULONG Siosetcompatmode : 1;
    ULONG Sioportreserve : 1;
} WINSOCK_SQM_SOCK_OPTIONS, *PWINSOCK_SQM_SOCK_OPTIONS;

typedef struct _WINSOCK_SQM_MISC_BEHAVIOR
{
    ULONG SendEverPended : 1;
    ULONG RecvEverPended : 1;
    ULONG ShutdownSend : 1;
    ULONG ShutdownRecv : 1;
    ULONG WSASocketProtocol : 1;
} WINSOCK_SQM_MISC_BEHAVIOR, *PWINSOCK_SQM_MISC_BEHAVIOR;

typedef struct _AFD_SQM_INFO
{
    AFD_SQM_CONTROL Control;
    WCHAR AppName[16];
    WCHAR LSPName[16];
    ULONG AppVersion;
    WINSOCK_SQM_SOCKTYPE_DESC SocketTypeDescriptor;
    WINSOCK_SQM_SOCKTYPE_COUNTS SocketTypeCounts;
    WINSOCK_SQM_NONCORE_FUNC NonCoreFunctions;
    WINSOCK_SQM_DEPRECATION_LIST DeprecationList;
    WINSOCK_SQM_SOCK_OPTIONS SocketOptions;
    WINSOCK_SQM_MISC_BEHAVIOR MiscBehavior;
} AFD_SQM_INFO, *PAFD_SQM_INFO;

typedef enum _AFD_RIO_COMMAND
{
    AfdRioCommandIdCreateCq = 0,
    AfdRioCommandIdDestroyCq = 1,
    AfdRioCommandIdNotifyCq = 2,
    AfdRioCommandIdCreateRqPair = 3,
    AfdRioCommandIdRegisterBuffer = 4,
    AfdRioCommandIdDeregisterBuffer = 5,
    AfdRioCommandIdPokeSend = 6,
    AfdRioCommandIdPokeReceive = 7,
    AfdRioCommandIdResizeCq = 8,
    AfdRioCommandIdResizeRqPair = 9,
    AfdRioCommandIdMaximum = 10,
} AFD_RIO_COMMAND, *PAFD_RIO_COMMAND;

typedef struct _AFD_RIO_COMMAND_HEADER
{
    AFD_RIO_COMMAND Command;
} AFD_RIO_COMMAND_HEADER, *PAFD_RIO_COMMAND_HEADER;

typedef enum _AFD_RIO_NOTIFICATION_COMPLETION_TYPE
{
    AfdRioNoCompletion = 0,
    AfdRioEventCompletion = 1,
    AfdRioIocpCompletion = 2,
} AFD_RIO_NOTIFICATION_COMPLETION_TYPE, *PAFD_RIO_NOTIFICATION_COMPLETION_TYPE;

typedef struct _AFD_RIO_COMPLETION_QUEUE
{
    ULONG QHead;
    ULONG QTail;
    BOOLEAN Corrupted;
    RIORESULT QEntry[ANYSIZE_ARRAY];
} AFD_RIO_COMPLETION_QUEUE, *PAFD_RIO_COMPLETION_QUEUE;

typedef struct _AFD_RIO_COMMAND_CREATE_CQ
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG QSize;
    AFD_RIO_NOTIFICATION_COMPLETION_TYPE NotificationType;
    ULONGLONG NotificationHandle;
    ULONGLONG NotificationContext;
    ULONGLONG NotificationContext2;
    ULONG BufferSize;
    ULONGLONG Buffer;
} AFD_RIO_COMMAND_CREATE_CQ, *PAFD_RIO_COMMAND_CREATE_CQ;

typedef struct _AFD_RIO_COMMAND_CREATE_CQ_RESULT
{
    ULONG CqId;
} AFD_RIO_COMMAND_CREATE_CQ_RESULT, *PAFD_RIO_COMMAND_CREATE_CQ_RESULT;

typedef struct _AFD_RIO_COMMAND_DESTROY_CQ
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG CqId;
} AFD_RIO_COMMAND_DESTROY_CQ, *PAFD_RIO_COMMAND_DESTROY_CQ;

typedef struct _AFD_RIO_COMMAND_NOTIFY_CQ
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG Index;
} AFD_RIO_COMMAND_NOTIFY_CQ, *PAFD_RIO_COMMAND_NOTIFY_CQ;

typedef struct _AFD_RIO_BUF
{
    ULONG BufferId;
    ULONG Offset;
    ULONG Length;
} AFD_RIO_BUF, *PAFD_RIO_BUF;

typedef struct _AFD_RIO_REQUEST_QUEUE_ENTRY
{
    AFD_RIO_BUF Data;
    AFD_RIO_BUF SourceAddress;
    AFD_RIO_BUF DestinationAddress;
    AFD_RIO_BUF Control;
    AFD_RIO_BUF FlagsBuffer;
    ULONG Flags;
    ULONGLONG Context;
} AFD_RIO_REQUEST_QUEUE_ENTRY, *PAFD_RIO_REQUEST_QUEUE_ENTRY;

typedef struct _AFD_RIO_REQUEST_QUEUE
{
    ULONG Start;
    ULONG End;
    BOOLEAN PokeRequired;
    AFD_RIO_REQUEST_QUEUE_ENTRY Entries[ANYSIZE_ARRAY];
} AFD_RIO_REQUEST_QUEUE, *PAFD_RIO_REQUEST_QUEUE;

typedef struct _AFD_RIO_COMMAND_CREATE_RQ_PAIR
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG SendCompletionQueue;
    ULONG ReceiveCompletionQueue;
    ULONG SendQueueEntryCount;
    ULONG SendQueueBufferSize;
    ULONGLONG SendQueueBuffer;
    ULONG ReceiveQueueEntryCount;
    ULONG ReceiveQueueBufferSize;
    ULONGLONG ReceiveQueueBuffer;
    ULONGLONG EndpointHandle;
    ULONGLONG SocketContext;
} AFD_RIO_COMMAND_CREATE_RQ_PAIR, *PAFD_RIO_COMMAND_CREATE_RQ_PAIR;

typedef struct _AFD_RIO_COMMAND_REGISTER_BUFFER
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONGLONG Buffer;
    ULONG BufferLength;
} AFD_RIO_COMMAND_REGISTER_BUFFER, *PAFD_RIO_COMMAND_REGISTER_BUFFER;

typedef struct _AFD_RIO_COMMAND_REGISTER_BUFFER_RESULT
{
    ULONG BufferId;
} AFD_RIO_COMMAND_REGISTER_BUFFER_RESULT, *PAFD_RIO_COMMAND_REGISTER_BUFFER_RESULT;

typedef struct _AFD_RIO_COMMAND_DEREGISTER_BUFFER
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG BufferId;
} AFD_RIO_COMMAND_DEREGISTER_BUFFER, *PAFD_RIO_COMMAND_DEREGISTER_BUFFER;

typedef struct _AFD_RIO_COMMAND_POKE_SEND
{
    AFD_RIO_COMMAND_HEADER Header;
} AFD_RIO_COMMAND_POKE_SEND, *PAFD_RIO_COMMAND_POKE_SEND;

typedef struct _AFD_RIO_COMMAND_POKE_RECEIVE
{
    AFD_RIO_COMMAND_HEADER Header;
} AFD_RIO_COMMAND_POKE_RECEIVE, *PAFD_RIO_COMMAND_POKE_RECEIVE;

typedef struct _AFD_RIO_COMMAND_RESIZE_CQ
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG CqId;
    ULONG QSize;
    ULONG BufferSize;
    ULONGLONG Buffer;
} AFD_RIO_COMMAND_RESIZE_CQ, *PAFD_RIO_COMMAND_RESIZE_CQ;

typedef struct _AFD_RIO_COMMAND_RESIZE_RQ_PAIR
{
    AFD_RIO_COMMAND_HEADER Header;
    ULONG SendQueueEntryCount;
    ULONG SendQueueBufferSize;
    ULONGLONG SendQueueBuffer;
    ULONG ReceiveQueueEntryCount;
    ULONG ReceiveQueueBufferSize;
    ULONGLONG ReceiveQueueBuffer;
} AFD_RIO_COMMAND_RESIZE_RQ_PAIR, *PAFD_RIO_COMMAND_RESIZE_RQ_PAIR;

#endif
