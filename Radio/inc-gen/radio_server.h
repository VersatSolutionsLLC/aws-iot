

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef RADIO_INTERFACE_H_INCLUDE_GUARD
#define RADIO_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t radio_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t radio_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void radio_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'radio_DataConnectionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct radio_DataConnectionStateHandler* radio_DataConnectionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for connection state changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*radio_DataConnectionStateHandlerFunc_t)
(
    const char* LE_NONNULL intfName,
        ///< Interface name for the data connection
    bool isConnected,
        ///< State: connected or disconnected
    void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'radio_DataConnectionState'
 *
 * This event provides information on connection state changes
 */
//--------------------------------------------------------------------------------------------------
radio_DataConnectionStateHandlerRef_t radio_AddDataConnectionStateHandler
(
    radio_DataConnectionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'radio_DataConnectionState'
 */
//--------------------------------------------------------------------------------------------------
void radio_RemoveDataConnectionStateHandler
(
    radio_DataConnectionStateHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void radio_Info
(
    char* params,
        ///< [OUT]
    size_t paramsSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void radio_Connect
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
void radio_Disconnect
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
bool radio_IsConnected
(
    void
);


#endif // RADIO_INTERFACE_H_INCLUDE_GUARD