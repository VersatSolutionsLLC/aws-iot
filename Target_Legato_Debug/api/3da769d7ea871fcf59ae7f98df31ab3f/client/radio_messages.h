
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

#define PROTOCOL_ID_STR "3af1920b94ff7bbdfe3a6b94cd9dcf47"

#ifdef MK_TOOLS_BUILD
    extern const char** radio_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*radio_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "printer"
#endif


#define _MAX_MSG_SIZE 19

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_radio_Signal 0
#define _MSGID_radio_Temperature 1
#define _MSGID_radio_Rat 2
#define _MSGID_radio_Rssi 3
#define _MSGID_radio_Ber 4


#endif // RADIO_MESSAGES_H_INCLUDE_GUARD