

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef AWS_INTERFACE_H_INCLUDE_GUARD
#define AWS_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*aws_DisconnectHandler_t)(void *);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Connect the current client thread to the service providing this API. Block until the service is
 * available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void aws_ConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Try to connect the current client thread to the service providing this API. Return with an error
 * if the service is not available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
le_result_t aws_TryConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void aws_SetServerDisconnectHandler
(
    aws_DisconnectHandler_t disconnectHandler,
    void *contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Disconnect the current client thread from the service providing this API.
 *
 * Normally, this function doesn't need to be called. After this function is called, there's no
 * longer a connection to the service, and the functions in this API can't be used. For details, see
 * @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void aws_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'aws_SubscribeEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct aws_SubscribeEventHandler* aws_SubscribeEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
typedef void (*aws_SubscribeEventHandlerFunc_t)
(
    const char* LE_NONNULL payload,
        ///<
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t aws_Connect
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t aws_Publish
(
    const char* LE_NONNULL topic,
        ///< [IN]
    int32_t topicLen,
        ///< [IN]
    int32_t qosType,
        ///< [IN]
    const char* LE_NONNULL payload,
        ///< [IN]
    int32_t payloadLen
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t aws_Subscribe
(
    const char* LE_NONNULL topic,
        ///< [IN]
    int32_t topicLen,
        ///< [IN]
    int32_t qosType
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t aws_UnSubscribe
(
    const char* LE_NONNULL topic,
        ///< [IN]
    int32_t topicLen
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t aws_disconnect
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'aws_SubscribeEvent'
 */
//--------------------------------------------------------------------------------------------------
aws_SubscribeEventHandlerRef_t aws_AddSubscribeEventHandler
(
    aws_SubscribeEventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'aws_SubscribeEvent'
 */
//--------------------------------------------------------------------------------------------------
void aws_RemoveSubscribeEventHandler
(
    aws_SubscribeEventHandlerRef_t handlerRef
        ///< [IN]
);

#endif // AWS_INTERFACE_H_INCLUDE_GUARD