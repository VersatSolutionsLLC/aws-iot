

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_gnss GNSS
 *
 * @ref le_gnss_interface.h "API Reference" <br>
 * @ref platformConstraintsGnss <br>
 * @ref howToGNSS "How To Use GNSS" <br>
 * @ref toolsTarget_gnss "GNSS target tool" <br>
 *
 * <HR>
 *
 * This API provides access to the GNSS device.
 *
 * GNSS or Global Navigation Satellite System is a satellite navigation system with global coverage.
 *
 * This API provides function to configure the GNSS device and retrieve position information.
 *
 * @section le_gnss_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b positioningService application service.
 *
 * Here's a code sample binding to Positioning services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_gnss -> positioningService.le_gnss
   }
   @endverbatim
 *
 * @section le_gnss_ControlApi GNSS Control API
 *
 * @subsection le_gnss_EnableDisable Enable/Disable GNSS device
 *
 * The application can enable/disable the GNSS device with the le_gnss_Enable()
 * / le_gnss_Disable() functions.
 * By default the GNSS device is enabled for the positioningService application service.
 * Also see @ref howToGNSS.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeEnableDisable
 *
 * @subsection le_gnss_StartStop Start/Stop GNSS device
 *
 * The application can start/stop the GNSS device with the le_gnss_Start() / le_gnss_Stop()
 * functions.
 * The default "HOT" start condition is applied and all assistance data are used.
 * Also see @ref howToGNSS.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeStartStop
 *
 * @subsubsection le_gnss_StartingMode Starting mode
 *
 * Starting modes are used only for test purposes and allow start performance measurement.
 * @note For more information about start performances, please refer to your specific
 * platform documentation.
 *
 * @subsubsection SettingConfiguration Setting configuration
 *
 * @warning The GNSS setting configuration depends on the platform. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 *
 * @subsubsection le_gnss_Data GNSS data
 * The following table describes the <b>minimum</b> required data for those starting modes:
 *
 * | GNSS Data / Starting mode  | HOT  | WARM  | COLD  | FACTORY |
 *| -----------------------| -----| ------| ----- | ------- |
 *| Broadcasted Ephemeris  | Used |  |  |  |
 *| Extended Ephemeris     | Used(1) | Used(2) | Used(2) | Removed (3) |
 *| Approximate Time and Position  | Used | Used |  |  |
 *| Almanac   | Used | Used | Used | Used (Factory) |
 *
 * For example, a requested HOT start without valid broadcasted ephemeris will be treated
 * as a WARM start.
 *
 * @note (1) Extended Ephemeris can be used if Broadcasted Ephemeris are not valid.
 * The Extended Ephemeris could be loaded using the le_gnss_LoadExtendedEphemerisFile()
 * function.
 * @note (2) Extended Ephemeris is used if the Extended Ephemeris file is loaded and valid.
 * @note (3) Extended Ephemeris are removed when a FACTORY start is requested.
 * The Extended Ephemeris could be loaded again using the le_gnss_LoadExtendedEphemerisFile()
 * function.
 *
 * @subsubsection le_gnss_TTFF Time To First Fix (TTFF)
 * The le_gnss_GetTtff() function provides the TTFF (first between 2-Dimensional or 3-Dimensional
 * position fix) of the last position fix.
 * Please refer to @ref le_gnss_APIcallsRequirements. Also see @ref howToGNSS.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeReStart
 *
 * @subsubsection le_gnss_InjectUtcTime Inject UTC time
 * The le_gnss_InjectUtcTime() function injects the UTC time into the location engine. Providing an
 * accurate UTC time reduces the time to find the first fix.
 * @note It is mandatory to enable the 'Extended Ephemeris' file injection into the GNSS device with
 * le_gnss_EnableExtendedEphemerisFile() before injecting time with le_gnss_InjectUtcTime API.
 * @warning Your GNSS device may require a restart to take into account the enabling of the
 * 'Extended Ephemeris' file injection.
 * Please refer to your platform documentation @ref platformConstraintsGnss_SettingConfiguration for
 * further details.
 *
 * @subsubsection le_gnss_ForceHotRestart Force HOT restart
 * The le_gnss_ForceHotRestart() function performs a "HOT" restart of the GNSS device. The current
 * GNSS session is stopped, then started using the available @ref le_gnss_Data.
 * Please refer to @ref le_gnss_APIcallsRequirements. Also see @ref howToGNSS.
 *
 * @subsubsection le_gnss_ForceWarmRestart Force WARM restart
 * The le_gnss_ForceWarmRestart() function performs a "WARM" restart of the GNSS device. The current
 * GNSS session is stopped, then started using the available @ref le_gnss_Data.
 * Please refer to @ref le_gnss_APIcallsRequirements. Also see @ref howToGNSS.
 * @Note This API has a platform dependent feature. Please refer to
 * @ref platformConstraintsGnss_WarmRestart for further details.
 *
 * @subsubsection le_gnss_ForceColdRestart Force COLD restart
 * The le_gnss_ForceColdRestart() function performs a "COLD" restart of the GNSS device. The current
 * GNSS session is stopped, then started using the available @ref le_gnss_Data.
 * Please refer to @ref le_gnss_APIcallsRequirements. Also see @ref howToGNSS.
 *
 * @subsubsection le_gnss_ForceFactoryRestart Force FACTORY restart
 * The le_gnss_ForceFactoryRestart() function performs a "FACTORY" restart of the GNSS device.
 * The current GNSS session is stopped, then started using the available @ref le_gnss_Data.
 * Please refer to @ref le_gnss_APIcallsRequirements. Also see @ref howToGNSS.
 *
 * @subsection le_gnss_AcquisitionRate Acquisition rate
 * The GNSS position is computed and delivered each acquisition rate. The acquisition rate defines
 * the time interval that must elapse between two final GPS positions calculation and reports.
 * Its default value is 1 second.
 * The application can configure/retreive the GNSS device acquisition rate with the
 * le_gnss_SetAcquisitionRate() / le_gnss_GetAcquisitionRate() functions.
 *
 * Please refer to @ref le_gnss_APIcallsRequirements.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeAcquisitionRate
 *
 * @subsection le_gnss_SetGetConstellation GNSS constellation selection
 *
 * The le_gnss_SetConstellation() function selects the GNSS constellation(s) used in solution.
 * If all GNSS constellations are disabled, the GNSS engine is disabled.
 *
 * @warning Your GNSS device may require a restart to take into account this change.
 * Please refer to your platform documentation @ref platformConstraintsGnss_SettingConfiguration for
 * further details.
 *
 * Combinations of constellation for GPS, GLONASS, BDS, GALILEO and QZSS satellites are currently
 * supported. Constellation for SBAS satellites is not supported.
 *
 * All supported GNSS constellations are enabled by default.
 * The le_gnss_GetConstellation() function gets the GNSS constellation(s) enabled to be used
 * in solution.
 *
 * le_gnss_SetConstellationArea sets the area for a GNSS constellation. le_gnss_GetConstellationArea
 * gets the area for a GNSS constellation.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeConstellation
 *
 * @subsection le_gnss_SetGetMinElevation GNSS minimum elevation selection
 *
 * The le_gnss_SetMinElevation() function sets the GNSS minimum elevation.
 * Satellites with elevation lower than the minimum elevation will be ignored.
 *
 * The le_gnss_GetMinElevation() function gets the GNSS minimum elevation.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeMinElevation
 *
 * @subsection le_gnss_NmeaSentences NMEA sentences selection
 *
 * The le_gnss_SetNmeaSentences() function selects the enabled NMEA sentences in the
 * @ref le_gnss_NMEA with a bit mask. The supported values are listed in @ref le_gnss_NmeaBitMask_t.
 *
 * @note This function may be subject to limitations depending on the platform. Please refer to the
 * @ref platformConstraintsGnss page.
 *
 * @warning Your GNSS device may require a restart to take into account this change.
 * Please refer to your platform documentation @ref platformConstraintsGnss_SettingConfiguration for
 * further details.
 *
 * @warning Some NMEA sentences are unsupported depending on the plateform. Please refer to
 * @ref platformConstraintsGnss_nmeaMask section for full details.
 *
 * @note All supported NMEA sentences are enabled by default.
 *
 * The le_gnss_GetNmeaSentences() function gets the bit mask of the enabled NMEA sentences in the
 * @ref le_gnss_NMEA.
 *
 * Please refer to @ref le_gnss_APIcallsRequirements.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeNmeaSentences
 *
 * @subsection le_gnss_APIcallsRequirements API calls Requirements
 * The following table shows the pre-requisites when using the GNSS service API function set.
 * ''LE_OK or error code'' means the function is authorized in the corresponding state, the request
 * is performed and the result is returned; otherwise the returned error is indicated for each state
 *.
 * | Function / GNSS state  | UNINITIALIZED  | READY  | ACTIVE  | DISABLED |
 *| ----------------------------- | --------------------------- | -----------------------------| --------------------------- | --------------------------- |
 *| le_gnss_Start() | LE_NOT_PERMITTED | LE_OK or error code | LE_DUPLICATE | LE_NOT_PERMITTED |
 *| le_gnss_Stop() | LE_NOT_PERMITTED | LE_DUPLICATE | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_ForceHotRestart() | LE_NOT_PERMITTED | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_ForceWarmRestart() | LE_NOT_PERMITTED | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_ForceColdRestart() | LE_NOT_PERMITTED | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_ForceFactoryRestart() | LE_NOT_PERMITTED | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_Disable() | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED | LE_DUPLICATE |
 *| le_gnss_Enable() | LE_NOT_PERMITTED | LE_DUPLICATE | LE_DUPLICATE | LE_OK or error code |
 *| le_gnss_SetConstellation() | LE_NOT_PERMITTED |  LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_GetConstellation() | LE_NOT_PERMITTED |  LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_SetConstellationArea() | LE_NOT_PERMITTED |  LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_GetConstellationArea() | LE_NOT_PERMITTED |  LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_GetTtff() | LE_NOT_PERMITTED |  LE_OK or error code | LE_OK or error code | LE_NOT_PERMITTED |
 *| le_gnss_SetAcquisitionRate() | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_GetAcquisitionRate() | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_SetNmeaSentences() | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *| le_gnss_GetNmeaSentences() | LE_NOT_PERMITTED | LE_OK or error code | LE_NOT_PERMITTED | LE_NOT_PERMITTED |
 *
 * Also see @ref howToGNSS.
 *
 * @section le_gnss_Information GNSS position information
 * @ref le_gnss_NMEA
 *
 * @ref le_gnss_GetInfo
 *
 * @subsection le_gnss_NMEA NMEA Flow
 * The National Marine Electronics Association (NMEA) standard defines an electrical interface
 * and data protocol for communications between marine instrumentation.
 *
 * The NMEA-0183, scope of this document, defines a set of frame prefixed by
 * $GP (concerning Global Positioning System),
 * $GL (concerning GLONASS)
 * and $GN (concerning combination of navigation systems).
 * For more details about NMEA standards, please refer to http://www.nmea.org/.
 *
 * That NMEA frames flow can be retrieved from the "/dev/nmea" device folder, using for example
 * the shell command $<EM> cat /dev/nmea | grep '$G'</EM>
 *
 * @subsection le_gnss_GetInfo Get position information
 * The position information is referenced to a position sample object.
 *
 * An application can register a handler to be notified of the
 * updated position each @ref le_gnss_AcquisitionRate, returning a position sample object.
 *
 * The GNSS information commonly used such as
 * device state, position, time and date, satellites information and accuracy
 * can be queried using the following functions:
 * - le_gnss_GetState()
 * - le_gnss_GetPositionState()
 * - le_gnss_GetLocation()
 * - le_gnss_GetAltitude()
 * - le_gnss_GetDate()
 * - le_gnss_GetTime()
 * - le_gnss_GetGpsTime()
 * - le_gnss_GetGpsLeapSeconds()
 * - le_gnss_GetEpochTime()
 * - le_gnss_GetTimeAccuracy()
 * - le_gnss_GetHorizontalSpeed()
 * - le_gnss_GetVerticalSpeed()
 * - le_gnss_GetDirection()
 * - le_gnss_GetSatellitesInfo()
 * - le_gnss_GetSbasConstellationCategory()
 * - le_gnss_GetSatellitesStatus()
 * - le_gnss_GetDop()
 * - le_gnss_GetDilutionOfPrecision()
 * - le_gnss_GetAltitudeOnWgs84()
 * - le_gnss_GetMagneticDeviation()
 *
 * le_gnss_GetDilutionOfPrecision() gets the DOP parameters (Dilution Of Precision) with a
 * resolution of 3 decimal places by default. This resolution can be modified by calling the
 * @c le_gnss_SetDopResolution() function first.
 *
 * As @c le_gnss_SetDopResolution() sets the resolution per client session,
 * @c le_gnss_SetDopResolution() and @c le_gnss_GetDilutionOfPrecision() functions should be called
 * in the same thread or client session.
 *
 * The handler can be managed using le_gnss_AddPositionHandler()
 * and le_gnss_RemovePositionHandler().
 * When a position is computed, the handler is called.
 *
 * The application has to release each position sample object received by the handler,
 * using the le_gnss_ReleaseSampleRef().
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodePosition
 *
 * @section le_gnss_Assisted_GNSS Assisted GNSS
 *
 * @ref le_gnss_Assisted_GNSS_EE
 * @ref le_gnss_Assisted_GNSS_UP
 *
 * @subsection le_gnss_Assisted_GNSS_EE Server based Extended Ephemeris
 *
 * @todo Add detailed documentation.
 *
 * With le_gnss_LoadExtendedEphemerisFile() , you can load an 'Extended Ephemeris' file into
 * the GNSS device from the filesystem.
 * You have to download the file before loading it.
 * @warning Ensure to check that the downloaded file is supported for your specific platform.
 *
 * With le_gnss_GetExtendedEphemerisValidity(), you will to get the validity of the last injected
 * Extended Ephemeris.
 *
 * You can enable/disable the use of the 'Extended Ephemeris' file into the GNSS device with
 * le_gnss_EnableExtendedEphemerisFile() / le_gnss_DisableExtendedEphemerisFile() functions.
 * @warning Ensure to check configuration capabilities for your specific platform.
 * A reboot must be required if your platform doesn't allow run-time configuration.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_gnssSampleCodeXtra
 *
 * @subsection le_gnss_Assisted_GNSS_UP 3GPP User Plane (OMA SUPL)
 *
 * That 3GPP User Plane A-GNSS (Assisted GNSS) protocol is defined by two different standardization
 * bodies, 3GPP and Open Mobile Alliance (OMA). For more information, please refer to the standard.
 *
 * Both MS-Assisted and MS-Based position determination methods are supported in the User Plane.
 *
 * In MS-Assisted mode, the MS (Mobile Station) measures the signals from the GNSS satellites
 *, then returns the retrieved GNSS data to the SUPL (Secure User Plan Location) server, where the
 * position calculation is performed.
 *
 * In MS-Based mode, the MS gets the assistance data from the SUPL (Secure User Plan Location)
 * server. The MS measures the signals from the GNSS satellites and makes the position calculation.
 *
 * The data transport over User Plan is done using the TCP/IP protocol.
 *
 * The Assisted-GNSS mode can be configured thru the le_gnss_SetSuplAssistedMode() function.
 *
 * The supported modes are the following:
 * - Standalone mode: That 3GPP User Plane A-GNSS feature is deactivated.
 * - MS-Based mode
 * - MS-Assisted mode
 *
 * Moreover, the le_gnss_GetSuplAssistedMode() function reads the configured Assisted-GNSS mode.
 *
 * The SUPL server is configured using the le_gnss_SetSuplServerUrl() function. That function sets
 * the SUPL server URL and optionally the port number.
 *
 * The SUPL certificate to be used in A-GNSS sessions is injected through the
 * le_gnss_InjectSuplCertificate() function and deleted through the le_gnss_DeleteSuplCertificate()
 * function.
 * The SUPL certificate lenght given as parameter to le_gnss_InjectSuplCertificate() must be less
 * than LE_GNSS_SUPL_CERTIFICATE_MAX_LEN.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @page c_gnssSampleCode Sample code for GNSS device
 *
 * @include "apps/test/positioning/gnssTest/gnssTest/gnssTest.c"
 */
/**
 * @page c_gnssSampleCodeEnableDisable Sample code for Enable/Disable GNSS device
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssEnable
 */
/**
 * @page c_gnssSampleCodeStartStop Sample code for Start/Stop GNSS device
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssControl
 */
/**
 * @page c_gnssSampleCodeReStart Sample code for restart GNSS device and get TTFF
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssReStart
 */
/**
 * @page c_gnssSampleCodeAcquisitionRate Sample code for GNSS acquisition rate configuration
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssControl
 */
/**
 * @page c_gnssSampleCodeConstellation Sample code for GNSS constellation selection
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssControl
 */
/**
 * @page c_gnssSampleCodeMinElevation Sample code for GNSS minimum elevation
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssControl
 */
/**
 * @page c_gnssSampleCodeNmeaSentences Sample code for GNSS NMEA sentences selection
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssControl
 */
/**
 * @page c_gnssSampleCodePosition Sample code for GNSS position information
 *
 * @snippet "apps/test/positioning/gnssTest/gnssTest/gnssTest.c" GnssPosition
 */
/**
 * @page c_gnssSampleCodeXtra Sample code for GNSS Server based Extended Ephemeris
 *
 * @include "apps/test/positioning/gnssXtraTest/gnssXtraTest/xtraTest.c"
 */
/**
 * @file le_gnss_interface.h
 *
 * Legato @ref c_gnss include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_GNSS_INTERFACE_H_INCLUDE_GUARD
#define LE_GNSS_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_gnss_DisconnectHandler_t)(void *);

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
void le_gnss_ConnectService
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
le_result_t le_gnss_TryConnectService
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
void le_gnss_SetServerDisconnectHandler
(
    le_gnss_DisconnectHandler_t disconnectHandler,
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
void le_gnss_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the SUP Server URL string.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_SUPL_SERVER_URL_MAX_LEN 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the SUP Server URL string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_SUPL_SERVER_URL_MAX_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum SUPL certificate size.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_SUPL_CERTIFICATE_MAX_LEN 2000

//--------------------------------------------------------------------------------------------------
/**
 * Maximum SUPL certificate string size.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_SUPL_CERTIFICATE_MAX_BYTES 2001

//--------------------------------------------------------------------------------------------------
/**
 * Define the maximum length of the Satellites Vehicle information list
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_SV_INFO_MAX_LEN 80

//--------------------------------------------------------------------------------------------------
/**
 * Define the maximal bit mask for enabled NMEA sentences
 *
 * @note This maximal value should be coherent with @ref le_gnss_NmeaBitMask_t
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_NMEA_SENTENCES_MAX 4194303

//--------------------------------------------------------------------------------------------------
/**
 * @deprecated LE_GNSS_NMEA_MASK_PQXFI is deprecated. LE_GNSS_NMEA_MASK_PTYPE is to be used instead.
 * Setting LE_GNSS_NMEA_MASK_PTYPE will also set LE_GNSS_NMEA_MASK_PQXFI.
 * LE_GNSS_NMEA_MASK_PTYPE supports all Proprietary type masks: PQXFI, PQGSA and PQGSV.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_NMEA_MASK_PQXFI 16384

//--------------------------------------------------------------------------------------------------
/**
 * Define the maximal degree for the minimal elevation
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_MIN_ELEVATION_MAX_DEGREE 90

//--------------------------------------------------------------------------------------------------
/**
 * Satellite Vehicle (SV) ID to PRN offset definitions
 *
 * @note SV ID is given both in le_gnss_GetSatellitesInfo() API and NMEA flow. Its corresponding
 *       PRN code can be computed adding the following offset according to its constellation.
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_GPS 0

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_SBAS 87

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_GLONASS -64

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_GALILEO -300

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_BEIDOU -200

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_GNSS_PRN_OFFSET_QZSS 0

//--------------------------------------------------------------------------------------------------
/**
 *  Reference type for dealing with GNSS position samples.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_gnss_Sample* le_gnss_SampleRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration for GNSS device state
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_STATE_UNINITIALIZED = 0,
        ///< The GNSS device is not initialized
    LE_GNSS_STATE_READY = 1,
        ///< The GNSS device is ready
    LE_GNSS_STATE_ACTIVE = 2,
        ///< The GNSS device is active
    LE_GNSS_STATE_DISABLED = 3,
        ///< The GNSS device is disabled
    LE_GNSS_STATE_MAX = 4
        ///< Do not use
}
le_gnss_State_t;


//--------------------------------------------------------------------------------------------------
/**
 *  GNSS fix position states.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_STATE_FIX_NO_POS = 0,
        ///< The GNSS fix position is not fixed.
    LE_GNSS_STATE_FIX_2D = 1,
        ///< 2-Dimensional position fix.
    LE_GNSS_STATE_FIX_3D = 2,
        ///< 3-Dimensional position fix.
    LE_GNSS_STATE_FIX_ESTIMATED = 3
        ///< Estimated (i.e. forward predicted) position fix.
}
le_gnss_FixState_t;


//--------------------------------------------------------------------------------------------------
/**
 *  Resolutions values.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_RES_ZERO_DECIMAL = 0,
        ///< Resolution of 0 decimal place.
    LE_GNSS_RES_ONE_DECIMAL = 1,
        ///< Resolution of 1 decimal place.
    LE_GNSS_RES_TWO_DECIMAL = 2,
        ///< Resolution of 2 decimal places.
    LE_GNSS_RES_THREE_DECIMAL = 3,
        ///< Resolution of 3 decimal places (default).
    LE_GNSS_RES_UNKNOWN = 4
        ///< Unknown resolution.
}
le_gnss_Resolution_t;


//--------------------------------------------------------------------------------------------------
/**
 *  SBAS constellation category
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_SBAS_EGNOS = 0,
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) EGNOS constellation.
    LE_GNSS_SBAS_WAAS = 1,
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) WAAS constellation.
    LE_GNSS_SBAS_GAGAN = 2,
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) GAGAN constellation.
    LE_GNSS_SBAS_MSAS = 3,
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) MSAS constellation.
    LE_GNSS_SBAS_SDCM = 4,
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) SDCM constellation.
    LE_GNSS_SBAS_UNKNOWN = 5
        ///< Satellite Vehicle of Satellite-based augmentation system
        ///< (SBAS) unknown constellation.
}
le_gnss_SbasConstellationCategory_t;


//--------------------------------------------------------------------------------------------------
/**
 *  GNSS constellation type.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_SV_CONSTELLATION_UNDEFINED = 0,
        ///< GNSS constellation field not defined.
    LE_GNSS_SV_CONSTELLATION_GPS = 1,
        ///< Satellite Vehicle of GPS constellation.
        ///< North American satellite navigation system
    LE_GNSS_SV_CONSTELLATION_SBAS = 2,
        ///< Satellite Vehicle of SBAS constellation.
        ///< Satellite-based augmentation system
    LE_GNSS_SV_CONSTELLATION_GLONASS = 3,
        ///< Satellite Vehicle of Glonass constellation.
        ///< Russian satellite navigation system
    LE_GNSS_SV_CONSTELLATION_GALILEO = 4,
        ///< Satellite Vehicle of Galileo constellation.
        ///< European Union satellite navigation system
    LE_GNSS_SV_CONSTELLATION_BEIDOU = 5,
        ///< Satellite Vehicle of Beidou constellation.
        ///< Chinese satellite navigation system
    LE_GNSS_SV_CONSTELLATION_QZSS = 6,
        ///< Satellite Vehicle of QZSS constellation.
        ///< Japanese satellite navigation system
    LE_GNSS_SV_CONSTELLATION_MAX = 7
        ///< Maximum value.
}
le_gnss_Constellation_t;


//--------------------------------------------------------------------------------------------------
/**
 * Assisted-GNSS mode
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_STANDALONE_MODE = 0,
        ///< Standalone mode.
    LE_GNSS_MS_BASED_MODE = 1,
        ///< MS-Based mode.
    LE_GNSS_MS_ASSISTED_MODE = 2
        ///< MS-Assisted mode.
}
le_gnss_AssistedMode_t;


//--------------------------------------------------------------------------------------------------
/**
 * Dilution of precision type
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_PDOP = 0,
        ///< Position dilution of precision.
    LE_GNSS_HDOP = 1,
        ///< Horizontal dilution of precision.
    LE_GNSS_VDOP = 2,
        ///< Vertical dilution of precision.
    LE_GNSS_GDOP = 3,
        ///< Geometric dilution of precision.
    LE_GNSS_TDOP = 4,
        ///< Time dilution of precision.
    LE_GNSS_DOP_LAST = 5
        ///<
}
le_gnss_DopType_t;


//--------------------------------------------------------------------------------------------------
/**
 * GNSS constellation area type
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_UNSET_AREA = 0,
        ///< Unset area.
    LE_GNSS_WORLDWIDE_AREA = 1,
        ///< Worldwide area.
    LE_GNSS_OUTSIDE_US_AREA = 2
        ///< Outside US area.
}
le_gnss_ConstellationArea_t;


//--------------------------------------------------------------------------------------------------
/**
 * GNSS constellation Bit Mask (8 bits) indicating the GNSS constellation(s)
 * used in solution.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_CONSTELLATION_GPS = 0x1,        ///< GPS constellation used in solution.
        ///< North American satellite navigation system
    LE_GNSS_CONSTELLATION_GLONASS = 0x2,        ///< GLONASS constellation used in solution.
        ///< Russian satellite navigation system
    LE_GNSS_CONSTELLATION_BEIDOU = 0x4,        ///< BEIDOU constellation used in solution.
        ///< Chinese satellite navigation system
    LE_GNSS_CONSTELLATION_GALILEO = 0x8,        ///< GALILEO constellation used in solution.
        ///< European Union satellite navigation system
    LE_GNSS_CONSTELLATION_SBAS = 0x10,        ///< SBAS constellation used in solution.
    LE_GNSS_CONSTELLATION_QZSS = 0x20        ///< QZSS constellation used in solution.
        ///< Japanese satellite navigation system
}
le_gnss_ConstellationBitMask_t;


//--------------------------------------------------------------------------------------------------
/**
 * NMEA sentences Bit Mask indicating the NMEA sentences enabled in the NMEA flow.
 *
 * @warning The supported NMEA sentences depend on the platform. Please refer to your platform
 * documentation for further details.
 *
 * @note The bit mask values should be coherent with @ref LE_GNSS_NMEA_SENTENCES_MAX
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_GNSS_NMEA_MASK_GPGGA = 0x1,        ///< GPGGA type enabled: GPS fix data.
    LE_GNSS_NMEA_MASK_GPGSA = 0x2,        ///< GPGSA type enabled: GPS DOP and active satellites.
    LE_GNSS_NMEA_MASK_GPGSV = 0x4,        ///< GPGSV type enabled: GPS satellites in view.
    LE_GNSS_NMEA_MASK_GPRMC = 0x8,        ///< GPRMC type enabled: GPS recommended minimum data.
    LE_GNSS_NMEA_MASK_GPVTG = 0x10,        ///< GPVTG type enabled: GPS vector track and speed over the ground.
    LE_GNSS_NMEA_MASK_GLGSV = 0x20,        ///< GLGSV type enabled: GLONASS satellites in view.
    LE_GNSS_NMEA_MASK_GNGNS = 0x40,        ///< GNGNS type enabled: GNSS fix data.
    LE_GNSS_NMEA_MASK_GNGSA = 0x80,        ///< GNGSA type enabled: GNSS DOP and active satellites.
    LE_GNSS_NMEA_MASK_GAGGA = 0x100,        ///< GAGGA type enabled: Galileo fix data.
    LE_GNSS_NMEA_MASK_GAGSA = 0x200,        ///< GAGSA type enabled: Galileo DOP and active satellites.
    LE_GNSS_NMEA_MASK_GAGSV = 0x400,        ///< GAGSV type enabled: Galileo satellites in view.
    LE_GNSS_NMEA_MASK_GARMC = 0x800,        ///< GARMC type enabled: Galileo recommended minimum data.
    LE_GNSS_NMEA_MASK_GAVTG = 0x1000,        ///< GAVTG type enabled: Galileo vector track and speed over the ground.
    LE_GNSS_NMEA_MASK_PSTIS = 0x2000,        ///< PSTIS type enabled: GPS session start indication.
    LE_GNSS_NMEA_MASK_REMOVED = 0x4000,        ///<
    LE_GNSS_NMEA_MASK_PTYPE = 0x8000,        ///< PTYPE type enabled: Proprietary Type mask.
    LE_GNSS_NMEA_MASK_GPGRS = 0x10000,        ///< GPGRS type enabled: GPS Range residuals.
    LE_GNSS_NMEA_MASK_GPGLL = 0x20000,        ///< GPGLL type enabled: GPS Geographic position, latitude / longitude.
    LE_GNSS_NMEA_MASK_DEBUG = 0x40000,        ///< DEBUG type enabled: Debug NMEA indication.
    LE_GNSS_NMEA_MASK_GPDTM = 0x80000,        ///< GPDTM type enabled: Local geodetic datum and datum offset from
    LE_GNSS_NMEA_MASK_GAGNS = 0x100000        ///< GAGNS type enabled: Fix data for Galileo.
}
le_gnss_NmeaBitMask_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_gnss_Position'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_gnss_PositionHandler* le_gnss_PositionHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for position information.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_gnss_PositionHandlerFunc_t)
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< Position's sample reference
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the GNSS constellation bit mask
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_UNSUPPORTED   If the request is not supported.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetConstellation
(
    le_gnss_ConstellationBitMask_t constellationMask
        ///< [IN] GNSS constellation used in solution.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the GNSS constellation bit mask
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetConstellation
(
    le_gnss_ConstellationBitMask_t* constellationMaskPtr
        ///< [OUT] GNSS constellation used in solution.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the area for the GNSS constellation
 *
 * @return
 *  - LE_OK            The function succeeded.
 *  - LE_FAULT         The function failed.
 *  - LE_UNSUPPORTED   If the request is not supported.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 *  - LE_BAD_PARAMETER Invalid constellation area.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetConstellationArea
(
    le_gnss_Constellation_t satConstellation,
        ///< [IN] GNSS constellation type.
    le_gnss_ConstellationArea_t constellationArea
        ///< [IN] GNSS constellation area.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the area for the GNSS constellation
 *
 * @return
 *  - LE_OK            On success
 *  - LE_FAULT         On failure
 *  - LE_UNSUPPORTED   Request not supported
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetConstellationArea
(
    le_gnss_Constellation_t satConstellation,
        ///< [IN] GNSS constellation type.
    le_gnss_ConstellationArea_t* constellationAreaPtr
        ///< [OUT] GNSS constellation area.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function enables the use of the 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_EnableExtendedEphemerisFile
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function disables the use of the 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_DisableExtendedEphemerisFile
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to load an 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed to inject the 'Extended Ephemeris' file.
 *  - LE_TIMEOUT       A time-out occurred.
 *  - LE_FORMAT_ERROR  'Extended Ephemeris' file format error.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_LoadExtendedEphemerisFile
(
    int fd
        ///< [IN] Extended ephemeris file descriptor
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the validity of the last injected Extended Ephemeris.
 *
 * @return
 *  - LE_FAULT         The function failed to get the validity
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetExtendedEphemerisValidity
(
    uint64_t* startTimePtr,
        ///< [OUT] Start time in seconds (since Jan. 1, 1970)
    uint64_t* stopTimePtr
        ///< [OUT] Stop time in seconds (since Jan. 1, 1970)
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to inject the UTC time into the GNSS device.
 *
 * @return
 *  - LE_OK            The function succeeded.
 *  - LE_FAULT         The function failed to inject the UTC time.
 *  - LE_TIMEOUT       A time-out occurred.
 *
 * @note It is mandatory to enable the 'Extended Ephemeris' file injection into the GNSS device with
 * le_gnss_EnableExtendedEphemerisFile() before injecting time with this API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_InjectUtcTime
(
    uint64_t timeUtc,
        ///< [IN] [IN] UTC time since Jan. 1, 1970 in milliseconds
    uint32_t timeUnc
        ///< [IN] [IN] Time uncertainty in milliseconds
);

//--------------------------------------------------------------------------------------------------
/**
 * This function starts the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already started.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or disabled.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Start
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function stops the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already stopped.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or disabled.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Stop
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "HOT" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceHotRestart
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "WARM" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 * @Note This API has a platform dependent feature. Please refer to
 * @ref platformConstraintsGnss_WarmRestart for further details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceWarmRestart
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "COLD" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceColdRestart
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "FACTORY" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceFactoryRestart
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the TTFF in milliseconds
 *
 * @return
 *  - LE_BUSY          The position is not fixed and TTFF can't be measured.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            Function succeeded.
 *  - LE_FAULT         If there are some other errors.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTtff
(
    uint32_t* ttffPtr
        ///< [OUT] TTFF in milliseconds
);

//--------------------------------------------------------------------------------------------------
/**
 * This function enables the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already enabled.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Enable
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function disables the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already disabled.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or started.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Disable
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the GNSS device acquisition rate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *  - LE_TIMEOUT a time-out occurred
 *  - LE_NOT_PERMITTED If the GNSS device is not in "ready" state.
 *  - LE_OUT_OF_RANGE  if acquisition rate value is equal to zero
 *
 * @warning This function may be subject to limitations depending on the platform. Please refer to
 *          the @ref platformConstraintsGnss page.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetAcquisitionRate
(
    uint32_t rate
        ///< [IN] Acquisition rate in milliseconds.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the GNSS device acquisition rate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_NOT_PERMITTED If the GNSS device is not in "ready" state.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAcquisitionRate
(
    uint32_t* ratePtr
        ///< [OUT] Acquisition rate in milliseconds.
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_gnss_Position'
 *
 * This event provides information on position.
 *
 *  - A handler reference, which is only needed for later removal of the handler.
 *
 * @note Doesn't return on failure, so there's no need to check the return value for errors.
 */
//--------------------------------------------------------------------------------------------------
le_gnss_PositionHandlerRef_t le_gnss_AddPositionHandler
(
    le_gnss_PositionHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_gnss_Position'
 */
//--------------------------------------------------------------------------------------------------
void le_gnss_RemovePositionHandler
(
    le_gnss_PositionHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the position sample's fix state
 *
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetPositionState
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    le_gnss_FixState_t* statePtr
        ///< [OUT] Position fix state.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the location's data (Latitude, Longitude, Horizontal accuracy).
 *
 * @return
 *  - LE_FAULT         Function failed to get the location's data
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note latitudePtr, longitudePtr and hAccuracyPtr can be set to NULL if not needed.
 *
 * @note The latitude and longitude values are based on the WGS84 standard coordinate system.
 *
 * @note The latitude and longitude values are given in degrees with 6 decimal places like:
 *       Latitude +48858300 = 48.858300 degrees North
 *       Longitude +2294400 = 2.294400 degrees East
 *       (The latitude and longitude values are given in degrees, minutes, seconds in NMEA frame)
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetLocation
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* latitudePtr,
        ///< [OUT] WGS84 Latitude in degrees, positive North [resolution 1e-6].
    int32_t* longitudePtr,
        ///< [OUT] WGS84 Longitude in degrees, positive East [resolution 1e-6].
    int32_t* hAccuracyPtr
        ///< [OUT] Horizontal position's accuracy in meters [resolution 1e-2].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's altitude.
 *
 * @return
 *  - LE_FAULT         Function failed to get the altitude. Invalid Position reference provided.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note Altitude is in meters, above Mean Sea Level, with 3 decimal places (3047 = 3.047 meters).
 *
 * @note For a 2D position fix, the altitude will be indicated as invalid and set to INT32_MAX
 *
 * @note Vertical position accuracy is in meters with 1 decimal places (3047 = 3.0 meters).
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note altitudePtr, altitudeAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAltitude
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* altitudePtr,
        ///< [OUT] Altitude in meters, above Mean Sea Level [resolution 1e-3].
    int32_t* vAccuracyPtr
        ///< [OUT] Vertical position's accuracy in meters [resolution 1e-1].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's time.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's GPS time.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetGpsTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* gpsWeekPtr,
        ///< [OUT] GPS week number from midnight, Jan. 6, 1980.
    uint32_t* gpsTimeOfWeekPtr
        ///< [OUT] Amount of time in milliseconds into the GPS week.
);

//--------------------------------------------------------------------------------------------------
/**
 *  Get the position sample's epoch time.
 *
 * @return
 *  - LE_FAULT         Function failed to acquire the epoch time.
 *  - LE_OK            Function succeeded.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *
 * @note The epoch time is the number of seconds elapsed since January 1, 1970
 *       (midnight UTC/GMT), not counting leaps seconds.
 *
 * @note If the caller is passing an invalid position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetEpochTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint64_t* millisecondsPtr
        ///< [OUT] Milliseconds since Jan. 1, 1970.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's time accurary.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time accuracy is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTimeAccuracy
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* timeAccuracyPtr
        ///< [OUT] Estimated time accuracy in nanoseconds
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's UTC leap seconds in advance
 *
 * @return
 *  - LE_FAULT         Function failed to get the leap seconds.
 *  - LE_OUT_OF_RANGE  The retrieved time accuracy is invalid (set to UINT8_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note The leap seconds in advance is the accumulated time in seconds since the start of GPS Epoch
 * time (Jan 6, 1980). This value has to be added to the UTC time (since Jan. 1, 1970)
 *
 * @note Insertion of each UTC leap second is usually decided about six months in advance by the
 * International Earth Rotation and Reference Systems Service (IERS).
 *
 * @note If the caller is passing an invalid position sample reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetGpsLeapSeconds
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint8_t* leapSecondsPtr
        ///< [OUT] UTC leap seconds in advance in seconds
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's date.
 *
 * @return
 *  - LE_FAULT         Function failed to get the date.
 *  - LE_OUT_OF_RANGE  The retrieved date is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDate
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2014].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's horizontal speed.
 *
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note hSpeedPtr, hSpeedAccuracyPtr can be set to NULL if not needed.
 *
 * @note Horizontal speed is in meters/second with 2 decimal places (3047 = 30.47 meters/second).
 *
 * @note Horizontal speed accuracy estimate is in meters/second with 1 decimal place
 *       (304 = 30.4 meters/second).
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @warning The Horizontal speed accuracy is platform dependent. Please refer to
 *          @ref platformConstraintsGnss_speedAccuracies section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetHorizontalSpeed
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* hspeedPtr,
        ///< [OUT] Horizontal speed in meters/second [resolution 1e-2].
    uint32_t* hspeedAccuracyPtr
        ///< [OUT] Horizontal speed's accuracy estimate
        ///< in meters/second [resolution 1e-1].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's vertical speed.
 *
 * @return
 *  - LE_FAULT         The function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is not valid (set to INT32_MAX).
 *  - LE_OK            The function succeeded.
 *
 * @note vSpeedPtr, vSpeedAccuracyPtr can be set to NULL if not needed.
 *
 * @note For a 2D position Fix, the vertical speed will be indicated as invalid
 * and set to INT32_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @warning The Vertical speed accuracy is platform dependent. Please refer to
 *          @ref platformConstraintsGnss_speedAccuracies section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetVerticalSpeed
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* vspeedPtr,
        ///< [OUT] Vertical speed in meters/second [resolution 1e-2],
        ///< positive up.
    int32_t* vspeedAccuracyPtr
        ///< [OUT] Vertical speed's accuracy estimate
        ///< in meters/second [resolution 1e-1].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's direction. Direction of movement is the direction that the vehicle or
 * person is actually moving.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note Direction and direction accuracy are given in degrees with 1 decimal place: 1755 = 175.5
 *       degrees.
 *       Direction ranges from 0 to 359.9 degrees, where 0 is True North.
 *
 * @note directionPtr, directionAccuracyPtr can be set to NULL if not needed.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDirection
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* directionPtr,
        ///< [OUT] Direction in degrees [resolution 1e-1].
        ///< Range: 0 to 359.9, where 0 is True North
    uint32_t* directionAccuracyPtr
        ///< [OUT] Direction's accuracy estimate
        ///< in degrees [resolution 1e-1].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Satellites Vehicle information.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid.
 *  - LE_OK            Function succeeded.
 *
 * @note satId[] can be set to 0 if that information list index is not configured, so
 * all satellite parameters (satConst[], satSnr[],satAzim[], satElev[]) are fixed to 0.
 *
 * @note For LE_OUT_OF_RANGE returned code, invalid value depends on field type:
 * UINT16_MAX for satId, LE_GNSS_SV_CONSTELLATION_UNDEFINED for satConst, false for satUsed,
 * UINT8_MAX for satSnr, UINT16_MAX for satAzim, UINT8_MAX for satElev.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSatellitesInfo
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* satIdPtr,
        ///< [OUT] Satellites in View ID number, referring
        ///< to NMEA standard.
    size_t* satIdSizePtr,
        ///< [INOUT]
    le_gnss_Constellation_t* satConstPtr,
        ///< [OUT] GNSS constellation type.
    size_t* satConstSizePtr,
        ///< [INOUT]
    bool* satUsedPtr,
        ///< [OUT] TRUE if satellite in View Used
        ///< for Navigation.
    size_t* satUsedSizePtr,
        ///< [INOUT]
    uint8_t* satSnrPtr,
        ///< [OUT] Satellites in View Signal To
        ///< Noise Ratio [dBHz].
    size_t* satSnrSizePtr,
        ///< [INOUT]
    uint16_t* satAzimPtr,
        ///< [OUT] Satellites in View Azimuth [degrees].
        ///< Range: 0 to 360
        ///< If Azimuth angle is currently unknown,
        ///< the value is set to UINT16_MAX.
    size_t* satAzimSizePtr,
        ///< [INOUT]
    uint8_t* satElevPtr,
        ///< [OUT] Satellites in View Elevation [degrees].
        ///< Range: 0 to 90
        ///< If Elevation angle is currently unknown,
        ///< the value is set to UINT8_MAX.
    size_t* satElevSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the SBAS constellation category given the SBAS satellite number ID.
 *
 */
//--------------------------------------------------------------------------------------------------
le_gnss_SbasConstellationCategory_t le_gnss_GetSbasConstellationCategory
(
    uint16_t satId
        ///< [IN] SBAS satellite number ID, referring to NMEA standard.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Satellites Vehicle status.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid.
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSatellitesStatus
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint8_t* satsInViewCountPtr,
        ///< [OUT] Number of satellites expected to be in view.
    uint8_t* satsTrackingCountPtr,
        ///< [OUT] Number of satellites in view, when tracking.
    uint8_t* satsUsedCountPtr
        ///< [OUT] Number of satellites in view used for Navigation.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the DOP parameters (Dilution Of Precision) for the fixed position.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @deprecated This function is deprecated, le_gnss_GetDilutionOfPrecision() should be used for
 *             new code.
 *
 * @note The DOP values are given with 3 decimal places like: DOP value 2200 = 2.200
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDop
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* hdopPtr,
        ///< [OUT] Horizontal Dilution of Precision [resolution 1e-3].
    uint16_t* vdopPtr,
        ///< [OUT] Vertical Dilution of Precision [resolution 1e-3].
    uint16_t* pdopPtr
        ///< [OUT] Position Dilution of Precision [resolution 1e-3].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the DOP parameter (Dilution Of Precision) for the fixed position.
 *
 * @return
 *  - LE_FAULT         Function failed to find the DOP value.
 *  - LE_OUT_OF_RANGE  The retrieved parameter is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note This function replaces the deprecated function le_gnss_GetDop().
 *
 * @note The DOP value is given with 3 decimal places by default like: DOP value 2200 = 2.200
 *       The resolution can be modified by calling the @c le_gnss_SetDopResolution() function.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDilutionOfPrecision
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    le_gnss_DopType_t dopType,
        ///< [IN] Dilution of Precision type.
    uint16_t* dopPtr
        ///< [OUT] Dilution of Precision corresponding to the dopType
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's altitude with respect to the WGS-84 ellipsoid
 *
 * @return
 *  - LE_FAULT         Function failed to get the altitude.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note altitudeOnWgs84 is in meters, with respect to the WGS-84 ellipsoid with 3 decimal
 *       places (3047 = 3.047 meters).
 *
 * @note For a 2D position fix, the altitude with respect to the WGS-84 ellipsoid will be indicated
 *       as invalid and set to INT32_MAX.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAltitudeOnWgs84
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* altitudeOnWgs84Ptr
        ///< [OUT] Altitude in meters, between WGS-84 earth ellipsoid
        ///< and mean sea level [resolution 1e-3].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's magnetic deviation. It is the difference between the bearing to
 * true north and the bearing shown on a magnetic compass. The deviation is positive when the
 * magnetic north is east of true north.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note magneticDeviation is in degrees, with 1 decimal places (47 = 4.7 degree).
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetMagneticDeviation
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* magneticDeviationPtr
        ///< [OUT] MagneticDeviation in degrees [resolution 1e-1].
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the last updated position sample object reference.
 *
 * @return A reference to last Position's sample.
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 */
//--------------------------------------------------------------------------------------------------
le_gnss_SampleRef_t le_gnss_GetLastSampleRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to release the position sample.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_gnss_ReleaseSampleRef
(
    le_gnss_SampleRef_t positionSampleRef
        ///< [IN] Position sample's reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the SUPL Assisted-GNSS mode.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *  - LE_TIMEOUT a time-out occurred
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetSuplAssistedMode
(
    le_gnss_AssistedMode_t assistedMode
        ///< [IN] Assisted-GNSS mode.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the SUPL Assisted-GNSS mode.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSuplAssistedMode
(
    le_gnss_AssistedMode_t* assistedModePtr
        ///< [OUT] Assisted-GNSS mode.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the SUPL server URL.
 * That server URL is a NULL-terminated string with a maximum string length (including NULL
 * terminator) equal to 256. Optionally the port number is specified after a colon.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 *
 * @note If the SUPL server URL size is bigger than the maximum string length (including NULL
 * terminator) size, it is a fatal error, the function will not return.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetSuplServerUrl
(
    const char* LE_NONNULL suplServerUrl
        ///< [IN] SUPL server URL.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function injects the SUPL certificate to be used in A-GNSS sessions. Certificates must
 * be encoded in DER. Other certificate encryptions (e.g., PEM, CER and CRT)
 * aren't supported.
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER on invalid parameter
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 *
 * @note If the SUPL certificate size is bigger than the Maximum SUPL certificate size,
 * it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_InjectSuplCertificate
(
    uint8_t suplCertificateId,
        ///< [IN] ID of the SUPL certificate.
        ///< Certificate ID range is 0 to 9
    uint16_t suplCertificateLen,
        ///< [IN] SUPL certificate size in Bytes.
    const char* LE_NONNULL suplCertificate
        ///< [IN] SUPL certificate contents.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function deletes the SUPL certificate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER on invalid parameter
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_DeleteSuplCertificate
(
    uint8_t suplCertificateId
        ///< [IN] ID of the SUPL certificate.
        ///< Certificate ID range is 0 to 9
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the enabled NMEA sentences using a bit mask.
 *
 * @return
 *  - LE_OK             Success
 *  - LE_BAD_PARAMETER  Bit mask exceeds the maximal value
 *  - LE_FAULT          Failure
 *  - LE_BUSY           Service is busy
 *  - LE_TIMEOUT        Timeout occurred
 *  - LE_NOT_PERMITTED  GNSS device is not in "ready" state
 *
 * @warning This function may be subject to limitations depending on the platform. Please refer to
 *          the @ref platformConstraintsGnss page.
 *
 * @note Some NMEA sentences are unsupported depending on the plateform. Please refer to
 *       @ref platformConstraintsGnss_nmeaMask section for full details. Setting an unsuported NMEA
 *       sentence won't report an error.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 *
 * @deprecated LE_GNSS_NMEA_MASK_PQXFI is deprecated. LE_GNSS_NMEA_MASK_PTYPE should be used
 *             instead. Setting LE_GNSS_NMEA_MASK_PTYPE will also set LE_GNSS_NMEA_MASK_PQXFI.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetNmeaSentences
(
    le_gnss_NmeaBitMask_t nmeaMask
        ///< [IN] Bit mask for enabled NMEA sentences.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the bit mask for the enabled NMEA sentences.
 *
 * @return
 *  - LE_OK             Success
 *  - LE_FAULT          Failure
 *  - LE_BUSY           Service is busy
 *  - LE_TIMEOUT        Timeout occurred
 *  - LE_NOT_PERMITTED  GNSS device is not in "ready" state
 *
 * @note If the caller is passing an null pointer to this function, it is a fatal error
 *       and the function will not return.
 *
 * @note Some NMEA sentences are unsupported depending on the plateform. Please refer to
 *       @ref platformConstraintsGnss_nmeaMask section for full details. The bit mask for an unset
 *       or unsupported NMEA sentence is zero.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetNmeaSentences
(
    le_gnss_NmeaBitMask_t* nmeaMaskPtrPtr
        ///< [OUT] Bit mask for enabled NMEA sentences.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the status of the GNSS device.
 *
 */
//--------------------------------------------------------------------------------------------------
le_gnss_State_t le_gnss_GetState
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the GNSS minimum elevation.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_OUT_OF_RANGE if the minimum elevation is above range
 *  - LE_UNSUPPORTED request not supported
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetMinElevation
(
    uint8_t minElevation
        ///< [IN] Minimum elevation in degrees [range 0..90].
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the GNSS minimum elevation.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *
 * @note If the caller is passing an null pointer to this function, it is a fatal error
 *       and the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetMinElevation
(
    uint8_t* minElevationPtrPtr
        ///< [OUT] Minimum elevation in degrees [range 0..90].
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the resolution for the DOP parameters
 *
 * @return LE_OK               Function succeeded.
 * @return LE_BAD_PARAMETER    Invalid parameter provided.
 * @return LE_FAULT            Function failed.
 *
 * @note The function sets the same resolution to all DOP values returned by
 *       le_gnss_GetDilutionOfPrecision() API. The resolution setting takes effect immediately.
 *
 * @note The resolution setting is done per client session.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetDopResolution
(
    le_gnss_Resolution_t resolution
        ///< [IN] Resolution.
);

#endif // LE_GNSS_INTERFACE_H_INCLUDE_GUARD