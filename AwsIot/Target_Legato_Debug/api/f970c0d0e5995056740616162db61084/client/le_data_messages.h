
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DATA_MESSAGES_H_INCLUDE_GUARD
#define LE_DATA_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "90cfa25c0c741d60538dab629329e530"

#ifdef MK_TOOLS_BUILD
    extern const char** le_data_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_data_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_data"
#endif


#define _MAX_MSG_SIZE 113

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_data_AddConnectionStateHandler 0
#define _MSGID_le_data_RemoveConnectionStateHandler 1
#define _MSGID_le_data_Request 2
#define _MSGID_le_data_Release 3
#define _MSGID_le_data_SetTechnologyRank 4
#define _MSGID_le_data_GetFirstUsedTechnology 5
#define _MSGID_le_data_GetNextUsedTechnology 6
#define _MSGID_le_data_GetTechnology 7
#define _MSGID_le_data_GetDefaultRouteStatus 8
#define _MSGID_le_data_AddRoute 9
#define _MSGID_le_data_DelRoute 10
#define _MSGID_le_data_GetCellularProfileIndex 11
#define _MSGID_le_data_SetCellularProfileIndex 12
#define _MSGID_le_data_GetDate 13
#define _MSGID_le_data_GetTime 14


#endif // LE_DATA_MESSAGES_H_INCLUDE_GUARD