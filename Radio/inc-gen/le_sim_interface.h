

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_sim SIM
 *
 * @ref le_sim_interface.h "API Reference" <br>
 * @ref platformConstraintsSim "SIM constraints"
 *
 * <HR>
 *
 * This file contains prototype definitions for SIM API.
 *
 * A subscriber identity module or subscriber identification module (SIM) is an integrated circuit
 * that securely stores the international mobile subscriber identity (IMSI) and related key used to
 * identify and authenticate subscribers on M2M devices.
 *
 * Most SIM cards can store a number of SMS messages and phone book contacts.
 *
 * le_sim_GetSelectedCard() returns the selected SIM card number.
 *
 * @section le_sim_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b modemService.
 *
 * Here's a code sample binding to modem services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_sim -> modemService.le_sim
   }
   @endverbatim
 *
 *
 * @section le_sim_SelectCard Select a card to use
 * le_sim_SelectCard() function is used to select the SIM identifier. By default, the SIM in slot 1
 * is used.
 * Additionally, any Legato SIM API with a SIM card identifier passed in parameter, selects that
 * SIM identifier. le_sim_GetSelectedCard() returns the current selected card, not necessarily the
 * one set previously by le_sim_SelectCard().
 *
 * @note The SIM selection is not reset persistent; this function has to be called at each start-up.
 *
 * @note It is recommended to wait for a SIM handler notification after a new SIM selection before
 * calling le_sim API functions.
 *
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestSelect
 *
 * @section le_sim_id SIM identification information
 * \b ICCID:
 * Each SIM is internationally identified by its integrated circuit card identifier (ICCID). ICCIDs
 * are stored in the SIM cards and engraved or printed on the SIM card body.
 * The ICCID is defined by the ITU-T recommendation E.118 as the
 * Primary Account Number. According to E.118, the number is up to 19 digits long, including a
 * single check digit calculated using the Luhn algorithm. However, the GSM Phase 1 (ETSI
 * Recommendation GSM 11.11) defined the ICCID length as 10 octets (20 digits) with
 * operator-specific structure.
 *
 * le_sim_GetICCID() API reads the identification number (ICCID).
 *
 * Using this API selects the requested SIM.
 *
 * \b IMSI:
 * The International Mobile Subscriber Identity or IMSI is a unique identification associated with
 * all cellular networks. The IMSI is used in any mobile network that connects with other
 * networks. For GSM, UMTS and LTE network, this number is provisioned in the SIM card.
 *
 * An IMSI is usually presented as a 15 digit long number, but can be shorter. The first 3 digits
 * are the mobile country code (MCC), are followed by the mobile network code (MNC), either 2
 * digits (European standard) or 3 digits (North American standard). The length of the MNC depends
 * on the value of the MCC. The remaining digits are the mobile subscription identification number
 * (MSIN) within the network's customer base.
 *
 * \b EID:
 * The EID (also called eUICCID) is the unique identifier for the embedded Universal Integrated
 * Circuit Card (eUICC).
 * A eUICC is a SIM card with a remote provisioning function, and is designed to not be removed or
 * changed. It is able to store multiple communication profiles but only one is enabled (recognized
 * by the device and used for communication). With conventional SIM cards, the ICCID is used as the
 * unique key to identify the SIM card, but with eUICC, the ICCID is the key used to identify a
 * profile, and a new identifier is defined, called the eUICCID (EID), which is used as the unique
 * key for the embedded SIM.
 * le_sim_GetEID() API reads the EID.
 *
 * \b Home \b Network \b Name:
 * le_sim_GetHomeNetworkOperator() retrieves the Home Network Name.
 *
 * le_sim_GetIMSI() API reads the international mobile subscriber identity (IMSI).
 *
 * Using this API selects the requested SIM.
 *
 * \b Phone \b Number:
 * le_sim_GetSubscriberPhoneNumber() API reads the Phone Number associated to the SIM.
 * If the phone number has not been provisioned, it will return the empty string.
 *
 * Using this API selects the requested SIM.
 *
 * \b Home \b Network \b Information:
 * - le_sim_GetHomeNetworkOperator()function retrieves the Home Network Name.
 * - le_sim_GetHomeNetworkMccMnc()function retrieves the Home Network MCC (Mobile Country Code)
 *      and MNC (Mobile Network Code).
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestIdentification
 *
 * @section le_sim_auth SIM Authentication
 * le_sim_EnterPIN() enters the PIN (Personal Identification Number) code that's
 * required before any Mobile equipment functionality can be used.
 *
 * Using this API selects the requested SIM.
 *
 * le_sim_GetRemainingPINTries() returns the number of remaining PIN entry attempts
 * before the SIM will become blocked.
 *
 * Using this API selects the requested SIM.
 *
 * le_sim_GetRemainingPUKTries() returns the number of remaining PUK entry attempts
 * before the SIM will become blocked.
 *
 * Using this API selects the requested SIM.
 *
 * le_sim_ChangePIN() must be called to change the PIN code.
 *
 * Using this API selects the requested SIM.
 *
 *  le_sim_Lock() locks the SIM card: it enables requests for the PIN code.
 *
 * Using this API selects the requested SIM.
 *
 *  le_sim_Unlock() unlocks the SIM card: it disables requests for the PIN code.
 *
 * Using this API selects the requested SIM.
 *
 * le_sim_Unblock() unblocks the SIM card. The SIM card is blocked after X unsuccessful
 * attempts to enter the PIN. le_sim_Unblock() requires the PUK (Personal Unblocking) code
 * to set a new PIN code.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestAuthentication
 *
 * @section le_sim_state SIM states
 * le_sim_IsPresent() API advises the SIM is inserted (and locked) or removed.
 *
 * Using this API selects the requested SIM.
 *
 * le_sim_IsReady() API advises the SIM is ready (PIN code correctly entered
 * or not required).
 *
 * Using this API selects the requested SIM.
 *
 * The le_sim_GetState() API retrieves the SIM state:
 * - LE_SIM_INSERTED      : SIM card is inserted and locked.
 * - LE_SIM_ABSENT        : SIM card is absent.
 * - LE_SIM_READY         : SIM card is inserted and unlocked.
 * - LE_SIM_BLOCKED       : SIM card is blocked.
 * - LE_SIM_BUSY          : SIM card is busy.
 * - LE_SIM_STATE_UNKNOWN : Unknown SIM state.
 *
 * Using this API selects the requested SIM.
 *
 * A handler function must be registered to receive SIM's state notifications.
 * le_sim_AddNewStateHandler() API allows the User to register that handler.
 *
 * The handler must satisfy the following prototype:
 * typedef void(*le_sim_NewStateHandlerFunc_t)(@ref le_sim_Id_t simId, @c le_sim_States_t simState);
 *
 * When a new SIM's state is notified, the handler is called.
 *
 * Call le_sim_GetState() to retrieve the new state of the SIM.
 *
 * @note If two (or more) applications have registered a handler function for notifications, they
 * will all receive it and will be passed the same SIM.
 *
 * The application can uninstall the handler function by calling le_sim_RemoveNewStateHandler() API.
 *
 * @warning Your platform might need a reboot to detect a SIM insertion or removal.
 * Please refer to the @ref platformConstraintsSim "SIM constraints" page or your platform
 * documentation for further details.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestStates
 *
 * @section le_sim_profile_switch Local SIM profile switch
 *
 * As soon as there are several subscriptions/profiles in the eUICC (multi-profile), and one of
 * them is dedicated to emergency calls (ex: eCall, ERA-Glonass), local swap is needed to swap as
 * quickly as possible to the emergency profile in case of need.
 *
 * “Local swap” means that the User's application must be able to directly request the eUICC to
 * swap to Emergency Call Subscription (ECS).
 *
 * Local swap puts the eUICC in a temporary state, meaning the commercial subscription is replaced
 * by emergency subscription for a limited time, event triggering the swap back to commercial
 * subscription being controlled by the User's application.
 *
 * The le_sim_LocalSwapToEmergencyCallSubscription() function requests the multi-profile eUICC to
 * swap to ECS and to refresh. The User's application must wait for eUICC reboot to be finished and
 * network connection available.
 *
 * The le_sim_LocalSwapToCommercialSubscription() function requests the multi-profile eUICC to swap
 * back to commercial subscription and to refresh. The User's application must wait for eUICC reboot
 * to be finished and network connection available.
 *
 * The User's application can install an handler with le_sim_AddNewStateHandler() to receive eUICC's
 * state notifications.
 *
 * @warning
 * - If you use a Morpho or Oberthur card, the SIM_REFRESH PRO-ACTIVE command must be accepted with
 *   le_sim_AcceptSimToolkitCommand() in order to complete the profile swap procedure.
 * - If you use a Giesecke & Devrient (G&D) card, be sure that your platform has disabled
 *   security restrictions for channel management APDU commands, otherwise local SIM profile switch
 *   could not work.
 *
 * The le_sim_IsEmergencyCallSubscriptionSelected() function must be called to get the current
 * subscription.
 *
 * @warning There is no standard method to interrogate the current selected subscription. The
 * returned value of this function is based on the last executed local swap command. This means
 * that this function will always return LE_NOT_FOUND error at Legato startup.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestProfileSwitch
 *
 * @section le_sim_Reset SIM Reset
 *
 * The le_sim_Reset() function resets the SIM card.
 *
 * @section le_sim_FPLMNList Read / Write FPLMN List from SIM
 *
 * The le_sim_CreateFPLMNList() function creates the empty FPLMN list.
 *
 * The le_sim_AddFPLMNOperator() function adds the FPLMN network into FPLMN list.
 *
 * The le_sim_WriteFPLMNList() function writes FPLMN list into the SIM.
 *
 * The le_sim_ReadFPLMNList() function reads the FPLMN list from the SIM card.
 *
 * The le_sim_GetFirstFPLMNOperator() function fetches the first FPLMN operator from FPLMN list.
 *
 * The le_sim_GetNextFPLMNOperator() function fetches the next FPLMN operator from FPLMN list.
 *
 * The le_sim_DeleteFPLMNList() function releases all allocated resources associated with the
 * List object.
 *
 * @note Some platforms do not support FPLMN APIs. Please refer to the @ref platformConstraintsSim
 * "SIM constraints" page or your platform documentation for further details.
 *
 * @section le_sim_stk SIM Toolkit
 *
 * The SIM application Toolkit allows the SIM card to initiate commands or asking input from the
 * modem to accept/reject SIM operations.
 *
 * One of the use case is the remote provisioning of an embedded UICC (eUICC): the eUICC format
 * supports multiple subscription profiles, which can be remotely provisioned, updated or selected
 * through SIM Toolkit procedures (Bearer Independent Protocol @em BIP, SIM refresh).
 *
 * It is mainly used for in-vehicle emergency call service (eCall).
 *
 * An eUICC can be remotely managed to change the Mobile Network Operator subscription.
 *
 * The le_sim_AddSimToolkitEventHandler() API registers a handler to be notified of SIM
 * Toolkit events. The le_sim_RemoveSimToolkitEventHandler() API unregisters the handler.
 *
 * The last received SIM Toolkit command can:
 * - Either be accepted by the device with the le_sim_AcceptSimToolkitCommand() API
 * - Or be rejected by the device with the le_sim_RejectSimToolkitCommand() API.
 * Note that if no handler is registered, refresh requests are automatically accepted.
 *
 * In case the last SIM Toolkit command is a Refresh command (@ref LE_SIM_REFRESH), additional
 * information can be retrieved:
 * - The le_sim_GetSimToolkitRefreshStage() API gets the stage of the Refresh command.
 * - The le_sim_GetSimToolkitRefreshMode() API gets the mode of the Refresh command, as defined in
 *   ETSI TS 102 223 sections 6.4.7 and 8.6.
 *
 * A sample code using the SIM Toolkit APIs can be seen in the following page:
 * - @subpage c_simTestSimToolkit
 *
 * Information related to SIM Toolkit platform constraints can be found in the
 * @ref platformConstraintsStk "SIM Toolkit platform constraints" page.
 *
 * @section le_sim_access SIM access
 *
 * The SIM card content can be accessed and/or modified by several methods.
 *
 * @subsection le_sim_accessApdu APDU
 *
 * The application can send an APDU (Application Protocol Data Unit) to the SIM using
 * le_sim_SendApdu() API. The user must encode the APDU as specified by in recommendation 3GPP
 * 11.11, 3GPP 51.011, 3GPP 31.102, 3GPP 31.103 or ETSI TS 102 221.
 * @note Between two successive calls to le_sim_SendApdu() API, there is no locking
 * protection. In this situation, some command types and parameters can modify SIM files
 * incorrectly.
 *
 * @subsection le_sim_accessCommand Commands
 *
 * Using le_sim_SendCommand(), the application has easier but more limited access to the
 * SIM database. The command is transmitted to the SIM, which gives information through swi1 and
 * swi2 about the execution of the command (see 3GPP recommendation previously mentioned for
 * their coding).
 * Some parameters are platform dependent, see @subpage platformConstraintsSim "SIM constraints" for
 * their coding.
 *
 * @subsection le_sim_accessLogicalChannel Logical channels
 *
 * Logical channels are specified by the standard ETSI TS 102 221 in the section 8.7. If they are
 * supported by the SIM card, logical channels allow to send independent APDUs on the different
 * channels. In this case:
 * - the basic channel 0 is always available and opened.
 * - upon request, the card assigns a number to open a new channel. This channel remains open until
 *   it is explicitly closed or the SIM card is deactivated.
 *
 * Use le_sim_OpenLogicalChannel() API to open a logical channel on the SIM card. APDUs can then
 * be sent to the SIM card with le_sim_SendApduOnChannel(). When the logical channel is not
 * needed anymore, it can be closed using le_sim_CloseLogicalChannel().
 *
 * @subsection le_sim_accessSampleCode Sample code
 *
 * A sample code can be seen in the following page:
 * - @subpage c_simTestApdu
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_sim_interface.h
 *
 * Legato @ref c_sim include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @page c_simTestProfileSwitch Sample code for Local SIM profile switch
 *
 * @include "apps/test/modemServices/sim/simProfileSwap/simTestComp/simTest.c"
 */
/**
 * @page c_simTestSimToolkit Sample code for Local SIM Toolkit
 *
 * @include "apps/test/modemServices/sim/simToolkit/simToolkitComp/simToolkitTest.c"
 */
/**
 * @page c_simTestAuthentication Sample code for SIM Authentication
 *
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Define
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/main.c" Print
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Authentication
 */
/**
 * @page c_simTestIdentification Sample code for SIM Identification
 *
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/main.c" Print
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Identification
 */
/**
 * @page c_simTestSelect Sample code for SIM Select
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Select
 */
/**
 * @page c_simTestStates Sample code for SIM States
 *
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/main.c" Print
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Display
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" State handler
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" State
 */
/**
 * @page c_simTestApdu Sample code for SIM access
 * @snippet "apps/test/modemServices/sim/simIntegrationTest/simTestComp/simTest.c" Apdu
 */

#ifndef LE_SIM_INTERFACE_H_INCLUDE_GUARD
#define LE_SIM_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdmDefs_interface.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_sim_DisconnectHandler_t)(void *);

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
void le_sim_ConnectService
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
le_result_t le_sim_TryConnectService
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
void le_sim_SetServerDisconnectHandler
(
    le_sim_DisconnectHandler_t disconnectHandler,
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
void le_sim_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Minimum PIN length (4 digits)
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PIN_MIN_LEN 4

//--------------------------------------------------------------------------------------------------
/**
 * Maximum PIN length (8 digits)
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PIN_MAX_LEN 8

//--------------------------------------------------------------------------------------------------
/**
 * Maximum PIN length (8 digits)
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PIN_MAX_BYTES 9

//--------------------------------------------------------------------------------------------------
/**
 * PUK length (8 digits)
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PUK_MAX_LEN 8

//--------------------------------------------------------------------------------------------------
/**
 * PUK length (8 digits)
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PUK_MAX_BYTES 9

//--------------------------------------------------------------------------------------------------
/**
 * ICCID length
 * According to GSM Phase 1
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_ICCID_LEN 20

//--------------------------------------------------------------------------------------------------
/**
 * ICCID length
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_ICCID_BYTES 21

//--------------------------------------------------------------------------------------------------
/**
 * IMSI length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_IMSI_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * IMSI length
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_IMSI_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * APDU length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_APDU_MAX_BYTES 255

//--------------------------------------------------------------------------------------------------
/**
 * SIM response length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_RESPONSE_MAX_BYTES 256

//--------------------------------------------------------------------------------------------------
/**
 * SIM file identifier length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_FILE_ID_LEN 4

//--------------------------------------------------------------------------------------------------
/**
 * SIM file identifier length
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_FILE_ID_BYTES 5

//--------------------------------------------------------------------------------------------------
/**
 * SIM data command length
 *
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_DATA_MAX_BYTES 100

//--------------------------------------------------------------------------------------------------
/**
 * SIM file path length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PATH_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * SIM file path length
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_PATH_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * eUICC identifier (EID) length
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_EID_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * eUICC identifier (EID) length
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_SIM_EID_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Reference type for FPLMN list.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_sim_FPLMNList* le_sim_FPLMNListRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM states.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_INSERTED = 0,
        ///< SIM card is inserted and locked.
    LE_SIM_ABSENT = 1,
        ///< SIM card is absent.
    LE_SIM_READY = 2,
        ///< SIM card is inserted and unlocked.
    LE_SIM_BLOCKED = 3,
        ///< SIM card is blocked.
    LE_SIM_BUSY = 4,
        ///< SIM card is busy.
    LE_SIM_STATE_UNKNOWN = 5
        ///< Unknown SIM state.
}
le_sim_States_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM identifiers.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_EMBEDDED = 0,
        ///< Embedded SIM
    LE_SIM_EXTERNAL_SLOT_1 = 1,
        ///< SIM inserted in external slot 1
    LE_SIM_EXTERNAL_SLOT_2 = 2,
        ///< SIM inserted in external slot 2
    LE_SIM_REMOTE = 3,
        ///< Remote SIM
    LE_SIM_ID_MAX = 4
        ///<
}
le_sim_Id_t;


//--------------------------------------------------------------------------------------------------
/**
 * Card Manufacturer.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_OBERTHUR = 0,
        ///< Oberthur.
    LE_SIM_GEMALTO = 1,
        ///< Gemalto.
    LE_SIM_G_AND_D = 2,
        ///< G&D.
    LE_SIM_MORPHO = 3,
        ///< Morpho.
    LE_SIM_VALID = 4,
        ///< Valid.
    LE_SIM_MANUFACTURER_MAX = 5
        ///<
}
le_sim_Manufacturer_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM commands.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_READ_RECORD = 0,
        ///< Read a file record.
    LE_SIM_READ_BINARY = 1,
        ///< Read a transparent elementary file.
    LE_SIM_UPDATE_RECORD = 2,
        ///< Update a file record.
    LE_SIM_UPDATE_BINARY = 3,
        ///< Update a transparent elementary file.
    LE_SIM_COMMAND_MAX = 4
        ///< Max value
}
le_sim_Command_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM Toolkit events.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_OPEN_CHANNEL = 0,
        ///< SIM card asks to open a logical channel.
    LE_SIM_REFRESH = 1,
        ///< SIM card asks for a refresh.
    LE_SIM_STK_EVENT_MAX = 2
        ///< Unknown SIM Toolkit event.
}
le_sim_StkEvent_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM Toolkit Refresh modes, defined in ETSI TS 102 223 sections 6.4.7 and 8.6.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_REFRESH_INIT_FULL_FCN = 0,
        ///< Initialization and Full File Change Notification.
    LE_SIM_REFRESH_FCN = 1,
        ///< File Change Notification.
    LE_SIM_REFRESH_INIT_FCN = 2,
        ///< Initialization and File Change Notification.
    LE_SIM_REFRESH_INIT = 3,
        ///< Initialization.
    LE_SIM_REFRESH_RESET = 4,
        ///< SIM reset.
    LE_SIM_REFRESH_APP_RESET = 5,
        ///< Application reset.
    LE_SIM_REFRESH_SESSION_RESET = 6,
        ///< Session reset.
    LE_SIM_REFRESH_STEERING_OF_ROAMING = 7,
        ///< Steering of Roaming as defined in 3GPP TS 23.122.
    LE_SIM_REFRESH_STEERING_OF_ROAMING_IWLAN = 8,
        ///< Steering of Roaming for I-WLAN
        ///< as defined in 3GPP TS 24.234.
    LE_SIM_REFRESH_EUICC_PROFILE_CHANGE = 9,
        ///< eUICC Profile State Change.
    LE_SIM_REFRESH_MODE_MAX = 10
        ///< Invalid SIM Toolkit Refresh mode.
}
le_sim_StkRefreshMode_t;


//--------------------------------------------------------------------------------------------------
/**
 * SIM Toolkit Refresh stages.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_SIM_STAGE_WAITING_FOR_OK = 0,
        ///< Waiting for OK to refresh.
    LE_SIM_STAGE_END_WITH_SUCCESS = 1,
        ///< Refresh succeeded.
    LE_SIM_STAGE_END_WITH_FAILURE = 2,
        ///< Refresh failed.
    LE_SIM_STAGE_MAX = 3
        ///< Invalid SIM Toolkit Refresh stage.
}
le_sim_StkRefreshStage_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_sim_NewState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_sim_NewStateHandler* le_sim_NewStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_sim_SimToolkitEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_sim_SimToolkitEventHandler* le_sim_SimToolkitEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for sim state changes.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_sim_NewStateHandlerFunc_t)
(
    le_sim_Id_t simId,
        ///< The SIM identifier.
    le_sim_States_t simState,
        ///< The SIM state.
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for Sim Toolkit Events.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_sim_SimToolkitEventHandlerFunc_t)
(
    le_sim_Id_t simId,
        ///< The SIM identifier.
    le_sim_StkEvent_t stkEvent,
        ///< The SIM state.
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_sim_NewState'
 *
 * This event provides information on sim state changes.
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_NewStateHandlerRef_t le_sim_AddNewStateHandler
(
    le_sim_NewStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_NewState'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveNewStateHandler
(
    le_sim_NewStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_sim_SimToolkitEvent'
 *
 * This event provides information on Sim Toolkit application.
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_SimToolkitEventHandlerRef_t le_sim_AddSimToolkitEventHandler
(
    le_sim_SimToolkitEventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_SimToolkitEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveSimToolkitEventHandler
(
    le_sim_SimToolkitEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current selected card.
 *
 * @return Number of the current selected SIM card.
 */
//--------------------------------------------------------------------------------------------------
le_sim_Id_t le_sim_GetSelectedCard
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Select a SIM.
 *
 * @return LE_FAULT         Function failed to select the requested SIM
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SelectCard
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the integrated circuit card identifier (ICCID) of the SIM card (20 digits)
 *
 * @return LE_OK             ICCID was successfully retrieved.
 * @return LE_OVERFLOW       iccidPtr buffer was too small for the ICCID.
 * @return LE_BAD_PARAMETER  Invalid parameters.
 * @return LE_FAULT          The ICCID could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetICCID
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* iccid,
        ///< [OUT] ICCID
    size_t iccidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the identifier for the embedded Universal Integrated Circuit Card identifier (EID)
 * (32 digits)
 *
 * @return LE_OK             EID was successfully retrieved.
 * @return LE_OVERFLOW       eidPtr buffer was too small for the EID.
 * @return LE_BAD_PARAMETER  Invalid parameters.
 * @return LE_FAULT          The EID could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @warning le_sim_GetEID() function is platform dependent. Please refer to the
 *          @ref platformConstraintsSim_EID section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetEID
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* eid,
        ///< [OUT] EID
    size_t eidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the identification number (IMSI) of the SIM card. (max 15 digits)
 *
 * @return LE_OVERFLOW      The imsiPtr buffer was too small for the IMSI.
 * @return LE_BAD_PARAMETER The parameters are invalid.
 * @return LE_FAULT         The function failed.
 * @return LE_TIMEOUT       No response was received.
 * @return LE_OK            The function succeeded.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetIMSI
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* imsi,
        ///< [OUT] IMSI
    size_t imsiSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM card is present or not.
 *
 * @return true   SIM card is present.
 * @return false  SIM card is absent
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsPresent
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM is ready (PIN code correctly inserted or not
 * required).
 *
 * @return true   PIN is correctly inserted or not required.
 * @return false  PIN must be inserted
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsReady
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to enter the PIN code.
 *
 * @return LE_BAD_PARAMETER The parameters are invalid.
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     The PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to enter the PIN code.
 * @return LE_OK            The function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_EnterPIN
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Change the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is/are not long enough (min 4 digits).
 * @return LE_FAULT         Function failed to change the PIN code.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_ChangePIN
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL oldpin,
        ///< [IN] The old PIN code.
    const char* LE_NONNULL newpin
        ///< [IN] The new PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of remaining PIN insertion tries.
 *
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_FAULT         The function failed to get the number of remaining PIN insertion tries.
 * @return A positive value The function succeeded. The number of remaining PIN insertion tries.
 *
 */
//--------------------------------------------------------------------------------------------------
int32_t le_sim_GetRemainingPINTries
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of remaining PUK insertion tries.
 *
 * @return LE_OK            On success.
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_FAULT         The function failed to get the number of remaining PUK insertion tries.
 *
 * @note If the caller is passing an null pointer to this function, it is a fatal error
 *       and the function will not return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetRemainingPUKTries
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    uint32_t* remainingPukTriesPtrPtr
        ///< [OUT] The number of remaining PUK insertion tries.
);

//--------------------------------------------------------------------------------------------------
/**
 * Unlock the SIM card: it disables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unlock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Lock the SIM card: it enables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Lock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Unblock the SIM card.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_OUT_OF_RANGE  PUK code length is not correct (8 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If new PIN or puk code are too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unblock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL puk,
        ///< [IN] The PUK code.
    const char* LE_NONNULL newpin
        ///< [IN] The PIN code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM state.
 *
 * @return Current SIM state.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_sim_States_t le_sim_GetState
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM Phone Number.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the Phone Number can't fit in phoneNumberStr
 *      - LE_BAD_PARAMETER if a parameter is invalid
 *      - LE_FAULT on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSubscriberPhoneNumber
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* phoneNumberStr,
        ///< [OUT] The phone Number.
    size_t phoneNumberStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Home Network Name information.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the Home Network Name can't fit in nameStr
 *      - LE_BAD_PARAMETER if a parameter is invalid
 *      - LE_FAULT on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @note The home network name can be given even if the device is not registered on the network.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetHomeNetworkOperator
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* nameStr,
        ///< [OUT] the home network Name
    size_t nameStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Home Network MCC MNC.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if Home Network has not been provisioned
 *      - LE_FAULT for unexpected error
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetHomeNetworkMccMnc
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* mccPtr,
        ///< [OUT] Mobile Country Code
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code
    size_t mncPtrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to request the multi-profile eUICC to swap to ECS and to refresh.
 * The User's application must wait for eUICC reboot to be finished and network connection
 * available.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier
 *      - LE_BUSY when a profile swap is already in progress
 *      - LE_FAULT for unexpected error
 *
 * @warning If you use a Morpho or Oberthur card, the SIM_REFRESH PRO-ACTIVE command must be
 *          accepted with le_sim_AcceptSimToolkitCommand() in order to complete the profile swap
 *          procedure.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_LocalSwapToEmergencyCallSubscription
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Manufacturer_t manufacturer
        ///< [IN] The card manufacturer.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to request the multi-profile eUICC to swap back to commercial
 * subscription and to refresh.
 * The User's application must wait for eUICC reboot to be finished and network connection
 * available.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier
 *      - LE_BUSY when a profile swap is already in progress
 *      - LE_FAULT for unexpected error
 *
 * @warning If you use a Morpho or Oberthur card, the SIM_REFRESH PRO-ACTIVE command must be
 *          accepted with le_sim_AcceptSimToolkitCommand() in order to complete the profile swap
 *          procedure.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_LocalSwapToCommercialSubscription
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Manufacturer_t manufacturer
        ///< [IN] The card manufacturer.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the current subscription.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier or null ECS pointer is passed
 *      - LE_NOT_FOUND cannot determine the current selected subscription
 *      - LE_FAULT for unexpected errors
 *
 * @warning There is no standard method to interrogate the current selected subscription. The
 * returned value of this function is based on the last executed local swap command. This means
 * that this function will always return LE_NOT_FOUND error at Legato startup.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_IsEmergencyCallSubscriptionSelected
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier
    bool* isEcsPtr
        ///< [OUT] true if Emergency Call Subscription (ECS) is selected,
        ///<       false if Commercial Subscription is selected
);

//--------------------------------------------------------------------------------------------------
/**
 * Accept the last SIM Toolkit command.
 *
 * @return
 *  - LE_OK       The function succeeded.
 *  - LE_FAULT    The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_AcceptSimToolkitCommand
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Reject the last SIM Toolkit command.
 *
 * @return
 *  - LE_OK       The function succeeded.
 *  - LE_FAULT    The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_RejectSimToolkitCommand
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the mode of the last SIM Toolkit Refresh command.
 * The modes are defined in ETSI TS 102 223 sections 6.4.7 and 8.6.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNAVAILABLE    The last SIM Toolkit command is not a Refresh command.
 *  - LE_FAULT          The function failed.
 *  - LE_UNSUPPORTED    The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSimToolkitRefreshMode
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_StkRefreshMode_t* refreshModePtr
        ///< [OUT] The Refresh mode.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the stage of the last SIM Toolkit Refresh command.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNAVAILABLE    The last SIM Toolkit command is not a Refresh command.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSimToolkitRefreshStage
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_StkRefreshStage_t* refreshStagePtr
        ///< [OUT] The Refresh stage.
);

//--------------------------------------------------------------------------------------------------
/**
 * Send APDU command to the SIM.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          The function failed.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      The function failed to select the SIM card for this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendApdu
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const uint8_t* commandApduPtr,
        ///< [IN] APDU command.
    size_t commandApduSize,
        ///< [IN]
    uint8_t* responseApduPtr,
        ///< [OUT] SIM response.
    size_t* responseApduSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Send a command to the SIM.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          The function failed.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      - The function failed to select the SIM card for this operation
 *                          - The requested SIM file is not found
 *      - LE_OVERFLOW       Response buffer is too small to copy the SIM answer.
 *      - LE_UNSUPPORTED    The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendCommand
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Command_t command,
        ///< [IN] The SIM command.
    const char* LE_NONNULL fileIdentifier,
        ///< [IN] File identifier
    uint8_t p1,
        ///< [IN] Parameter P1 passed to the SIM
    uint8_t p2,
        ///< [IN] Parameter P2 passed to the SIM
    uint8_t p3,
        ///< [IN] Parameter P3 passed to the SIM
    const uint8_t* dataPtr,
        ///< [IN] data command.
    size_t dataSize,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN] path of the elementary file
    uint8_t* sw1Ptr,
        ///< [OUT] Status Word 1 received from the SIM
    uint8_t* sw2Ptr,
        ///< [OUT] Status Word 2 received from the SIM
    uint8_t* responsePtr,
        ///< [OUT] SIM response.
    size_t* responseSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Reset the SIM.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Reset
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Create empty FPLMN list to insert FPLMN operators.
 *
 * @return
 *      - Reference to the List object.
 *      - Null pointer if not able to create list reference.
 */
//--------------------------------------------------------------------------------------------------
le_sim_FPLMNListRef_t le_sim_CreateFPLMNList
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Add FPLMN network into the newly created FPLMN list.
 * If the FPLMNListRef, mcc or mnc is not valid then this function will kill the calling client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      If FPLMN operator can not be inserted into FPLMN list.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_AddFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code.
    const char* LE_NONNULL mnc
        ///< [IN] Mobile Network Code.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write FPLMN list into the SIM.
 * If the FPLMNListRef is not valid then this function will kill the calling client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_WriteFPLMNList
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_FPLMNListRef_t FPLMNListRef
        ///< [IN] FPLMN list reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read FPLMN list from the SIM.
 *
 * @return
 *      - Reference to the List object.
 *      - Null pointer if FPLMN list is not able to read from the SIM.
 */
//--------------------------------------------------------------------------------------------------
le_sim_FPLMNListRef_t le_sim_ReadFPLMNList
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read the first FPLMN network from the list of FPLMN objects retrieved with
 * le_sim_ReadFPLMNList().
 * If the FPLMNListRef, mccPtr or mncPtr is not valid then this function will kill the calling
 * client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      MCC/MNC string size is greater than string length parameter which has
 *                         been given into this function.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetFirstFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    char* mccPtr,
        ///< [OUT] Mobile Country Code.
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code.
    size_t mncPtrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Read the next FPLMN network from the list of FPLMN objects retrieved with le_sim_ReadFPLMNList().
 * If the FPLMNListRef, mccPtr or mncPtr is not valid then this function will kill the calling
 * client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      MCC/MNC string size is greater than string length parameter which has
 *                         been given into this function.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetNextFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    char* mccPtr,
        ///< [OUT] Mobile Country Code.
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code.
    size_t mncPtrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete the FPLMN list created by le_sim_ReadFPLMNList() or le_sim_CreateFPLMNList().
 * If the FPLMNListRef is not valid then this function will kill the calling client.
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 */
//--------------------------------------------------------------------------------------------------
void le_sim_DeleteFPLMNList
(
    le_sim_FPLMNListRef_t FPLMNListRef
        ///< [IN] FPLMN list reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Open a logical channel on the SIM card.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_OpenLogicalChannel
(
    uint8_t* channelPtrPtr
        ///< [OUT] The number of the opened logical channel.
);

//--------------------------------------------------------------------------------------------------
/**
 * Close a logical channel on the SIM card.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_CloseLogicalChannel
(
    uint8_t channel
        ///< [IN] The number of the logical channel to close.
);

//--------------------------------------------------------------------------------------------------
/**
 * Send APDU command on a dedicated logical channel.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      The function failed to select the SIM card for this operation.
 *      - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendApduOnChannel
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    uint8_t channel,
        ///< [IN] The logical channel number.
    const uint8_t* commandApduPtr,
        ///< [IN] APDU command.
    size_t commandApduSize,
        ///< [IN]
    uint8_t* responseApduPtr,
        ///< [OUT] SIM response.
    size_t* responseApduSizePtr
        ///< [INOUT]
);

#endif // LE_SIM_INTERFACE_H_INCLUDE_GUARD