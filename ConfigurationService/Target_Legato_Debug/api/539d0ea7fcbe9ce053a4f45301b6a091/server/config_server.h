

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
int32_t config_GetString
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
int32_t config_GetInteger
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
int32_t config_GetFloat
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
int32_t config_GetBoolean
(
    const char* LE_NONNULL section,
        ///< [IN]
    const char* LE_NONNULL key,
        ///< [IN]
    bool defaultValue,
        ///< [IN]
    bool* keyValuePtr
        ///< [OUT]
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
int32_t config_PutString
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
int32_t config_OpenFile
(
    void
);


#endif // CONFIG_INTERFACE_H_INCLUDE_GUARD