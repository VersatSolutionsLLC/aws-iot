
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_POSCTRL_MESSAGES_H_INCLUDE_GUARD
#define LE_POSCTRL_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "d47cc30512e69387f546399a44c565e4"

#ifdef MK_TOOLS_BUILD
    extern const char** le_posCtrl_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_posCtrl_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_posCtrl"
#endif


#define _MAX_MSG_SIZE 12

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_posCtrl_Request 0
#define _MSGID_le_posCtrl_Release 1


#endif // LE_POSCTRL_MESSAGES_H_INCLUDE_GUARD