/*
 * Boot Configuration Data (BCD) support functions
 *
 * This file is part of System Informer.
 */

#ifndef _NTBCD_H
#define _NTBCD_H


#ifndef PHNT_INLINE_BCD_GUIDS
DEFINE_GUID(GUID_BAD_MEMORY_GROUP, 0x5189B25C, 0x5558, 0x4BF2, 0xBC, 0xA4, 0x28, 0x9B, 0x11, 0xBD, 0x29, 0xE2);
DEFINE_GUID(GUID_BOOT_LOADER_SETTINGS_GROUP, 0x6EFB52BF, 0x1766, 0x41DB, 0xA6, 0xB3, 0x0E, 0xE5, 0xEF, 0xF7, 0x2B, 0xD7);
DEFINE_GUID(GUID_CURRENT_BOOT_ENTRY, 0xFA926493, 0x6F1C, 0x4193, 0xA4, 0x14, 0x58, 0xF0, 0xB2, 0x45, 0x6D, 0x1E);
DEFINE_GUID(GUID_DEBUGGER_SETTINGS_GROUP, 0x4636856E, 0x540F, 0x4170, 0xA1, 0x30, 0xA8, 0x47, 0x76, 0xF4, 0xC6, 0x54);
DEFINE_GUID(GUID_DEFAULT_BOOT_ENTRY, 0x1CAE1EB7, 0xA0DF, 0x4D4D, 0x98, 0x51, 0x48, 0x60, 0xE3, 0x4E, 0xF5, 0x35);
DEFINE_GUID(GUID_EMS_SETTINGS_GROUP, 0x0CE4991B, 0xE6B3, 0x4B16, 0xB2, 0x3C, 0x5E, 0x0D, 0x92, 0x50, 0xE5, 0xD9);
DEFINE_GUID(GUID_FIRMWARE_BOOTMGR, 0xA5A30FA2, 0x3D06, 0x4E9F, 0xB5, 0xF4, 0xA0, 0x1D, 0xF9, 0xD1, 0xFC, 0xBA);
DEFINE_GUID(GUID_GLOBAL_SETTINGS_GROUP, 0x7EA2E1AC, 0x2E61, 0x4728, 0xAA, 0xA3, 0x89, 0x6D, 0x9D, 0x0A, 0x9F, 0x0E);
DEFINE_GUID(GUID_HYPERVISOR_SETTINGS_GROUP, 0x7FF607E0, 0x4395, 0x11DB, 0xB0, 0xDE, 0x08, 0x00, 0x20, 0x0C, 0x9A, 0x66);
DEFINE_GUID(GUID_KERNEL_DEBUGGER_SETTINGS_GROUP, 0x313E8EED, 0x7098, 0x4586, 0xA9, 0xBF, 0x30, 0x9C, 0x61, 0xF8, 0xD4, 0x49);
DEFINE_GUID(GUID_RESUME_LOADER_SETTINGS_GROUP, 0x1AFA9C49, 0x16AB, 0x4A5C, 0x4A, 0x90, 0x21, 0x28, 0x02, 0xDA, 0x94, 0x60);
DEFINE_GUID(GUID_WINDOWS_BOOTMGR, 0x9DEA862C, 0x5CDD, 0x4E70, 0xAC, 0xC1, 0xF3, 0x2B, 0x34, 0x4D, 0x47, 0x95);
DEFINE_GUID(GUID_WINDOWS_LEGACY_NTLDR, 0x466F5A88, 0x0AF2, 0x4F76, 0x90, 0x38, 0x09, 0x5B, 0x17, 0x0D, 0xC2, 0x1C);
DEFINE_GUID(GUID_WINDOWS_MEMORY_TESTER, 0xB2721D73, 0x1DB4, 0x4C62, 0xBF, 0x78, 0xC5, 0x48, 0xA8, 0x80, 0x14, 0x2D);
DEFINE_GUID(GUID_WINDOWS_OS_TARGET_TEMPLATE_EFI, 0xB012B84D, 0xC47C, 0x4ED5, 0xB7, 0x22, 0xC0, 0xC4, 0x21, 0x63, 0xE5, 0x69);
DEFINE_GUID(GUID_WINDOWS_OS_TARGET_TEMPLATE_PCAT, 0xA1943BBC, 0xEA85, 0x487C, 0x97, 0xC7, 0xC9, 0xED, 0xE9, 0x08, 0xA3, 0x8A);
DEFINE_GUID(GUID_WINDOWS_RESUME_TARGET_TEMPLATE_EFI, 0x0C334284, 0x9A41, 0x4DE1, 0x99, 0xB3, 0xA7, 0xE8, 0x7E, 0x8F, 0xF0, 0x7E);
DEFINE_GUID(GUID_WINDOWS_RESUME_TARGET_TEMPLATE_PCAT, 0x98B02A23, 0x0674, 0x4CE7, 0xBD, 0xAD, 0xE0, 0xA1, 0x5A, 0x8F, 0xF9, 0x7B);
DEFINE_GUID(GUID_WINDOWS_SETUP_EFI, 0x7254A080, 0x1510, 0x4E85, 0xAC, 0x0F, 0xE7, 0xFB, 0x3D, 0x44, 0x47, 0x36);
DEFINE_GUID(GUID_WINDOWS_SETUP_PCAT, 0xCBD971BF, 0xB7B8, 0x4885, 0x95, 0x1A, 0xFA, 0x03, 0x04, 0x4F, 0x5D, 0x71);
DEFINE_GUID(GUID_WINDOWS_SETUP_RAMDISK_OPTIONS, 0xAE5534E0, 0xA924, 0x466C, 0xB8, 0x36, 0x75, 0x85, 0x39, 0xA3, 0xEE, 0x3A);
DEFINE_GUID(GUID_WINDOWS_SETUP_BOOT_ENTRY, 0x7619dcc9, 0xfafe, 0x11d9, 0xb4, 0x11, 0x00, 0x04, 0x76, 0xeb, 0xa2, 0x5f);
DEFINE_GUID(GUID_VHD_BOOT_OPTIONS, 0xa62c8016, 0xca4e, 0x4687, 0x80, 0x32, 0xd6, 0x66, 0xc5, 0x1a, 0x28, 0x0c);
DEFINE_GUID(GUID_VMBFS_BOOT_INSTANCE, 0xc63c9bdf, 0x5fa5, 0x4208, 0xb0, 0x3f, 0x6b, 0x45, 0x8b, 0x36, 0x55, 0x92);
DEFINE_GUID(PARTITION_BASIC_DATA_GUID, 0xebd0a0a2, 0xb9e5, 0x4433, 0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7);
DEFINE_GUID(PARTITION_CLUSTER_GUID, 0xdb97dba9, 0x0840, 0x4bae, 0x97, 0xf0, 0xff, 0xb9, 0xa3, 0x27, 0xc7, 0xe1);
DEFINE_GUID(PARTITION_ENTRY_UNUSED_GUID, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
DEFINE_GUID(PARTITION_LDM_DATA_GUID, 0xaf9b60a0, 0x1431, 0x4f62, 0xbc, 0x68, 0x33, 0x11, 0x71, 0x4a, 0x69, 0xad);
DEFINE_GUID(PARTITION_LDM_METADATA_GUID, 0x5808c8aa, 0x7e8f, 0x42e0, 0x85, 0xd2, 0xe1, 0xe9, 0x04, 0x34, 0xcf, 0xb3);
DEFINE_GUID(PARTITION_MSFT_RECOVERY_GUID, 0xde94bba4, 0x06d1, 0x4d40, 0xa1, 0x6a, 0xbf, 0xd5, 0x01, 0x79, 0xd6, 0xac);
DEFINE_GUID(PARTITION_MSFT_RESERVED_GUID, 0xe3c9e316, 0x0b5c, 0x4db8, 0x81, 0x7d, 0xf9, 0x2d, 0xf0, 0x02, 0x15, 0xae);
DEFINE_GUID(PARTITION_MSFT_SNAPSHOT_GUID, 0xcaddebf1, 0x4400, 0x4de8, 0xb1, 0x03, 0x12, 0x11, 0x7d, 0xcf, 0x3c, 0xcf);
DEFINE_GUID(PARTITION_SPACES_GUID, 0xe75caf8f, 0xf680, 0x4cee, 0xaf, 0xa3, 0xb0, 0x01, 0xe5, 0x6e, 0xfc, 0x2d);
DEFINE_GUID(PARTITION_SYSTEM_GUID, 0xc12a7328, 0xf81f, 0x11d2, 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b);
#else
NTSYSAPI GUID GUID_BAD_MEMORY_GROUP;
NTSYSAPI GUID GUID_BOOT_LOADER_SETTINGS_GROUP;
NTSYSAPI GUID GUID_CURRENT_BOOT_ENTRY;
NTSYSAPI GUID GUID_DEBUGGER_SETTINGS_GROUP;
NTSYSAPI GUID GUID_DEFAULT_BOOT_ENTRY;
NTSYSAPI GUID GUID_EMS_SETTINGS_GROUP;
NTSYSAPI GUID GUID_FIRMWARE_BOOTMGR;
NTSYSAPI GUID GUID_GLOBAL_SETTINGS_GROUP;
NTSYSAPI GUID GUID_HYPERVISOR_SETTINGS_GROUP;
NTSYSAPI GUID GUID_KERNEL_DEBUGGER_SETTINGS_GROUP;
NTSYSAPI GUID GUID_RESUME_LOADER_SETTINGS_GROUP;
NTSYSAPI GUID GUID_WINDOWS_BOOTMGR;
NTSYSAPI GUID GUID_WINDOWS_LEGACY_NTLDR;
NTSYSAPI GUID GUID_WINDOWS_MEMORY_TESTER;
NTSYSAPI GUID GUID_WINDOWS_OS_TARGET_TEMPLATE_EFI;
NTSYSAPI GUID GUID_WINDOWS_OS_TARGET_TEMPLATE_PCAT;
NTSYSAPI GUID GUID_WINDOWS_RESUME_TARGET_TEMPLATE_EFI;
NTSYSAPI GUID GUID_WINDOWS_RESUME_TARGET_TEMPLATE_PCAT;
NTSYSAPI GUID GUID_WINDOWS_SETUP_EFI;
NTSYSAPI GUID GUID_WINDOWS_SETUP_PCAT;
NTSYSAPI GUID GUID_WINDOWS_SETUP_RAMDISK_OPTIONS;
NTSYSAPI GUID GUID_VHD_BOOT_OPTIONS;
NTSYSAPI GUID PARTITION_BASIC_DATA_GUID;
NTSYSAPI GUID PARTITION_CLUSTER_GUID;
NTSYSAPI GUID PARTITION_ENTRY_UNUSED_GUID;
NTSYSAPI GUID PARTITION_LDM_DATA_GUID;
NTSYSAPI GUID PARTITION_LDM_METADATA_GUID;
NTSYSAPI GUID PARTITION_MSFT_RECOVERY_GUID;
NTSYSAPI GUID PARTITION_MSFT_RESERVED_GUID;
NTSYSAPI GUID PARTITION_MSFT_SNAPSHOT_GUID;
NTSYSAPI GUID PARTITION_SPACES_GUID;
NTSYSAPI GUID PARTITION_SYSTEM_GUID;
#endif

/**
 * The BCD_MESSAGE_TYPE enumeration represents the types of messages for BCD operations.
 */
typedef enum _BCD_MESSAGE_TYPE
{
    BCD_MESSAGE_TYPE_NONE,
    BCD_MESSAGE_TYPE_TRACE,
    BCD_MESSAGE_TYPE_INFORMATION,
    BCD_MESSAGE_TYPE_WARNING,
    BCD_MESSAGE_TYPE_ERROR,
    BCD_MESSAGE_TYPE_MAXIMUM
} BCD_MESSAGE_TYPE;

typedef _Function_class_(BCD_MESSAGE_CALLBACK)
VOID NTAPI BCD_MESSAGE_CALLBACK(
    _In_ BCD_MESSAGE_TYPE type,
    _In_ PCWSTR Message
    );
typedef BCD_MESSAGE_CALLBACK* PBCD_MESSAGE_CALLBACK;

/**
 * Sets the logging level and callback function for BCD messages.
 *
 * \param BcdLoggingLevel The logging level to set.
 * \param BcdMessageCallbackRoutine The callback routine for BCD messages.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdSetLogging(
    _In_ BCD_MESSAGE_TYPE BcdLoggingLevel,
    _In_ PBCD_MESSAGE_CALLBACK BcdMessageCallbackRoutine
    );

/**
 * The BcdInitializeBcdSyncMutant function initializes the BCD synchronization mutant.
 */
NTSYSAPI
VOID
NTAPI
BcdInitializeBcdSyncMutant(
    VOID
    );

/**
 * The BcdGetSystemStorePath function retrieves the file name for the BCD.
 *
 * \param BcdSystemStorePath The pointer to receive the system store path.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdGetSystemStorePath(
    _Out_ PWSTR* BcdSystemStorePath
    );

/**
 * The BcdSetSystemStoreDevice function sets the device for the system BCD store.
 *
 * \param SystemPartition The system partition to set.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdSetSystemStoreDevice(
    _In_ PCUNICODE_STRING SystemPartition
    );

/**
 * The BcdOpenSystemStore function opens the BCD system store.
 *
 * \param BcdStoreHandle The handle to receive the system store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdOpenSystemStore(
    _Out_ PHANDLE BcdStoreHandle
    );

/**
 * The BcdOpenStoreFromFile function opens a BCD store from a file.
 *
 * \param BcdFilePath The file path of the BCD store.
 * \param BcdStoreHandle The handle to receive the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdOpenStoreFromFile(
    _In_ PCUNICODE_STRING BcdFilePath,
    _Out_ PHANDLE BcdStoreHandle
    );

/**
 * The BcdCreateStore function creates a BCD store.
 *
 * \param BcdFilePath The file path to create the BCD store.
 * \param BcdStoreHandle The handle to receive the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCreateStore(
    _In_ PCUNICODE_STRING BcdFilePath,
    _Out_ PHANDLE BcdStoreHandle
    );

/**
 * The BcdExportStore function exports the BCD store to a file.
 *
 * \param BcdFilePath The file path to export the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdExportStore(
    _In_ PCUNICODE_STRING BcdFilePath
    );

#if (PHNT_VERSION > PHNT_WINDOWS_11)
/**
 * The BcdExportStoreEx function exports the BCD store to a file with additional flags.
 *
 * \param BcdStoreHandle The handle to the BCD store.
 * \param Flags The flags for exporting the store.
 * \param BcdFilePath The file path to export the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdExportStoreEx(
    _In_ HANDLE BcdStoreHandle,
    _In_ ULONG Flags,
    _In_ PCUNICODE_STRING BcdFilePath
    );
#endif

/**
 * The BcdImportStore function imports a BCD store from a file.
 *
 * \param BcdFilePath The file path to import the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdImportStore(
    _In_ PCUNICODE_STRING BcdFilePath
    );

typedef enum _BCD_IMPORT_FLAGS
{
    BCD_IMPORT_NONE,
    BCD_IMPORT_DELETE_FIRMWARE_OBJECTS
} BCD_IMPORT_FLAGS;

/**
 * The BcdImportStoreWithFlags function imports a BCD store from a file with additional flags.
 *
 * \param BcdFilePath The file path to import the BCD store.
 * \param BcdImportFlags The flags for importing the store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdImportStoreWithFlags(
    _In_ PCUNICODE_STRING BcdFilePath,
    _In_ BCD_IMPORT_FLAGS BcdImportFlags
    );

/**
 * The BcdDeleteObjectReferences function deletes object references in the BCD store.
 *
 * \param BcdStoreHandle The handle to the BCD store.
 * \param Identifier The identifier of the object to delete references for.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdDeleteObjectReferences(
    _In_ HANDLE BcdStoreHandle,
    _In_ PCGUID Identifier
    );

/**
 * The BcdDeleteSystemStore function deletes the system store for BCD.
 *
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdDeleteSystemStore(
    VOID
    );

typedef enum _BCD_OPEN_FLAGS
{
    BCD_OPEN_NONE,
    BCD_OPEN_OPEN_STORE_OFFLINE,
    BCD_OPEN_SYNC_FIRMWARE_ENTRIES
} BCD_OPEN_FLAGS;

/**
 * The BcdOpenStore function opens a BCD store with additional flags.
 *
 * \param BcdFilePath The file path of the BCD store.
 * \param BcdOpenFlags The flags for opening the store.
 * \param BcdStoreHandle The handle to receive the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdOpenStore(
    _In_ PCUNICODE_STRING BcdFilePath,
    _In_ BCD_OPEN_FLAGS BcdOpenFlags,
    _Out_ PHANDLE BcdStoreHandle
    );

/**
 * The BcdCloseStore function closes a BCD store.
 *
 * \param BcdStoreHandle The handle to the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCloseStore(
    _In_ HANDLE BcdStoreHandle
    );

/**
 * The BcdFlushStore function flushes a BCD store.
 *
 * \param BcdStoreHandle The handle to the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdFlushStore(
    _In_ HANDLE BcdStoreHandle
    );

/**
 * The BcdForciblyUnloadStore function forcibly unloads a BCD store.
 *
 * \param BcdStoreHandle The handle to the BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdForciblyUnloadStore(
    _In_ HANDLE BcdStoreHandle
    );

/**
 * The BcdMarkAsSystemStore function marks the specified BCD store as the system store
 * and used by the boot manager and other boot components as the primary source of boot configuration data.
 *
 * \param BcdStoreHandle Handle to the BCD store to be marked as the system store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdMarkAsSystemStore(
    _In_ HANDLE BcdStoreHandle
    );

/**
 * The BCD_OBJECT_TYPE enumeration represents the types of BCD (Boot Configuration Data) objects.
 */
typedef enum _BCD_OBJECT_TYPE
{
    BCD_OBJECT_TYPE_NONE,
    BCD_OBJECT_TYPE_APPLICATION,
    BCD_OBJECT_TYPE_INHERITED,
    BCD_OBJECT_TYPE_DEVICE,
} BCD_OBJECT_TYPE;

/**
 * The BCD_APPLICATION_OBJECT_TYPE enumeration represents the various types
 * of application objects in the Boot Configuration Data (BCD).
 */
typedef enum _BCD_APPLICATION_OBJECT_TYPE
{
    BCD_APPLICATION_OBJECT_NONE = 0,
    BCD_APPLICATION_OBJECT_FIRMWARE_BOOT_MANAGER = 1,
    BCD_APPLICATION_OBJECT_WINDOWS_BOOT_MANAGER = 2,
    BCD_APPLICATION_OBJECT_WINDOWS_BOOT_LOADER = 3,
    BCD_APPLICATION_OBJECT_WINDOWS_RESUME_APPLICATION = 4,
    BCD_APPLICATION_OBJECT_MEMORY_TESTER = 5,
    BCD_APPLICATION_OBJECT_LEGACY_NTLDR = 6,
    BCD_APPLICATION_OBJECT_LEGACY_SETUPLDR = 7,
    BCD_APPLICATION_OBJECT_BOOT_SECTOR = 8,
    BCD_APPLICATION_OBJECT_STARTUP_MODULE = 9,
    BCD_APPLICATION_OBJECT_GENERIC_APPLICATION = 10,
    BCD_APPLICATION_OBJECT_RESERVED = 0xFFFFF
} BCD_APPLICATION_OBJECT_TYPE;

/**
 * The BCD_APPLICATION_IMAGE_TYPE enumeration represents the types of images for BCD applications.
 */
typedef enum _BCD_APPLICATION_IMAGE_TYPE
{
    BCD_APPLICATION_IMAGE_NONE,
    BCD_APPLICATION_IMAGE_FIRMWARE_APPLICATION,
    BCD_APPLICATION_IMAGE_BOOT_APPLICATION,
    BCD_APPLICATION_IMAGE_LEGACY_LOADER,
    BCD_APPLICATION_IMAGE_REALMODE_CODE,
} BCD_APPLICATION_IMAGE_TYPE;

/**
 * The BCD_INHERITED_CLASS_TYPE enumeration represents the classes of inherited BCD objects.
 */
typedef enum _BCD_INHERITED_CLASS_TYPE
{
    BCD_INHERITED_CLASS_NONE,
    BCD_INHERITED_CLASS_LIBRARY,
    BCD_INHERITED_CLASS_APPLICATION,
    BCD_INHERITED_CLASS_DEVICE
} BCD_INHERITED_CLASS_TYPE;

/**
 * Constructs a BCD (Boot Configuration Data) object identifier with the specified
 * object type, image type, and application type into a single ULONG value.
 * \param ObjectType The type of the BCD object (placed in the highest 4 bits).
 * \param ImageType The type of the image (placed in the next 4 bits).
 * \param ApplicationType The type of the application (placed in the lowest 20 bits).
 * \returns A ULONG value representing the combined BCD object identifier.
 * \remarks Bit Layout:
 * | 31 ... 28 | 27 ... 24 | 23 ... 20 | 19 .......... 0 |
 * | ObjectType| Reserved  | ImageType | ApplicationType |
 */
#define MAKE_BCD_OBJECT(ObjectType, ImageType, ApplicationType) \
    (((ULONG)(ObjectType) << 28) | \
    (((ULONG)(ImageType) & 0xF) << 20) | \
    ((ULONG)(ApplicationType) & 0xFFFFF))

/**
 * Constructs a BCD (Boot Configuration Data) application object identifier.
 * This macro creates a BCD object identifier for an application by combining the object type,
 * image type, and application type into a single value using the MAKE_BCD_OBJECT macro.
 * \param ImageType The type of the image (e.g., Windows Boot Loader, Windows Resume Application).
 * \param ApplicationType The specific application type within the image type.
 * \return A value representing the BCD application object identifier.
 */
#define MAKE_BCD_APPLICATION_OBJECT(ImageType, ApplicationType) \
    MAKE_BCD_OBJECT(BCD_OBJECT_TYPE_APPLICATION, (ULONG)(ImageType), (ULONG)(ApplicationType))

/**
 * Macro to extract the BCD (Boot Configuration Data) object type from a given DataType value.
 * The macro shifts the input DataType 28 bits to the right and masks the result with 0xF,
 * effectively extracting the upper 4 bits, which represent the BCD object type.
 * \param DataType The value containing the BCD object type in its upper 4 bits.
 * \return The extracted BCD_OBJECT_TYPE value.
 */
#define GET_BCD_OBJECT_TYPE(DataType) \
    ((BCD_OBJECT_TYPE)(((((ULONG)(DataType))) >> 28) & 0xF))

/**
 * Extracts the BCD (Boot Configuration Data) application image type from a given data type value.
 * This macro shifts the input value 20 bits to the right and masks the result with 0xF (4 bits),
 * effectively extracting bits 20-23, which represent the BCD application image type.
 * \param DataType The value containing the BCD application image type encoded in bits 20-23.
 * \return The extracted BCD_APPLICATION_IMAGE_TYPE value.
 */
#define GET_BCD_APPLICATION_IMAGE(DataType) \
    ((BCD_APPLICATION_IMAGE_TYPE)(((((ULONG)(DataType))) >> 20) & 0xF))

/**
 * Macro to extract the BCD application object type from a given data type value.
 * \param DataType The value containing the BCD application object type information.
 * \return The extracted BCD_APPLICATION_OBJECT_TYPE, obtained by masking the lower 20 bits of DataType.
 */
#define GET_BCD_APPLICATION_OBJECT(DataType) \
    ((BCD_APPLICATION_OBJECT_TYPE)((((ULONG)(DataType))) & 0xFFFFF))

#define BCD_OBJECT_OSLOADER_TYPE \
    MAKE_BCD_APPLICATION_OBJECT(BCD_APPLICATION_IMAGE_BOOT_APPLICATION, BCD_APPLICATION_OBJECT_WINDOWS_BOOT_LOADER)

/**
 * The BCD_OBJECT_DATATYPE provides multiple views of the packed value for different BCD (Boot Configuration Data) objects.
 * \remarks This union allows for flexible interpretation of BCD object data depending on the context in which it is used.
 */
typedef union _BCD_OBJECT_DATATYPE
{
    ULONG PackedValue;
    union
    {
        struct
        {
            ULONG Reserved : 28;
            BCD_OBJECT_TYPE ObjectType : 4;
        };
        struct
        {
            BCD_APPLICATION_OBJECT_TYPE ApplicationType : 20;
            BCD_APPLICATION_IMAGE_TYPE ImageType : 4;
            ULONG Reserved : 4;
            BCD_OBJECT_TYPE ObjectType : 4;
        } Application;
        struct
        {
            ULONG Value : 20;
            BCD_INHERITED_CLASS_TYPE Class : 4;
            ULONG Reserved : 4;
            BCD_OBJECT_TYPE ObjectType : 4;
        } Inherit;
        struct
        {
            ULONG Reserved : 28;
            BCD_OBJECT_TYPE ObjectType : 4;
        } Device;
    };
} BCD_OBJECT_DATATYPE, *PBCD_OBJECT_DATATYPE;

static_assert(sizeof(BCD_OBJECT_DATATYPE) == sizeof(ULONG), "sizeof(BCD_OBJECT_DATATYPE) is invalid.");

#define BCD_OBJECT_DESCRIPTION_VERSION 0x1

/**
 * The BCD_OBJECT_DESCRIPTION structure contains metadata about a BCD object, including its version and type.
 */
typedef struct _BCD_OBJECT_DESCRIPTION
{
    ULONG Version;
    ULONG Type;
} BCD_OBJECT_DESCRIPTION, *PBCD_OBJECT_DESCRIPTION;

/**
 * The BCD_OBJECT structure contains the identifier and description of a BCD object.
 */
typedef struct _BCD_OBJECT
{
    GUID Identifier;
    PBCD_OBJECT_DESCRIPTION Description;
} BCD_OBJECT, *PBCD_OBJECT;

/**
 * The BcdEnumerateObjects function enumerates BCD objects in the specified BCD store.
 *
 * \param BcdStoreHandle Handle to the BCD store.
 * \param BcdEnumDescriptor Pointer to a BCD_OBJECT_DESCRIPTION structure that specifies the type of objects to enumerate.
 * \param Buffer Optional pointer to a buffer that receives an array of BCD_OBJECT structures.
 * \param BufferSize On input, the size of the buffer in bytes. On output, the required or actual size.
 * \param ObjectCount Receives the number of objects returned.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdEnumerateObjects(
    _In_ HANDLE BcdStoreHandle,
    _In_ PBCD_OBJECT_DESCRIPTION BcdEnumDescriptor,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize,
    _Out_ PULONG ObjectCount
    );

/**
 * The BcdOpenObject function opens a BCD object by its identifier.
 *
 * \param BcdStoreHandle Handle to the BCD store.
 * \param Identifier Pointer to the GUID of the object to open.
 * \param BcdObjectHandle Receives the handle to the opened BCD object.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdOpenObject(
    _In_ HANDLE BcdStoreHandle,
    _In_ PCGUID Identifier,
    _Out_ PHANDLE BcdObjectHandle
    );

/**
 * The BcdCreateObject function creates a new BCD object in the specified store.
 *
 * \param BcdStoreHandle Handle to the BCD store.
 * \param Identifier Pointer to the GUID for the new object.
 * \param Description Pointer to a BCD_OBJECT_DESCRIPTION structure describing the object.
 * \param BcdObjectHandle Receives the handle to the created BCD object.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCreateObject(
    _In_ HANDLE BcdStoreHandle,
    _In_ PCGUID Identifier,
    _In_ PBCD_OBJECT_DESCRIPTION Description,
    _Out_ PHANDLE BcdObjectHandle
    );

/**
 * The BcdDeleteObject function deletes a BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object to delete.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdDeleteObject(
    _In_ HANDLE BcdObjectHandle
    );

/**
 * The BcdCloseObject function closes a handle to a BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object to close.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCloseObject(
    _In_ HANDLE BcdObjectHandle
    );

/**
 * The BCD_COPY_FLAGS specify the behavior of Boot Configuration Data (BCD) copy operations,
 * controlling how objects and elements are handled during BCD copy procedures.
 */
_Enum_is_bitflag_
typedef enum _BCD_COPY_FLAGS
{
    BCD_COPY_NONE = 0x0,
    BCD_COPY_COPY_CREATE_NEW_OBJECT_IDENTIFIER = 0x1,
    BCD_COPY_COPY_DELETE_EXISTING_OBJECT = 0x2,
    BCD_COPY_COPY_UNKNOWN_FIRMWARE_APPLICATION = 0x4,
    BCD_COPY_IGNORE_SETUP_TEMPLATE_ELEMENTS = 0x8,
    BCD_COPY_RETAIN_ELEMENT_DATA = 0x10,
    BCD_COPY_MIGRATE_ELEMENT_DATA = 0x20
} BCD_COPY_FLAGS;
DEFINE_ENUM_FLAG_OPERATORS(BCD_COPY_FLAGS);

/**
 * The BcdCopyObject function copies a BCD object from one store to another.
 *
 * \param BcdStoreHandle Handle to the source BCD store.
 * \param BcdObjectHandle Handle to the BCD object to copy.
 * \param BcdCopyFlags Flags that control the copy operation.
 * \param TargetStoreHandle Handle to the target BCD store.
 * \param TargetObjectHandle Receives the handle to the copied BCD object in the target store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCopyObject(
    _In_ HANDLE BcdStoreHandle,
    _In_ HANDLE BcdObjectHandle,
    _In_ BCD_COPY_FLAGS BcdCopyFlags,
    _In_ HANDLE TargetStoreHandle,
    _Out_ PHANDLE TargetObjectHandle
    );

/**
 * The BcdCopyObjectEx function copies a BCD object from one store to another, specifying the target object identifier.
 *
 * \param BcdStoreHandle Handle to the source BCD store.
 * \param BcdObjectHandle Handle to the BCD object to copy.
 * \param BcdCopyFlags Flags that control the copy operation.
 * \param TargetStoreHandle Handle to the target BCD store.
 * \param TargetObjectId Pointer to the GUID for the new object in the target store.
 * \param TargetObjectHandle Receives the handle to the copied BCD object in the target store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCopyObjectEx(
    _In_ HANDLE BcdStoreHandle,
    _In_ HANDLE BcdObjectHandle,
    _In_ BCD_COPY_FLAGS BcdCopyFlags,
    _In_ HANDLE TargetStoreHandle,
    _In_ PCGUID TargetObjectId,
    _Out_ PHANDLE TargetObjectHandle
    );

/**
 * The BcdCopyObjects function copies multiple BCD objects from one store to another based on specified characteristics.
 *
 * \param BcdStoreHandle Handle to the source BCD store.
 * \param Characteristics Pointer to a BCD_OBJECT_DESCRIPTION structure specifying the objects to copy.
 * \param BcdCopyFlags Flags that control the copy operation.
 * \param TargetStoreHandle Handle to the target BCD store.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdCopyObjects(
    _In_ HANDLE BcdStoreHandle,
    _In_ PBCD_OBJECT_DESCRIPTION Characteristics,
    _In_ BCD_COPY_FLAGS BcdCopyFlags,
    _In_ HANDLE TargetStoreHandle
    );

/**
 * The BcdMigrateObjectElementValues function migrates element values from a source BCD object to a target BCD object using a template.
 *
 * \param TemplateObjectHandle Handle to the template BCD object.
 * \param SourceObjectHandle Handle to the source BCD object.
 * \param TargetObjectHandle Handle to the target BCD object.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdMigrateObjectElementValues(
    _In_ HANDLE TemplateObjectHandle,
    _In_ HANDLE SourceObjectHandle,
    _In_ HANDLE TargetObjectHandle
    );

/**
 * The BcdQueryObject function queries a BCD object for its description and identifier.
 *
 * \param BcdObjectHandle Handle to the BCD object to query.
 * \param BcdVersion The version of the BCD object description structure (use BCD_OBJECT_DESCRIPTION_VERSION).
 * \param Description Receives the BCD_OBJECT_DESCRIPTION structure for the object.
 * \param Identifier Receives the GUID identifier of the object.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdQueryObject(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdVersion,
    _Out_ BCD_OBJECT_DESCRIPTION Description,
    _Out_ PGUID Identifier
    );

/**
 * The BCD_ELEMENT_DATATYPE_FORMAT enumeration represents the data formats for BCD elements.
 */
typedef enum _BCD_ELEMENT_DATATYPE_FORMAT
{
    BCD_ELEMENT_DATATYPE_FORMAT_UNKNOWN,
    BCD_ELEMENT_DATATYPE_FORMAT_DEVICE,
    BCD_ELEMENT_DATATYPE_FORMAT_STRING,
    BCD_ELEMENT_DATATYPE_FORMAT_OBJECT,
    BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST,
    BCD_ELEMENT_DATATYPE_FORMAT_INTEGER,
    BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN,
    BCD_ELEMENT_DATATYPE_FORMAT_INTEGERLIST,
    BCD_ELEMENT_DATATYPE_FORMAT_BINARY
} BCD_ELEMENT_DATATYPE_FORMAT;

/**
 * The BCD_ELEMENT_DATATYPE_CLASS enumeration represents the classes for BCD elements.
 */
typedef enum _BCD_ELEMENT_DATATYPE_CLASS
{
    BCD_ELEMENT_DATATYPE_CLASS_NONE,
    BCD_ELEMENT_DATATYPE_CLASS_LIBRARY,
    BCD_ELEMENT_DATATYPE_CLASS_APPLICATION,
    BCD_ELEMENT_DATATYPE_CLASS_DEVICE,
    BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE,
    BCD_ELEMENT_DATATYPE_CLASS_OEM
} BCD_ELEMENT_DATATYPE_CLASS;

/**
 * The BCD_ELEMENT_DEVICE_TYPE enumeration represents the types of devices for BCD elements.
 */
typedef enum _BCD_ELEMENT_DEVICE_TYPE
{
    BCD_ELEMENT_DEVICE_TYPE_NONE,
    BCD_ELEMENT_DEVICE_TYPE_BOOT_DEVICE,
    BCD_ELEMENT_DEVICE_TYPE_PARTITION,
    BCD_ELEMENT_DEVICE_TYPE_FILE,
    BCD_ELEMENT_DEVICE_TYPE_RAMDISK,
    BCD_ELEMENT_DEVICE_TYPE_UNKNOWN,
    BCD_ELEMENT_DEVICE_TYPE_QUALIFIED_PARTITION,
    BCD_ELEMENT_DEVICE_TYPE_VMBUS,
    BCD_ELEMENT_DEVICE_TYPE_LOCATE_DEVICE,
    BCD_ELEMENT_DEVICE_TYPE_URI,
    BCD_ELEMENT_DEVICE_TYPE_COMPOSITE
} BCD_ELEMENT_DEVICE_TYPE;

/**
 * Macro to construct a BCD element data type value from its class, format, and subtype.
 *
 * \param Class The class of the BCD element.
 * \param Format The format of the BCD element data.
 * \param Subtype The subtype identifier for the element.
 * \return A ULONG value representing the combined BCD element data type.
 */
#define MAKE_BCDE_DATA_TYPE(Class, Format, Subtype) \
    (((((ULONG)(Class)) & 0xF) << 28) | ((((ULONG)(Format)) & 0xF) << 24) | (((ULONG)(Subtype)) & 0x00FFFFFF))

/**
 * Macro to extract the BCD element class from a given data type value.
 *
 * \param DataType The value containing the BCD element information.
 * \return The extracted BCD_ELEMENT_DATATYPE_CLASS.
 */
#define GET_BCDE_DATA_CLASS(DataType) \
    ((BCD_ELEMENT_DATATYPE_CLASS)(((((ULONG)(DataType))) >> 28) & 0xF))

/**
 * Macro to extract the BCD element data format from a given data type value.
 *
 * \param DataType The value containing the BCD element information.
 * \return The extracted BCD_ELEMENT_DATATYPE_FORMAT.
 */
#define GET_BCDE_DATA_FORMAT(DataType) \
    ((BCD_ELEMENT_DATATYPE_FORMAT)(((((ULONG)(DataType))) >> 24) & 0xF))

/**
 * Macro to extract the BCD element subtype identifier from a given data type value.
 *
 * \param DataType The value containing the BCD element information.
 * \return The extracted subtype identifier as a ULONG value.
 */
#define GET_BCDE_DATA_SUBTYPE(DataType) \
    ((ULONG)((((ULONG)(DataType))) & 0x00FFFFFF))

/**
 * The BCD_ELEMENT_DATATYPE union represents the data type for a BCD element.
 */
typedef union _BCD_ELEMENT_DATATYPE
{
    ULONG PackedValue;
    struct
    {
        ULONG SubType : 24;
        BCD_ELEMENT_DATATYPE_FORMAT Format : 4;
        BCD_ELEMENT_DATATYPE_CLASS Class : 4;
    };
} BCD_ELEMENT_DATATYPE, *PBCD_ELEMENT_DATATYPE;

static_assert(sizeof(BCD_ELEMENT_DATATYPE) == sizeof(ULONG), "sizeof(BCD_ELEMENT_DATATYPE) is invalid.");

/**
 * The BcdEnumerateElementTypes function enumerates the element types present in a specified BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object whose element types are to be enumerated.
 * \param Buffer Optional pointer to a buffer that receives an array of BCD_ELEMENT_DATATYPE values.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \param ElementCount Receives the number of element types returned in the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdEnumerateElementTypes(
    _In_ HANDLE BcdObjectHandle,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize,
    _Out_ PULONG ElementCount
    );

/**
 * The BCD_ELEMENT_DEVICE_QUALIFIED_PARTITION structure contains information about a qualified partition.
 */
typedef struct _BCD_ELEMENT_DEVICE_QUALIFIED_PARTITION
{
    ULONG PartitionStyle;
    ULONG Reserved;
    struct
    {
        union
        {
            ULONG DiskSignature;
            ULONG64 PartitionOffset;
        } Mbr;
        union
        {
            GUID DiskSignature;
            GUID PartitionSignature;
        } Gpt;
    };
} BCD_ELEMENT_DEVICE_QUALIFIED_PARTITION, *PBCD_ELEMENT_DEVICE_QUALIFIED_PARTITION;

/**
 * The BCD_ELEMENT_DEVICE structure contains information about a BCD device element.
 */
typedef struct _BCD_ELEMENT_DEVICE
{
    ULONG DeviceType;
    GUID AdditionalOptions;
    struct
    {
        union
        {
            ULONG ParentOffset;
            WCHAR Path[ANYSIZE_ARRAY];
        } File;
        union
        {
            WCHAR Path[ANYSIZE_ARRAY];
        } Partition;
        union
        {
            ULONG Type;
            ULONG ParentOffset;
            ULONG ElementType;
            WCHAR Path[ANYSIZE_ARRAY];
        } Locate;
        union
        {
            GUID InterfaceInstance;
        } Vmbus;
        union
        {
            ULONG Data[ANYSIZE_ARRAY];
        } Unknown;
        BCD_ELEMENT_DEVICE_QUALIFIED_PARTITION QualifiedPartition;
    };
} BCD_ELEMENT_DEVICE, *PBCD_ELEMENT_DEVICE;

/**
 * The BCD_ELEMENT_STRING structure contains a string value for a BCD element.
 */
typedef struct _BCD_ELEMENT_STRING
{
    WCHAR Value[ANYSIZE_ARRAY];
} BCD_ELEMENT_STRING, *PBCD_ELEMENT_STRING;

/**
 * The BCD_ELEMENT_OBJECT structure contains a GUID value for a BCD object element.
 */
typedef struct _BCD_ELEMENT_OBJECT
{
    GUID Object;
} BCD_ELEMENT_OBJECT, *PBCD_ELEMENT_OBJECT;

/**
 * The BCD_ELEMENT_OBJECT_LIST structure contains a list of GUIDs for a BCD object list element.
 */
typedef struct _BCD_ELEMENT_OBJECT_LIST
{
    GUID ObjectList[ANYSIZE_ARRAY];
} BCD_ELEMENT_OBJECT_LIST, *PBCD_ELEMENT_OBJECT_LIST;

/**
 * The BCD_ELEMENT_INTEGER structure contains a 64-bit integer value for a BCD element.
 */
typedef struct _BCD_ELEMENT_INTEGER
{
    ULONG64 Value;
} BCD_ELEMENT_INTEGER, *PBCD_ELEMENT_INTEGER;

/**
 * The BCD_ELEMENT_INTEGER_LIST structure contains a list of 64-bit integers for a BCD element.
 */
typedef struct _BCD_ELEMENT_INTEGER_LIST
{
    ULONG64 Value[ANYSIZE_ARRAY];
} BCD_ELEMENT_INTEGER_LIST, *PBCD_ELEMENT_INTEGER_LIST;

/**
 * The BCD_ELEMENT_BOOLEAN structure contains a boolean value for a BCD element.
 */
typedef struct _BCD_ELEMENT_BOOLEAN
{
    BOOLEAN Value;
} BCD_ELEMENT_BOOLEAN, *PBCD_ELEMENT_BOOLEAN;

#define BCD_ELEMENT_DESCRIPTION_VERSION 0x1

/**
 * The BCD_ELEMENT_DESCRIPTION structure contains information about a BCD element.
 */
typedef struct BCD_ELEMENT_DESCRIPTION
{
    ULONG Version;
    ULONG Type;
    ULONG DataSize;
} BCD_ELEMENT_DESCRIPTION, *PBCD_ELEMENT_DESCRIPTION;

/**
 * The BCD_ELEMENT structure contains information about a BCD element.
 */
typedef struct _BCD_ELEMENT
{
    PBCD_ELEMENT_DESCRIPTION Description;
    PVOID Data;
} BCD_ELEMENT, *PBCD_ELEMENT;

/**
 * The BcdEnumerateElements function enumerates the elements present in a specified BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object whose elements are to be enumerated.
 * \param Buffer Optional pointer to a buffer that receives an array of BCD_ELEMENT structures.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \param ElementCount Receives the number of elements returned in the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdEnumerateElements(
    _In_ HANDLE BcdObjectHandle,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize,
    _Out_ PULONG ElementCount
    );

/**
 * The BCD_FLAGS enumeration defines flags that control the behavior of BCD object enumeration,
 * device translation, and policy checks in the context of system boot configuration.
 */
typedef enum _BCD_FLAGS
{
    BCD_FLAG_NONE = 0x0,
    BCD_FLAG_QUALIFIED_PARTITION = 0x1,
    BCD_FLAG_NO_DEVICE_TRANSLATION = 0x2,
    BCD_FLAG_ENUMERATE_INHERITED_OBJECTS = 0x4,
    BCD_FLAG_ENUMERATE_DEVICE_OPTIONS = 0x8,
    BCD_FLAG_OBSERVE_PRECEDENCE = 0x10,
    BCD_FLAG_DISABLE_VHD_NT_TRANSLATION = 0x20,
    BCD_FLAG_DISABLE_VHD_DEVICE_DETECTION = 0x40,
    BCD_FLAG_DISABLE_POLICY_CHECKS = 0x80
} BCD_FLAGS;

/**
 * The BcdEnumerateElementsWithFlags function enumerates the elements present in a specified BCD object, using additional flags to control enumeration behavior.
 *
 * \param BcdObjectHandle Handle to the BCD object whose elements are to be enumerated.
 * \param BcdFlags Flags that control the enumeration behavior (see BCD_FLAGS).
 * \param Buffer Optional pointer to a buffer that receives an array of BCD_ELEMENT structures.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \param ElementCount Receives the number of elements returned in the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdEnumerateElementsWithFlags(
    _In_ HANDLE BcdObjectHandle,
    _In_ BCD_FLAGS BcdFlags,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize,
    _Out_ PULONG ElementCount
    );

/**
 * The BcdEnumerateAndUnpackElements function enumerates and unpacks the elements of a specified BCD object from a BCD store, using additional flags.
 *
 * \param BcdStoreHandle Handle to the BCD store.
 * \param BcdObjectHandle Handle to the BCD object whose elements are to be enumerated and unpacked.
 * \param BcdFlags Flags that control the enumeration and unpacking behavior.
 * \param Buffer Optional pointer to a buffer that receives an array of BCD_ELEMENT structures.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \param ElementCount Receives the number of elements returned in the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdEnumerateAndUnpackElements(
    _In_ HANDLE BcdStoreHandle,
    _In_ HANDLE BcdObjectHandle,
    _In_ BCD_FLAGS BcdFlags,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize,
    _Out_ PULONG ElementCount
    );

/**
 * The BcdGetElementData function retrieves the data for a specified element in a BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object.
 * \param BcdElement The element type to retrieve (BCD_ELEMENT_DATATYPE).
 * \param Buffer Optional pointer to a buffer that receives the element data.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdGetElementData(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdElement,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    );

/**
 * The BcdGetElementDataWithFlags function retrieves the data for a specified element in a BCD object, using additional flags.
 *
 * \param BcdObjectHandle Handle to the BCD object.
 * \param BcdElement The element type to retrieve (BCD_ELEMENT_DATATYPE).
 * \param BcdFlags Flags that control the retrieval behavior.
 * \param Buffer Optional pointer to a buffer that receives the element data.
 * If this parameter is NULL, the function will return the required buffer size in BufferSize.
 * \param BufferSize On input, specifies the size of the buffer in bytes. On output, receives the required or actual size of the buffer.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdGetElementDataWithFlags(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdElement,
    _In_ BCD_FLAGS BcdFlags,
    _Out_writes_bytes_opt_(*BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    );

/**
 * The BcdSetElementData function sets the data for a specified element in a BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object.
 * \param BcdElement The element type to set (BCD_ELEMENT_DATATYPE).
 * \param Buffer Pointer to the buffer containing the data to set.
 * \param BufferSize The size of the buffer in bytes.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdSetElementData(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdElement,
    _In_reads_bytes_opt_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize
    );

/**
 * The BcdSetElementDataWithFlags function sets the data for a specified element in a BCD object, using additional flags.
 *
 * \param BcdObjectHandle Handle to the BCD object.
 * \param BcdElement The element type to set (BCD_ELEMENT_DATATYPE).
 * \param BcdFlags Flags that control the set operation.
 * \param Buffer Pointer to the buffer containing the data to set.
 * \param BufferSize The size of the buffer in bytes.
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdSetElementDataWithFlags(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdElement,
    _In_ BCD_FLAGS BcdFlags,
    _In_reads_bytes_opt_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize
    );

/**
 * The BcdDeleteElement function deletes a specified element from a BCD object.
 *
 * \param BcdObjectHandle Handle to the BCD object.
 * \param BcdElement The element type to delete (BCD_ELEMENT_DATATYPE).
 * \return NTSTATUS Successful or errant status.
 */
NTSYSAPI
NTSTATUS
NTAPI
BcdDeleteElement(
    _In_ HANDLE BcdObjectHandle,
    _In_ ULONG BcdElement
    );


/**
 * Specifies the device object element types.
 * \sa https://learn.microsoft.com/en-us/previous-versions/windows/desktop/bcd/bcddeviceobjectelementtypes
 */
typedef enum BcdDeviceObjectElementTypes
{
    BcdDeviceInteger_RamdiskImageOffset = 0x35000001,
    BcdDeviceInteger_TftpClientPort = 0x35000002,
    BcdDeviceInteger_SdiDevice = 0x31000003,
    BcdDeviceInteger_SdiPath = 0x32000004,
    BcdDeviceInteger_RamdiskImageLength = 0x35000005,
    BcdDeviceBoolean_RamdiskExportAsCd = 0x36000006,
    BcdDeviceInteger_RamdiskTftpBlockSize = 0x36000007,
    BcdDeviceInteger_RamdiskTftpWindowSize = 0x36000008,
    BcdDeviceBoolean_RamdiskMulticastEnabled = 0x36000009,
    BcdDeviceBoolean_RamdiskMulticastTftpFallback = 0x3600000A,
    BcdDeviceBoolean_RamdiskTftpVarWindow = 0x3600000B
} BcdDeviceObjectElementTypes;

/**
 * BCD configuration elements for the Boot Manager types.
 */
typedef enum _BcdBootMgrElementTypes
{
    BcdBootMgrObjectList_DisplayOrder = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 1),
    BcdBootMgrObjectList_BootSequence = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 2),
    BcdBootMgrObject_DefaultObject = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECT, 3),
    BcdBootMgrInteger_Timeout = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 4),
    BcdBootMgrBoolean_AttemptResume = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 5),
    BcdBootMgrObject_ResumeObject = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECT, 6),
    BcdBootMgrObjectList_StartupSequence = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 7),
    BcdBootMgrObjectList_ToolsDisplayOrder = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 16),
    BcdBootMgrBoolean_DisplayBootMenu = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 32),
    BcdBootMgrBoolean_NoErrorDisplay = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 33),
    BcdBootMgrDevice_BcdDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 34),
    BcdBootMgrString_BcdFilePath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 35),
    BcdBootMgrBoolean_HormEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 36),
    BcdBootMgrBoolean_HiberRoot = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 37),
    BcdBootMgrString_PasswordOverride = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 38),
    BcdBootMgrString_PinpassPhraseOverride = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 39),
    BcdBootMgrBoolean_ProcessCustomActionsFirst = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 40),
    BcdBootMgrIntegerList_CustomActionsList = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGERLIST, 48),
    BcdBootMgrBoolean_PersistBootSequence = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 49),
    BcdBootMgrBoolean_SkipStartupSequence = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 50),
} BcdBootMgrElementTypes;

/**
 * Specifies the policy for using the first megabyte of memory.
 */
typedef enum _BcdLibrary_FirstMegabytePolicy
{
    /* Use none of the first megabyte of memory. */
    FirstMegabytePolicyUseNone,
    /* Use all of the first megabyte of memory. */
    FirstMegabytePolicyUseAll,
    /* Reserved for future use. */
    FirstMegabytePolicyUsePrivate
} BcdLibrary_FirstMegabytePolicy;

/**
 * Specifies the type of debugger.
 */
typedef enum _BcdLibrary_DebuggerType
{
    /* Serial debugger. */
    DebuggerSerial = 0,
    /* 1394 debugger. */
    Debugger1394 = 1,
    /* USB debugger. */
    DebuggerUsb = 2,
    /* Network debugger. */
    DebuggerNet = 3,
    /* Local debugger. */
    DebuggerLocal = 4
} BcdLibrary_DebuggerType;

/**
 * Specifies the start policy for the debugger.
 */
typedef enum _BcdLibrary_DebuggerStartPolicy
{
    /* The debugger will start active. */
    DebuggerStartActive,
    /**
     * The debugger will start in the auto-enabled state.
     * If a debugger is attached it will be used; otherwise the debugger port will be available for other applications.
     */
    DebuggerStartAutoEnable,
    /* The debugger will not start. */
    DebuggerStartDisable
} BcdLibrary_DebuggerStartPolicy;

/**
 * Specifies the access policy for PCI configuration space.
 */
typedef enum _BcdLibrary_ConfigAccessPolicy
{
    /**
     * Access to PCI configuration space through the memory-mapped region is allowed.
     */
    ConfigAccessPolicyDefault,
    /**
     * Access to PCI configuration space through the memory-mapped region is not allowed.
     * This setting is used for platforms that implement memory-mapped configuration space incorrectly.
     * The CFC/CF8 access mechanism can be used to access configuration space on these platforms.
     */
    ConfigAccessPolicyDisallowMmConfig
} BcdLibrary_ConfigAccessPolicy;

/**
 * Enumeration for UX Display Message Types in the Boot Configuration Data (BCD) library.
 * This enumeration defines the different UX display message types that can be specified in the BCD library.
 */
typedef enum _BcdLibrary_UxDisplayMessageType
{
    DisplayMessageTypeDefault = 0,
    DisplayMessageTypeResume = 1,
    DisplayMessageTypeHyperV = 2,
    DisplayMessageTypeRecovery = 3,
    DisplayMessageTypeStartupRepair = 4,
    DisplayMessageTypeSystemImageRecovery = 5,
    DisplayMessageTypeCommandPrompt = 6,
    DisplayMessageTypeSystemRestore = 7,
    DisplayMessageTypePushButtonReset = 8
} BcdLibrary_UxDisplayMessageType;

/**
 * Enumeration for Safe Boot options in the Boot Configuration Data (BCD) library.
 * This enumeration defines the different safe boot modes that can be specified in the BCD library.
 */
typedef enum BcdLibrary_SafeBoot
{
    /**
     * Load the drivers and services specified by name or group under the following registry key:
     * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SafeBoot\Minimal.
     */
    SafemodeMinimal = 0,
    /**
     * Load the drivers and services specified by name or group under the following registry key:
     * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\SafeBoot\Network.
     */
    SafemodeNetwork = 1,
    /**
     * Boot the system into a repair mode that restores the Active Directory service from backup medium.
     */
    SafemodeDsRepair = 2
} BcdLibrary_SafeBoot;

typedef enum _BcdLibrary_BootUxPolicy
{
    BootUxPolicyDisabled = 0,
    BootUxPolicyBasic = 1,
    BootUxPolicyStandard = 2,
} BcdLibrary_BootUxPolicy;

typedef enum _BcdLibraryElementTypes
{
    BcdLibraryDevice_ApplicationDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 1),
    BcdLibraryString_ApplicationPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 2),
    BcdLibraryString_Description = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 4),
    BcdLibraryString_PreferredLocale = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 5),
    BcdLibraryObjectList_InheritedObjects = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 6),
    BcdLibraryInteger_TruncatePhysicalMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 7),
    BcdLibraryObjectList_RecoverySequence = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_OBJECTLIST, 8),
    BcdLibraryBoolean_AutoRecoveryEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 9),
    BcdLibraryIntegerList_BadMemoryList = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGERLIST, 10),
    BcdLibraryBoolean_AllowBadMemoryAccess = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 11),
    BcdLibraryInteger_FirstMegabytePolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 12),
    BcdLibraryInteger_RelocatePhysicalMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 13),
    BcdLibraryInteger_AvoidLowPhysicalMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 14),
    BcdLibraryBoolean_TraditionalKsegMappings = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 15),
    BcdLibraryBoolean_DebuggerEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 16),
    BcdLibraryInteger_DebuggerType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 17),
    BcdLibraryInteger_SerialDebuggerPortAddress = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 18),
    BcdLibraryInteger_SerialDebuggerPort = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 19),
    BcdLibraryInteger_SerialDebuggerBaudRate = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 20),
    BcdLibraryInteger_1394DebuggerChannel = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 21),
    BcdLibraryString_UsbDebuggerTargetName = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 22),
    BcdLibraryBoolean_DebuggerIgnoreUsermodeExceptions = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 23),
    BcdLibraryInteger_DebuggerStartPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 24),
    BcdLibraryString_DebuggerBusParameters = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 25),
    BcdLibraryInteger_DebuggerNetHostIP = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 26),
    BcdLibraryInteger_DebuggerNetPort = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 27),
    BcdLibraryBoolean_DebuggerNetDhcp = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 28),
    BcdLibraryString_DebuggerNetKey = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 29),
    BcdLibraryBoolean_DebuggerNetVM = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 30),
    BcdLibraryString_DebuggerNetHostIpv6 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 31),
    BcdLibraryBoolean_EmsEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 32),
    BcdLibraryInteger_EmsPort = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 34),
    BcdLibraryInteger_EmsBaudRate = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 35),
    BcdLibraryString_LoadOptionsString = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 48),
    BcdLibraryBoolean_AttemptNonBcdStart = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 49),
    BcdLibraryBoolean_DisplayAdvancedOptions = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 64),
    BcdLibraryBoolean_DisplayOptionsEdit = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 65),
    BcdLibraryInteger_FVEKeyRingAddress = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 66),
    BcdLibraryDevice_BsdLogDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 67),
    BcdLibraryString_BsdLogPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 68),
    BcdLibraryBoolean_BsdPreserveLog = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 69),
    BcdLibraryBoolean_GraphicsModeDisabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 70),
    BcdLibraryInteger_ConfigAccessPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 71),
    BcdLibraryBoolean_DisableIntegrityChecks = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 72),
    BcdLibraryBoolean_AllowPrereleaseSignatures = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 73),
    BcdLibraryString_FontPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 74),
    BcdLibraryInteger_SiPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 75),
    BcdLibraryInteger_FveBandId = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 76),
    BcdLibraryBoolean_ConsoleExtendedInput = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 80),
    BcdLibraryInteger_InitialConsoleInput = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 81),
    BcdLibraryInteger_GraphicsResolution = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 82),
    BcdLibraryBoolean_RestartOnFailure = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 83),
    BcdLibraryBoolean_GraphicsForceHighestMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 84),
    BcdLibraryBoolean_IsolatedExecutionContext = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 96),
    BcdLibraryInteger_BootUxDisplayMessage = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 101),
    BcdLibraryInteger_BootUxDisplayMessageOverride = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 102),
    BcdLibraryBoolean_BootUxLogoDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 103),
    BcdLibraryBoolean_BootUxTextDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 104),
    BcdLibraryBoolean_BootUxProgressDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 105),
    BcdLibraryBoolean_BootUxFadeDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 106),
    BcdLibraryBoolean_BootUxReservePoolDebug = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 107),
    BcdLibraryBoolean_BootUxDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 108),
    BcdLibraryInteger_BootUxFadeFrames = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 109),
    BcdLibraryBoolean_BootUxDumpStats = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 110),
    BcdLibraryBoolean_BootUxShowStats = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 111),
    BcdLibraryBoolean_MultiBootSystem = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 113),
    BcdLibraryBoolean_ForceNoKeyboard = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 114),
    BcdLibraryInteger_AliasWindowsKey = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 115),
    BcdLibraryBoolean_BootShutdownDisabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 116),
    BcdLibraryInteger_PerformanceFrequency = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 117),
    BcdLibraryInteger_SecurebootRawPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 118),
    BcdLibraryIntegerList_AllowedInMemorySettings = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 119),
    BcdLibraryInteger_BootUxBitmapTransitionTime = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 121),
    BcdLibraryBoolean_TwoBootImages = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 122),
    BcdLibraryBoolean_ForceFipsCrypto = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 123),
    BcdLibraryInteger_BootErrorUx = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 125),
    BcdLibraryBoolean_AllowFlightSignatures = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 126),
    BcdLibraryInteger_BootMeasurementLogFormat = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 127),
    BcdLibraryInteger_DisplayRotation = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 128),
    BcdLibraryInteger_LogControl = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 129),
    BcdLibraryBoolean_NoFirmwareSync = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 130),
    BcdLibraryDevice_WindowsSystemDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 132),
    BcdLibraryBoolean_NumLockOn = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 135),
    BcdLibraryString_AdditionalCiPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 136),
    BcdLibraryInteger_LinearAddress57 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_LIBRARY, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 136),
} BcdLibraryElementTypes;

typedef enum _BcdTemplateElementTypes
{
    BcdSetupInteger_DeviceType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 1),
    BcdSetupString_ApplicationRelativePath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 2),
    BcdSetupString_RamdiskDeviceRelativePath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 3),
    BcdSetupBoolean_OmitOsLoaderElements = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 4),
    BcdSetupIntegerList_ElementsToMigrateList = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_INTEGERLIST, 6),
    BcdSetupBoolean_RecoveryOs = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_SETUPTEMPLATE, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 16),
} BcdTemplateElementTypes;

/**
 * Specifies the no-execute page protection policies.
 */
typedef enum _BcdOSLoader_NxPolicy
{
    /**
     * The no-execute page protection is off by default.
     */
    NxPolicyOptIn = 0,
    /**
     * The no-execute page protection is on by default.
     */
    NxPolicyOptOut = 1,
    /**
     * The no-execute page protection is always off.
     */
    NxPolicyAlwaysOff = 2,
    /**
     * The no-execute page protection is always on.
     */
    NxPolicyAlwaysOn = 3
} BcdOSLoader_NxPolicy;

/**
 * Specifies the Physical Address Extension (PAE) policies.
 */
typedef enum _BcdOSLoader_PAEPolicy
{
    /**
     * Enable PAE if hot-pluggable memory is defined above 4GB.
     */
    PaePolicyDefault = 0,
    /**
     * PAE is enabled.
     */
    PaePolicyForceEnable = 1,
    /**
     * PAE is disabled.
     */
    PaePolicyForceDisable = 2
} BcdOSLoader_PAEPolicy;

/**
 * Specifies the boot status policies.
 */
typedef enum _BcdOSLoader_BootStatusPolicy
{
    /**
     * Display all boot failures.
     */
    BootStatusPolicyDisplayAllFailures = 0,
    /**
     * Ignore all boot failures.
     */
    BootStatusPolicyIgnoreAllFailures = 1,
    /**
     * Ignore all shutdown failures.
     */
    BootStatusPolicyIgnoreShutdownFailures = 2,
    /**
     * Ignore all boot failures.
     */
    BootStatusPolicyIgnoreBootFailures = 3,
    /**
     * Ignore checkpoint failures.
     */
    BootStatusPolicyIgnoreCheckpointFailures = 4,
    /**
     * Display shutdown failures.
     */
    BootStatusPolicyDisplayShutdownFailures = 5,
    /**
     * Display boot failures.
     */
    BootStatusPolicyDisplayBootFailures = 6,
    /**
     * Display checkpoint failures.
     */
    BootStatusPolicyDisplayCheckpointFailures = 7,
    /**
     * Always display startup failures.
     */
    BootStatusPolicyAlwaysDisplayStartupFailures = 8
} BcdOSLoaderBootStatusPolicy;

typedef enum _BcdOSLoaderElementTypes
{
    BcdOSLoaderDevice_OSDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 1),
    BcdOSLoaderString_SystemRoot = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 2),
    BcdOSLoaderObject_AssociatedResumeObject = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_OBJECT, 3),
    BcdOSLoaderBoolean_StampDisks = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 4),
    BcdOSLoaderBoolean_DetectKernelAndHal = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 16),
    BcdOSLoaderString_KernelPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 17),
    BcdOSLoaderString_HalPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 18),
    BcdOSLoaderString_DbgTransportPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 19),
    BcdOSLoaderInteger_NxPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 32),
    BcdOSLoaderInteger_PAEPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 33),
    BcdOSLoaderBoolean_WinPEMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 34),
    BcdOSLoaderBoolean_DisableCrashAutoReboot = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 36),
    BcdOSLoaderBoolean_UseLastGoodSettings = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 37),
    BcdOSLoaderBoolean_DisableCodeIntegrityChecks = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 38),
    BcdOSLoaderBoolean_AllowPrereleaseSignatures = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 39),
    BcdOSLoaderBoolean_NoLowMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 48),
    BcdOSLoaderInteger_RemoveMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 49),
    BcdOSLoaderInteger_IncreaseUserVa = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 50),
    BcdOSLoaderInteger_PerformaceDataMemory = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 51),
    BcdOSLoaderBoolean_UseVgaDriver = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 64),
    BcdOSLoaderBoolean_DisableBootDisplay = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 65),
    BcdOSLoaderBoolean_DisableVesaBios = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 66),
    BcdOSLoaderBoolean_DisableVgaMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 67),
    BcdOSLoaderInteger_ClusterModeAddressing = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 80),
    BcdOSLoaderBoolean_UsePhysicalDestination = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 81),
    BcdOSLoaderInteger_RestrictApicCluster = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 82),
    BcdOSLoaderString_OSLoaderTypeEVStore = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 83),
    BcdOSLoaderBoolean_UseLegacyApicMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 84),
    BcdOSLoaderInteger_X2ApicPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 85),
    BcdOSLoaderBoolean_UseBootProcessorOnly = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 96),
    BcdOSLoaderInteger_NumberOfProcessors = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 97),
    BcdOSLoaderBoolean_ForceMaximumProcessors = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 98),
    BcdOSLoaderBoolean_ProcessorConfigurationFlags = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 99),
    BcdOSLoaderBoolean_MaximizeGroupsCreated = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 100),
    BcdOSLoaderBoolean_ForceGroupAwareness = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 101),
    BcdOSLoaderInteger_GroupSize = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 102),
    BcdOSLoaderInteger_UseFirmwarePciSettings = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 112),
    BcdOSLoaderInteger_MsiPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 113),
    BcdOSLoaderInteger_PciExpressPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 114),
    BcdOSLoaderInteger_SafeBoot = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 128),
    BcdOSLoaderBoolean_SafeBootAlternateShell = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 129),
    BcdOSLoaderBoolean_BootLogInitialization = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 144),
    BcdOSLoaderBoolean_VerboseObjectLoadMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 145),
    BcdOSLoaderBoolean_KernelDebuggerEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 160),
    BcdOSLoaderBoolean_DebuggerHalBreakpoint = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 161),
    BcdOSLoaderBoolean_UsePlatformClock = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 162),
    BcdOSLoaderBoolean_ForceLegacyPlatform = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 163),
    BcdOSLoaderBoolean_UsePlatformTick = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 164),
    BcdOSLoaderBoolean_DisableDynamicTick = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 165),
    BcdOSLoaderInteger_TscSyncPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 166),
    BcdOSLoaderBoolean_EmsEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 176),
    BcdOSLoaderInteger_ForceFailure = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 192),
    BcdOSLoaderInteger_DriverLoadFailurePolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 193),
    BcdOSLoaderInteger_BootMenuPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 194),
    BcdOSLoaderBoolean_AdvancedOptionsOneTime = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 195),
    BcdOSLoaderBoolean_OptionsEditOneTime = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 196),
    BcdOSLoaderInteger_BootStatusPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 224),
    BcdOSLoaderBoolean_DisableElamDrivers = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 225),
    BcdOSLoaderInteger_HypervisorLaunchType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 240),
    BcdOSLoaderString_HypervisorPath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 241),
    BcdOSLoaderBoolean_HypervisorDebuggerEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 242),
    BcdOSLoaderInteger_HypervisorDebuggerType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 243),
    BcdOSLoaderInteger_HypervisorDebuggerPortNumber = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 244),
    BcdOSLoaderInteger_HypervisorDebuggerBaudrate = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 245),
    BcdOSLoaderInteger_HypervisorDebugger1394Channel = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 246),
    BcdOSLoaderInteger_BootUxPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 247),
    BcdOSLoaderInteger_HypervisorSlatDisabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 248),
    BcdOSLoaderString_HypervisorDebuggerBusParams = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 249),
    BcdOSLoaderInteger_HypervisorNumProc = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 250),
    BcdOSLoaderInteger_HypervisorRootProcPerNode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 251),
    BcdOSLoaderBoolean_HypervisorUseLargeVTlb = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 252),
    BcdOSLoaderInteger_HypervisorDebuggerNetHostIp = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 253),
    BcdOSLoaderInteger_HypervisorDebuggerNetHostPort = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 254),
    BcdOSLoaderInteger_HypervisorDebuggerPages = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 255),
    BcdOSLoaderInteger_TpmBootEntropyPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 256),
    BcdOSLoaderString_HypervisorDebuggerNetKey = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 272),
    BcdOSLoaderString_HypervisorProductSkuType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 274),
    BcdOSLoaderInteger_HypervisorRootProc = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 275),
    BcdOSLoaderBoolean_HypervisorDebuggerNetDhcp = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 276),
    BcdOSLoaderInteger_HypervisorIommuPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 277),
    BcdOSLoaderBoolean_HypervisorUseVApic = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 278),
    BcdOSLoaderString_HypervisorLoadOptions = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 279),
    BcdOSLoaderInteger_HypervisorMsrFilterPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 280),
    BcdOSLoaderInteger_HypervisorMmioNxPolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 281),
    BcdOSLoaderInteger_HypervisorSchedulerType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 282),
    BcdOSLoaderString_HypervisorRootProcNumaNodes = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 283),
    BcdOSLoaderInteger_HypervisorPerfmon = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 284),
    BcdOSLoaderInteger_HypervisorRootProcPerCore = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 285),
    BcdOSLoaderString_HypervisorRootProcNumaNodeLps = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 286),
    BcdOSLoaderInteger_XSavePolicy = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 288),
    BcdOSLoaderInteger_XSaveAddFeature0 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 289),
    BcdOSLoaderInteger_XSaveAddFeature1 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 290),
    BcdOSLoaderInteger_XSaveAddFeature2 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 291),
    BcdOSLoaderInteger_XSaveAddFeature3 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 292),
    BcdOSLoaderInteger_XSaveAddFeature4 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 293),
    BcdOSLoaderInteger_XSaveAddFeature5 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 294),
    BcdOSLoaderInteger_XSaveAddFeature6 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 295),
    BcdOSLoaderInteger_XSaveAddFeature7 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 296),
    BcdOSLoaderInteger_XSaveRemoveFeature = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 297),
    BcdOSLoaderInteger_XSaveProcessorsMask = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 298),
    BcdOSLoaderInteger_XSaveDisable = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 299),
    BcdOSLoaderInteger_KernelDebuggerType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 300),
    BcdOSLoaderString_KernelDebuggerBusParameters = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 301),
    BcdOSLoaderInteger_KernelDebuggerPortAddress = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 302),
    BcdOSLoaderInteger_KernelDebuggerPortNumber = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 303),
    BcdOSLoaderInteger_ClaimedTpmCounter = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 304),
    BcdOSLoaderInteger_KernelDebugger1394Channel = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 305),
    BcdOSLoaderString_KernelDebuggerUsbTargetname = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 306),
    BcdOSLoaderInteger_KernelDebuggerNetHostIp = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 307),
    BcdOSLoaderInteger_KernelDebuggerNetHostPort = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 308),
    BcdOSLoaderBoolean_KernelDebuggerNetDhcp = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 309),
    BcdOSLoaderString_KernelDebuggerNetKey = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 310),
    BcdOSLoaderString_IMCHiveName = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 311),
    BcdOSLoaderDevice_IMCDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 312),
    BcdOSLoaderInteger_KernelDebuggerBaudrate = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 313),
    BcdOSLoaderString_ManufacturingMode = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 320),
    BcdOSLoaderBoolean_EventLoggingEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 321),
    BcdOSLoaderInteger_VsmLaunchType = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 322),
    BcdOSLoaderInteger_HypervisorEnforcedCodeIntegrity = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_INTEGER, 324),
    BcdOSLoaderBoolean_DtraceEnabled = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_BOOLEAN, 325),
    BcdOSLoaderDevice_SystemDataDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 336),
    BcdOSLoaderDevice_OsArcDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 337),
    BcdOSLoaderDevice_OsDataDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 339),
    BcdOSLoaderDevice_BspDevice = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 340),
    BcdOSLoaderDevice_BspFilepath = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_DEVICE, 341),
    BcdOSLoaderString_KernelDebuggerNetHostIpv6 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 342),
    BcdOSLoaderString_HypervisorDebuggerNetHostIpv6 = MAKE_BCDE_DATA_TYPE(BCD_ELEMENT_DATATYPE_CLASS_APPLICATION, BCD_ELEMENT_DATATYPE_FORMAT_STRING, 353),
} BcdOSLoaderElementTypes;

#endif
