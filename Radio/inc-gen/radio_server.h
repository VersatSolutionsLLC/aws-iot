

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
 */
//--------------------------------------------------------------------------------------------------
void radio_Info
(
    char* params,
        ///< [OUT]
    size_t paramsSize
        ///< [IN]
);


#endif // RADIO_INTERFACE_H_INCLUDE_GUARD