
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

#define PROTOCOL_ID_STR "3d0c7e069dd5c0bc98800a83b8761b70"

#ifdef MK_TOOLS_BUILD
    extern const char** aws_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*aws_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "aws"
#endif


#define _MAX_MSG_SIZE 1048

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_aws_InitConnectionParams 0
#define _MSGID_aws_InitTlsParams 1
#define _MSGID_aws_Init 2
#define _MSGID_aws_Connect 3
#define _MSGID_aws_Publish 4
#define _MSGID_aws_Subscribe 5
#define _MSGID_aws_UnSubscribe 6
#define _MSGID_aws_disconnect 7
#define _MSGID_aws_Yield 8


#endif // AWS_MESSAGES_H_INCLUDE_GUARD