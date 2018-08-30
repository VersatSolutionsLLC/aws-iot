

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
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t config_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t config_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void config_AdvertiseService
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