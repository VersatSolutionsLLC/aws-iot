

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @file le_mdmDefs_interface.h
 *
 * Definition file common to multiple modem related services.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_MDMDEFS_INTERFACE_H_INCLUDE_GUARD
#define LE_MDMDEFS_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdmDefs_DisconnectHandler_t)(void *);

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
void le_mdmDefs_ConnectService
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
le_result_t le_mdmDefs_TryConnectService
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
void le_mdmDefs_SetServerDisconnectHandler
(
    le_mdmDefs_DisconnectHandler_t disconnectHandler,
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
void le_mdmDefs_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Cf. ITU-T recommendations E.164/E.163. E.164 numbers can have a maximum of 15 digits except the
 * international prefix ('+' or '00'). One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDMDEFS_PHONE_NUM_MAX_LEN 17

//--------------------------------------------------------------------------------------------------
/**
 * Cf. ITU-T recommendations E.164/E.163. E.164 numbers can have a maximum of 15 digits except the
 * international prefix ('+' or '00'). One extra byte is added for the null character.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDMDEFS_PHONE_NUM_MAX_BYTES 18

//--------------------------------------------------------------------------------------------------
/**
 **  IP Version
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDMDEFS_IPV4 = 0,
        ///< IPv4 technology
    LE_MDMDEFS_IPV6 = 1,
        ///< IPv6 technology
    LE_MDMDEFS_IPMAX = 2
        ///<
}
le_mdmDefs_IpVersion_t;


#endif // LE_MDMDEFS_INTERFACE_H_INCLUDE_GUARD