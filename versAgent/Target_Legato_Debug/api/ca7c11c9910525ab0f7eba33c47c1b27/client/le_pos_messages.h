
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_POS_MESSAGES_H_INCLUDE_GUARD
#define LE_POS_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "c9b4a5b877bfe3915d5029d8393d9566"

#ifdef MK_TOOLS_BUILD
    extern const char** le_pos_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_pos_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_pos"
#endif


#define _MAX_MSG_SIZE 32

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_pos_AddMovementHandler 0
#define _MSGID_le_pos_RemoveMovementHandler 1
#define _MSGID_le_pos_Get2DLocation 2
#define _MSGID_le_pos_Get3DLocation 3
#define _MSGID_le_pos_GetTime 4
#define _MSGID_le_pos_GetDate 5
#define _MSGID_le_pos_GetMotion 6
#define _MSGID_le_pos_GetHeading 7
#define _MSGID_le_pos_GetDirection 8
#define _MSGID_le_pos_GetFixState 9
#define _MSGID_le_pos_sample_Get2DLocation 10
#define _MSGID_le_pos_sample_GetTime 11
#define _MSGID_le_pos_sample_GetDate 12
#define _MSGID_le_pos_sample_GetAltitude 13
#define _MSGID_le_pos_sample_GetHorizontalSpeed 14
#define _MSGID_le_pos_sample_GetVerticalSpeed 15
#define _MSGID_le_pos_sample_GetHeading 16
#define _MSGID_le_pos_sample_GetDirection 17
#define _MSGID_le_pos_sample_GetFixState 18
#define _MSGID_le_pos_sample_Release 19
#define _MSGID_le_pos_SetAcquisitionRate 20
#define _MSGID_le_pos_GetAcquisitionRate 21
#define _MSGID_le_pos_SetDistanceResolution 22


#endif // LE_POS_MESSAGES_H_INCLUDE_GUARD