

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef CONFIG_INTERFACE_H_INCLUDE_GUARD
#define CONFIG_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*config_DisconnectHandler_t)(void *);

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
void config_ConnectService
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
le_result_t config_TryConnectService
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
void config_SetServerDisconnectHandler
(
    config_DisconnectHandler_t disconnectHandler,
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
void config_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_sGetValue
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    const char* LE_NONNULL defaultValue,
        ///< [IN]
    char* keyValue,
        ///< [OUT]
    size_t keyValueSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_iGetValue
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    int32_t defaultValue,
        ///< [IN]
    int32_t* keyValuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_fGetValue
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    double defaultValue,
        ///< [IN]
    double* keyValuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_boolGetValue
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    int32_t defaultValue,
        ///< [IN]
    int32_t* keyValuePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_sPutValue
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    const char* LE_NONNULL keyValue
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void config_test
(
    char* params,
        ///< [OUT]
    size_t paramsSize
        ///< [IN]
);

#endif // CONFIG_INTERFACE_H_INCLUDE_GUARD