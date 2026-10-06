/*
 * File management support
 *
 * This file is part of System Informer.
 */

#ifndef _NTIOAPI_H
#define _NTIOAPI_H


#define FILE_SHARE_NONE                 0x00000000
#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004


#define FILE_SUPERSEDE                      0x00000000
#define FILE_OPEN                           0x00000001
#define FILE_CREATE                         0x00000002
#define FILE_OPEN_IF                        0x00000003
#define FILE_OVERWRITE                      0x00000004
#define FILE_OVERWRITE_IF                   0x00000005
#define FILE_MAXIMUM_DISPOSITION            0x00000005


#define FILE_DIRECTORY_FILE                 0x00000001
#define FILE_WRITE_THROUGH                  0x00000002
#define FILE_SEQUENTIAL_ONLY                0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING      0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT           0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT        0x00000020
#define FILE_NON_DIRECTORY_FILE             0x00000040
#define FILE_CREATE_TREE_CONNECTION         0x00000080

#define FILE_COMPLETE_IF_OPLOCKED           0x00000100
#define FILE_NO_EA_KNOWLEDGE                0x00000200
#define FILE_OPEN_REMOTE_INSTANCE           0x00000400
#define FILE_RANDOM_ACCESS                  0x00000800

#define FILE_DELETE_ON_CLOSE                0x00001000
#define FILE_OPEN_BY_FILE_ID                0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT         0x00004000
#define FILE_NO_COMPRESSION                 0x00008000

#define FILE_OPEN_REQUIRING_OPLOCK          0x00010000
#define FILE_DISALLOW_EXCLUSIVE             0x00020000
#define FILE_SESSION_AWARE                  0x00040000

#define FILE_RESERVE_OPFILTER               0x00100000
#define FILE_OPEN_REPARSE_POINT             0x00200000
#define FILE_OPEN_NO_RECALL                 0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY      0x00800000

#define TREE_CONNECT_WRITE_THROUGH          0x00000002
#define TREE_CONNECT_NO_CLIENT_BUFFERING    0x00000008


#define FILE_CONTAINS_EXTENDED_CREATE_INFORMATION   0x10000000
#define FILE_VALID_EXTENDED_OPTION_FLAGS            0x10000000

typedef struct _EXTENDED_CREATE_DUAL_OPLOCK_KEYS
{
    GUID ParentOplockKey;
    GUID TargetOplockKey;
} EXTENDED_CREATE_DUAL_OPLOCK_KEYS, *PEXTENDED_CREATE_DUAL_OPLOCK_KEYS;

typedef struct _EXTENDED_CREATE_INFORMATION
{
    LONGLONG ExtendedCreateFlags;
    PVOID EaBuffer;
    ULONG EaLength;
} EXTENDED_CREATE_INFORMATION, *PEXTENDED_CREATE_INFORMATION;

typedef struct _EXTENDED_CREATE_INFORMATION_32
{
    LONGLONG ExtendedCreateFlags;
    void* POINTER_32 EaBuffer;
    ULONG EaLength;
} EXTENDED_CREATE_INFORMATION_32, *PEXTENDED_CREATE_INFORMATION_32;

#define EX_CREATE_FLAG_FILE_SOURCE_OPEN_FOR_COPY 0x00000001
#define EX_CREATE_FLAG_FILE_DEST_OPEN_FOR_COPY   0x00000002

#define FILE_VALID_OPTION_FLAGS             0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS        0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS    0x00000032
#define FILE_VALID_SET_FLAGS                0x00000036

#define FILE_COPY_STRUCTURED_STORAGE        0x00000041
#define FILE_STRUCTURED_STORAGE             0x00000441


#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005


#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe


#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff


#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5)

#define MAXIMUM_FILENAME_LENGTH 256


#define FILE_NEED_EA                    0x00000080

#define FILE_EA_TYPE_BINARY             0xfffe
#define FILE_EA_TYPE_ASCII              0xfffd
#define FILE_EA_TYPE_BITMAP             0xfffb
#define FILE_EA_TYPE_METAFILE           0xfffa
#define FILE_EA_TYPE_ICON               0xfff9
#define FILE_EA_TYPE_EA                 0xffee
#define FILE_EA_TYPE_MVMT               0xffdf
#define FILE_EA_TYPE_MVST               0xffde
#define FILE_EA_TYPE_ASN1               0xffdd
#define FILE_EA_TYPE_FAMILY_IDS         0xff01


#define FILE_REMOVABLE_MEDIA                        0x00000001
#define FILE_READ_ONLY_DEVICE                       0x00000002
#define FILE_FLOPPY_DISKETTE                        0x00000004
#define FILE_WRITE_ONCE_MEDIA                       0x00000008
#define FILE_REMOTE_DEVICE                          0x00000010
#define FILE_DEVICE_IS_MOUNTED                      0x00000020
#define FILE_VIRTUAL_VOLUME                         0x00000040
#define FILE_AUTOGENERATED_DEVICE_NAME              0x00000080
#define FILE_DEVICE_SECURE_OPEN                     0x00000100
#define FILE_CHARACTERISTIC_PNP_DEVICE              0x00000800
#define FILE_CHARACTERISTIC_TS_DEVICE               0x00001000
#define FILE_CHARACTERISTIC_WEBDAV_DEVICE           0x00002000
#define FILE_CHARACTERISTIC_CSV                     0x00010000
#define FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL    0x00020000
#define FILE_PORTABLE_DEVICE                        0x00040000
#define FILE_REMOTE_DEVICE_VSMB                     0x00080000
#define FILE_DEVICE_REQUIRE_SECURITY_CHECK          0x00100000


#define FILE_PIPE_BYTE_STREAM_TYPE      0x00000000
#define FILE_PIPE_MESSAGE_TYPE          0x00000001
#define FILE_PIPE_ACCEPT_REMOTE_CLIENTS 0x00000000
#define FILE_PIPE_REJECT_REMOTE_CLIENTS 0x00000002
#define FILE_PIPE_TYPE_VALID_MASK       0x00000003

#define FILE_PIPE_QUEUE_OPERATION       0x00000000
#define FILE_PIPE_COMPLETE_OPERATION    0x00000001

#define FILE_PIPE_BYTE_STREAM_MODE      0x00000000
#define FILE_PIPE_MESSAGE_MODE          0x00000001

#define FILE_PIPE_INBOUND               0x00000000
#define FILE_PIPE_OUTBOUND              0x00000001
#define FILE_PIPE_FULL_DUPLEX           0x00000002

#define FILE_PIPE_DISCONNECTED_STATE    0x00000001
#define FILE_PIPE_LISTENING_STATE       0x00000002
#define FILE_PIPE_CONNECTED_STATE       0x00000003
#define FILE_PIPE_CLOSING_STATE         0x00000004

#define FILE_PIPE_CLIENT_END            0x00000000
#define FILE_PIPE_SERVER_END            0x00000001

#define FILE_PIPE_UNLIMITED_INSTANCES   0xffffffff


#define MAILSLOT_SIZE_AUTO 0

typedef struct _IO_STATUS_BLOCK
{
    union
    {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef _Function_class_(IO_APC_ROUTINE)
VOID NTAPI IO_APC_ROUTINE(
    _In_ PVOID ApcContext,
    _In_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG Reserved
    );
typedef IO_APC_ROUTINE* PIO_APC_ROUTINE;


typedef enum _FILE_INFORMATION_CLASS
{
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileObjectIdInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileQuotaInformation,
    FileReparsePointInformation,
    FileNetworkOpenInformation,
    FileAttributeTagInformation,
    FileTrackingInformation,
    FileIdBothDirectoryInformation,
    FileIdFullDirectoryInformation,
    FileValidDataLengthInformation,
    FileShortNameInformation,
    FileIoCompletionNotificationInformation,
    FileIoStatusBlockRangeInformation,
    FileIoPriorityHintInformation,
    FileSfioReserveInformation,
    FileSfioVolumeInformation,
    FileHardLinkInformation,
    FileProcessIdsUsingFileInformation,
    FileNormalizedNameInformation,
    FileNetworkPhysicalNameInformation,
    FileIdGlobalTxDirectoryInformation,
    FileIsRemoteDeviceInformation,
    FileUnusedInformation,
    FileNumaNodeInformation,
    FileStandardLinkInformation,
    FileRemoteProtocolInformation,
    FileRenameInformationBypassAccessCheck,
    FileLinkInformationBypassAccessCheck,
    FileVolumeNameInformation,
    FileIdInformation,
    FileIdExtdDirectoryInformation,
    FileReplaceCompletionInformation,
    FileHardLinkFullIdInformation,
    FileIdExtdBothDirectoryInformation,
    FileDispositionInformationEx,
    FileRenameInformationEx,
    FileRenameInformationExBypassAccessCheck,
    FileDesiredStorageClassInformation,
    FileStatInformation,
    FileMemoryPartitionInformation,
    FileStatLxInformation,
    FileCaseSensitiveInformation,
    FileLinkInformationEx,
    FileLinkInformationExBypassAccessCheck,
    FileStorageReserveIdInformation,
    FileCaseSensitiveInformationForceAccessCheck,
    FileKnownFolderInformation,
    FileStatBasicInformation,
    FileId64ExtdDirectoryInformation,
    FileId64ExtdBothDirectoryInformation,
    FileIdAllExtdDirectoryInformation,
    FileIdAllExtdBothDirectoryInformation,
    FileStreamReservationInformation,
    FileMupProviderInfo,
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

/**
 * The FILE_BASIC_INFORMATION structure contains timestamps and basic attributes of a file.
 * \li If you specify a value of zero for any of the XxxTime members, the file system keeps a file's current value for that time.
 * \li If you specify a value of -1 for any of the XxxTime members, time stamp updates are disabled for I/O operations performed on the file handle.
 * \li If you specify a value of -2 for any of the XxxTime members, time stamp updates are enabled for I/O operations performed on the file handle.
 * \remarks To set the members of this structure, the caller must have FILE_WRITE_ATTRIBUTES access to the file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_basic_information
 */
typedef struct _FILE_BASIC_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

/**
 * The FILE_STANDARD_INFORMATION structure contains standard information of a file.
 * \remarks EndOfFile specifies the byte offset to the end of the file.
 * Because this value is zero-based, it actually refers to the first free byte in the file; that is, it is the offset to the byte immediately following the last valid byte in the file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_standard_information
 */
typedef struct _FILE_STANDARD_INFORMATION
{
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

/**
 * The FILE_STANDARD_INFORMATION_EX structure is used as an argument to routines that query or set file information
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_standard_information_ex
 */
typedef struct _FILE_STANDARD_INFORMATION_EX
{
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG NumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
    BOOLEAN AlternateStream;
    BOOLEAN MetadataAttribute;
} FILE_STANDARD_INFORMATION_EX, *PFILE_STANDARD_INFORMATION_EX;

/**
 * The FILE_INTERNAL_INFORMATION structure is used to query for the file system's 8-byte file reference number for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_internal_information
 * \remark The IndexNumber member is the same as the FileId member of the FILE_ID_BOTH_DIR_INFORMATION and FILE_ID_FULL_DIR_INFORMATION structures.
 */
typedef struct _FILE_INTERNAL_INFORMATION
{
    union
    {
        ULARGE_INTEGER IndexNumber;
        struct
        {
            ULONGLONG MftRecordIndex : 48;
            ULONGLONG SequenceNumber : 16;
        };
    };
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

/**
 * The FILE_EA_INFORMATION structure is used to query for the size of the extended attributes (EA) for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_ea_information
 */
typedef struct _FILE_EA_INFORMATION
{
    ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

/**
 * The FILE_ACCESS_INFORMATION structure is used to query for or set the access rights of a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_access_information
 */
typedef struct _FILE_ACCESS_INFORMATION
{
    ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

/**
 * The FILE_POSITION_INFORMATION structure is used as an argument to routines that query or set file information.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_position_information
 */
typedef struct _FILE_POSITION_INFORMATION
{
    LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

/**
 * The FILE_MODE_INFORMATION structure is used to query or set the access mode of a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mode_information
 */
typedef struct _FILE_MODE_INFORMATION
{
    ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

/**
 * The FILE_ALIGNMENT_INFORMATION structure is used to query or set the buffer alignment required by the underlying device.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_alignment_information
 */
typedef struct _FILE_ALIGNMENT_INFORMATION
{
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

/**
 * The FILE_NAME_INFORMATION structure is used to query or set the file name and/or new short name for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_name_information
 */
typedef struct _FILE_NAME_INFORMATION
{
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

/**
 * The FILE_ALL_INFORMATION structure is used as a container for several FILE_XXX_INFORMATION structures.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_all_information
 */
typedef struct _FILE_ALL_INFORMATION
{
    FILE_BASIC_INFORMATION BasicInformation;
    FILE_STANDARD_INFORMATION StandardInformation;
    FILE_INTERNAL_INFORMATION InternalInformation;
    FILE_EA_INFORMATION EaInformation;
    FILE_ACCESS_INFORMATION AccessInformation;
    FILE_POSITION_INFORMATION PositionInformation;
    FILE_MODE_INFORMATION ModeInformation;
    FILE_ALIGNMENT_INFORMATION AlignmentInformation;
    FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

/**
 * The FILE_NETWORK_OPEN_INFORMATION structure is used to query for information that is commonly needed when a file is opened across a network.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_network_open_information
 */
typedef struct _FILE_NETWORK_OPEN_INFORMATION
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

/**
 * The FILE_ATTRIBUTE_TAG_INFORMATION structure is used to query for attribute and reparse tag information for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_attribute_tag_information
 */
typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION
{
    ULONG FileAttributes;
    ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;

/**
 * The FILE_ALLOCATION_INFORMATION structure is used to set the allocation size for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_allocation_information
 */
typedef struct _FILE_ALLOCATION_INFORMATION
{
    LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

/**
 * The FILE_COMPRESSION_INFORMATION structure describes the state of a compressed data buffer.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_compression_information
 */
typedef struct _FILE_COMPRESSION_INFORMATION
{
    LARGE_INTEGER CompressedFileSize;
    USHORT CompressionFormat;
    UCHAR CompressionUnitShift;
    UCHAR ChunkShift;
    UCHAR ClusterShift;
    UCHAR Reserved[3];
} FILE_COMPRESSION_INFORMATION, *PFILE_COMPRESSION_INFORMATION;

/**
 * The FILE_DISPOSITION_INFORMATION structure is used to mark a file for deletion.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_disposition_information
 */
typedef struct _FILE_DISPOSITION_INFORMATION
{
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;

/**
 * The FILE_END_OF_FILE_INFORMATION structure is used to set end-of-file information for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_end_of_file_information
 */
typedef struct _FILE_END_OF_FILE_INFORMATION
{
    LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

#define FLAGS_END_OF_FILE_INFO_EX_EXTEND_PAGING             0x00000001
#define FLAGS_END_OF_FILE_INFO_EX_NO_EXTRA_PAGING_EXTEND    0x00000002
#define FLAGS_END_OF_FILE_INFO_EX_TIME_CONSTRAINED          0x00000004
#define FLAGS_DELAY_REASONS_LOG_FILE_FULL                   0x00000001
#define FLAGS_DELAY_REASONS_BITMAP_SCANNED                  0x00000002

typedef struct _FILE_END_OF_FILE_INFORMATION_EX
{
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER PagingFileSizeInMM;
    LARGE_INTEGER PagingFileMaxSize;
    ULONG Flags;
} FILE_END_OF_FILE_INFORMATION_EX, *PFILE_END_OF_FILE_INFORMATION_EX;

/**
 * The FILE_VALID_DATA_LENGTH_INFORMATION structure is used to set the valid data length information for a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_file_valid_data_length_information
 */
typedef struct _FILE_VALID_DATA_LENGTH_INFORMATION
{
    LARGE_INTEGER ValidDataLength;
} FILE_VALID_DATA_LENGTH_INFORMATION, *PFILE_VALID_DATA_LENGTH_INFORMATION;

#define FILE_LINK_REPLACE_IF_EXISTS 0x00000001
#define FILE_LINK_POSIX_SEMANTICS   0x00000002

#define FILE_LINK_SUPPRESS_STORAGE_RESERVE_INHERITANCE  0x00000008
#define FILE_LINK_NO_INCREASE_AVAILABLE_SPACE           0x00000010
#define FILE_LINK_NO_DECREASE_AVAILABLE_SPACE           0x00000020
#define FILE_LINK_PRESERVE_AVAILABLE_SPACE              0x00000030
#define FILE_LINK_IGNORE_READONLY_ATTRIBUTE             0x00000040
#define FILE_LINK_FORCE_RESIZE_TARGET_SR                0x00000080
#define FILE_LINK_FORCE_RESIZE_SOURCE_SR                0x00000100
#define FILE_LINK_FORCE_RESIZE_SR                       0x00000180

/**
 * The FILE_LINK_INFORMATION structure is used to create an NTFS hard link to an existing file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_link_information
 */
typedef struct _FILE_LINK_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;

typedef struct _FILE_LINK_INFORMATION_EX
{
    ULONG Flags;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_INFORMATION_EX, *PFILE_LINK_INFORMATION_EX;

typedef struct _FILE_MOVE_CLUSTER_INFORMATION
{
    ULONG ClusterCount;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION, *PFILE_MOVE_CLUSTER_INFORMATION;

/**
 * The FILE_RENAME_INFORMATION structure is used to rename a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information
 */
typedef struct _FILE_RENAME_INFORMATION
{
    BOOLEAN ReplaceIfExists;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

#define FILE_RENAME_REPLACE_IF_EXISTS                       0x00000001
#define FILE_RENAME_POSIX_SEMANTICS                         0x00000002
#define FILE_RENAME_SUPPRESS_PIN_STATE_INHERITANCE          0x00000004
#define FILE_RENAME_SUPPRESS_STORAGE_RESERVE_INHERITANCE    0x00000008
#define FILE_RENAME_NO_INCREASE_AVAILABLE_SPACE             0x00000010
#define FILE_RENAME_NO_DECREASE_AVAILABLE_SPACE             0x00000020
#define FILE_RENAME_PRESERVE_AVAILABLE_SPACE                0x00000030
#define FILE_RENAME_IGNORE_READONLY_ATTRIBUTE               0x00000040
#define FILE_RENAME_FORCE_RESIZE_TARGET_SR                  0x00000080
#define FILE_RENAME_FORCE_RESIZE_SOURCE_SR                  0x00000100
#define FILE_RENAME_FORCE_RESIZE_SR                         0x00000180

/**
 * The FILE_RENAME_INFORMATION_EX structure is used to rename a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information
 */
typedef struct _FILE_RENAME_INFORMATION_EX
{
    ULONG Flags;
    HANDLE RootDirectory;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_RENAME_INFORMATION_EX, *PFILE_RENAME_INFORMATION_EX;

/**
 * The FILE_STREAM_INFORMATION structure contains information about a file stream.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_stream_information
 */
_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_STREAM_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG StreamNameLength;
    LARGE_INTEGER StreamSize;
    LARGE_INTEGER StreamAllocationSize;
    _Field_size_bytes_(StreamNameLength) WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

/**
 * The FILE_TRACKING_INFORMATION structure contains information used for tracking file operations.
 */
typedef struct _FILE_TRACKING_INFORMATION
{
    HANDLE DestinationFile;
    ULONG ObjectInformationLength;
    _Field_size_bytes_(ObjectInformationLength) CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

/**
 * The FILE_COMPLETION_INFORMATION structure contains the port handle and key for an I/O completion port created for a file handle.
 *
 * \remarks he FILE_COMPLETION_INFORMATION structure is used to replace the completion information for a port handle set in Port.
 * Completion information is replaced with the ZwSetInformationFile routine with the FileInformationClass parameter set to FileReplaceCompletionInformation.
 * The Port and Key members of FILE_COMPLETION_INFORMATION are set to their new values. To remove an existing completion port for a file handle, Port is set to NULL.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_completion_information
 */
typedef struct _FILE_COMPLETION_INFORMATION
{
    HANDLE Port;
    PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

/**
 * The FILE_PIPE_INFORMATION structure contains information about a named pipe that is not specific to the local or the remote end of the pipe.
 *
 * \remarks If ReadMode is set to FILE_PIPE_BYTE_STREAM_MODE, any attempt to change it must fail with a STATUS_INVALID_PARAMETER error code.
 * When CompletionMode is set to FILE_PIPE_QUEUE_OPERATION, if the pipe is connected to, read to, or written from,
 * the operation is not completed until there is data to read, all data is written, or a client is connected.
 * When CompletionMode is set to FILE_PIPE_COMPLETE_OPERATION, if the pipe is being connected to, read to, or written from, the operation is completed immediately.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_information
 */
typedef struct _FILE_PIPE_INFORMATION
{
     ULONG ReadMode;
     ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

/**
 * The FILE_PIPE_LOCAL_INFORMATION structure contains information about the local end of a named pipe.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_local_information
 */
typedef struct _FILE_PIPE_LOCAL_INFORMATION
{
     ULONG NamedPipeType;
     ULONG NamedPipeConfiguration;
     ULONG MaximumInstances;
     ULONG CurrentInstances;
     ULONG InboundQuota;
     ULONG ReadDataAvailable;
     ULONG OutboundQuota;
     ULONG WriteQuotaAvailable;
     ULONG NamedPipeState;
     ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

/**
 * The FILE_PIPE_REMOTE_INFORMATION structure contains information about the remote end of a named pipe.
 *
 * \remarks Remote information is not available for local pipes or for the server end of a remote pipe.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_pipe_remote_information
 */
typedef struct _FILE_PIPE_REMOTE_INFORMATION
{
    LARGE_INTEGER CollectDataTime;
    ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

/**
 * The FILE_MAILSLOT_QUERY_INFORMATION structure contains information about a mailslot.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mailslot_query_information
 */
typedef struct _FILE_MAILSLOT_QUERY_INFORMATION
{
    ULONG MaximumMessageSize;
    ULONG MailslotQuota;
    ULONG NextMessageSize;
    ULONG MessagesAvailable;
    LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION, *PFILE_MAILSLOT_QUERY_INFORMATION;

/**
 * The FILE_MAILSLOT_SET_INFORMATION structure is used to set a value on a mailslot.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_mailslot_set_information
 */
typedef struct _FILE_MAILSLOT_SET_INFORMATION
{
    PLARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION, *PFILE_MAILSLOT_SET_INFORMATION;

/**
 * The FILE_REPARSE_POINT_INFORMATION structure contains information about a reparse point.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_reparse_point_information
 */
typedef struct _FILE_REPARSE_POINT_INFORMATION
{
    LONGLONG FileReference;
    ULONG Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

/**
 * The FILE_LINK_ENTRY_INFORMATION structure contains information about a file link entry.
 */
_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_LINK_ENTRY_INFORMATION
{
    ULONG NextEntryOffset;
    LONGLONG ParentFileId;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_ENTRY_INFORMATION, *PFILE_LINK_ENTRY_INFORMATION;

/**
 * The FILE_LINKS_INFORMATION structure contains information about file links.
 */
typedef struct _FILE_LINKS_INFORMATION
{
    ULONG BytesNeeded;
    ULONG EntriesReturned;
    FILE_LINK_ENTRY_INFORMATION Entry;
} FILE_LINKS_INFORMATION, *PFILE_LINKS_INFORMATION;

/**
 * The FILE_NETWORK_PHYSICAL_NAME_INFORMATION structure contains information about the network physical name of a file.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_network_physical_name_information
 */
typedef struct _FILE_NETWORK_PHYSICAL_NAME_INFORMATION
{
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NETWORK_PHYSICAL_NAME_INFORMATION, *PFILE_NETWORK_PHYSICAL_NAME_INFORMATION;

/**
 * The FILE_STANDARD_LINK_INFORMATION structure contains standard information about a file link.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_standard_link_information
 */
typedef struct _FILE_STANDARD_LINK_INFORMATION
{
    ULONG NumberOfAccessibleLinks;
    ULONG TotalNumberOfLinks;
    BOOLEAN DeletePending;
    BOOLEAN Directory;
} FILE_STANDARD_LINK_INFORMATION, *PFILE_STANDARD_LINK_INFORMATION;

typedef struct _FILE_SFIO_RESERVE_INFORMATION
{
    ULONG RequestsPerPeriod;
    ULONG Period;
    BOOLEAN RetryFailures;
    BOOLEAN Discardable;
    ULONG RequestSize;
    ULONG NumOutstandingRequests;
} FILE_SFIO_RESERVE_INFORMATION, *PFILE_SFIO_RESERVE_INFORMATION;

typedef struct _FILE_SFIO_VOLUME_INFORMATION
{
    ULONG MaximumRequestsPerPeriod;
    ULONG MinimumPeriod;
    ULONG MinimumTransferSize;
} FILE_SFIO_VOLUME_INFORMATION, *PFILE_SFIO_VOLUME_INFORMATION;

/**
 * The IO_PRIORITY_HINT enumeration type specifies the priority hint for an IRP.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ne-wdm-_io_priority_hint
 */
typedef enum _IO_PRIORITY_HINT
{
    IoPriorityVeryLow,
    IoPriorityLow,
    IoPriorityNormal,
    IoPriorityHigh,
    IoPriorityCritical,
    MaxIoPriorityTypes
} IO_PRIORITY_HINT;

/**
 * The FILE_IO_PRIORITY_HINT_INFORMATION structure is used to query and set the default IRP priority hint for requests on the specified file handle.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_io_priority_hint_information
 */
typedef struct DECLSPEC_ALIGN(8) _FILE_IO_PRIORITY_HINT_INFORMATION
{
    IO_PRIORITY_HINT PriorityHint;
} FILE_IO_PRIORITY_HINT_INFORMATION, *PFILE_IO_PRIORITY_HINT_INFORMATION;

/**
 * The FILE_IO_PRIORITY_HINT_INFORMATION_EX structure is used to query and set the default IRP priority hint for requests on the specified file handle.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_io_priority_hint_information
 */
typedef struct _FILE_IO_PRIORITY_HINT_INFORMATION_EX
{
    IO_PRIORITY_HINT PriorityHint;
    BOOLEAN BoostOutstanding;
} FILE_IO_PRIORITY_HINT_INFORMATION_EX, *PFILE_IO_PRIORITY_HINT_INFORMATION_EX;

/**
 * FILE_SKIP_COMPLETION_PORT_ON_SUCCESS
 * Skip posting a completion packet to the I/O completion port if the operation completes successfully.
 */
#define FILE_SKIP_COMPLETION_PORT_ON_SUCCESS 0x1

/**
 * FILE_SKIP_SET_EVENT_ON_HANDLE
 * Skip setting the event on the file handle when the operation completes.
 */
#define FILE_SKIP_SET_EVENT_ON_HANDLE 0x2

/**
 * FILE_SKIP_SET_USER_EVENT_ON_FAST_IO
 * Skip setting the user event when a fast I/O operation completes.
 */
#define FILE_SKIP_SET_USER_EVENT_ON_FAST_IO 0x4

typedef struct _FILE_IO_COMPLETION_NOTIFICATION_INFORMATION
{
    ULONG Flags;
} FILE_IO_COMPLETION_NOTIFICATION_INFORMATION, *PFILE_IO_COMPLETION_NOTIFICATION_INFORMATION;

typedef struct _FILE_PROCESS_IDS_USING_FILE_INFORMATION
{
    ULONG NumberOfProcessIdsInList;
    _Field_size_(NumberOfProcessIdsInList) HANDLE ProcessIdList[1];
} FILE_PROCESS_IDS_USING_FILE_INFORMATION, *PFILE_PROCESS_IDS_USING_FILE_INFORMATION;

/**
 * The FILE_IS_REMOTE_DEVICE_INFORMATION structure indicates whether the file system that contains the file is a remote file system.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_is_remote_device_information
 */
typedef struct _FILE_IS_REMOTE_DEVICE_INFORMATION
{
    BOOLEAN IsRemote;
} FILE_IS_REMOTE_DEVICE_INFORMATION, *PFILE_IS_REMOTE_DEVICE_INFORMATION;

typedef struct _FILE_NUMA_NODE_INFORMATION
{
    USHORT NodeNumber;
} FILE_NUMA_NODE_INFORMATION, *PFILE_NUMA_NODE_INFORMATION;

typedef struct _FILE_IOSTATUSBLOCK_RANGE_INFORMATION
{
    PUCHAR IoStatusBlockRange;
    ULONG Length;
} FILE_IOSTATUSBLOCK_RANGE_INFORMATION, *PFILE_IOSTATUSBLOCK_RANGE_INFORMATION;

typedef struct _FILE_REMOTE_PROTOCOL_INFORMATION
{
    USHORT StructureVersion;
    USHORT StructureSize;

    ULONG Protocol;

    USHORT ProtocolMajorVersion;
    USHORT ProtocolMinorVersion;
    USHORT ProtocolRevision;

    USHORT Reserved;

    ULONG Flags;

    struct
    {
        ULONG Reserved[8];
    } GenericReserved;


    union
    {
        struct
        {
            struct
            {
                ULONG Capabilities;
            } Server;
            struct
            {
                ULONG Capabilities;
                ULONG ShareFlags;
                UCHAR ShareType;
                UCHAR Reserved0[3];
                ULONG Reserved1;
            } Share;
        } Smb2;
        ULONG Reserved[16];
    } ProtocolSpecific;
} FILE_REMOTE_PROTOCOL_INFORMATION, *PFILE_REMOTE_PROTOCOL_INFORMATION;

#define CHECKSUM_ENFORCEMENT_OFF 0x00000001

typedef struct _FILE_INTEGRITY_STREAM_INFORMATION
{
    USHORT ChecksumAlgorithm;
    UCHAR ChecksumChunkShift;
    UCHAR ClusterShift;
    ULONG Flags;
} FILE_INTEGRITY_STREAM_INFORMATION, *PFILE_INTEGRITY_STREAM_INFORMATION;

typedef struct _FILE_VOLUME_NAME_INFORMATION
{
    ULONG DeviceNameLength;
    _Field_size_bytes_(DeviceNameLength) WCHAR DeviceName[1];
} FILE_VOLUME_NAME_INFORMATION, *PFILE_VOLUME_NAME_INFORMATION;

#ifndef FILE_INVALID_FILE_ID
#define FILE_INVALID_FILE_ID ((LONGLONG)-1LL)
#endif

#define FILE_ID_IS_INVALID(FID) ((FID).QuadPart == FILE_INVALID_FILE_ID)

#define FILE_ID_128_IS_INVALID(FID128) \
    (((FID128).Identifier[0] == (UCHAR)-1) && \
    ((FID128).Identifier[1] == (UCHAR)-1) && \
    ((FID128).Identifier[2] == (UCHAR)-1) && \
    ((FID128).Identifier[3] == (UCHAR)-1) && \
    ((FID128).Identifier[4] == (UCHAR)-1) && \
    ((FID128).Identifier[5] == (UCHAR)-1) && \
    ((FID128).Identifier[6] == (UCHAR)-1) && \
    ((FID128).Identifier[7] == (UCHAR)-1) && \
    ((FID128).Identifier[8] == (UCHAR)-1) && \
    ((FID128).Identifier[9] == (UCHAR)-1) && \
    ((FID128).Identifier[10] == (UCHAR)-1) && \
    ((FID128).Identifier[11] == (UCHAR)-1) && \
    ((FID128).Identifier[12] == (UCHAR)-1) && \
    ((FID128).Identifier[13] == (UCHAR)-1) && \
    ((FID128).Identifier[14] == (UCHAR)-1) && \
    ((FID128).Identifier[15] == (UCHAR)-1))

#define MAKE_INVALID_FILE_ID_128(FID128) { \
    ((FID128).Identifier[0] = (UCHAR)-1); \
    ((FID128).Identifier[1] = (UCHAR)-1); \
    ((FID128).Identifier[2] = (UCHAR)-1); \
    ((FID128).Identifier[3] = (UCHAR)-1); \
    ((FID128).Identifier[4] = (UCHAR)-1); \
    ((FID128).Identifier[5] = (UCHAR)-1); \
    ((FID128).Identifier[6] = (UCHAR)-1); \
    ((FID128).Identifier[7] = (UCHAR)-1); \
    ((FID128).Identifier[8] = (UCHAR)-1); \
    ((FID128).Identifier[9] = (UCHAR)-1); \
    ((FID128).Identifier[10] = (UCHAR)-1); \
    ((FID128).Identifier[11] = (UCHAR)-1); \
    ((FID128).Identifier[12] = (UCHAR)-1); \
    ((FID128).Identifier[13] = (UCHAR)-1); \
    ((FID128).Identifier[14] = (UCHAR)-1); \
    ((FID128).Identifier[15] = (UCHAR)-1); \
}

/**
 * The FILE_ID_INFORMATION structure is used to query file identification information.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_id_information
 */
typedef struct _FILE_ID_INFORMATION
{
    ULONGLONG VolumeSerialNumber;
    union
    {
        FILE_ID_128 FileId;
        struct
        {
            union
            {
                FILE_INTERNAL_INFORMATION FileInternal;
                LONGLONG FileIdLowPart;
            };
            LONGLONG FileIdHighPart;
        };
    };
} FILE_ID_INFORMATION, *PFILE_ID_INFORMATION;

/**
 * The FILE_ID_EXTD_DIR_INFORMATION structure is used to query 128-bit file reference number information for the files in a directory.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-file_id_extd_dir_information
 */
_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    FILE_ID_128 FileId;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_EXTD_DIR_INFORMATION, *PFILE_ID_EXTD_DIR_INFORMATION;

#define FileIdExtdDirectoryInformationDefinition {                  \
    FileIdExtdDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_EXTD_DIR_INFORMATION, FileNameLength)      \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_LINK_ENTRY_FULL_ID_INFORMATION
{
    ULONG NextEntryOffset;
    FILE_ID_128 ParentFileId;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_LINK_ENTRY_FULL_ID_INFORMATION, *PFILE_LINK_ENTRY_FULL_ID_INFORMATION;

typedef struct _FILE_LINKS_FULL_ID_INFORMATION
{
    ULONG BytesNeeded;
    ULONG EntriesReturned;
    FILE_LINK_ENTRY_FULL_ID_INFORMATION Entry;
} FILE_LINKS_FULL_ID_INFORMATION, *PFILE_LINKS_FULL_ID_INFORMATION;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    FILE_ID_128 FileId;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_EXTD_BOTH_DIR_INFORMATION;

#define FileIdExtdBothDirectoryInformationDefinition {                  \
    FileIdExtdBothDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_ID_EXTD_BOTH_DIR_INFORMATION, FileNameLength)     \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_64_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_64_EXTD_DIR_INFORMATION, *PFILE_ID_64_EXTD_DIR_INFORMATION;

#define FileId64ExtdDirectoryInformationDefinition {                    \
    FileId64ExtdDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, NextEntryOffset),     \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, FileName),            \
    FIELD_OFFSET(FILE_ID_64_EXTD_DIR_INFORMATION, FileNameLength)       \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_64_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_64_EXTD_BOTH_DIR_INFORMATION;

#define FileId64ExtdBothDirectoryInformationDefinition {                    \
    FileId64ExtdBothDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_64_EXTD_BOTH_DIR_INFORMATION, FileNameLength)      \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_ALL_EXTD_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    FILE_ID_128 FileId128;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_ALL_EXTD_DIR_INFORMATION, *PFILE_ID_ALL_EXTD_DIR_INFORMATION;

#define FileIdAllExtdDirectoryInformationDefinition {                    \
    FileIdAllExtdDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, NextEntryOffset),     \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileName),            \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_DIR_INFORMATION, FileNameLength)       \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    ULONG ReparsePointTag;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    FILE_ID_128 FileId128;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, *PFILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION;

#define FileIdAllExtdBothDirectoryInformationDefinition {                    \
    FileIdAllExtdBothDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, NextEntryOffset),    \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileName),           \
    FIELD_OFFSET(FILE_ID_ALL_EXTD_BOTH_DIR_INFORMATION, FileNameLength)      \
}

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_STAT_INFORMATION
{
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
} FILE_STAT_INFORMATION, *PFILE_STAT_INFORMATION;

typedef struct _FILE_STAT_BASIC_INFORMATION
{
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ULONG DeviceType;
    ULONG DeviceCharacteristics;
    ULONG Reserved;
    LARGE_INTEGER VolumeSerialNumber;
    FILE_ID_128 FileId128;
} FILE_STAT_BASIC_INFORMATION, *PFILE_STAT_BASIC_INFORMATION;
#endif

/**
 * The FILE_MEMORY_PARTITION_INFORMATION structure is used to query information about a memory partition.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_file_memory_partition_information
 */
typedef struct _FILE_MEMORY_PARTITION_INFORMATION
{
    HANDLE OwnerPartitionHandle;
    union
    {
        struct
        {
            UCHAR NoCrossPartitionAccess;
            UCHAR Spare[3];
        };
        ULONG AllFlags;
    } Flags;
} FILE_MEMORY_PARTITION_INFORMATION, *PFILE_MEMORY_PARTITION_INFORMATION;

#define LX_FILE_METADATA_HAS_UID 0x1
#define LX_FILE_METADATA_HAS_GID 0x2
#define LX_FILE_METADATA_HAS_MODE 0x4
#define LX_FILE_METADATA_HAS_DEVICE_ID 0x8
#define LX_FILE_CASE_SENSITIVE_DIR 0x10

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_STAT_LX_INFORMATION
{
    FILE_INTERNAL_INFORMATION FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    ULONG FileAttributes;
    ULONG ReparseTag;
    ULONG NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
    ULONG LxFlags;
    ULONG LxUid;
    ULONG LxGid;
    ULONG LxMode;
    ULONG LxDeviceIdMajor;
    ULONG LxDeviceIdMinor;
} FILE_STAT_LX_INFORMATION, *PFILE_STAT_LX_INFORMATION;
#endif

typedef struct _FILE_STORAGE_RESERVE_ID_INFORMATION
{
    STORAGE_RESERVE_ID StorageReserveId;
} FILE_STORAGE_RESERVE_ID_INFORMATION, *PFILE_STORAGE_RESERVE_ID_INFORMATION;

#define FILE_CS_FLAG_CASE_SENSITIVE_DIR     0x00000001

#if !defined(NTDDI_WIN11_GE) || (NTDDI_VERSION < NTDDI_WIN11_GE)
typedef struct _FILE_CASE_SENSITIVE_INFORMATION
{
    ULONG Flags;
} FILE_CASE_SENSITIVE_INFORMATION, *PFILE_CASE_SENSITIVE_INFORMATION;
#endif

typedef enum _FILE_KNOWN_FOLDER_TYPE
{
    KnownFolderNone = 0,
    KnownFolderDesktop,
    KnownFolderDocuments,
    KnownFolderDownloads,
    KnownFolderMusic,
    KnownFolderPictures,
    KnownFolderVideos,
    KnownFolderOther,
    KnownFolderMax
} FILE_KNOWN_FOLDER_TYPE;

typedef struct _FILE_KNOWN_FOLDER_INFORMATION
{
    FILE_KNOWN_FOLDER_TYPE Type;
} FILE_KNOWN_FOLDER_INFORMATION, *PFILE_KNOWN_FOLDER_INFORMATION;

typedef struct _FILE_STREAM_RESERVATION_INFORMATION
{
    ULONG_PTR TrackedReservation;
    ULONG_PTR EnforcedReservation;
} FILE_STREAM_RESERVATION_INFORMATION, *PFILE_STREAM_RESERVATION_INFORMATION;

typedef struct _MUP_PROVIDER_INFORMATION
{
    ULONG Level;
    PVOID Buffer;
    PULONG BufferSize;
} MUP_PROVIDER_INFORMATION, *PMUP_PROVIDER_INFORMATION;


_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_INFORMATION_DEFINITION
{
    FILE_INFORMATION_CLASS Class;
    ULONG NextEntryOffset;
    ULONG FileNameOffset;
    ULONG FileNameLengthOffset;
} FILE_INFORMATION_DEFINITION, *PFILE_INFORMATION_DEFINITION;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_DIRECTORY_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

#define FileDirectoryInformationDefinition {                    \
    FileDirectoryInformation,                                   \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, NextEntryOffset),  \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, FileName),         \
    FIELD_OFFSET(FILE_DIRECTORY_INFORMATION, FileNameLength)    \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_FULL_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

#define FileFullDirectoryInformationDefinition {                \
    FileFullDirectoryInformation,                               \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_FULL_DIR_INFORMATION, FileNameLength)     \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_FULL_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;

#define FileIdFullDirectoryInformationDefinition {              \
    FileIdFullDirectoryInformation,                             \
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, NextEntryOffset),\
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, FileName),       \
    FIELD_OFFSET(FILE_ID_FULL_DIR_INFORMATION, FileNameLength)  \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

#define FileBothDirectoryInformationDefinition {                \
    FileBothDirectoryInformation,                               \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_BOTH_DIR_INFORMATION, FileNameLength)     \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_BOTH_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

#define FileIdBothDirectoryInformationDefinition {              \
    FileIdBothDirectoryInformation,                             \
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, NextEntryOffset),\
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, FileName),       \
    FIELD_OFFSET(FILE_ID_BOTH_DIR_INFORMATION, FileNameLength)  \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_NAMES_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    ULONG FileNameLength;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_NAMES_INFORMATION, *PFILE_NAMES_INFORMATION;

#define FileNamesInformationDefinition {                    \
    FileNamesInformation,                                   \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, NextEntryOffset),  \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, FileName),         \
    FIELD_OFFSET(FILE_NAMES_INFORMATION, FileNameLength)    \
}

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_ID_GLOBAL_TX_DIR_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    union
    {
        LARGE_INTEGER FileId;
        FILE_INTERNAL_INFORMATION FileInternal;
    };
    GUID LockingTransactionId;
    ULONG TxInfoFlags;
    _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_ID_GLOBAL_TX_DIR_INFORMATION, *PFILE_ID_GLOBAL_TX_DIR_INFORMATION;

#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_WRITELOCKED 0x00000001
#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_VISIBLE_TO_TX 0x00000002
#define FILE_ID_GLOBAL_TX_DIR_INFO_FLAG_VISIBLE_OUTSIDE_TX 0x00000004

#define FileIdGlobalTxDirectoryInformationDefinition {                  \
    FileIdGlobalTxDirectoryInformation,                                 \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, NextEntryOffset),   \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileName),          \
    FIELD_OFFSET(FILE_ID_GLOBAL_TX_DIR_INFORMATION, FileNameLength)     \
}

typedef struct _FILE_OBJECTID_INFORMATION
{
    ULONGLONG FileReference;
    UCHAR ObjectId[16];
    union
    {
        struct
        {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        };
        UCHAR ExtendedInfo[48];
    };
} FILE_OBJECTID_INFORMATION, *PFILE_OBJECTID_INFORMATION;

typedef struct _FILE_DIRECTORY_NEXT_INFORMATION
{
    ULONG NextEntryOffset;
} FILE_DIRECTORY_NEXT_INFORMATION, *PFILE_DIRECTORY_NEXT_INFORMATION;


_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_FULL_EA_INFORMATION
{
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    _Field_size_bytes_(EaNameLength) CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_GET_EA_INFORMATION
{
    ULONG NextEntryOffset;
    UCHAR EaNameLength;
    _Field_size_bytes_(EaNameLength) CHAR EaName[1];
} FILE_GET_EA_INFORMATION, *PFILE_GET_EA_INFORMATION;


_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_GET_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    _Field_size_bytes_(SidLength) SID Sid;
} FILE_GET_QUOTA_INFORMATION, *PFILE_GET_QUOTA_INFORMATION;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_QUOTA_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG SidLength;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER QuotaUsed;
    LARGE_INTEGER QuotaThreshold;
    LARGE_INTEGER QuotaLimit;
    _Field_size_bytes_(SidLength) SID Sid;
} FILE_QUOTA_INFORMATION, *PFILE_QUOTA_INFORMATION;

typedef enum _FSINFOCLASS
{
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation,
    FileFsAttributeInformation,
    FileFsControlInformation,
    FileFsFullSizeInformation,
    FileFsObjectIdInformation,
    FileFsDriverPathInformation,
    FileFsVolumeFlagsInformation,
    FileFsSectorSizeInformation,
    FileFsDataCopyInformation,
    FileFsMetadataSizeInformation,
    FileFsFullSizeInformationEx,
    FileFsGuidInformation,
    FileFsMaximumInformation
} FSINFOCLASS, *PFSINFOCLASS;
typedef enum _FSINFOCLASS FS_INFORMATION_CLASS;


typedef struct _FILE_FS_VOLUME_INFORMATION
{
    LARGE_INTEGER VolumeCreationTime;
    ULONG VolumeSerialNumber;
    ULONG VolumeLabelLength;
    BOOLEAN SupportsObjects;
    _Field_size_bytes_(VolumeLabelLength) WCHAR VolumeLabel[1];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

typedef struct _FILE_FS_LABEL_INFORMATION
{
    ULONG VolumeLabelLength;
    _Field_size_bytes_(VolumeLabelLength) WCHAR VolumeLabel[1];
} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;

typedef struct _FILE_FS_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

#define FILE_VC_QUOTA_NONE 0x00000000
#define FILE_VC_QUOTA_TRACK 0x00000001
#define FILE_VC_QUOTA_ENFORCE 0x00000002
#define FILE_VC_QUOTA_MASK 0x00000003
#define FILE_VC_CONTENT_INDEX_DISABLED 0x00000008
#define FILE_VC_LOG_QUOTA_THRESHOLD 0x00000010
#define FILE_VC_LOG_QUOTA_LIMIT 0x00000020
#define FILE_VC_LOG_VOLUME_THRESHOLD 0x00000040
#define FILE_VC_LOG_VOLUME_LIMIT 0x00000080
#define FILE_VC_QUOTAS_INCOMPLETE 0x00000100
#define FILE_VC_QUOTAS_REBUILDING 0x00000200
#define FILE_VC_VALID_MASK 0x000003ff

typedef struct _FILE_FS_CONTROL_INFORMATION
{
    LARGE_INTEGER FreeSpaceStartFiltering;
    LARGE_INTEGER FreeSpaceThreshold;
    LARGE_INTEGER FreeSpaceStopFiltering;
    LARGE_INTEGER DefaultQuotaThreshold;
    LARGE_INTEGER DefaultQuotaLimit;
    ULONG FileSystemControlFlags;
} FILE_FS_CONTROL_INFORMATION, *PFILE_FS_CONTROL_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION
{
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER CallerAvailableAllocationUnits;
    LARGE_INTEGER ActualAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION, *PFILE_FS_FULL_SIZE_INFORMATION;

typedef struct _FILE_FS_OBJECTID_INFORMATION
{
    UCHAR ObjectId[16];
    union
    {
        struct
        {
            UCHAR BirthVolumeId[16];
            UCHAR BirthObjectId[16];
            UCHAR DomainId[16];
        };
        UCHAR ExtendedInfo[48];
    };
} FILE_FS_OBJECTID_INFORMATION, *PFILE_FS_OBJECTID_INFORMATION;

typedef struct _FILE_FS_DEVICE_INFORMATION
{
    DEVICE_TYPE DeviceType;
    ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;

typedef struct _FILE_FS_ATTRIBUTE_INFORMATION
{
    ULONG FileSystemAttributes;
    LONG MaximumComponentNameLength;
    ULONG FileSystemNameLength;
    _Field_size_bytes_(FileSystemNameLength) WCHAR FileSystemName[1];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;

typedef struct _FILE_FS_DRIVER_PATH_INFORMATION
{
    BOOLEAN DriverInPath;
    ULONG DriverNameLength;
    _Field_size_bytes_(DriverNameLength) WCHAR DriverName[1];
} FILE_FS_DRIVER_PATH_INFORMATION, *PFILE_FS_DRIVER_PATH_INFORMATION;

typedef struct _FILE_FS_VOLUME_FLAGS_INFORMATION
{
    ULONG Flags;
} FILE_FS_VOLUME_FLAGS_INFORMATION, *PFILE_FS_VOLUME_FLAGS_INFORMATION;

#define SSINFO_FLAGS_ALIGNED_DEVICE 0x00000001
#define SSINFO_FLAGS_PARTITION_ALIGNED_ON_DEVICE 0x00000002
#define SSINFO_FLAGS_NO_SEEK_PENALTY 0x00000004
#define SSINFO_FLAGS_TRIM_ENABLED 0x00000008
#define SSINFO_FLAGS_BYTE_ADDRESSABLE 0x00000010

#define SSINFO_OFFSET_UNKNOWN 0xffffffff

typedef struct _FILE_FS_SECTOR_SIZE_INFORMATION
{
    ULONG LogicalBytesPerSector;
    ULONG PhysicalBytesPerSectorForAtomicity;
    ULONG PhysicalBytesPerSectorForPerformance;
    ULONG FileSystemEffectivePhysicalBytesPerSectorForAtomicity;
    ULONG Flags;
    ULONG ByteOffsetForSectorAlignment;
    ULONG ByteOffsetForPartitionAlignment;
} FILE_FS_SECTOR_SIZE_INFORMATION, *PFILE_FS_SECTOR_SIZE_INFORMATION;

typedef struct _FILE_FS_DATA_COPY_INFORMATION
{
    ULONG NumberOfCopies;
} FILE_FS_DATA_COPY_INFORMATION, *PFILE_FS_DATA_COPY_INFORMATION;

typedef struct _FILE_FS_METADATA_SIZE_INFORMATION
{
    LARGE_INTEGER TotalMetadataAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_METADATA_SIZE_INFORMATION, *PFILE_FS_METADATA_SIZE_INFORMATION;

typedef struct _FILE_FS_FULL_SIZE_INFORMATION_EX
{
    ULONGLONG ActualTotalAllocationUnits;
    ULONGLONG ActualAvailableAllocationUnits;
    ULONGLONG ActualPoolUnavailableAllocationUnits;
    ULONGLONG CallerTotalAllocationUnits;
    ULONGLONG CallerAvailableAllocationUnits;
    ULONGLONG CallerPoolUnavailableAllocationUnits;
    ULONGLONG UsedAllocationUnits;
    ULONGLONG TotalReservedAllocationUnits;
    ULONGLONG VolumeStorageReserveAllocationUnits;
    ULONGLONG AvailableCommittedAllocationUnits;
    ULONGLONG PoolAvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_FULL_SIZE_INFORMATION_EX, *PFILE_FS_FULL_SIZE_INFORMATION_EX;

typedef struct _FILE_FS_GUID_INFORMATION
{
    GUID FsGuid;
} FILE_FS_GUID_INFORMATION, *PFILE_FS_GUID_INFORMATION;


/**
 * The NtCreateFile routine creates a new file or directory, or opens an existing file, device, directory, or volume.
 *
 * \param[out] FileHandle Pointer to a variable that receives a handle to the pipe.
 * \param[in] DesiredAccess The requested access to the object.
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the object attributes, including pipe name.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] AllocationSize The initial allocation size in bytes for the file. Specify a non-zero value to eliminate disk fragmentation, since the file system pre-allocates the file using a contiguous block.
 * \param[in] FileAttributes The file attributes. Explicitly specified attributes are applied only when the file is created, superseded, or, in some cases, overwritten.
 * \param[in] ShareAccess The type of share access that the caller would like to use in the file.
 * \param[in] CreateDisposition Specifies how the file should be handled when the file already exists.
 * \param[in] CreateOptions Specifies the options to be applied when creating or opening the file.
 * \param[in] EaBuffer Pointer to an EA buffer used to pass extended attributes.
 * \param[in] EaLength Length of the EA buffer.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/api/Winternl/nf-winternl-ntcreatefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER AllocationSize,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
    _In_ ULONG EaLength
    );

/**
 * The NtCreateNamedPipeFile routine deletes the specified file.
 *
 * \param[out] FileHandle Pointer to a variable that receives a handle to the pipe.
 * \param[in] DesiredAccess The requested access to the object.
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the object attributes, including pipe name.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] ShareAccess Specifies the type of share access for the file.
 * \param[in] CreateDisposition Specifies how the file should be handled when the file already exists.
 * \param[in] CreateOptions Specifies the options to be applied when creating or opening the file.
 * \param[in] NamedPipeType Type of named pipe to create (byte-type or message-type).
 * \param[in] ReadMode Mode in which to read the pipe (byte-type or message-type).
 * \param[in] CompletionMode Specifies blocking or non-blocking mode of the pipe.
 * \param[in] MaximumInstances Maximum number of simultaneous instances of the named pipe.
 * \param[in] InboundQuota Specifies the pool quota that is reserved for writes to the inbound side of the named pipe.
 * \param[in] OutboundQuota Specifies the pool quota that is reserved for writes to the inbound side of the named pipe.
 * \param[in] DefaultTimeout An optional pointer to a timeout value that is used if a timeout value is not specified when waiting for an instance of a named pipe.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/nt-create-named-pipe-file
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateNamedPipeFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_ ULONG NamedPipeType,
    _In_ ULONG ReadMode,
    _In_ ULONG CompletionMode,
    _In_ ULONG MaximumInstances,
    _In_ ULONG InboundQuota,
    _In_ ULONG OutboundQuota,
    _In_ PLARGE_INTEGER DefaultTimeout
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateMailslotFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG CreateOptions,
    _In_ ULONG MailslotQuota,
    _In_ ULONG MaximumMessageSize,
    _In_ PLARGE_INTEGER ReadTimeout
    );

/**
 * The NtOpenFile routine opens an existing file, device, directory, or volume, and returns a handle for the file object.
 *
 * \param[out] FileHandle Pointer to a variable that receives a handle to the file.
 * \param[in] DesiredAccess The requested access to the object.
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] ShareAccess Specifies the type of share access for the file.
 * \param[in] OpenOptions Specifies the options to apply when opening the file.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntopenfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenFile(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG ShareAccess,
    _In_ ULONG OpenOptions
    );

/**
 * The NtDeleteFile routine deletes the specified file.
 *
 * \param[in] ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwdeletefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteFile(
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * The NtFlushBuffersFile routine sends a flush request for the specified file to the file system.
 *
 * \param[in] FileHandle A handle to the file whose buffers will be flushed.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwflushbuffersfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushBuffersFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock
    );



#define FLUSH_FLAGS_FILE_NORMAL 0x00000000
#define FLUSH_FLAGS_FILE_DATA_ONLY 0x00000001
#define FLUSH_FLAGS_NO_SYNC 0x00000002
#define FLUSH_FLAGS_FILE_DATA_SYNC_ONLY 0x00000004
#define FLUSH_FLAGS_FLUSH_AND_PURGE 0x00000008

#if (PHNT_VERSION >= PHNT_WINDOWS_8)

/**
 * The NtFlushBuffersFileEx routine sends a flush request for a given file to the file system. An optional flush operation flag can be set to control how file data is written to storage.
 *
 * \param FileHandle A handle to the file object representing the file, directory, device or volume.
 * \param Flags Flush operation flags.
 * \param Parameters Pointer to a block with additional parameters. This parameter must currently be set to NULL.
 * \param ParametersSize The size, in bytes, of the block that Parameters point to. This parameter must currently be set to 0.
 * \param IoStatusBlock Address of the caller's I/O status block. This parameter is required and cannot be NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntflushbuffersfileex
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushBuffersFileEx(
    _In_ HANDLE FileHandle,
    _In_ ULONG Flags,
    _In_reads_bytes_(ParametersSize) PVOID Parameters,
    _In_ ULONG ParametersSize,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock
    );
#endif

/**
 * The NtQueryInformationFile routine returns various kinds of information about a file object.
 *
 * \param FileHandle A handle to the file object representing the file or directory.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a caller-allocated buffer into which the routine writes the requested information about the file object.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass Specifies the type of information to be returned about the file, in the buffer that FileInformation points to.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS2)
/**
 * The NtQueryInformationByName routine returns various kinds of information about a file object by file name.
 *
 * \param ObjectAttributes Pointer to an OBJECT_ATTRIBUTES structure that contains the file's attributes, including file name.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a caller-allocated buffer into which the routine writes the requested information about the file object.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass Specifies the type of information to be returned about the file, in the buffer that FileInformation points to.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryinformationbyname
 * \remarks NtQueryInformationByName queries and returns the requested information without opening the actual file,
 * making it more efficient than NtQueryInformationFile, which requires a file open and subsequent file close.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryInformationByName(
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );
#endif

/**
 * The NtSetInformationFile routine changes various kinds of information about a file object.
 *
 * \param FileHandle A handle to the file object representing the file or directory.
 * \param IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param FileInformation Pointer to a buffer that contains the information to set for the file.
 * \param Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param FileInformationClass The type of information, supplied in the buffer pointed to by FileInformation, to set for the file.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntsetinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );

/**
 * The NtQueryDirectoryFile routine returns various kinds of information about files in the directory specified by a given file handle.
 *
 * \param[in] FileHandle A handle for the file object that represents the directory for which information is being requested.
 * \param[in] Event An optional handle for a caller-created event. The event is set to the Signaled state the requested operation is completed.
 * \param[in] ApcRoutine An address of an optional, caller-supplied APC routine to be called when the requested operation completes.
 * \param[in] ApcContext An address of an optional, caller-supplied APC or I/O completion object associated with the file object.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param[out] FileInformation A pointer to a buffer that receives the desired information about the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param[in] FileInformationClass The type of information to be returned about files in the directory.
 * \param[in] ReturnSingleEntry Set to TRUE if only a single entry should be returned, FALSE otherwise.
 * \param[in] FileName An optional pointer to a Unicode string search expression containing the name of a file (or multiple files, if wildcards are used) within the directory.
 * \param[in] RestartScan Set to TRUE if the scan is to start at the first entry in the directory. Set to FALSE if resuming the scan from a previous call.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDirectoryFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_opt_ PCUNICODE_STRING FileName,
    _In_ BOOLEAN RestartScan
    );

/**
 * The scan will start at the first entry in the directory. If this flag is not set, the scan will resume from where the last query ended.
 */
#define FILE_QUERY_RESTART_SCAN 0x00000001
/**
 * Normally the return buffer is packed with as many matching directory entries that fit.
 * If this flag is set, the file system will return only one directory entry at a time.
 * This does make the operation less efficient.
 */
#define FILE_QUERY_RETURN_SINGLE_ENTRY 0x00000002
/**
 * The scan should start at a specified indexed position in the directory.
 * This flag can only be set if you generate your own IRP_MJ_DIRECTORY_CONTROL IRP; the index is specified in the IRP.
 * How the position is specified varies from file system to file system.
 */
#define FILE_QUERY_INDEX_SPECIFIED 0x00000004
/**
 * Any file system filters that perform directory virtualization or just-in-time expansion should simply pass the request
 * through to the file system and return entries that are currently on disk. Not all file systems support this flag.
 */
#define FILE_QUERY_RETURN_ON_DISK_ENTRIES_ONLY 0x00000008
/**
 * File systems maintain per-FileObject directory cursor information. When multiple threads do queries using the same FileObject,
 * access to the per-FileObject structure is single threaded to prevent corruption of the cursor state.
 * This flag tells the file system to not update per-FileObject cursor state information thus allowing multiple threads
 * to query in parallel using the same handle. It behaves as if SL_RESTART_SCAN is specified on each call. If a wild card pattern
 * is given on the next call, the operation will not pick up where the last query ended.
 * This allows for true asynchronous directory query support. Not all file systems support this flag.
 */
#define FILE_QUERY_NO_CURSOR_UPDATE 0x00000010

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS3)
/**
 * The NtQueryDirectoryFileEx routine returns various kinds of information about files in the directory specified by a given file handle.
 *
 * \param[in] FileHandle A handle for the file object that represents the directory for which information is being requested.
 * \param[in] Event An optional handle for a caller-created event. The event is set to the Signaled state the requested operation is completed.
 * \param[in] ApcRoutine An address of an optional, caller-supplied APC routine to be called when the requested operation completes.
 * \param[in] ApcContext An address of an optional, caller-supplied APC or I/O completion object associated with the file object.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FileInformation.
 * \param[out] FileInformation A pointer to a buffer that receives the desired information about the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FileInformation.
 * \param[in] FileInformationClass The type of information to be returned about files in the directory.
 * \param[in] QueryFlags One or more of the flags contained in SL_QUERY_DIRECTORY_MASK
 * \param[in] FileName An optional pointer to a Unicode string search expression containing the name of a file (or multiple files, if wildcards are used) within the directory.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntquerydirectoryfileex
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDirectoryFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ ULONG QueryFlags,
    _In_opt_ PCUNICODE_STRING FileName
    );
#endif

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryEaFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_reads_bytes_opt_(EaListLength) PVOID EaList,
    _In_ ULONG EaListLength,
    _In_opt_ PULONG EaIndex,
    _In_ BOOLEAN RestartScan
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetEaFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryQuotaInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_reads_bytes_opt_(SidListLength) PVOID SidList,
    _In_ ULONG SidListLength,
    _In_opt_ PSID StartSid,
    _In_ BOOLEAN RestartScan
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetQuotaInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length
    );

/**
 * The NtQueryVolumeInformationFile routine retrieves information about the volume associated with a given file, directory, storage device, or volume.
 *
 * \param[in] FileHandle A handle to the file, directory, storage device, or volume for which volume information is being requested.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FsInformation.
 * \param[out] FsInformation A pointer to a caller-allocated buffer that receives the desired information about the volume.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FsInformation.
 * \param[in] FsInformationClass The type of information to be returned about the volume.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-ntqueryvolumeinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryVolumeInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FsInformation,
    _In_ ULONG Length,
    _In_ FSINFOCLASS FsInformationClass
    );

/**
 * The NtSetVolumeInformationFile routine modifies information about the volume associated with a given file, directory, storage device, or volume.
 *
 * \param[in] FileHandle A handle to the file, directory, storage device, or volume for which volume information is being requested.
 * \param[out] IoStatusBlock A pointer to an IO_STATUS_BLOCK structure that receives the final completion status, and the number of bytes written to the buffer pointed to by FsInformation.
 * \param[in] FsInformation A pointer to a caller-allocated buffer containing the volume information to be modified.
 * \param[in] Length The size, in bytes, of the buffer pointed to by FsInformation.
 * \param[in] FsInformationClass The type of information to set about the volume.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwsetvolumeinformationfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetVolumeInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FsInformation,
    _In_ ULONG Length,
    _In_ FSINFOCLASS FsInformationClass
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelIoFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelIoFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ PIO_STATUS_BLOCK IoRequestToCancel,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelSynchronousIoFile(
    _In_ HANDLE ThreadHandle,
    _In_opt_ PIO_STATUS_BLOCK IoRequestToCancel,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock
    );

/**
 * The NtDeviceIoControlFile function sends a control code directly to a specified device driver, causing the corresponding driver to perform the specified operation.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] IoControlCode IOCTL_XXX code that indicates which device I/O control operation is to be carried out on, usually by the underlying device driver.
 * \param[in] InputBuffer Pointer to a caller-allocated input buffer that contains device-specific information to be given to the target driver.
 * \param[in] InputBufferLength Size, in bytes, of the buffer at InputBuffer. This value is ignored if InputBuffer is NULL.
 * \param[out] OutputBuffer Pointer to a caller-allocated output buffer in which information is returned from the target driver.
 * \param[in] OutputBufferLength Size, in bytes, of the buffer at OutputBuffer. This value is ignored if OutputBuffer is NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwdeviceiocontrolfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeviceIoControlFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG IoControlCode,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength
    );

/**
 * The NtFsControlFile function sends a control code directly to a file system or filter driver, causing the corresponding driver to perform the specified action.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] FsControlCode FSCTL_XXX code that indicates which file system control operation is to be carried out.
 * \param[in] InputBuffer Pointer to a caller-allocated input buffer that contains device-specific information to be given to the target driver.
 * \param[in] InputBufferLength Size, in bytes, of the buffer at InputBuffer. This value is ignored if InputBuffer is NULL.
 * \param[out] OutputBuffer Pointer to a caller-allocated output buffer in which information is returned from the target driver.
 * \param[in] OutputBufferLength Size, in bytes, of the buffer at OutputBuffer. This value is ignored if OutputBuffer is NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/nf-ntifs-zwfscontrolfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtFsControlFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG FsControlCode,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength
    );

/**
 * The NtReadFile function reads data from an open file.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[out] Buffer Pointer to a caller-allocated buffer that receives the data read from the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by Buffer.
 * \param[in] ByteOffset Pointer to a variable that specifies the starting byte offset in the file where the read operation will begin.
 * \param[in] Key Device and intermediate drivers should set this pointer to NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwreadfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key
    );

/**
 * The NtWriteFile function writes data to an open file.
 *
 * \param[in] FileHandle A handle to the file object representing the file or directory on which the specified action is to be performed.
 * \param[in] Event A handle for a caller-created event. This parameter is optional and can be NULL. It must be NULL if the caller will wait for the FileHandle to be set to the Signaled state.
 * \param[in] ApcRoutine Address of a caller-supplied APC routine to be called when the requested operation completes. This parameter is optional and can be NULL.
 * \param[in] ApcContext Pointer to a caller-determined context area. This parameter value is used as the APC context if the caller supplies an APC, or is used as the completion context if an I/O completion object has been associated with the file object.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that receives the final completion status and information about the operation.
 * \param[in] Buffer Pointer to a caller-allocated buffer that contains the data to write to the file.
 * \param[in] Length The size, in bytes, of the buffer pointed to by Buffer.
 * \param[in] ByteOffset Pointer to a variable that specifies the starting byte offset in the file for beginning the write operation.
 * \param[in] Key Device and intermediate drivers should set this pointer to NULL.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwwritefile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReadFileScatter(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PFILE_SEGMENT_ELEMENT SegmentArray,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtWriteFileGather(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PFILE_SEGMENT_ELEMENT SegmentArray,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PLARGE_INTEGER ByteOffset,
    _In_ PLARGE_INTEGER Length,
    _In_ ULONG Key,
    _In_ BOOLEAN FailImmediately,
    _In_ BOOLEAN ExclusiveLock
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnlockFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ PLARGE_INTEGER ByteOffset,
    _In_ PLARGE_INTEGER Length,
    _In_ ULONG Key
    );

/**
 * The NtQueryAttributesFile function retrieves basic attributes for the specified file.
 *
 * \param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that supplies the attributes to be used for the file object.
 * \param FileInformation A pointer to a FILE_BASIC_INFORMATION structure to receive the returned file attribute information.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows/win32/devnotes/ntqueryattributesfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryAttributesFile(
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PFILE_BASIC_INFORMATION FileInformation
    );

/**
 * The NtQueryFullAttributesFile function retrieves network open information for the specified file.
 *
 * \param ObjectAttributes A pointer to an OBJECT_ATTRIBUTES structure that supplies the attributes to be used for the file object.
 * \param FileInformation A pointer to a FILE_NETWORK_OPEN_INFORMATION structure that receives the returned file attributes information.
 * \return NTSTATUS Successful or errant status.
 * \sa https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwqueryfullattributesfile
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryFullAttributesFile(
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PFILE_NETWORK_OPEN_INFORMATION FileInformation
    );


#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002
#define FILE_NOTIFY_CHANGE_NAME         0x00000003
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040
#define FILE_NOTIFY_CHANGE_EA           0x00000080
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100
#define FILE_NOTIFY_CHANGE_STREAM_NAME  0x00000200
#define FILE_NOTIFY_CHANGE_STREAM_SIZE  0x00000400
#define FILE_NOTIFY_CHANGE_STREAM_WRITE 0x00000800
#define FILE_NOTIFY_VALID_MASK          0x00000fff

#define FILE_ACTION_ADDED                   0x00000001
#define FILE_ACTION_REMOVED                 0x00000002
#define FILE_ACTION_MODIFIED                0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME        0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME        0x00000005
#define FILE_ACTION_ADDED_STREAM            0x00000006
#define FILE_ACTION_REMOVED_STREAM          0x00000007
#define FILE_ACTION_MODIFIED_STREAM         0x00000008
#define FILE_ACTION_REMOVED_BY_DELETE       0x00000009
#define FILE_ACTION_ID_NOT_TUNNELLED        0x0000000A
#define FILE_ACTION_TUNNELLED_ID_COLLISION  0x0000000B

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeDirectoryFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree
    );

typedef enum _DIRECTORY_NOTIFY_INFORMATION_CLASS
{
    DirectoryNotifyInformation = 1,
    DirectoryNotifyExtendedInformation,
    DirectoryNotifyFullInformation,
    DirectoryNotifyMaximumInformation
} DIRECTORY_NOTIFY_INFORMATION_CLASS, *PDIRECTORY_NOTIFY_INFORMATION_CLASS;

#if !defined(NTDDI_WIN10_RS5) || (NTDDI_VERSION < NTDDI_WIN10_RS5)
_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_NOTIFY_INFORMATION
{
   ULONG NextEntryOffset;
   ULONG Action;
   ULONG FileNameLength;
   WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_NOTIFY_EXTENDED_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Action;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastModificationTime;
    LARGE_INTEGER LastChangeTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER AllocatedLength;
    LARGE_INTEGER FileSize;
    ULONG FileAttributes;
    union
    {
        ULONG ReparsePointTag;
        ULONG EaSize;
    };
    FILE_INTERNAL_INFORMATION FileId;
    FILE_INTERNAL_INFORMATION ParentFileId;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_EXTENDED_INFORMATION, *PFILE_NOTIFY_EXTENDED_INFORMATION;
#endif

#define FILE_NAME_FLAG_HARDLINK      0
#define FILE_NAME_FLAG_NTFS          0x01
#define FILE_NAME_FLAG_DOS           0x02
#define FILE_NAME_FLAG_BOTH          0x03
#define FILE_NAME_FLAGS_UNSPECIFIED  0x80

#if !defined(NTDDI_WIN10_NI) || (NTDDI_VERSION < NTDDI_WIN10_NI)
_Struct_size_bytes_(NextEntryOffset)
typedef struct _FILE_NOTIFY_FULL_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG Action;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastModificationTime;
    LARGE_INTEGER LastChangeTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER AllocatedLength;
    LARGE_INTEGER FileSize;
    ULONG FileAttributes;
    union
    {
        ULONG ReparsePointTag;
        ULONG EaSize;
    };
    FILE_INTERNAL_INFORMATION FileId;
    FILE_INTERNAL_INFORMATION ParentFileId;
    USHORT FileNameLength;
    BYTE FileNameFlags;
    BYTE Reserved;
    WCHAR FileName[1];
} FILE_NOTIFY_FULL_INFORMATION, *PFILE_NOTIFY_FULL_INFORMATION;
#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_10_RS3)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeDirectoryFileEx(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_ ULONG CompletionFilter,
    _In_ BOOLEAN WatchTree,
    _In_opt_ DIRECTORY_NOTIFY_INFORMATION_CLASS DirectoryNotifyInformationClass
    );
#endif

/**
 * The NtLoadDriver function loads a driver specified by the DriverServiceName parameter.
 *
 * \param DriverServiceName A pointer to a UNICODE_STRING structure that specifies the name of the driver service to load.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadDriver(
    _In_ PCUNICODE_STRING DriverServiceName
    );

/**
 * The NtUnloadDriver function unloads a driver specified by the DriverServiceName parameter.
 *
 * \param DriverServiceName A pointer to a UNICODE_STRING structure that specifies the name of the driver service to unload.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadDriver(
    _In_ PCUNICODE_STRING DriverServiceName
    );


#ifndef IO_COMPLETION_QUERY_STATE
#define IO_COMPLETION_QUERY_STATE 0x0001
#endif

#ifndef IO_COMPLETION_MODIFY_STATE
#define IO_COMPLETION_MODIFY_STATE 0x0002
#endif

#ifndef IO_COMPLETION_ALL_ACCESS
#define IO_COMPLETION_ALL_ACCESS (IO_COMPLETION_QUERY_STATE|IO_COMPLETION_MODIFY_STATE|STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE)
#endif

typedef enum _IO_COMPLETION_INFORMATION_CLASS
{
    IoCompletionBasicInformation
} IO_COMPLETION_INFORMATION_CLASS;

typedef struct _IO_COMPLETION_BASIC_INFORMATION
{
    LONG Depth;
} IO_COMPLETION_BASIC_INFORMATION, *PIO_COMPLETION_BASIC_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateIoCompletion(
    _Out_ PHANDLE IoCompletionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ ULONG NumberOfConcurrentThreads
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenIoCompletion(
    _Out_ PHANDLE IoCompletionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryIoCompletion(
    _In_ HANDLE IoCompletionHandle,
    _In_ IO_COMPLETION_INFORMATION_CLASS IoCompletionInformationClass,
    _Out_writes_bytes_(IoCompletionInformationLength) PVOID IoCompletionInformation,
    _In_ ULONG IoCompletionInformationLength,
    _Out_opt_ PULONG ReturnLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetIoCompletion(
    _In_ HANDLE IoCompletionHandle,
    _In_opt_ PVOID KeyContext,
    _In_opt_ PVOID ApcContext,
    _In_ NTSTATUS IoStatus,
    _In_ ULONG_PTR IoStatusInformation
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetIoCompletionEx(
    _In_ HANDLE IoCompletionHandle,
    _In_ HANDLE IoCompletionPacketHandle,
    _In_opt_ PVOID KeyContext,
    _In_opt_ PVOID ApcContext,
    _In_ NTSTATUS IoStatus,
    _In_ ULONG_PTR IoStatusInformation
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRemoveIoCompletion(
    _In_ HANDLE IoCompletionHandle,
    _Out_ PVOID *KeyContext,
    _Out_ PVOID *ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER Timeout
    );

typedef struct _FILE_IO_COMPLETION_INFORMATION
{
    PVOID KeyContext;
    PVOID ApcContext;
    IO_STATUS_BLOCK IoStatusBlock;
} FILE_IO_COMPLETION_INFORMATION, *PFILE_IO_COMPLETION_INFORMATION;

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRemoveIoCompletionEx(
    _In_ HANDLE IoCompletionHandle,
    _Out_writes_to_(Count, *NumEntriesRemoved) PFILE_IO_COMPLETION_INFORMATION IoCompletionInformation,
    _In_ ULONG Count,
    _Out_ PULONG NumEntriesRemoved,
    _In_opt_ PLARGE_INTEGER Timeout,
    _In_ BOOLEAN Alertable
    );


#if (PHNT_VERSION >= PHNT_WINDOWS_8)

/**
 * The NtCreateWaitCompletionPacket routine creates a wait completion packet object.
 *
 * A wait completion packet is a kernel object that can be associated with wait
 * or I/O completion sources and later queried via I/O completion mechanisms.
 *
 * \param WaitCompletionPacketHandle Pointer to a variable that receives a handle
 *        to the newly created wait completion packet object.
 * \param DesiredAccess The access mask that specifies the requested access to
 *        the wait completion packet object.
 * \param ObjectAttributes Optional pointer to an OBJECT_ATTRIBUTES structure that
 *        supplies the object name and other attributes. May be NULL.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateWaitCompletionPacket(
    _Out_ PHANDLE WaitCompletionPacketHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ PCOBJECT_ATTRIBUTES ObjectAttributes
    );

/**
 * The NtAssociateWaitCompletionPacket routine associates a wait completion packet
 * with an I/O completion port or other target object so that a completion packet
 * will be queued when the target object becomes signaled or an I/O completes.
 *
 * This routine links the specified wait completion packet with the target
 * completion object so the packet will carry the supplied context and status
 * information when it is delivered.
 *
 * \param[in] WaitCompletionPacketHandle Handle to the wait completion packet object.
 * \param[in] IoCompletionHandle Handle to an I/O completion port (or compatible object)
 *        with which the wait completion packet should be associated.
 * \param[in] TargetObjectHandle Handle to the object to watch for completion or
 *        signalling (for example, a waitable kernel object).
 * \param[in] KeyContext Optional pointer to caller-specified context that will be
 *        stored in the completion packet and returned to the consumer.
 * \param[in] ApcContext Optional pointer to caller-specified APC/context value that
 *        will be stored in the completion packet and returned to the consumer.
 * \param[in] IoStatus The NTSTATUS value to be placed in the completion packet.
 * \param[in] IoStatusInformation Additional information ( ULONG_PTR ) to be placed
 *        in the completion packet (commonly used for number of bytes transferred).
 * \param[out] AlreadySignaled Optional pointer to a BOOLEAN that, on return, is set
 *        to TRUE if the packet was already signaled at the time of association;
 *        otherwise FALSE. May be NULL.
 * \return NTSTATUS Successful or errant status.
 * \remarks Use this routine to arrange for notification of a target object's
 *          completion by queuing a wait completion packet containing the
 *          supplied context and status.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtAssociateWaitCompletionPacket(
    _In_ HANDLE WaitCompletionPacketHandle,
    _In_ HANDLE IoCompletionHandle,
    _In_ HANDLE TargetObjectHandle,
    _In_opt_ PVOID KeyContext,
    _In_opt_ PVOID ApcContext,
    _In_ NTSTATUS IoStatus,
    _In_ ULONG_PTR IoStatusInformation,
    _Out_opt_ PBOOLEAN AlreadySignaled
    );

/**
 * The NtCancelWaitCompletionPacket routine cancels a previously associated wait
 * completion packet or removes a signaled packet from its queue.
 *
 * \param[in] WaitCompletionPacketHandle Handle to the wait completion packet object to cancel.
 * \param[in] RemoveSignaledPacket If TRUE and the packet is already signaled, remove
 *        the signaled packet from the target queue; if FALSE, cancellation will
 *        prevent future signaling but will not remove an already queued packet.
 * \return NTSTATUS Successful or errant status.
 * \remarks After successful cancellation, the wait completion packet will no
 *          longer be delivered as a result of the previously associated target.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCancelWaitCompletionPacket(
    _In_ HANDLE WaitCompletionPacketHandle,
    _In_ BOOLEAN RemoveSignaledPacket
    );

#endif

#if (PHNT_VERSION >= PHNT_WINDOWS_11)
/**
 * The NtCopyFileChunk routine copies a contiguous range of bytes (a chunk)
 * from a source file to a destination file. The operation may be performed
 * synchronously or asynchronously depending on the file handles and flags.
 *
 * \param[in] SourceHandle Handle to the source file. The handle must be opened
 *            with access that allows reading the specified range.
 * \param[in] DestinationHandle Handle to the destination file. The handle must
 *            be opened with access that allows writing to the specified range.
 * \param[in] EventHandle Optional handle to an event object. If provided,
 *            the event is set when the operation completes. May be NULL.
 * \param[out] IoStatusBlock Pointer to an IO_STATUS_BLOCK structure that
 *            receives the final completion status and information about the operation.
 * \param[in] Length The number of bytes to copy.
 * \param[in] SourceOffset Pointer to a LARGE_INTEGER specifying the byte
 *            offset in the source file at which copying begins.
 * \param[in] DestOffset Pointer to a LARGE_INTEGER specifying the byte
 *            offset in the destination file at which copying begins.
 * \param[in] SourceKey Optional pointer to a source file key. May be NULL.
 * \param[in] DestKey Optional pointer to a destination file key. May be NULL.
 * \param[in] Flags Additional flags controlling copy semantics.
 * \return NTSTATUS Successful or errant status.
 * \remarks The exact meaning and required privileges for `Flags` and keys may
 *          depend on the Windows version and file system. Consumers should
 *          verify handle access rights and the platform's support for this call.
 */
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCopyFileChunk(
    _In_ HANDLE SourceHandle,
    _In_ HANDLE DestinationHandle,
    _In_opt_ HANDLE EventHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG Length,
    _In_ PLARGE_INTEGER SourceOffset,
    _In_ PLARGE_INTEGER DestOffset,
    _In_opt_ PULONG SourceKey,
    _In_opt_ PULONG DestKey,
    _In_ ULONG Flags
    );
#endif


#if (PHNT_VERSION >= PHNT_WINDOWS_11)
NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateIoRing(
    _Out_ PHANDLE IoRingHandle,
    _In_ ULONG CreateParametersLength,
    _In_ PVOID CreateParameters,
    _In_ ULONG OutputParametersLength,
    _Out_ PVOID OutputParameters
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSubmitIoRing(
    _In_ HANDLE IoRingHandle,
    _In_ ULONG Flags,
    _In_opt_ ULONG WaitOperations,
    _In_opt_ PLARGE_INTEGER Timeout
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryIoRingCapabilities(
    _In_ SIZE_T IoRingCapabilitiesLength,
    _Out_ PVOID IoRingCapabilities
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationIoRing(
    _In_ HANDLE IoRingHandle,
    _In_ ULONG IoRingInformationClass,
    _In_ ULONG IoRingInformationLength,
    _In_ PVOID IoRingInformation
    );
#endif


typedef enum _INTERFACE_TYPE
{
    InterfaceTypeUndefined = -1,
    Internal = 0,
    Isa = 1,
    Eisa = 2,
    MicroChannel = 3,
    TurboChannel = 4,
    PCIBus = 5,
    VMEBus = 6,
    NuBus = 7,
    PCMCIABus = 8,
    CBus = 9,
    MPIBus = 10,
    MPSABus = 11,
    ProcessorInternal = 12,
    InternalPowerBus = 13,
    PNPISABus = 14,
    PNPBus = 15,
    Vmcs = 16,
    ACPIBus = 17,
    MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;

typedef enum _DMA_WIDTH
{
    Width8Bits,
    Width16Bits,
    Width32Bits,
    Width64Bits,
    WidthNoWrap,
    MaximumDmaWidth
} DMA_WIDTH, *PDMA_WIDTH;

typedef enum _DMA_SPEED
{
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    TypeF,
    MaximumDmaSpeed
} DMA_SPEED, *PDMA_SPEED;

typedef enum _BUS_DATA_TYPE
{
    ConfigurationSpaceUndefined = -1,
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    PNPISAConfiguration,
    SgiInternalConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;



#define SYMLINK_FLAG_RELATIVE 0x00000001
#define SYMLINK_DIRECTORY 0x80000000
#define SYMLINK_FILE 0x40000000

typedef struct _REPARSE_DATA_BUFFER
{
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;

    _Field_size_bytes_(ReparseDataLength)
    union
    {
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG Flags;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct
        {
            ULONG StringCount;
            WCHAR StringList[1];
        } AppExecLinkReparseBuffer;
        struct
        {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE UFIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)


typedef struct _REPARSE_DATA_BUFFER_EX
{
    ULONG Flags;


    ULONG ExistingReparseTag;


    GUID ExistingReparseGuid;

    ULONGLONG Reserved;

    union
    {
        REPARSE_DATA_BUFFER ReparseDataBuffer;
        REPARSE_GUID_DATA_BUFFER ReparseGuidDataBuffer;
    };
} REPARSE_DATA_BUFFER_EX, *PREPARSE_DATA_BUFFER_EX;

#define REPARSE_DATA_EX_FLAG_GIVEN_TAG_OR_NONE              (0x00000001)

#define REPARSE_GUID_DATA_BUFFER_EX_HEADER_SIZE \
    UFIELD_OFFSET(REPARSE_DATA_BUFFER_EX, ReparseGuidDataBuffer.GenericReparseBuffer)

#define REPARSE_DATA_BUFFER_EX_HEADER_SIZE \
    UFIELD_OFFSET(REPARSE_DATA_BUFFER_EX, ReparseDataBuffer.GenericReparseBuffer)

#define DEVICE_NAMED_PIPE L"\\Device\\NamedPipe\\"

#define FSCTL_PIPE_ASSIGN_EVENT             CTL_CODE(FILE_DEVICE_NAMED_PIPE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_DISCONNECT               CTL_CODE(FILE_DEVICE_NAMED_PIPE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_LISTEN                   CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_PEEK                     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 3, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_PIPE_QUERY_EVENT              CTL_CODE(FILE_DEVICE_NAMED_PIPE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_TRANSCEIVE               CTL_CODE(FILE_DEVICE_NAMED_PIPE, 5, METHOD_NEITHER,  FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_PIPE_WAIT                     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_IMPERSONATE              CTL_CODE(FILE_DEVICE_NAMED_PIPE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SET_CLIENT_PROCESS       CTL_CODE(FILE_DEVICE_NAMED_PIPE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_QUERY_CLIENT_PROCESS     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_GET_PIPE_ATTRIBUTE       CTL_CODE(FILE_DEVICE_NAMED_PIPE, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SET_PIPE_ATTRIBUTE       CTL_CODE(FILE_DEVICE_NAMED_PIPE, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_GET_CONNECTION_ATTRIBUTE CTL_CODE(FILE_DEVICE_NAMED_PIPE, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SET_CONNECTION_ATTRIBUTE CTL_CODE(FILE_DEVICE_NAMED_PIPE, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_GET_HANDLE_ATTRIBUTE     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SET_HANDLE_ATTRIBUTE     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_FLUSH                    CTL_CODE(FILE_DEVICE_NAMED_PIPE, 16, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_PIPE_DISABLE_IMPERSONATE      CTL_CODE(FILE_DEVICE_NAMED_PIPE, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_PIPE_SILO_ARRIVAL             CTL_CODE(FILE_DEVICE_NAMED_PIPE, 18, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_PIPE_CREATE_SYMLINK           CTL_CODE(FILE_DEVICE_NAMED_PIPE, 19, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_PIPE_DELETE_SYMLINK           CTL_CODE(FILE_DEVICE_NAMED_PIPE, 20, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define FSCTL_PIPE_QUERY_CLIENT_PROCESS_V2  CTL_CODE(FILE_DEVICE_NAMED_PIPE, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define FSCTL_PIPE_INTERNAL_READ            CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2045, METHOD_BUFFERED, FILE_READ_DATA)
#define FSCTL_PIPE_INTERNAL_WRITE           CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2046, METHOD_BUFFERED, FILE_WRITE_DATA)
#define FSCTL_PIPE_INTERNAL_TRANSCEIVE      CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2047, METHOD_NEITHER, FILE_READ_DATA | FILE_WRITE_DATA)
#define FSCTL_PIPE_INTERNAL_READ_OVFLOW     CTL_CODE(FILE_DEVICE_NAMED_PIPE, 2048, METHOD_BUFFERED, FILE_READ_DATA)


#define FILE_PIPE_READ_DATA 0x00000000
#define FILE_PIPE_WRITE_SPACE 0x00000001

typedef struct _FILE_PIPE_ASSIGN_EVENT_BUFFER
{
    HANDLE EventHandle;
    ULONG KeyValue;
} FILE_PIPE_ASSIGN_EVENT_BUFFER, *PFILE_PIPE_ASSIGN_EVENT_BUFFER;

typedef struct _FILE_PIPE_PEEK_BUFFER
{
    ULONG NamedPipeState;
    ULONG ReadDataAvailable;
    ULONG NumberOfMessages;
    ULONG MessageLength;
    _Field_size_bytes_(MessageLength) CHAR Data[1];
} FILE_PIPE_PEEK_BUFFER, *PFILE_PIPE_PEEK_BUFFER;

typedef struct _FILE_PIPE_EVENT_BUFFER
{
    ULONG NamedPipeState;
    ULONG EntryType;
    ULONG ByteCount;
    ULONG KeyValue;
    ULONG NumberRequests;
} FILE_PIPE_EVENT_BUFFER, *PFILE_PIPE_EVENT_BUFFER;

typedef struct _FILE_PIPE_WAIT_FOR_BUFFER
{
    LARGE_INTEGER Timeout;
    ULONG NameLength;
    BOOLEAN TimeoutSpecified;
    _Field_size_bytes_(NameLength) WCHAR Name[1];
} FILE_PIPE_WAIT_FOR_BUFFER, *PFILE_PIPE_WAIT_FOR_BUFFER;

typedef struct _FILE_PIPE_CLIENT_PROCESS_BUFFER
{
#if !defined(BUILD_WOW6432)
    PVOID ClientSession;
    PVOID ClientProcess;
#else
    ULONGLONG ClientSession;
    ULONGLONG ClientProcess;
#endif
} FILE_PIPE_CLIENT_PROCESS_BUFFER, *PFILE_PIPE_CLIENT_PROCESS_BUFFER;


typedef struct _FILE_PIPE_CLIENT_PROCESS_BUFFER_V2
{
     ULONGLONG ClientSession;
#if !defined(BUILD_WOW6432)
     PVOID ClientProcess;
#else
     ULONGLONG ClientProcess;
#endif
} FILE_PIPE_CLIENT_PROCESS_BUFFER_V2, *PFILE_PIPE_CLIENT_PROCESS_BUFFER_V2;

#define FILE_PIPE_COMPUTER_NAME_LENGTH 15

typedef struct _FILE_PIPE_CLIENT_PROCESS_BUFFER_EX
{
#if !defined(BUILD_WOW6432)
    PVOID ClientSession;
    PVOID ClientProcess;
#else
    ULONGLONG ClientSession;
    ULONGLONG ClientProcess;
#endif
    USHORT ClientComputerNameLength;
    WCHAR ClientComputerBuffer[FILE_PIPE_COMPUTER_NAME_LENGTH + 1];
} FILE_PIPE_CLIENT_PROCESS_BUFFER_EX, *PFILE_PIPE_CLIENT_PROCESS_BUFFER_EX;


typedef struct _FILE_PIPE_SILO_ARRIVAL_INPUT
{
    HANDLE JobHandle;
} FILE_PIPE_SILO_ARRIVAL_INPUT, *PFILE_PIPE_SILO_ARRIVAL_INPUT;


#define FILE_PIPE_SYMLINK_FLAG_GLOBAL   0x1

#define FILE_PIPE_SYMLINK_FLAG_RELATIVE 0x2

#define FILE_PIPE_SYMLINK_VALID_FLAGS \
    (FILE_PIPE_SYMLINK_FLAG_GLOBAL | FILE_PIPE_SYMLINK_FLAG_RELATIVE)


typedef struct _FILE_PIPE_CREATE_SYMLINK_INPUT
{
    USHORT NameOffset;
    USHORT NameLength;
    USHORT SubstituteNameOffset;
    USHORT SubstituteNameLength;
    ULONG Flags;
} FILE_PIPE_CREATE_SYMLINK_INPUT, *PFILE_PIPE_CREATE_SYMLINK_INPUT;


typedef struct _FILE_PIPE_DELETE_SYMLINK_INPUT
{
    USHORT NameOffset;
    USHORT NameLength;
} FILE_PIPE_DELETE_SYMLINK_INPUT, *PFILE_PIPE_DELETE_SYMLINK_INPUT;


#define MAILSLOT_CLASS_FIRSTCLASS 1
#define MAILSLOT_CLASS_SECONDCLASS 2

#define FSCTL_MAILSLOT_PEEK             CTL_CODE(FILE_DEVICE_MAILSLOT, 0, METHOD_NEITHER, FILE_READ_DATA)

typedef struct _FILE_MAILSLOT_PEEK_BUFFER
{
    ULONG ReadDataAvailable;
    ULONG NumberOfMessages;
    ULONG MessageLength;
} FILE_MAILSLOT_PEEK_BUFFER, *PFILE_MAILSLOT_PEEK_BUFFER;


#define MOUNTMGR_DEVICE_NAME L"\\Device\\MountPointManager"
#define MOUNTMGRCONTROLTYPE 0x0000006D
#define MOUNTDEVCONTROLTYPE 0x0000004D

#define IOCTL_MOUNTMGR_CREATE_POINT                 CTL_CODE(MOUNTMGRCONTROLTYPE, 0, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_DELETE_POINTS                CTL_CODE(MOUNTMGRCONTROLTYPE, 1, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_QUERY_POINTS                 CTL_CODE(MOUNTMGRCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTMGR_DELETE_POINTS_DBONLY         CTL_CODE(MOUNTMGRCONTROLTYPE, 3, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_NEXT_DRIVE_LETTER            CTL_CODE(MOUNTMGRCONTROLTYPE, 4, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_AUTO_DL_ASSIGNMENTS          CTL_CODE(MOUNTMGRCONTROLTYPE, 5, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_CREATED   CTL_CODE(MOUNTMGRCONTROLTYPE, 6, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_VOLUME_MOUNT_POINT_DELETED   CTL_CODE(MOUNTMGRCONTROLTYPE, 7, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_CHANGE_NOTIFY                CTL_CODE(MOUNTMGRCONTROLTYPE, 8, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MOUNTMGR_KEEP_LINKS_WHEN_OFFLINE      CTL_CODE(MOUNTMGRCONTROLTYPE, 9, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_CHECK_UNPROCESSED_VOLUMES    CTL_CODE(MOUNTMGRCONTROLTYPE, 10, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MOUNTMGR_VOLUME_ARRIVAL_NOTIFICATION  CTL_CODE(MOUNTMGRCONTROLTYPE, 11, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MOUNTMGR_QUERY_DOS_VOLUME_PATH        CTL_CODE(MOUNTMGRCONTROLTYPE, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTMGR_QUERY_DOS_VOLUME_PATHS       CTL_CODE(MOUNTMGRCONTROLTYPE, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTMGR_SCRUB_REGISTRY               CTL_CODE(MOUNTMGRCONTROLTYPE, 14, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_QUERY_AUTO_MOUNT             CTL_CODE(MOUNTMGRCONTROLTYPE, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MOUNTMGR_SET_AUTO_MOUNT               CTL_CODE(MOUNTMGRCONTROLTYPE, 16, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_BOOT_DL_ASSIGNMENT           CTL_CODE(MOUNTMGRCONTROLTYPE, 17, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_TRACELOG_CACHE               CTL_CODE(MOUNTMGRCONTROLTYPE, 18, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_MOUNTMGR_PREPARE_VOLUME_DELETE        CTL_CODE(MOUNTMGRCONTROLTYPE, 19, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_CANCEL_VOLUME_DELETE         CTL_CODE(MOUNTMGRCONTROLTYPE, 20, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_MOUNTMGR_SILO_ARRIVAL                 CTL_CODE(MOUNTMGRCONTROLTYPE, 21, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOCTL_MOUNTDEV_QUERY_DEVICE_NAME            CTL_CODE(MOUNTDEVCONTROLTYPE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MOUNTMGR_CREATE_POINT_INPUT
{
    USHORT SymbolicLinkNameOffset;
    USHORT SymbolicLinkNameLength;
    USHORT DeviceNameOffset;
    USHORT DeviceNameLength;
} MOUNTMGR_CREATE_POINT_INPUT, *PMOUNTMGR_CREATE_POINT_INPUT;

typedef struct _MOUNTMGR_MOUNT_POINT
{
    ULONG SymbolicLinkNameOffset;
    USHORT SymbolicLinkNameLength;
    USHORT Reserved1;
    ULONG UniqueIdOffset;
    USHORT UniqueIdLength;
    USHORT Reserved2;
    ULONG DeviceNameOffset;
    USHORT DeviceNameLength;
    USHORT Reserved3;
} MOUNTMGR_MOUNT_POINT, *PMOUNTMGR_MOUNT_POINT;

_Struct_size_bytes_(Size)
typedef struct _MOUNTMGR_MOUNT_POINTS
{
    ULONG Size;
    ULONG NumberOfMountPoints;
    _Field_size_(NumberOfMountPoints) MOUNTMGR_MOUNT_POINT MountPoints[1];
} MOUNTMGR_MOUNT_POINTS, *PMOUNTMGR_MOUNT_POINTS;

typedef struct _MOUNTMGR_DRIVE_LETTER_TARGET
{
    USHORT DeviceNameLength;
    _Field_size_bytes_(DeviceNameLength) WCHAR DeviceName[1];
} MOUNTMGR_DRIVE_LETTER_TARGET, *PMOUNTMGR_DRIVE_LETTER_TARGET;

typedef struct _MOUNTMGR_DRIVE_LETTER_INFORMATION
{
    BOOLEAN DriveLetterWasAssigned;
    UCHAR CurrentDriveLetter;
} MOUNTMGR_DRIVE_LETTER_INFORMATION, *PMOUNTMGR_DRIVE_LETTER_INFORMATION;

typedef struct _MOUNTMGR_VOLUME_MOUNT_POINT
{
    USHORT SourceVolumeNameOffset;
    USHORT SourceVolumeNameLength;
    USHORT TargetVolumeNameOffset;
    USHORT TargetVolumeNameLength;
} MOUNTMGR_VOLUME_MOUNT_POINT, *PMOUNTMGR_VOLUME_MOUNT_POINT;

typedef struct _MOUNTMGR_CHANGE_NOTIFY_INFO
{
    ULONG EpicNumber;
} MOUNTMGR_CHANGE_NOTIFY_INFO, *PMOUNTMGR_CHANGE_NOTIFY_INFO;

typedef struct _MOUNTMGR_TARGET_NAME
{
    USHORT DeviceNameLength;
    _Field_size_bytes_(DeviceNameLength) WCHAR DeviceName[1];
} MOUNTMGR_TARGET_NAME, *PMOUNTMGR_TARGET_NAME;

typedef enum _MOUNTMGR_AUTO_MOUNT_STATE
{
    Disabled = 0,
    Enabled
} MOUNTMGR_AUTO_MOUNT_STATE;

typedef struct _MOUNTMGR_QUERY_AUTO_MOUNT
{
    MOUNTMGR_AUTO_MOUNT_STATE CurrentState;
} MOUNTMGR_QUERY_AUTO_MOUNT, *PMOUNTMGR_QUERY_AUTO_MOUNT;

typedef struct _MOUNTMGR_SET_AUTO_MOUNT
{
    MOUNTMGR_AUTO_MOUNT_STATE NewState;
} MOUNTMGR_SET_AUTO_MOUNT, *PMOUNTMGR_SET_AUTO_MOUNT;

typedef struct _MOUNTMGR_SILO_ARRIVAL_INPUT
{
    HANDLE JobHandle;
} MOUNTMGR_SILO_ARRIVAL_INPUT, *PMOUNTMGR_SILO_ARRIVAL_INPUT;

#define MOUNTMGR_IS_DRIVE_LETTER(s) ( \
    (s)->Length == 28 && \
    (s)->Buffer[0] == '\\' && \
    (s)->Buffer[1] == 'D' && \
    (s)->Buffer[2] == 'o' && \
    (s)->Buffer[3] == 's' && \
    (s)->Buffer[4] == 'D' && \
    (s)->Buffer[5] == 'e' && \
    (s)->Buffer[6] == 'v' && \
    (s)->Buffer[7] == 'i' && \
    (s)->Buffer[8] == 'c' && \
    (s)->Buffer[9] == 'e' && \
    (s)->Buffer[10] == 's' && \
    (s)->Buffer[11] == '\\' && \
    (s)->Buffer[12] >= 'A' && \
    (s)->Buffer[12] <= 'Z' && \
    (s)->Buffer[13] == ':')

#define MOUNTMGR_IS_VOLUME_NAME(s) ( \
     ((s)->Length == 96 || ((s)->Length == 98 && (s)->Buffer[48] == '\\')) && \
     (s)->Buffer[0] == '\\' && \
     ((s)->Buffer[1] == '?' || (s)->Buffer[1] == '\\') && \
     (s)->Buffer[2] == '?' && \
     (s)->Buffer[3] == '\\' && \
     (s)->Buffer[4] == 'V' && \
     (s)->Buffer[5] == 'o' && \
     (s)->Buffer[6] == 'l' && \
     (s)->Buffer[7] == 'u' && \
     (s)->Buffer[8] == 'm' && \
     (s)->Buffer[9] == 'e' && \
     (s)->Buffer[10] == '{' && \
     (s)->Buffer[19] == '-' && \
     (s)->Buffer[24] == '-' && \
     (s)->Buffer[29] == '-' && \
     (s)->Buffer[34] == '-' && \
     (s)->Buffer[47] == '}')

typedef struct _MOUNTDEV_NAME
{
    USHORT NameLength;
    _Field_size_bytes_(NameLength) WCHAR Name[1];
} MOUNTDEV_NAME, *PMOUNTDEV_NAME;

typedef struct _MOUNTMGR_VOLUME_PATHS
{
    ULONG MultiSzLength;
    _Field_size_bytes_(MultiSzLength) WCHAR MultiSz[1];
} MOUNTMGR_VOLUME_PATHS, *PMOUNTMGR_VOLUME_PATHS;

#define MOUNTMGR_IS_DOS_VOLUME_NAME(s) ( \
     MOUNTMGR_IS_VOLUME_NAME(s) && \
     (s)->Length == 96 && \
     (s)->Buffer[1] == '\\')

#define MOUNTMGR_IS_DOS_VOLUME_NAME_WB(s) ( \
     MOUNTMGR_IS_VOLUME_NAME(s) && \
     (s)->Length == 98 && \
     (s)->Buffer[1] == '\\')

#define MOUNTMGR_IS_NT_VOLUME_NAME(s) ( \
     MOUNTMGR_IS_VOLUME_NAME(s) && \
     (s)->Length == 96 && \
     (s)->Buffer[1] == '?')

#define MOUNTMGR_IS_NT_VOLUME_NAME_WB(s) ( \
     MOUNTMGR_IS_VOLUME_NAME(s) && \
     (s)->Length == 98 && \
     (s)->Buffer[1] == '?')


#define FLT_PORT_CONNECT 0x0001
#define FLT_PORT_ALL_ACCESS (FLT_PORT_CONNECT | STANDARD_RIGHTS_ALL)

#define FLT_SYMLINK_NAME     L"\\Global??\\FltMgr"
#define FLT_MSG_SYMLINK_NAME L"\\Global??\\FltMgrMsg"
#define FLT_DEVICE_NAME      L"\\FileSystem\\Filters\\FltMgr"
#define FLT_MSG_DEVICE_NAME  L"\\FileSystem\\Filters\\FltMgrMsg"

typedef struct _FLT_CONNECT_CONTEXT
{
    PUNICODE_STRING PortName;
    PUNICODE_STRING64 PortName64;
    USHORT SizeOfContext;
    UCHAR Padding[6];
    _Field_size_bytes_(SizeOfContext) UCHAR Context[ANYSIZE_ARRAY];
} FLT_CONNECT_CONTEXT, *PFLT_CONNECT_CONTEXT;

#define FLT_PORT_EA_NAME "FLTPORT"
#define FLT_PORT_CONTEXT_MAX 0xFFE8

typedef struct _FLT_PORT_FULL_EA
{
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[8];
    FLT_CONNECT_CONTEXT EaValue;
} FLT_PORT_FULL_EA, *PFLT_PORT_FULL_EA;

#define FLT_PORT_FULL_EA_SIZE \
    (sizeof(FILE_FULL_EA_INFORMATION) + (sizeof(FLT_PORT_EA_NAME) - sizeof(ANSI_NULL)))
#define FLT_PORT_FULL_EA_VALUE_SIZE \
    RTL_SIZEOF_THROUGH_FIELD(FLT_CONNECT_CONTEXT, Padding)


#define FLT_CTL_LOAD                CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 1, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define FLT_CTL_UNLOAD              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 2, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define FLT_CTL_LINK_HANDLE         CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 3, METHOD_BUFFERED, FILE_READ_ACCESS)
#define FLT_CTL_ATTACH              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 4, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define FLT_CTL_DETACH              CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 5, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define FLT_CTL_SEND_MESSAGE        CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 6, METHOD_NEITHER, FILE_WRITE_ACCESS)
#define FLT_CTL_GET_MESSAGE         CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 7, METHOD_NEITHER, FILE_READ_ACCESS)
#define FLT_CTL_REPLY_MESSAGE       CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 8, METHOD_NEITHER, FILE_WRITE_ACCESS)

#define FLT_CTL_FIND_FIRST          CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 9, METHOD_BUFFERED, FILE_READ_ACCESS)
#define FLT_CTL_FIND_NEXT           CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 10, METHOD_BUFFERED, FILE_READ_ACCESS)
#define FLT_CTL_GET_INFORMATION     CTL_CODE(FILE_DEVICE_DISK_FILE_SYSTEM, 11, METHOD_BUFFERED, FILE_READ_ACCESS)


typedef struct _FLT_LOAD_PARAMETERS
{
    USHORT FilterNameSize;
    _Field_size_bytes_(FilterNameSize) WCHAR FilterName[ANYSIZE_ARRAY];
} FLT_LOAD_PARAMETERS, *PFLT_LOAD_PARAMETERS;

typedef enum _FLT_LINK_TYPE
{
    FILTER = 0,
    FILTER_INSTANCE = 1,
    FILTER_VOLUME = 2,
    FILTER_MANAGER = 3,
    FILTER_MANAGER_VOLUME = 4,
} FLT_LINK_TYPE, *PFLT_LINK_TYPE;

typedef struct _FLT_LINK
{
    FLT_LINK_TYPE Type;
    ULONG ParametersOffset;
} FLT_LINK, *PFLT_LINK;

typedef struct _FLT_FILTER_PARAMETERS
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset;
} FLT_FILTER_PARAMETERS, *PFLT_FILTER_PARAMETERS;

typedef struct _FLT_INSTANCE_PARAMETERS
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset;
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset;
    USHORT InstanceNameSize;
    USHORT InstanceNameOffset;
} FLT_INSTANCE_PARAMETERS, *PFLT_INSTANCE_PARAMETERS;

typedef struct _FLT_VOLUME_PARAMETERS
{
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset;
} FLT_VOLUME_PARAMETERS, *PFLT_VOLUME_PARAMETERS;

typedef enum _ATTACH_TYPE
{
    AltitudeBased = 0,
    InstanceNameBased = 1,
} ATTACH_TYPE, *PATTACH_TYPE;

typedef struct _FLT_ATTACH
{
    USHORT FilterNameSize;
    USHORT FilterNameOffset;
    USHORT VolumeNameSize;
    USHORT VolumeNameOffset;
    ATTACH_TYPE Type;
    USHORT InstanceNameSize;
    USHORT InstanceNameOffset;
    USHORT AltitudeSize;
    USHORT AltitudeOffset;
} FLT_ATTACH, *PFLT_ATTACH;


#define FSCTL_MUP_GET_UNC_CACHE_INFO                CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MUP_GET_UNC_PROVIDER_LIST             CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MUP_GET_SURROGATE_PROVIDER_LIST       CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MUP_GET_UNC_HARDENING_CONFIGURATION   CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FSCTL_MUP_GET_UNC_HARDENING_CONFIGURATION_FOR_PATH  CTL_CODE(FILE_DEVICE_MULTI_UNC_PROVIDER, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MUP_FSCTL_UNC_CACHE_ENTRY
{
    ULONG TotalLength;
    ULONG UncNameOffset;
    USHORT UncNameLength;
    ULONG ProviderNameOffset;
    USHORT ProviderNameLength;
    ULONG SurrogateNameOffset;
    USHORT SurrogateNameLength;
    ULONG ProviderPriority;
    ULONG EntryTtl;
    WCHAR Strings[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_CACHE_ENTRY, *PMUP_FSCTL_UNC_CACHE_ENTRY;

typedef struct _MUP_FSCTL_UNC_CACHE_INFORMATION
{
    ULONG MaxCacheSize;
    ULONG CurrentCacheSize;
    ULONG EntryTimeout;
    ULONG TotalEntries;
    MUP_FSCTL_UNC_CACHE_ENTRY CacheEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_CACHE_INFORMATION, *PMUP_FSCTL_UNC_CACHE_INFORMATION;

typedef struct _MUP_FSCTL_UNC_PROVIDER_ENTRY
{
    ULONG TotalLength;
    LONG ReferenceCount;
    ULONG ProviderPriority;
    ULONG ProviderState;
    ULONG ProviderId;
    USHORT ProviderNameLength;
    WCHAR ProviderName[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_PROVIDER_ENTRY, *PMUP_FSCTL_UNC_PROVIDER_ENTRY;

typedef struct _MUP_FSCTL_UNC_PROVIDER_INFORMATION
{
    ULONG TotalEntries;
    MUP_FSCTL_UNC_PROVIDER_ENTRY ProviderEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_UNC_PROVIDER_INFORMATION, *PMUP_FSCTL_UNC_PROVIDER_INFORMATION;

typedef struct _MUP_FSCTL_SURROGATE_PROVIDER_ENTRY
{
    ULONG TotalLength;
    LONG ReferenceCount;
    ULONG SurrogateType;
    ULONG SurrogateState;
    ULONG SurrogatePriority;
    USHORT SurrogateNameLength;
    WCHAR SurrogateName[ANYSIZE_ARRAY];
} MUP_FSCTL_SURROGATE_PROVIDER_ENTRY, *PMUP_FSCTL_SURROGATE_PROVIDER_ENTRY;

typedef struct _MUP_FSCTL_SURROGATE_PROVIDER_INFORMATION
{
    ULONG TotalEntries;
    MUP_FSCTL_SURROGATE_PROVIDER_ENTRY SurrogateEntry[ANYSIZE_ARRAY];
} MUP_FSCTL_SURROGATE_PROVIDER_INFORMATION, *PMUP_FSCTL_SURROGATE_PROVIDER_INFORMATION;

typedef struct _MUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY
{
    ULONG NextOffset;
    ULONG PrefixNameOffset;
    USHORT PrefixNameCbLength;
    union
    {
        ULONG RequiredHardeningCapabilities;
        struct
        {
            ULONG RequiresMutualAuth : 1;
            ULONG RequiresIntegrity : 1;
            ULONG RequiresPrivacy : 1;
        };
    };
    ULONGLONG OpenCount;
} MUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY, *PMUP_FSCTL_UNC_HARDENING_PREFIX_TABLE_ENTRY;

typedef struct _MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN
{
    ULONG Size;
    ULONG UncPathOffset;
    USHORT UncPathCbLength;
} MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN, *PMUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_IN;

typedef struct _MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT
{
    ULONG Size;
    union
    {
        ULONG RequiredHardeningCapabilities;
        struct
        {
            ULONG RequiresMutualAuth : 1;
            ULONG RequiresIntegrity : 1;
            ULONG RequiresPrivacy : 1;
        };
    };
} MUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT, *PMUP_FSCTL_QUERY_UNC_HARDENING_CONFIGURATION_OUT;

#if (PHNT_MODE != PHNT_MODE_KERNEL)

#define IRP_MJ_CREATE                                0x00
#define IRP_MJ_CREATE_NAMED_PIPE                     0x01
#define IRP_MJ_CLOSE                                 0x02
#define IRP_MJ_READ                                  0x03
#define IRP_MJ_WRITE                                 0x04
#define IRP_MJ_QUERY_INFORMATION                     0x05
#define IRP_MJ_SET_INFORMATION                       0x06
#define IRP_MJ_QUERY_EA                              0x07
#define IRP_MJ_SET_EA                                0x08
#define IRP_MJ_FLUSH_BUFFERS                         0x09
#define IRP_MJ_QUERY_VOLUME_INFORMATION              0x0a
#define IRP_MJ_SET_VOLUME_INFORMATION                0x0b
#define IRP_MJ_DIRECTORY_CONTROL                     0x0c
#define IRP_MJ_FILE_SYSTEM_CONTROL                   0x0d
#define IRP_MJ_DEVICE_CONTROL                        0x0e
#define IRP_MJ_INTERNAL_DEVICE_CONTROL               0x0f
#define IRP_MJ_SHUTDOWN                              0x10
#define IRP_MJ_LOCK_CONTROL                          0x11
#define IRP_MJ_CLEANUP                               0x12
#define IRP_MJ_CREATE_MAILSLOT                       0x13
#define IRP_MJ_QUERY_SECURITY                        0x14
#define IRP_MJ_SET_SECURITY                          0x15
#define IRP_MJ_POWER                                 0x16
#define IRP_MJ_SYSTEM_CONTROL                        0x17
#define IRP_MJ_DEVICE_CHANGE                         0x18
#define IRP_MJ_QUERY_QUOTA                           0x19
#define IRP_MJ_SET_QUOTA                             0x1a
#define IRP_MJ_PNP                                   0x1b
#define IRP_MJ_PNP_POWER                             IRP_MJ_PNP
#define IRP_MJ_MAXIMUM_FUNCTION                      0x1b
#define IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION   ((UCHAR)-1)
#define IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION   ((UCHAR)-2)
#define IRP_MJ_ACQUIRE_FOR_MOD_WRITE                 ((UCHAR)-3)
#define IRP_MJ_RELEASE_FOR_MOD_WRITE                 ((UCHAR)-4)
#define IRP_MJ_ACQUIRE_FOR_CC_FLUSH                  ((UCHAR)-5)
#define IRP_MJ_RELEASE_FOR_CC_FLUSH                  ((UCHAR)-6)
#define IRP_MJ_QUERY_OPEN                            ((UCHAR)-7)
#define IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE             ((UCHAR)-13)
#define IRP_MJ_NETWORK_QUERY_OPEN                    ((UCHAR)-14)
#define IRP_MJ_MDL_READ                              ((UCHAR)-15)
#define IRP_MJ_MDL_READ_COMPLETE                     ((UCHAR)-16)
#define IRP_MJ_PREPARE_MDL_WRITE                     ((UCHAR)-17)
#define IRP_MJ_MDL_WRITE_COMPLETE                    ((UCHAR)-18)
#define IRP_MJ_VOLUME_MOUNT                          ((UCHAR)-19)
#define IRP_MJ_VOLUME_DISMOUNT                       ((UCHAR)-20)
#define FLT_INTERNAL_OPERATION_COUNT                 22

#define IRP_MN_SCSI_CLASS                   0x01
#define IRP_MN_START_DEVICE                 0x00
#define IRP_MN_QUERY_REMOVE_DEVICE          0x01
#define IRP_MN_REMOVE_DEVICE                0x02
#define IRP_MN_CANCEL_REMOVE_DEVICE         0x03
#define IRP_MN_STOP_DEVICE                  0x04
#define IRP_MN_QUERY_STOP_DEVICE            0x05
#define IRP_MN_CANCEL_STOP_DEVICE           0x06
#define IRP_MN_QUERY_DEVICE_RELATIONS       0x07
#define IRP_MN_QUERY_INTERFACE              0x08
#define IRP_MN_QUERY_CAPABILITIES           0x09
#define IRP_MN_QUERY_RESOURCES              0x0A
#define IRP_MN_QUERY_RESOURCE_REQUIREMENTS  0x0B
#define IRP_MN_QUERY_DEVICE_TEXT            0x0C
#define IRP_MN_FILTER_RESOURCE_REQUIREMENTS 0x0D
#define IRP_MN_READ_CONFIG                  0x0F
#define IRP_MN_WRITE_CONFIG                 0x10
#define IRP_MN_EJECT                        0x11
#define IRP_MN_SET_LOCK                     0x12
#define IRP_MN_QUERY_ID                     0x13
#define IRP_MN_QUERY_PNP_DEVICE_STATE       0x14
#define IRP_MN_QUERY_BUS_INFORMATION        0x15
#define IRP_MN_DEVICE_USAGE_NOTIFICATION    0x16
#define IRP_MN_SURPRISE_REMOVAL             0x17
#define IRP_MN_DEVICE_ENUMERATED            0x19

#define IRP_MN_WAIT_WAKE                    0x00
#define IRP_MN_POWER_SEQUENCE               0x01
#define IRP_MN_SET_POWER                    0x02
#define IRP_MN_QUERY_POWER                  0x03
#define IRP_MN_QUERY_ALL_DATA               0x00
#define IRP_MN_QUERY_SINGLE_INSTANCE        0x01
#define IRP_MN_CHANGE_SINGLE_INSTANCE       0x02
#define IRP_MN_CHANGE_SINGLE_ITEM           0x03
#define IRP_MN_ENABLE_EVENTS                0x04
#define IRP_MN_DISABLE_EVENTS               0x05
#define IRP_MN_ENABLE_COLLECTION            0x06
#define IRP_MN_DISABLE_COLLECTION           0x07
#define IRP_MN_REGINFO                      0x08
#define IRP_MN_EXECUTE_METHOD               0x09
#define IRP_MN_REGINFO_EX                   0x0b

#define FLTFL_CALLBACK_DATA_REISSUE_MASK        0x0000FFFF
#define FLTFL_CALLBACK_DATA_IRP_OPERATION       0x00000001
#define FLTFL_CALLBACK_DATA_FAST_IO_OPERATION   0x00000002
#define FLTFL_CALLBACK_DATA_FS_FILTER_OPERATION 0x00000004
#define FLTFL_CALLBACK_DATA_SYSTEM_BUFFER       0x00000008
#define FLTFL_CALLBACK_DATA_GENERATED_IO        0x00010000
#define FLTFL_CALLBACK_DATA_REISSUED_IO         0x00020000
#define FLTFL_CALLBACK_DATA_DRAINING_IO         0x00040000
#define FLTFL_CALLBACK_DATA_POST_OPERATION      0x00080000
#define FLTFL_CALLBACK_DATA_NEW_SYSTEM_BUFFER   0x00100000
#define FLTFL_CALLBACK_DATA_DIRTY               0x80000000

#define IRP_NOCACHE                     0x00000001
#define IRP_PAGING_IO                   0x00000002
#define IRP_MOUNT_COMPLETION            0x00000002
#define IRP_SYNCHRONOUS_API             0x00000004
#define IRP_ASSOCIATED_IRP              0x00000008
#define IRP_BUFFERED_IO                 0x00000010
#define IRP_DEALLOCATE_BUFFER           0x00000020
#define IRP_INPUT_OPERATION             0x00000040
#define IRP_SYNCHRONOUS_PAGING_IO       0x00000040
#define IRP_CREATE_OPERATION            0x00000080
#define IRP_READ_OPERATION              0x00000100
#define IRP_WRITE_OPERATION             0x00000200
#define IRP_CLOSE_OPERATION             0x00000400
#define IRP_DEFER_IO_COMPLETION         0x00000800
#define IRP_OB_QUERY_NAME               0x00001000
#define IRP_HOLD_DEVICE_QUEUE           0x00002000
#define IRP_UM_DRIVER_INITIATED_IO      0x00400000

#define FO_FILE_OPEN                    0x00000001
#define FO_SYNCHRONOUS_IO               0x00000002
#define FO_ALERTABLE_IO                 0x00000004
#define FO_NO_INTERMEDIATE_BUFFERING    0x00000008
#define FO_WRITE_THROUGH                0x00000010
#define FO_SEQUENTIAL_ONLY              0x00000020
#define FO_CACHE_SUPPORTED              0x00000040
#define FO_NAMED_PIPE                   0x00000080
#define FO_STREAM_FILE                  0x00000100
#define FO_MAILSLOT                     0x00000200
#define FO_GENERATE_AUDIT_ON_CLOSE      0x00000400
#define FO_QUEUE_IRP_TO_THREAD          FO_GENERATE_AUDIT_ON_CLOSE
#define FO_DIRECT_DEVICE_OPEN           0x00000800
#define FO_FILE_MODIFIED                0x00001000
#define FO_FILE_SIZE_CHANGED            0x00002000
#define FO_CLEANUP_COMPLETE             0x00004000
#define FO_TEMPORARY_FILE               0x00008000
#define FO_DELETE_ON_CLOSE              0x00010000
#define FO_OPENED_CASE_SENSITIVE        0x00020000
#define FO_HANDLE_CREATED               0x00040000
#define FO_FILE_FAST_IO_READ            0x00080000
#define FO_RANDOM_ACCESS                0x00100000
#define FO_FILE_OPEN_CANCELLED          0x00200000
#define FO_VOLUME_OPEN                  0x00400000
#define FO_BYPASS_IO_ENABLED            0x00800000
#define FO_REMOTE_ORIGIN                0x01000000
#define FO_DISALLOW_EXCLUSIVE           0x02000000
#define FO_SKIP_COMPLETION_PORT         FO_DISALLOW_EXCLUSIVE
#define FO_SKIP_SET_EVENT               0x04000000
#define FO_SKIP_SET_FAST_IO             0x08000000
#define FO_INDIRECT_WAIT_OBJECT         0x10000000
#define FO_SECTION_MINSTORE_TREATMENT   0x20000000

#define SL_PENDING_RETURNED                0x01
#define SL_ERROR_RETURNED                  0x02
#define SL_INVOKE_ON_CANCEL                0x20
#define SL_INVOKE_ON_SUCCESS               0x40
#define SL_INVOKE_ON_ERROR                 0x80
#define SL_FORCE_ACCESS_CHECK              0x01
#define SL_OPEN_PAGING_FILE                0x02
#define SL_OPEN_TARGET_DIRECTORY           0x04
#define SL_STOP_ON_SYMLINK                 0x08
#define SL_IGNORE_READONLY_ATTRIBUTE       0x40
#define SL_CASE_SENSITIVE                  0x80
#define SL_KEY_SPECIFIED                   0x01
#define SL_OVERRIDE_VERIFY_VOLUME          0x02
#define SL_WRITE_THROUGH                   0x04
#define SL_FT_SEQUENTIAL_WRITE             0x08
#define SL_FORCE_DIRECT_WRITE              0x10
#define SL_REALTIME_STREAM                 0x20
#define SL_PERSISTENT_MEMORY_FIXED_MAPPING 0x20
#define SL_BYPASS_IO                       0x40
#define SL_FORCE_ASYNCHRONOUS              0x01
#define SL_READ_ACCESS_GRANTED             0x01
#define SL_WRITE_ACCESS_GRANTED            0x04
#define SL_FAIL_IMMEDIATELY                0x01
#define SL_EXCLUSIVE_LOCK                  0x02
#define SL_RESTART_SCAN                    0x01
#define SL_RETURN_SINGLE_ENTRY             0x02
#define SL_INDEX_SPECIFIED                 0x04
#define SL_RETURN_ON_DISK_ENTRIES_ONLY     0x08
#define SL_NO_CURSOR_UPDATE                0x10
#define SL_QUERY_DIRECTORY_MASK            0x1b
#define SL_WATCH_TREE                      0x01
#define SL_ALLOW_RAW_MOUNT                 0x01
#define SL_BYPASS_ACCESS_CHECK             0x01
#define SL_INFO_FORCE_ACCESS_CHECK         0x01
#define SL_INFO_IGNORE_READONLY_ATTRIBUTE  0x40

#define DO_VERIFY_VOLUME                0x00000002
#define DO_BUFFERED_IO                  0x00000004
#define DO_EXCLUSIVE                    0x00000008
#define DO_DIRECT_IO                    0x00000010
#define DO_MAP_IO_BUFFER                0x00000020
#define DO_DEVICE_INITIALIZING          0x00000080
#define DO_SHUTDOWN_REGISTERED          0x00000800
#define DO_BUS_ENUMERATED_DEVICE        0x00001000
#define DO_POWER_PAGABLE                0x00002000
#define DO_POWER_INRUSH                 0x00004000
#define DO_DEVICE_TO_BE_RESET           0x04000000
#define DO_DAX_VOLUME                   0x10000000

typedef enum _FS_FILTER_SECTION_SYNC_TYPE
{
    SyncTypeOther = 0,
    SyncTypeCreateSection
} FS_FILTER_SECTION_SYNC_TYPE, *PFS_FILTER_SECTION_SYNC_TYPE;

typedef enum _CREATE_FILE_TYPE
{
    CreateFileTypeNone,
    CreateFileTypeNamedPipe,
    CreateFileTypeMailslot
} CREATE_FILE_TYPE;

typedef struct _NAMED_PIPE_CREATE_PARAMETERS
{
    ULONG NamedPipeType;
    ULONG ReadMode;
    ULONG CompletionMode;
    ULONG MaximumInstances;
    ULONG InboundQuota;
    ULONG OutboundQuota;
    LARGE_INTEGER DefaultTimeout;
    BOOLEAN TimeoutSpecified;
} NAMED_PIPE_CREATE_PARAMETERS, *PNAMED_PIPE_CREATE_PARAMETERS;

typedef struct _MAILSLOT_CREATE_PARAMETERS
{
    ULONG MailslotQuota;
    ULONG MaximumMessageSize;
    LARGE_INTEGER ReadTimeout;
    BOOLEAN TimeoutSpecified;
} MAILSLOT_CREATE_PARAMETERS, *PMAILSLOT_CREATE_PARAMETERS;

typedef struct _OPLOCK_KEY_ECP_CONTEXT
{
    GUID OplockKey;
    ULONG Reserved;
} OPLOCK_KEY_ECP_CONTEXT, *POPLOCK_KEY_ECP_CONTEXT;

typedef struct _OPLOCK_KEY_CONTEXT
{
    USHORT Version;
    USHORT Flags;
    GUID ParentOplockKey;
    GUID TargetOplockKey;
    ULONG Reserved;
} OPLOCK_KEY_CONTEXT, *POPLOCK_KEY_CONTEXT;

#define OPLOCK_KEY_VERSION_WIN7    0x0001
#define OPLOCK_KEY_VERSION_WIN8    0x0002

#define OPLOCK_KEY_FLAG_PARENT_KEY 0x0001
#define OPLOCK_KEY_FLAG_TARGET_KEY 0x0002

#define SUPPORTED_FS_FEATURES_OFFLOAD_READ    0x00000001
#define SUPPORTED_FS_FEATURES_OFFLOAD_WRITE   0x00000002
#define SUPPORTED_FS_FEATURES_QUERY_OPEN      0x00000004
#define SUPPORTED_FS_FEATURES_BYPASS_IO       0x00000008

#define SUPPORTED_FS_FEATURES_VALID_MASK_V3 (SUPPORTED_FS_FEATURES_OFFLOAD_READ | \
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE | \
                                               SUPPORTED_FS_FEATURES_QUERY_OPEN | \
                                               SUPPORTED_FS_FEATURES_BYPASS_IO)
#define SUPPORTED_FS_FEATURES_VALID_MASK_V2 (SUPPORTED_FS_FEATURES_OFFLOAD_READ | \
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE | \
                                               SUPPORTED_FS_FEATURES_QUERY_OPEN)
#define SUPPORTED_FS_FEATURES_VALID_MASK_V1 (SUPPORTED_FS_FEATURES_OFFLOAD_READ | \
                                               SUPPORTED_FS_FEATURES_OFFLOAD_WRITE)

#define SUPPORTED_FS_FEATURES_VALID_MASK SUPPORTED_FS_FEATURES_VALID_MASK_V3

#endif

#endif
