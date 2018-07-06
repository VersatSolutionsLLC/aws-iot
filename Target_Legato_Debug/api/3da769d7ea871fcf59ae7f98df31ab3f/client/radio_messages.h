
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

#define PROTOCOL_ID_STR "9950888954ed8e88ef4bb882dcb705a3"

#ifdef MK_TOOLS_BUILD
    extern const char** radio_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*radio_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "printer"
#endif


#define _MAX_MSG_SIZE 12

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_radio_Signal 0
#define _MSGID_radio_Temparature 1


#endif // RADIO_MESSAGES_H_INCLUDE_GUARD