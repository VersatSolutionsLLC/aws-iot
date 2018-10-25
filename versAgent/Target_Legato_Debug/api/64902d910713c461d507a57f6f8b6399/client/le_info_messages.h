
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_INFO_MESSAGES_H_INCLUDE_GUARD
#define LE_INFO_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "975257ab76b4bd700b883f96a47b6508"

#ifdef MK_TOOLS_BUILD
    extern const char** le_info_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_info_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_info"
#endif


#define _MAX_MSG_SIZE 272

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_info_GetImei 0
#define _MSGID_le_info_GetImeiSv 1
#define _MSGID_le_info_GetFirmwareVersion 2
#define _MSGID_le_info_GetResetInformation 3
#define _MSGID_le_info_GetBootloaderVersion 4
#define _MSGID_le_info_GetDeviceModel 5
#define _MSGID_le_info_GetMeid 6
#define _MSGID_le_info_GetEsn 7
#define _MSGID_le_info_GetMdn 8
#define _MSGID_le_info_GetPrlVersion 9
#define _MSGID_le_info_GetPrlOnlyPreference 10
#define _MSGID_le_info_GetMin 11
#define _MSGID_le_info_GetNai 12
#define _MSGID_le_info_GetManufacturerName 13
#define _MSGID_le_info_GetPriId 14
#define _MSGID_le_info_GetCarrierPri 15
#define _MSGID_le_info_GetPlatformSerialNumber 16
#define _MSGID_le_info_GetRfDeviceStatus 17
#define _MSGID_le_info_GetSku 18
#define _MSGID_le_info_GetExpectedResetsCount 19
#define _MSGID_le_info_GetUnexpectedResetsCount 20


#endif // LE_INFO_MESSAGES_H_INCLUDE_GUARD