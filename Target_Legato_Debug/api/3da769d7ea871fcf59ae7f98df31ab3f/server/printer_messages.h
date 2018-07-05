
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef PRINTER_MESSAGES_H_INCLUDE_GUARD
#define PRINTER_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "2911d68f18abb927d8a931f619072940"

#ifdef MK_TOOLS_BUILD
    extern const char** printer_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*printer_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "printer"
#endif


#define _MAX_MSG_SIZE 9

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_printer_Print 0


#endif // PRINTER_MESSAGES_H_INCLUDE_GUARD