

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_mdc Modem Data Control
 *
 * @ref le_mdc_interface.h "API Reference"
 *
 * <HR>
 *
 * A data session is useful for  applications that need to send or receive data over a network
 * where SMS messages are insufficient.  To start a data session, a data profile must
 * be configured as specified by the target network.
 *
 * The Modem Data Control (mdc) API is used to manage data profiles and data sessions.
 *
 * @section le_mdc_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b modemService service.
 *
 * Here's a code sample binding to modem services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_mdc -> modemService.le_mdc
   }
   @endverbatim
 *
 * @section le_mdc_profile Data Profiles
 *
 * If a pre-defined data profile has been configured then this profile can
 * be loaded using le_mdc_GetProfile().
 * le_mdc_GetProfile() must be called with @c LE_MDC_DEFAULT_PROFILE to retrieve the default index
 * used by the modem for data connection.
 * le_mdc_GetProfile() must be called with @c LE_MDC_SIMTOOLKIT_BIP_DEFAULT_PROFILE to retrieve the
 * default index used by the modem for Bearer Independent Protocol (BIP).
 * The maximum number of data profiles supported is modem dependent and can be retrieved with
 * le_mdc_NumProfiles().
 *
 * @note le_mdc_GetProfile() creates a new profile if the profile's index can't be found.
 *
 * @warning 0 is not a valid index.
 *
 * A pre-defined data profile can be retrieved using le_mdc_GetProfileFromApn() thanks to its
 * APN.
 *
 * A default APN can be set for a defined profile with le_mdc_SetDefaultAPN(), based on the SIM
 * identification number (ICCID). If no match is found in the database using the ICCID, the search
 * falls back on the home network (MCC/MNC) to determine the default APN.
 *
 * @warning Ensure to check the list of supported data profiles for your specific platform.
 *
 * The following data profile parameters can be retrieved:
 *  - Packet Data Protocol using le_mdc_GetPDP().
 *  - Access Point Name using le_mdc_GetAPN().
 *  - Authentication settings using le_mdc_GetAuthentication().
 *
 * The following data profile parameters can be set:
 *  - Packet Data Protocol using le_mdc_SetPDP().
 *  - Access Point Name using le_mdc_SetAPN().
 *  - Authentication settings using le_mdc_SetAuthentication().
 *
 * @warning The maximum APN length might be limited by the platform.
 *          Please refer to the platform documentation.
 *
 *  A sample code can be seen in the following page:
 * - @subpage c_mdcDataProfiles
 *
 * @section le_mdc_session Data Sessions
 *
 * @subsection le_mdc_session_MO Mobile Originated (MO-PDP context activation)
 *
 * le_mdc_MapProfileOnNetworkInterface() may be used to map a data session with a network interface.
 * To take effect, this API has to be called before starting the data session. Otherwise, the
 * mapping will be taken into account at the next start of the data profile.
 *
 * A data session can be started using le_mdc_StartSession(). To start a data session, a
 * data profile must be created and written to the modem, or  an existing data profile
 * can be used. A data session can be stopped using le_mdc_StopSession(). The number of
 * simultaneous data sessions supported is dependent on the modem, but cannot be more than the
 * maximum number of supported profiles.
 *
 * A data session can be started using le_mdc_StartSessionAsync() and stopped using
 * le_mdc_StopSessionAsync(). These functions are not blocking. The response will be returned
 * with the @c le_mdc_SessionHandlerFunc_t handler function.
 *
 * The current state of a data session can be queried using le_mdc_GetSessionState(). An application
 * can also a register handler to be notified when the session state changes. The handler
 * can be managed using le_mdc_AddSessionStateHandler() and le_mdc_RemoveSessionStateHandler().
 *
 * @subsection le_mdc_session_MT Mobile Terminated (MT-PDP context activation)
 * To be notified by the MT-PDP context request, the application has to subscribe to the state
 * handler using le_mdc_AddMtPdpSessionStateHandler().
 * When the incoming MT-PDP context request is notified,
 * the application takes responsibility to trigger the data session for that MT PDP request using
 * le_mdc_StartSession(). That data session can be stopped using le_mdc_StopSession().
 *
 * The network interface settings of that MT-PDP context can be retrieved using the provided
 * Data Profile.
 * Please refer to @ref le_mdc_session_networkItf.
 *
 * A data session can be rejected using le_mdc_RejectMtPdpSession().
 * The number of simultaneous data sessions supported is dependent on the modem,
 * but cannot be more than the maximum number of supported profiles.
 *
 * The handler can be released using le_mdc_RemoveMtPdpSessionStateHandler().
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 *
 * @subsection le_mdc_session_networkItf Network interface settings
 * Once a data session starts, a Linux network interface is created.  It's the application's
 * responsibility to configure the network interface, usually through a DHCP client. Query the
 * interface name using le_mdc_GetInterfaceName(). The IP Preference can be checked with
 * le_mdc_IsIPv4() or le_mdc_IsIPv6() when the profile is connected. The IP address for the
 * current data session
 * can be retrieved by  le_mdc_GetIPv4Address() or le_mdc_GetIPv6Address(). The Gateway and
 * DNS
 * addresses can be retrieved using le_mdc_GetIPv4GatewayAddress(),
 * le_mdc_GetIPv4DNSAddresses() or le_mdc_GetIPv6GatewayAddress(), le_mdc_GetIPv6DNSAddresses().
 * The Access Point Name can be retrieved by le_mdc_GetAPN(). The Data bearer
 * Technology can be retrieved by le_mdc_GetDataBearerTechnology().
 *
 * le_mdc_GetDisconnectionReason() or le_mdc_GetPlatformSpecificDisconnectionCode() let you get
 * the reason for disconnection of data session by retrieving the call end failure code
 * from @c le_mdc_ProfileRef_t.
 *
 * le_mdc_GetPlatformSpecificFailureConnectionReason() let you get the data session connection
 * failure reason by retrieving the  call connection failure code and type from
 * @c le_mdc_ProfileRef_t.
 *
 * Please refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * disconnection code description.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_mdcDataSessions
 *
 * @section le_mdc_dataStatistics Data Statistics
 *
 * The amount of received and transmitted data can be retrieved through le_mdc_GetBytesCounters().
 * The returned values correspond to the number of received and transmitted
 * bytes since the last call to le_mdc_ResetBytesCounter().
 *
 * The data statistics collection can be enabled with le_mdc_StartBytesCounter() and disabled
 * without resetting the counters with le_mdc_StopBytesCounter().
 *
 * @note The data statistics collection activation and the data counters are persistent even after
 * a reboot of the platform.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_mdcDataStatistics
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @page c_mdcDataProfiles Sample code for Data Profiles
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Profiles
 */
/**
 * @page c_mdcDataSessions Sample code for network interface settings
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Sessions
 */
/**
 * @page c_mdcDataStatistics Sample code for Data Statistics
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Statistics
 */
/**
 * @file le_mdc_interface.h
 *
 * Legato @ref c_mdc include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_MDC_INTERFACE_H_INCLUDE_GUARD
#define LE_MDC_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdc_DisconnectHandler_t)(void *);

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
void le_mdc_ConnectService
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
le_result_t le_mdc_TryConnectService
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
void le_mdc_SetServerDisconnectHandler
(
    le_mdc_DisconnectHandler_t disconnectHandler,
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
void le_mdc_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a interface name (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_INTERFACE_NAME_MAX_LEN 20

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a profile name (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_INTERFACE_NAME_MAX_BYTES 21

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in an IPv4 address (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_IPV4_ADDR_MAX_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in an IPv4 address (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_IPV4_ADDR_MAX_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in an IPv6 address (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_IPV6_ADDR_MAX_LEN 45

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in an IPv6 address (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_IPV6_ADDR_MAX_BYTES 46

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a apn name (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_APN_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a apn name (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_APN_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a user name (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_USER_NAME_MAX_LEN 64

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a user name (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_USER_NAME_MAX_BYTES 65

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a password name (not including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_PASSWORD_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes in a password name (including the null-terminator).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_PASSWORD_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Default profile definition.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_DEFAULT_PROFILE -1

//--------------------------------------------------------------------------------------------------
/**
 * Default profile definition for Bearer Independent Protocol (BIP).
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_SIMTOOLKIT_BIP_DEFAULT_PROFILE -2

//--------------------------------------------------------------------------------------------------
/**
 * @deprecated LE_MDC_DISC_USER_AUTHENTIFICATION_FAILURE is deprecated.
 * LE_MDC_DISC_USER_AUTHENTICATION_FAILURE should be used instead.
 */
//--------------------------------------------------------------------------------------------------
#define LE_MDC_DISC_USER_AUTHENTIFICATION_FAILURE 7

//--------------------------------------------------------------------------------------------------
/**
 * Reference to a modem data connection profile.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_mdc_Profile* le_mdc_ProfileRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of data bearer technologies.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDC_DATA_BEARER_TECHNOLOGY_UNKNOWN = 0,
        ///< Unknown
    LE_MDC_DATA_BEARER_TECHNOLOGY_GSM = 1,
        ///< GSM
    LE_MDC_DATA_BEARER_TECHNOLOGY_GPRS = 2,
        ///< GPRS
    LE_MDC_DATA_BEARER_TECHNOLOGY_EGPRS = 3,
        ///< Enhanced GPRS (EDGE)
    LE_MDC_DATA_BEARER_TECHNOLOGY_WCDMA = 4,
        ///< WCDMA (UMTS)
    LE_MDC_DATA_BEARER_TECHNOLOGY_HSPA = 5,
        ///< HSPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HSPA_PLUS = 6,
        ///< HSPA+
    LE_MDC_DATA_BEARER_TECHNOLOGY_DC_HSPA_PLUS = 7,
        ///< Dual Cell - HSPA+
    LE_MDC_DATA_BEARER_TECHNOLOGY_HSDPA = 8,
        ///< HSDPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HSUPA = 9,
        ///< HSUPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_DC_HSUPA = 10,
        ///< Dual Cell - HSUPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_DC_HSPA = 11,
        ///< Dual Cell - HSPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_LTE = 12,
        ///< LTE
    LE_MDC_DATA_BEARER_TECHNOLOGY_LTE_FDD = 13,
        ///< LTE FDD
    LE_MDC_DATA_BEARER_TECHNOLOGY_LTE_TDD = 14,
        ///< LTE TDD
    LE_MDC_DATA_BEARER_TECHNOLOGY_LTE_CA_DL = 15,
        ///< LTE CA DL
    LE_MDC_DATA_BEARER_TECHNOLOGY_LTE_CA_UL = 16,
        ///< LTE CA UL
    LE_MDC_DATA_BEARER_TECHNOLOGY_TD_SCDMA = 17,
        ///< TD-SCDMA
    LE_MDC_DATA_BEARER_TECHNOLOGY_CDMA2000_1X = 18,
        ///< CDMA2000 1X
    LE_MDC_DATA_BEARER_TECHNOLOGY_CDMA2000_EVDO = 19,
        ///< CDMA2000 HRPD (1xEV-DO)
    LE_MDC_DATA_BEARER_TECHNOLOGY_CDMA2000_EVDO_REVA = 20,
        ///< CDMA2000 HRPD (1xEV-DO RevA)
    LE_MDC_DATA_BEARER_TECHNOLOGY_CDMA2000_EHRPD = 21,
        ///< CDMA2000 EHRPD
    LE_MDC_DATA_BEARER_TECHNOLOGY_IS95_1X = 22,
        ///< IS95 1X
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REV0_DPA = 23,
        ///< HDR REV0 DPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVA_DPA = 24,
        ///< HDR REVA DPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVB_DPA = 25,
        ///< HDR REVB DPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVA_MPA = 26,
        ///< HDR REVA MPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVB_MPA = 27,
        ///< HDR REVB MPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVA_EMPA = 28,
        ///< HDR REVA EMPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVB_EMPA = 29,
        ///< HDR REVB EMPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_REVB_MMPA = 30,
        ///< HDR REVB MMPA
    LE_MDC_DATA_BEARER_TECHNOLOGY_HDR_EVDO_FMC = 31,
        ///< HDR EVDO FMC
    LE_MDC_DATA_BEARER_TECHNOLOGY_64_QAM = 32,
        ///< 64 QAM
    LE_MDC_DATA_BEARER_TECHNOLOGY_S2B = 33
        ///< S2B
}
le_mdc_DataBearerTechnology_t;


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of Packet Data Protocol.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDC_PDP_UNKNOWN = 0,
        ///< Unknown
    LE_MDC_PDP_IPV4 = 1,
        ///< IPv4
    LE_MDC_PDP_IPV6 = 2,
        ///< IPv6
    LE_MDC_PDP_IPV4V6 = 3
        ///< IPv4 and IPv6
}
le_mdc_Pdp_t;


//--------------------------------------------------------------------------------------------------
/**
 * Authentication bit mask.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDC_AUTH_NONE = 0x1,        ///< no authentication
    LE_MDC_AUTH_PAP = 0x2,        ///< PAP protocol
    LE_MDC_AUTH_CHAP = 0x4        ///< CHAP protocol
}
le_mdc_Auth_t;


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of connection state.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDC_DISCONNECTED = 0,
        ///< Data session is disconnected
    LE_MDC_AUTHENTICATING = 1,
        ///< Authenticating data session
    LE_MDC_CONNECTED = 2,
        ///< Data session is connected
    LE_MDC_SUSPENDING = 3,
        ///< Suspending data session
    LE_MDC_INCOMING = 4
        ///< Incoming data session (MT-PDP context request)
}
le_mdc_ConState_t;


//--------------------------------------------------------------------------------------------------
/**
 *  Enumeration of the possible reasons for the disconnection.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_MDC_DISC_NO_SERVICE = 0,
        ///< Modem has no service
    LE_MDC_DISC_OPERATOR_DETERMINED_BARRING = 1,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_INSUFFICIENT_MBMS_CAPABILITIES = 2,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_LLC_SNDCP_FAILURE = 3,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_INSUFFICIENT_RESOURCES = 4,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_MISSING_OR_UNKNOWN_APN = 5,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_UNKNOWN_PDP_ADDR_OR_TYPE = 6,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_USER_AUTHENTICATION_FAILURE = 7,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_ACTIVATION_REJECTED_BY_GGSN_OR_GW = 8,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_ACTIVATION_REJECTED_UNSPECIFIED = 9,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_SERVICE_OPTION_NOT_SUPPORTED = 10,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_REQUESTED_SERVICE_OPTION_UNSUBSCRIBED = 11,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_REQUESTED_SERVICE_OPTION_OUT_OF_ORDER = 12,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_NSAPI_ALREADY_USED = 13,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_REGULAR_DEACTIVATION = 14,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_QOS_NOT_ACCEPTED = 15,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_NETWORK_FAILURE = 16,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_REACTIVATION_REQUESTED = 17,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_FEATURE_NOT_SUPPORTED = 18,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_TFT_SEMANTIC_ERROR = 19,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_TFT_SYNTACTICAL_ERROR = 20,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_UNKNOWN_PDP_CONTEXT = 21,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_PACKET_FILTER_SEMANTIC_ERROR = 22,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_PACKET_FILTER_SYNTACTICAL_ERROR = 23,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_PDP_CONTEXT_WITHOUT_ACTIVE_TFT = 24,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_MULTICAST_TIME_OUT = 25,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_BEARER_CONTROL_MODE_VIOLATION = 26,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_IPV4_ONLY_ALLOWED = 27,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_IPV6_ONLY_ALLOWED = 28,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_SINGLE_ADDR_ONLY_ALLOWED = 29,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_COLLISION_WITH_NETWORK_INITIATED_REQ = 30,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_BEARER_HANDLING_UNSUPPORTED = 31,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_MAX_PDP_CONTEXTS = 32,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_APN_UNSUPPORTED_FOR_RAT_AND_PLMN = 33,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_INCOMPATIBLE_APN = 34,
        ///< cf. 3GPP 24.008 Annex I1
    LE_MDC_DISC_INVALID_TRANSACTION_ID = 35,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_MESSAGE_INCORRECT_SEMANTIC = 36,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_INVALID_MANDATORY_INFORMATION = 37,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_UNSUPPORTED_MESSAGE_TYPE = 38,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_MESSAGE_AND_PROTOCOL_STATE_UNCOMPATIBLE = 39,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_UNKNOWN_INFORMATION_ELEMENT = 40,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_CONDITIONAL_IE_ERROR = 41,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_MESSAGE_AND_STATE_UNCOMPATIBLE = 42,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_PROTOCOL_ERROR_UNSPECIFIED = 43,
        ///< cf. 3GPP 24.008 Annex I2
    LE_MDC_DISC_PLATFORM_SPECIFIC = 44,
        ///< Platform specific code.
    LE_MDC_DISC_UNDEFINED = 45
        ///< Undefined reason.
}
le_mdc_DisconnectionReason_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_mdc_SessionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_mdc_SessionStateHandler* le_mdc_SessionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_mdc_MtPdpSessionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_mdc_MtPdpSessionStateHandler* le_mdc_MtPdpSessionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Data session connection state changes.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdc_SessionStateHandlerFunc_t)
(
    le_mdc_ProfileRef_t profileRef,
        ///< Profile reference
    le_mdc_ConState_t ConnectionState,
        ///< Data session connection state.
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for MT-PDP Data session connection state changes.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdc_MtPdpSessionStateHandlerFunc_t)
(
    le_mdc_ConState_t ConnectionState,
        ///< MT-PDP Data session connection state.
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 *  Handler for asynchronous session start and stop result response
 *
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdc_SessionHandlerFunc_t)
(
    le_mdc_ProfileRef_t profileRef,
        ///< Profile reference
    le_result_t result,
        ///< Session start or stop result response
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mdc_SessionState'
 *
 * This event provides information on data session connection state changes for the given profileRef.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mdc_SessionStateHandlerRef_t le_mdc_AddSessionStateHandler
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] The profile object of interest
    le_mdc_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_SessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveSessionStateHandler
(
    le_mdc_SessionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mdc_MtPdpSessionState'
 *
 * This event provides information on data session connection state changes for the given profileRef.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mdc_MtPdpSessionStateHandlerRef_t le_mdc_AddMtPdpSessionStateHandler
(
    le_mdc_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_MtPdpSessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveMtPdpSessionStateHandler
(
    le_mdc_MtPdpSessionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get Profile Reference for index
 *
 * @note Create a new profile if the profile's index can't be found.
 *
 * @warning 0 is not a valid index.
 *
 * @warning Ensure to check the list of supported data profiles for your specific platform.
 *
 * @return
 *      - Reference to the data profile
 *      - NULL if the profile index does not exist
 */
//--------------------------------------------------------------------------------------------------
le_mdc_ProfileRef_t le_mdc_GetProfile
(
    uint32_t index
        ///< [IN] index of the profile.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the index for the given Profile.
 *
 * @return
 *      - index of the profile in the modem
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mdc_GetProfileIndex
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Start profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if input parameter is incorrect
 *      - LE_DUPLICATE if the data session is already connected for the given profile
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StartSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Start data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Start profile data session.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_StartSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Start data session for this profile object
    le_mdc_SessionHandlerFunc_t handlerPtr,
        ///< [IN] Handler for start data session result
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the input parameter is not valid
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StopSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Stop data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop profile data session.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_StopSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Stop data session for this profile object
    le_mdc_SessionHandlerFunc_t handlerPtr,
        ///< [IN] Handler for stop data session result
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Reject MT-PDP profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the input parameter is not valid
 *      - LE_UNSUPPORTED if not supported by the target
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_RejectMtPdpSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Reject MT-PDP data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current data session state.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT on failure
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetSessionState
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_ConState_t* connectionStatePtr
        ///< [OUT] The data session state
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the network interface name, if the data session is connected.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the interface name would not fit in interfaceNameStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetInterfaceName
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* interfaceName,
        ///< [OUT] The name of the network interface
    size_t interfaceNameSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the IPv4 address for the given profile, if the data session is connected and has an IPv4
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in ipAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4Address
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* ipAddr,
        ///< [OUT] The IP address in dotted format
    size_t ipAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the gateway IPv4 address for the given profile, if the data session is connected and has an
 * IPv4 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in gatewayAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4GatewayAddress
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* gatewayAddr,
        ///< [OUT] The gateway IP address in dotted format
    size_t gatewayAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the primary/secondary DNS v4 addresses for the given profile, if the data session is
 * connected and has an IPv4 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - If only one DNS address is available, then it will be returned, and an empty string will
 *        be returned for the unavailable address
 *      - The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4DNSAddresses
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* dns1AddrStr,
        ///< [OUT] The primary DNS IP address in dotted format
    size_t dns1AddrStrSize,
        ///< [IN]
    char* dns2AddrStr,
        ///< [OUT] The secondary DNS IP address in dotted format
    size_t dns2AddrStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the IPv6 address for the given profile, if the data session is connected and has an IPv6
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in ipAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6Address
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* ipAddr,
        ///< [OUT] The IP address in dotted format
    size_t ipAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the gateway IPv6 address for the given profile, if the data session is connected and has an
 * IPv6 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in gatewayAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6GatewayAddress
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* gatewayAddr,
        ///< [OUT] The gateway IP address in dotted format
    size_t gatewayAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the primary/secondary DNS v6 addresses, if the data session is connected and has an IPv6
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address can't fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - If only one DNS address is available, it will be returned, and an empty string will
 *        be returned for the unavailable address.
 *      - The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6DNSAddresses
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* dns1AddrStr,
        ///< [OUT] The primary DNS IP address in dotted format
    size_t dns1AddrStrSize,
        ///< [IN]
    char* dns2AddrStr,
        ///< [OUT] The secondary DNS IP address in dotted format
    size_t dns2AddrStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Allow the caller to know if the given profile is actually supporting IPv4, if the data session
 * is connected.
 *
 * @return TRUE if PDP type is IPv4, FALSE otherwise.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_mdc_IsIPv4
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Allow the caller to know if the given profile is actually supporting IPv6, if the data session
 * is connected.
 *
 * @return TRUE if PDP type is IPv6, FALSE otherwise.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_mdc_IsIPv6
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Data Bearer Technology for the given profile, if the data session is connected.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetDataBearerTechnology
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_DataBearerTechnology_t* downlinkDataBearerTechPtrPtr,
        ///< [OUT] downlink data bearer technology
    le_mdc_DataBearerTechnology_t* uplinkDataBearerTechPtrPtr
        ///< [OUT] uplink data bearer technology
);

//--------------------------------------------------------------------------------------------------
/**
 * Get number of bytes received/transmitted without error since the last reset.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - The process exits, if an invalid pointer is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetBytesCounters
(
    uint64_t* rxBytesPtr,
        ///< [OUT] bytes amount received since the last counter reset
    uint64_t* txBytesPtr
        ///< [OUT] bytes amount transmitted since the last counter reset
);

//--------------------------------------------------------------------------------------------------
/**
 * Reset received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_ResetBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop collecting received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StopBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Start collecting received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StartBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Packet Data Protocol (PDP) for the given profile.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the PDP is not supported
 *      - LE_FAULT if the data session is currently connected for the given profile
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetPDP
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Pdp_t pdp
        ///< [IN] The Packet Data Protocol
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Packet Data Protocol (PDP) for the given profile.
 *
 * @return
 *      - packet data protocol value
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_mdc_Pdp_t le_mdc_GetPDP
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Access Point Name (APN) for the given profile.
 *
 * The APN must be an ASCII string.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT if the data session is currently connected for the given profile
 *
 * @note If APN is too long (max APN_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 *
 * @warning The maximum APN length might be limited by the platform.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetAPN
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    const char* LE_NONNULL apnStr
        ///< [IN] The Access Point Name
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Access Point Name (APN) for the given profile according to the SIM identification
 * number (ICCID). If no APN is found using the ICCID, fall back on the home network (MCC/MNC)
 * to determine the default APN.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetDefaultAPN
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Access Point Name (APN) for the given profile.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_OVERFLOW if the APN is is too long
 *      - LE_FAULT on failed
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetAPN
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* apnStr,
        ///< [OUT] The Access Point Name
    size_t apnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Set authentication property
 *
 * @return
 *      - LE_OK on success
 *
 * @note
 *      - The process exits, if userName or password are NULL when type is not PA_MDC_AUTH_NONE
 *      - The process exits, if an invalid profile object is given
 * @note If userName is too long (max USER_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 * @note If password is too long (max PASSWORD_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 * @note Both PAP and CHAP authentication can be set for 3GPP network: in this case, the device
 *       decides which authentication procedure is performed. For example, the device can have a
 *       policy to select the most secure authentication mechanism.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetAuthentication
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Auth_t type,
        ///< [IN] Authentication type
    const char* LE_NONNULL userName,
        ///< [IN] UserName used by authentication
    const char* LE_NONNULL password
        ///< [IN] Password used by authentication
);

//--------------------------------------------------------------------------------------------------
/**
 * Get authentication property
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_OVERFLOW userName or password are too small
 *      - LE_FAULT on failed
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetAuthentication
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Auth_t* typePtr,
        ///< [OUT] Authentication type
    char* userName,
        ///< [OUT] UserName used by authentication
    size_t userNameSize,
        ///< [IN]
    char* password,
        ///< [OUT] Password used by authentication
    size_t passwordSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of profiles on the modem.
 *
 * @return
 *      - number of profiles existing on modem
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mdc_NumProfiles
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get a profile selected by its APN
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_NOT_FOUND if the requested APN is not found
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetProfileFromApn
(
    const char* LE_NONNULL apnStr,
        ///< [IN] The Access Point Name
    le_mdc_ProfileRef_t* profileRefPtr
        ///< [OUT] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the disconnection reason.
 *
 * @return The disconnection reason.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_mdc_DisconnectionReason_t le_mdc_GetDisconnectionReason
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific disconnection code.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * disconnection code description.
 *
 * @return The platform specific disconnection code.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mdc_GetPlatformSpecificDisconnectionCode
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific connection failure reason.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_GetPlatformSpecificFailureConnectionReason
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] profile reference
    int32_t* failureTypePtr,
        ///< [OUT] platform specific failure type
    int32_t* failureCodePtr
        ///< [OUT] platform specific failure code
);

//--------------------------------------------------------------------------------------------------
/**
 * Map a profile on a network interface
 *
 * * @return
 *      - LE_OK on success
 *      - LE_UNSUPPORTED if not supported by the target
 *      - LE_FAULT for all other errors
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_MapProfileOnNetworkInterface
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Profile reference
    const char* LE_NONNULL interfaceName
        ///< [IN] Network interface name
);

#endif // LE_MDC_INTERFACE_H_INCLUDE_GUARD