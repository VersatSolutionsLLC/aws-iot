

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_le_data Data Connection
 *
 * @ref le_data_interface.h "API Reference" <br>
 * @ref le_data_sample <br>
 *
 * A data connection is needed for applications that exchange data with devices where SMS messages
 * are insufficient or not possible.  The data connection can be established over a mobile network,
 * WiFi or a fixed link (e.g., ethernet).
 *
 * The data connection service provides a basic API for requesting and releasing a data connection.
 *
 * @note This interface does not support yet data connection over a fixed link.
 *
 * @section le_data_IPCbinding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b dataConnectionService application service.
 *
 * Here's a code sample binding to Data Connection services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_data -> dataConnectionService.le_data
   }
   @endverbatim
 *
 * @section c_le_data_default Default Data Connection
 *
 * Default data connection is obtained using le_data_Request().  Before the data connection is
 * requested, an application registers a connection state handler using
 * le_data_AddConnectionStateHandler().  Once the data connection is established, the handler will
 * be called indicating it's now connected. The interface name is sent only when the data connection
 * is in connected state. If the state of the data connection changes, then the handler will be
 * called with the new state. To release a data connection, an application can use le_data_Release().
 *
 * The technology of the established connection can be retrieved with le_data_GetTechnology().
 *
 * If the default data connection is not currently available when le_data_Request() is called, the
 * data connection service first ensures all pre-conditions are satisfied (e.g., modem is registered
 * on the network), before trying to start the data connection.
 *
 * If the default data connection is already available when le_data_Request() is called, a new
 * connection will not be started.  Instead, the existing data connection will be used. This happens
 * if another application also requested the default data connection. This is how multiple
 * applications can share the same data connection.
 *
 * Once an application makes a data connection request, it should monitor the connection state
 * reported to the registered connection state handler.  The application should only try
 * transmitting data when the state is connected, and should stop transmitting data when the state
 * is not connected. If the state is not connected, the data connection service will try to
 * establish or re-establish the connection endlessly until le_data_Release() is called. There's no
 * need for an application to issue a new connection request.
 *
 * The default data connection will not necessarily be released when an application calls
 * le_data_Release(). The data connection will be released only after le_data_Release() is called by
 * all applications that previously called le_data_Request().
 *
 * @note When using the cellular technology, the data connection service can use a specific cellular
 * data profile if it has been set with the le_data_SetCellularProfileIndex() API or in the
 * configuration tree. Otherwise the default cellular profile is used. The profile in use can be
 * retrieved by le_data_GetCellularProfileIndex(). The profile to use is loaded when the first data
 * connection is initiated. The cellular data profile can be configured by le_mdc APIs or
 * cm data tool.
 *
 * @section le_data_sample Code sample
 *
 * A sample code that implements the establishment of a data connection service can be found in
 * \b dcsTest.c file (please refer to @ref c_dataConnectionServiceSample page).
 *
 * @section c_le_data_routing Data connection routing
 *
 * @subsection c_le_data_defaultRoute Default route
 *
 * By default, the default route is automatically configured by the data connection service using
 * the modem parameters when the cellular technology is connected. If an application wishes to
 * configure its own default route, it should deactivate it in the data connection service by
 * setting the parameter @c useDefaultRoute to false in the configuration tree (see @ref
 * c_le_data_configdb):
 * @verbatim
   $ config set dataConnectionService:/routing/useDefaultRoute false bool
   @endverbatim
 *
 * @note The default route activation status can be retrieved with le_data_GetDefaultRouteStatus().
 *
 * @warning The default route activation status is only read at start-up and the change will only
 * be effective after a Legato restart.
 *
 * A sample code showing how to set the modem default route if it isn't set by the data connection
 * server is presented below:
 * @snippet "apps/test/dataConnectionService/dataConnectionServiceRouteTest/dcsRouteTestComp/dcsRouteTest.c" DefaultRoute
 *
 * @subsection c_le_data_addRemoveRoute Add or remove a route
 *
 * Specific routes can be added for the cellular connection with le_data_AddRoute(), which is
 * used to route IP packets to a specific address through the data connection service interface.
 * When the routes are not necessary anymore, they can be removed with le_data_DelRoute().
 * @code
 *   // Add a route to the 8.8.8.8 IP address for the cellular connection
 *   le_data_AddRoute("8.8.8.8");
 *
 *   // Remove a route to the 8.8.8.8 IP address for the cellular connection
 *   le_data_DelRoute("8.8.8.8");
 *   @endcode
 *
 * @section c_le_data_rank Technology rank
 *
 * The technology to use for the default data connection can be chosen by the applications
 * with an ordered list. If the connection becomes unavailable through a technology, the next
 * one in the list is used for the default data connection. If the connection is also unavailable
 * through the last technology of the list, the first technology will be used again.
 * The default sequence is @ref LE_DATA_WIFI at rank #1 and @ref LE_DATA_CELLULAR at rank #2.
 *
 * @note
 * - Only one list is available and therefore only one application should set the technology
 * preferences for the default data connection.
 * - The list should not be modified while the default data connection is established.
 * .
 *
 *
 * - le_data_SetTechnologyRank() sets the rank of the technology to use for the data connection
 * service.
 * le_data_SetTechnologyRank() inserts a technology into a list, so all the technologies
 * previously set with ranks @c r and @c r+n are automatically shifted to ranks @c r+1
 * and @c r+n+1. Technologies with ranks under @c r are not impacted. If the technology is already
 * in the list, it is removed from its current rank and added to the new rank.
 *
 * - le_data_GetFirstUsedTechnology() and le_data_GetNextUsedTechnology() let you retrieve
 * the different technologies of the ordered list to use for the default connection data.
 *
 * @section c_le_data_time Date and time
 *
 * When the data connection service is connected, the date and time can be retrieved from a distant
 * server using le_data_GetDate() and le_data_GetTime(). The time protocol and time server to use
 * are configured through the @ref c_le_data_configdb database:
 * - Time protocol:
 *      - @ref LE_DATA_TP to use the Time Protocol, defined in
 *        <a href="https://tools.ietf.org/html/rfc868">RFC 868</a>.
 *      - @ref LE_DATA_NTP to use the Network Time Protocol, defined in
 *        <a href="https://tools.ietf.org/html/rfc5905">RFC 5905</a>.
 * - Time server: address of the time server to connect to. If not set, the default value is
 * <tt> time.nist.gov </tt> for @ref LE_DATA_TP and to <tt> pool.ntp.org </tt> for @ref LE_DATA_NTP.
 *
 * @note The configured time protocol needs to be supported by your platform to be used by the data
 * connection service.
 *
 * @section c_le_data_configdb Configuration tree
 * @copydoc c_le_data_configdbPage_Hide
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @interface c_le_data_configdbPage_Hide
 *
 * The configuration database of the @c dataConnectionService allows configuring:
 * - the default routing
 * - the Wi-Fi access point
 * - the cellular profile
 * - the time protocol and server.
 *
 * The configuration is stored under the following path:
 * @verbatim
    dataConnectionService:/
        routing/
            useDefaultRoute<bool> == true
        wifi/
            SSID<string> == TestSsid
            secProtocol<int> == 3
            passphrase<string> == Passw0rd
        cellular/
            profileIndex<int> == index
        time/
            protocol<int> == 0
            server<string> == my.time.server.com
   @endverbatim
 *
 * @note
 * - The security protocol is one of the supported protocols defined in the
 * @ref le_wifiClient_SecurityProtocol_t enumerator.
 * - The time protocol is one of the supported protocols defined in the
 * @ref le_data_TimeProtocol_t enumerator.
 *
 * @todo This solution is temporary, as the list of access points to connect to should be
 * managed by the Wi-Fi client. Until its API is modified, the config tree is used to configure
 * the only access point to use for the default data connection.
 *
 */
/**
 * @file le_data_interface.h
 *
 * Legato @ref c_le_data include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @page c_dataConnectionServiceSample Sample code for data connection service establishment
 *
 * @include "apps/test/dataConnectionService/dataConnectionServiceTest/dcsTestComp/dcsTest.c"
 */

#ifndef LE_DATA_INTERFACE_H_INCLUDE_GUARD
#define LE_DATA_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdc_interface.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_data_DisconnectHandler_t)(void *);

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
void le_data_ConnectService
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
le_result_t le_data_TryConnectService
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
void le_data_SetServerDisconnectHandler
(
    le_data_DisconnectHandler_t disconnectHandler,
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
void le_data_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DATA_INTERFACE_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length. One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DATA_INTERFACE_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Request function and used by Release function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_data_RequestObj* le_data_RequestObjRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Technologies.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DATA_WIFI = 0,
        ///< Wi-Fi
    LE_DATA_CELLULAR = 1,
        ///< Cellular
    LE_DATA_MAX = 2
        ///< Unknown state.
}
le_data_Technology_t;


//--------------------------------------------------------------------------------------------------
/**
 * Time protocols.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DATA_TP = 0,
        ///< Time Protocol, defined in RFC 868
    LE_DATA_NTP = 1
        ///< Network Time Protocol, defined in RFC 5905
}
le_data_TimeProtocol_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_data_ConnectionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_data_ConnectionStateHandler* le_data_ConnectionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for connection state changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_data_ConnectionStateHandlerFunc_t)
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
 * Add handler function for EVENT 'le_data_ConnectionState'
 *
 * This event provides information on connection state changes
 */
//--------------------------------------------------------------------------------------------------
le_data_ConnectionStateHandlerRef_t le_data_AddConnectionStateHandler
(
    le_data_ConnectionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_data_ConnectionState'
 */
//--------------------------------------------------------------------------------------------------
void le_data_RemoveConnectionStateHandler
(
    le_data_ConnectionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Request the default data connection
 *
 * @return
 *      - Reference to the data connection (to be used later for releasing the connection)
 *      - NULL if the data connection requested could not be processed
 */
//--------------------------------------------------------------------------------------------------
le_data_RequestObjRef_t le_data_Request
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Release a previously requested data connection
 */
//--------------------------------------------------------------------------------------------------
void le_data_Release
(
    le_data_RequestObjRef_t requestRef
        ///< [IN] Reference to a previously requested data connection
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the rank of the technology used for the data connection service
 *
 * @return
 *      - @ref LE_OK if the technology is added to the list
 *      - @ref LE_BAD_PARAMETER if the technology is unknown
 *      - @ref LE_UNSUPPORTED if the technology is not available
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_SetTechnologyRank
(
    uint32_t rank,
        ///< [IN] Rank of the used technology
    le_data_Technology_t technology
        ///< [IN] Technology
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the first technology to use
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator if the list is not empty
 *      - @ref LE_DATA_MAX if the list is empty
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetFirstUsedTechnology
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the next technology to use
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator if the list is not empty
 *      - @ref LE_DATA_MAX if the list is empty or the end of the list is reached
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetNextUsedTechnology
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the technology currently used for the default data connection
 *
 * @return
 *      - One of the technologies from @ref le_data_Technology_t enumerator
 *      - @ref LE_DATA_MAX if the current technology is not set
 *
 * @note The supported technologies are @ref LE_DATA_WIFI and @ref LE_DATA_CELLULAR
 */
//--------------------------------------------------------------------------------------------------
le_data_Technology_t le_data_GetTechnology
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the default route activation status for the data connection service interface.
 *
 * @return
 *      - true:  the default route is set by the data connection service
 *      - false: the default route is not set by the data connection service
 */
//--------------------------------------------------------------------------------------------------
bool le_data_GetDefaultRouteStatus
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Add a route on the data connection service interface, if the data session is connected using
 * the cellular technology and has an IPv4 or IPv6 address.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    cellular technology not currently used
 *      - LE_BAD_PARAMETER  incorrect IP address
 *      - LE_FAULT          for all other errors
 *
 * @note Limitations:
 *      - only IPv4 is supported for the moment
 *      - route only added for a cellular connection
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_AddRoute
(
    const char* LE_NONNULL ipDestAddrStr
        ///< [IN] The destination IP address in dotted
        ///< format
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete a route on the data connection service interface, if the data session is connected using
 * the cellular technology and has an IPv4 or IPv6 address.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_UNSUPPORTED    cellular technology not currently used
 *      - LE_BAD_PARAMETER  incorrect IP address
 *      - LE_FAULT          for all other errors
 *
 * @note Limitations:
 *      - only IPv4 is supported for the moment
 *      - route only removed for a cellular connection
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_DelRoute
(
    const char* LE_NONNULL ipDestAddrStr
        ///< [IN] The destination IP address in dotted
        ///< format
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the cellular profile index used by the data connection service when the cellular technology
 * is active.
 *
 * @return
 *      - Cellular profile index
 */
//--------------------------------------------------------------------------------------------------
int32_t le_data_GetCellularProfileIndex
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the cellular profile index used by the data connection service when the cellular technology
 * is active.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if the profile index is invalid
 *      - LE_BUSY           the cellular connection is in use
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_SetCellularProfileIndex
(
    int32_t profileIndex
        ///< [IN] Cellular profile index to be used
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the date from the configured server using the configured time protocol.
 *
 * @warning An active data connection is necessary to retrieve the date.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if a parameter is incorrect
 *      - LE_FAULT          if an error occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_GetDate
(
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2017].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the time from the configured server using the configured time protocol.
 *
 * @warning An active data connection is necessary to retrieve the time.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if a parameter is incorrect
 *      - LE_FAULT          if an error occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_data_GetTime
(
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
);

#endif // LE_DATA_INTERFACE_H_INCLUDE_GUARD