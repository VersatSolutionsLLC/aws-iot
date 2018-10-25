
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

#define PROTOCOL_ID_STR "12ca23b1a4deb0baa3f1f7a9be9131d2"

#ifdef MK_TOOLS_BUILD
    extern const char** aws_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*aws_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "vers_awsIot"
#endif


#define _MAX_MSG_SIZE 2080

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_aws_Publish 0
#define _MSGID_aws_Subscribe 1
#define _MSGID_aws_Unsubscribe 2
#define _MSGID_aws_Hibernate 3
#define _MSGID_aws_IsInHibernation 4
#define _MSGID_aws_AddSubscribeEventHandler 5
#define _MSGID_aws_RemoveSubscribeEventHandler 6


#endif // AWS_MESSAGES_H_INCLUDE_GUARD