
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef AWS_MESSAGES_H_INCLUDE_GUARD
#define AWS_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "9bf53e492919fe5352339555537be39f"

#ifdef MK_TOOLS_BUILD
    extern const char** aws_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*aws_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "aws"
#endif


#define _MAX_MSG_SIZE 2080

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_aws_Connect 0
#define _MSGID_aws_Publish 1
#define _MSGID_aws_Subscribe 2
#define _MSGID_aws_UnSubscribe 3
#define _MSGID_aws_disconnect 4
#define _MSGID_aws_AddSubscribeEventHandler 5
#define _MSGID_aws_RemoveSubscribeEventHandler 6


#endif // AWS_MESSAGES_H_INCLUDE_GUARD