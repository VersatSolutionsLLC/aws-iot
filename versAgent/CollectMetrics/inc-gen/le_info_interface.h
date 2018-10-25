

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_info Modem Information
 *
 * @ref le_info_interface.h "API Reference"
 *
 * <HR>
 *
 * This file contains prototype definitions for Modem Information APIs.
 *
 * @section le_info_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b modemService.
 *
 * Here's a code sample binding to modem services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_info -> modemService.le_info
   }
   @endverbatim
 *
 * @section le_info_version Query Firmware Version
 *
 * le_info_GetFirmwareVersion() is used to query the current firmware version.
 *  le_info_GetBootloaderVersion() is used to query the current bootloader version.
 *  In both cases, the version is returned as a human readable string.
 *
 * The linux kernel version can be queried using standard linux methods, such as the uname
 * command, or the uname() API.
 *
 * @section le_info_model Query Device information
 *
 * le_info_GetDeviceModel() is used to retrieve the target hardware platform identity.
 *
 * le_info_GetImei() is used to retrieve the International Mobile Equipment Identity (IMEI).
 *
 * le_info_GetImeiSv() is used to retrieve the International Mobile Equipment Identity software
 * version number(IMEISV).
 *
 * le_info_GetMeid() is used to retrieve the CDMA device Mobile Equipment Identifier (MEID).
 *
 * le_info_GetEsn() is used to retrieve Electronic Serial Number (ESN) of the device.
 *
 * le_info_GetMdn() is used to retrieve the Mobile Directory Number (MDN) of the device.
 *
 * le_info_GetMin() is used to retrieve the CDMA Mobile Identification Number (MIN).
 *
 * le_info_GetPrlVersion() is used to retrieve the CDMA version of Preferred Roaming List (PRL).
 *
 * le_info_GetPrlOnlyPreference() is used to retrieve the CDMA Preferred Roaming List (PRL)
 * only preferences status.
 *
 * le_info_GetNai() is used to retrieve the CDMA Network Access Identifier (NAI) string.
 *
 * le_info_GetManufacturerName() is used to retrieve the Manufacturer name.
 *
 * le_info_GetPriId() is used to retrieve the Product Requirement Information Identifier (PRI ID)
 * Part Number and the Revision Number.
 *
 * le_info_GetCarrierPri() is used to retrieve the Carrier Product Requirement Information (CAPRI)
 * Name and the Revision number. Revision Number string is in major.minor_buildver format.
 *
 * le_info_GetSku() is used to retrieve the product stock keeping unit number (SKU).
 *
 * le_info_GetPlatformSerialNumber() is used to retrieve the Platform Serial Number (PSN) string.
 *
 * le_info_GetRfDeviceStatus() is used to retrieve the RF devices working status (i.e. working or
 * broken) of modem's RF devices such as power amplifier, antenna switch and transceiver.
 * That status is updated every time the module power on.
 *
 * @section le_info_reset Query Reset Information
 *
 * le_info_GetResetInformation() is used to retrieve the last reset reason.
 * For each reset reason, an information string is provided to specify the reset cause. These
 * strings are listed in the table below. This list may be partially supported by some plateforms.
 *
 * @note See platform constraints @subpage platformConstraintsInfo
 *
 * | Reset type            | Information string             | Description                                   |
 * |:---------------------:|:------------------------------:|:---------------------------------------------:|
 * | LE_INFO_RESET_USER    | "Reset, User Requested"        | User requested the reboot                     |
 * | LE_INFO_RESET_HARD    | "Reset, Hardware Switch"       | Hardware switch pushed                        |
 * | LE_INFO_RESET_UPDATE  | "Reset, Swap"                  | System Swap (dual platforms)                  |
 * | LE_INFO_RESET_UPDATE  | "Reset, Swap_sync"             | System Swap and synchronize (dual platforms)  |
 * | LE_INFO_RESET_UPDATE  | "Reset, Software Update"       | Software Update                               |
 * | LE_INFO_RESET_UPDATE  | "Reset, Configuration Update"  | Reset caused by a configuration update        |
 * | LE_INFO_RESET_UPDATE  | "Reset, LWM2M Update"          | Reset caused by a LWM2M update                |
 * | LE_INFO_RESET_UPDATE  | "Reset, OMA-DM Update"         | Reset caused by a OMA-DM update               |
 * | LE_INFO_RESET_UPDATE  | "Reset, FOTA Update"           | Reset caused by a FOTA update                 |
 * | LE_INFO_POWER_DOWN    | "Reset, Crash"                 | Software crash                                |
 * | LE_INFO_POWER_DOWN    | "Power Down"                   | Power source unplugged                        |
 * | LE_INFO_TEMP_CRIT     | "Power Down, Critical Temp"    | Power down due to a critical voltage level    |
 * | LE_INFO_VOLT_CRIT     | "Power Down, Critical Voltage" | Power down due to a ciritcal temperature level|
 *
 *
 * le_info_GetExpectedResetsCount() returns the number of expected resets
 *
 * Expected resets can be due to normal system shutdown/restart or a software update
 *
 * le_info_GetUnexpectedResetsCount() return the number of unexpected resets
 *
 * Unexpected resets can be due to software crash, a critical voltage level or a critical
 * temperature level
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_info_interface.h
 *
 * Legato @ref c_info include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_INFO_INTERFACE_H_INCLUDE_GUARD
#define LE_INFO_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_info_DisconnectHandler_t)(void *);

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
void le_info_ConnectService
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
le_result_t le_info_TryConnectService
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
void le_info_SetServerDisconnectHandler
(
    le_info_DisconnectHandler_t disconnectHandler,
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
void le_info_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Maximum IMEI length (15 digits), excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEI_MAX_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum IMEI length (15 digits)
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEI_MAX_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the IMEISV string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEISV_MAX_LEN 255

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the IMEISV string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEISV_MAX_BYTES 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a version string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_VERS_LEN 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a version string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_VERS_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a model string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MODEL_LEN 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a model string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MODEL_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Electronic Serial Number (ESN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_ESN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Electronic Serial Number (ESN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_ESN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a reset specific information string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_RESET_LEN 50

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a reset specific information string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_RESET_BYTES 51

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Equipment Identifier (MEID) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MEID_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Equipment Identifier (MEID) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MEID_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Directory Number (MDN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MDN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Directory Number (MDN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MDN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (MSISDN) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MIN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (MSISDN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MIN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (NAI) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_NAI_LEN 72

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (NAI) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_NAI_BYTES 73

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Manufacturer Name string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MFR_NAME_LEN 128

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Manufacturer Name string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MFR_NAME_BYTES 129

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Part Number string
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_PN_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Part Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_PN_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Revision Number string
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_REV_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Revision Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_REV_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Name string.
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_NAME_LEN 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Name string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_NAME_BYTES 17

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Revision Number string.
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_REV_LEN 12

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Revision Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_REV_BYTES 13

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Platform Serial Number (PSN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PSN_LEN 14

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Platform Serial Number (PSN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PSN_BYTES 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum fields for the RF devices status request.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_RF_DEVICES_STATUS_MAX 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the product stock keeping unit number (SKU) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_SKU_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the product stock keeping unit number (SKU) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_SKU_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Reset type enum
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_INFO_RESET_UNKNOWN = 0,
        ///< Unknown
    LE_INFO_RESET_USER = 1,
        ///< User request
    LE_INFO_RESET_HARD = 2,
        ///< Hardware switch
    LE_INFO_RESET_UPDATE = 3,
        ///< Software update
    LE_INFO_RESET_CRASH = 4,
        ///< Software crash
    LE_INFO_POWER_DOWN = 5,
        ///< Power Down
    LE_INFO_VOLT_CRIT = 6,
        ///< Power Down due to a critical voltage level
    LE_INFO_TEMP_CRIT = 7
        ///< Power Down due to a critical temperature level
}
le_info_Reset_t;


//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the International Mobile Equipment Identity (IMEI).
 *
 * @return LE_FAULT       Function failed to retrieve the IMEI.
 * @return LE_OVERFLOW     IMEI length exceed the maximum length.
 * @return LE_OK          Function succeeded.
 *
 * @note If the caller passes a bad pointer into this function, it's a fatal error the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetImei
(
    char* imei,
        ///< [OUT] IMEI string.
    size_t imeiSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the International Mobile Equipment Identity software version number (IMEISV).
 *
 * @return LE_FAULT       Function failed to retrieve the IMEISV.
 * @return LE_OVERFLOW    IMEISV length exceed the maximum length.
 * @return LE_OK          Function succeeded.
 *
 * @note If the caller passes a bad pointer into this function, it's a fatal error; the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetImeiSv
(
    char* imeiSv,
        ///< [OUT] IMEISV string.
    size_t imeiSvSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the firmware version string
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the version string is not available
 *      - LE_OVERFLOW if version string to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetFirmwareVersion
(
    char* version,
        ///< [OUT] Firmware version string
    size_t versionSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the last reset information reason
 *
 * @return
 *      - LE_OK          on success
 *      - LE_UNSUPPORTED if it is not supported by the platform
 *        LE_OVERFLOW    specific reset information length exceeds the maximum length.
 *      - LE_FAULT       for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetResetInformation
(
    le_info_Reset_t* resetPtr,
        ///< [OUT] Reset information
    char* resetSpecificInfoStr,
        ///< [OUT] Reset specific information
    size_t resetSpecificInfoStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the bootloader version string
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the version string is not available
 *      - LE_OVERFLOW if version string to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetBootloaderVersion
(
    char* version,
        ///< [OUT] Bootloader version string
    size_t versionSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the device model identity (Target Hardware Platform).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The device model identity length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetDeviceModel
(
    char* modelPtr,
        ///< [OUT] The model identity string (null-terminated).
    size_t modelPtrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA device Mobile Equipment Identifier (MEID).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The device Mobile Equipment identifier length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetMeid
(
    char* meidStr,
        ///< [OUT] The Mobile Equipment identifier (MEID)
        ///<  string (null-terminated).
    size_t meidStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Electronic Serial Number (ESN) of the device.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Electronic Serial Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetEsn
(
    char* esnStr,
        ///< [OUT] The Electronic Serial Number (ESN) of the device.
        ///<  string (null-terminated).
    size_t esnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Mobile Directory Number (MDN) of the device.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Mobile Directory Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetMdn
(
    char* mdnStr,
        ///< [OUT] The Mobile Directory Number (MDN)
        ///<  string (null-terminated).
    size_t mdnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA version of Preferred Roaming List (PRL).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_NOT_FOUND     The information is not available.
 *      - LE_FAULT         The function failed to get the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetPrlVersion
(
    uint16_t* prlVersionPtr
        ///< [OUT] The Preferred Roaming List (PRL) version.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Preferred Roaming List (PRL) only preferences status.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_NOT_FOUND     The information is not available.
 *      - LE_FAULT         The function failed to get the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetPrlOnlyPreference
(
    bool* prlOnlyPreferencePtr
        ///< [OUT] The CDMA Preferred Roaming List only preferences status.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Mobile Identification Number (MIN).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The CDMA Mobile Identification Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetMin
(
    char* msisdnStr,
        ///< [OUT] The Mobile Identification Number (MIN)
        ///<  string (null-terminated).
    size_t msisdnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Network Access Identifier (NAI) string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Network Access Identifier (NAI) length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetNai
(
    char* naiStr,
        ///< [OUT] The Network Access Identifier (NAI)
        ///<  string (null-terminated).
    size_t naiStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Manufacturer Name string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Manufacturer Name length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetManufacturerName
(
    char* mfrNameStr,
        ///< [OUT] The Manufacturer Name string (null-terminated).
    size_t mfrNameStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Product Requirement Information Part Number and Revision Number strings in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Part or the Revision Number strings length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetPriId
(
    char* priIdPnStr,
        ///< [OUT] The Product Requirement Information Identifier
        ///< (PRI ID) Part Number string (null-terminated).
    size_t priIdPnStrSize,
        ///< [IN]
    char* priIdRevStr,
        ///< [OUT] The Product Requirement Information Identifier
        ///< (PRI ID) Revision Number string
        ///< (null-terminated).
    size_t priIdRevStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Carrier PRI Name and Revision Number strings in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Part or the Revision Number strings length exceed the maximum length.
 *      - LE_UNSUPPORTED   The function is not supported on the platform.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetCarrierPri
(
    char* capriNameStr,
        ///< [OUT] The Carrier Product Requirement Information
        ///< (CAPRI) Name string (null-terminated).
    size_t capriNameStrSize,
        ///< [IN]
    char* capriRevStr,
        ///< [OUT] The Carrier Product Requirement Information
        ///< (CAPRI) Revision Number string
        ///< (null-terminated).
    size_t capriRevStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform Serial Number (PSN) string.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if Platform Serial Number to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetPlatformSerialNumber
(
    char* platformSerialNumberStr,
        ///< [OUT] Platform Serial Number string.
    size_t platformSerialNumberStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the RF devices working status (i.e. working or broken) of modem's RF devices such as
 * power amplifier, antenna switch and transceiver. That status is updated every time the module
 * power on.
 *
 * @return
 *      - LE_OK on success
 *      - LE_UNSUPPORTED request not supported
 *      - LE_FAULT function failed to get the RF devices working status
 *      - LE_OVERFLOW the number of statuses exceeds the maximum size
 *        (LE_INFO_RF_DEVICES_STATUS_MAX)
 *      - LE_BAD_PARAMETER Null pointers provided
 *
 * @note If the caller is passing null pointers to this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetRfDeviceStatus
(
    uint16_t* manufacturedIdPtr,
        ///< [OUT] Manufactured identifier (MID)
    size_t* manufacturedIdSizePtr,
        ///< [INOUT]
    uint8_t* productIdPtr,
        ///< [OUT] Product identifier (PID)
    size_t* productIdSizePtr,
        ///< [INOUT]
    bool* statusPtr,
        ///< [OUT] Status of the RF device (MID,PID):
        ///< 0 means something wrong
        ///< 1 means no error found
    size_t* statusSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the product stock keeping unit number (SKU) string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The SKU number string length exceeds the maximum length.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetSku
(
    char* skuIdStr,
        ///< [OUT] Product SKU ID string.
    size_t skuIdStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of expected resets
 *
 * @return
 *      - LE_OK             Success
 *      - LE_BAD_PARAMETER  Input prameter is a null pointer
 *      - LE_FAULT          Failed to get the number if expected resets
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetExpectedResetsCount
(
    uint64_t* resetsCountPtrPtr
        ///< [OUT] Number of expected resets
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of unexpected resets
 *
 * @return
 *      - LE_OK             Success
 *      - LE_BAD_PARAMETER  Input prameter is a null pointer
 *      - LE_FAULT          Failed to get the number if expected resets
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_info_GetUnexpectedResetsCount
(
    uint64_t* resetsCountPtrPtr
        ///< [OUT] Number of expected resets
);

#endif // LE_INFO_INTERFACE_H_INCLUDE_GUARD