
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef CONFIG_MESSAGES_H_INCLUDE_GUARD
#define CONFIG_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "a61c66f8ffe35155bee52d8b53e24aa4"

#ifdef MK_TOOLS_BUILD
    extern const char** config_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*config_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "config"
#endif


#define _MAX_MSG_SIZE 1348

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_config_sGetValue 0
#define _MSGID_config_iGetValue 1
#define _MSGID_config_fGetValue 2
#define _MSGID_config_boolGetValue 3
#define _MSGID_config_sPutValue 4
#define _MSGID_config_test 5


#endif // CONFIG_MESSAGES_H_INCLUDE_GUARD