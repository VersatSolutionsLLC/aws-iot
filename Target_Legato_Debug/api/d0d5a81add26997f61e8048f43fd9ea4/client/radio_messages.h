
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef RADIO_MESSAGES_H_INCLUDE_GUARD
#define RADIO_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "0ff0471e7b28dec57f8204204218e976"

#ifdef MK_TOOLS_BUILD
    extern const char** radio_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*radio_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "radio"
#endif


#define _MAX_MSG_SIZE 1036

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_radio_AddDataConnectionStateHandler 0
#define _MSGID_radio_RemoveDataConnectionStateHandler 1
#define _MSGID_radio_Info 2
#define _MSGID_radio_Connect 3
#define _MSGID_radio_Disconnect 4
#define _MSGID_radio_IsConnected 5


#endif // RADIO_MESSAGES_H_INCLUDE_GUARD