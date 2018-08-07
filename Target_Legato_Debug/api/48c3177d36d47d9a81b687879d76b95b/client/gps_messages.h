
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef GPS_MESSAGES_H_INCLUDE_GUARD
#define GPS_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "677267ede6d95b85bd11d5f8a103e761"

#ifdef MK_TOOLS_BUILD
    extern const char** gps_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*gps_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "gps"
#endif


#define _MAX_MSG_SIZE 1036

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_gps_GetLocation 0


#endif // GPS_MESSAGES_H_INCLUDE_GUARD