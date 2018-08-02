

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_temp Temperature Monitoring
 *
 * @ref le_temp_interface.h "API Reference" <br>
 * @ref platformConstraintsTemperature Constraints
 *
 * <HR>
 *
 * The Temperature API is used to retrieve temperature values and set warning and critical
 * thresholds.
 *
 * @warning When a Critical event occurs, some platform can automatically switch off.
 *
 * @section le_temp_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b modemService application service.
 *
 * Here's a code sample binding to modem services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_temp -> modemServices.le_temp
   }
   @endverbatim
 *
 *
 * @section le_temp_monitoring Monitoring
 *
 * This functionality allows to monitor the temperature by getting the temperature sensor and
 * by setting several temperature thresholds.
 *
 * @warning Ensure to check the names of supported sensors and thresholds for your specific
 * platform. Please refer to @subpage platformConstraintsTemperature platform constraints page.
 *
 * @warning An hysteresis can be dependent of the platform. Be sure to verify your platform's
 * specifications.
 *
 * @warning On some platforms, the thresholds parameters are persistent and a Platform reboot is
 * required for thresholds change takes effect.
 *
 * When temperature thresholds are reached, an event is sent with a string type that specifies the
 * raised threshold.
 *
 * - le_temp_Request() function allows the application to monitor a requested temperature sensor.
 *
 * - le_temp_GetSensorName() function allows the application to retrieve the temperature sensor's
 *   name from its reference.
 *
 * - le_temp_GetTemperature() function allows the application to retrieve the temperature of a
 *   sensor.
 *
 * - le_temp_GetThreshold() function allows the application to get the configured threshold
 *   for a sensor.
 *
 * - le_temp_SetThreshold() function allows the application to configure a threshold for a sensor.
 *
 * - le_temp_StartMonitoring() function allows to start the temperature monitoring with the
 *   thresholds configured by le_temp_SetThreshold() function.
 *
 * - le_temp_AddThresholdEventHandler() API adds a handler to notify when a temperature
 *   threshold is reached.
 *
 * - le_temp_RemoveThresholdEventHandler() API removes the temperature handler.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_temp_interface.h
 *
 * Legato @ref c_temp include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_TEMP_INTERFACE_H_INCLUDE_GUARD
#define LE_TEMP_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_temp_DisconnectHandler_t)(void *);

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
void le_temp_ConnectService
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
le_result_t le_temp_TryConnectService
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
void le_temp_SetServerDisconnectHandler
(
    le_temp_DisconnectHandler_t disconnectHandler,
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
void le_temp_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Maximum sensor's name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_SENSOR_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum sensor's name string length.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_SENSOR_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Maximum threshold's name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_THRESHOLD_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum threshold's name string length.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_THRESHOLD_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 *  Reference type for a temperature sensor.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_temp_Sensor* le_temp_SensorRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_temp_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_temp_ThresholdEventHandler* le_temp_ThresholdEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Temperature event.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_temp_ThresholdEventHandlerFunc_t)
(
    le_temp_SensorRef_t sensor,
        ///< Temperature sensor reference.
    const char* LE_NONNULL threshold,
        ///< Name of the raised threshold.
    void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Request a temperature sensor reference.
 *
 * @return
 *      - Reference to the temperature sensor.
 *      - NULL when the requested sensor is not supported.
 */
//--------------------------------------------------------------------------------------------------
le_temp_SensorRef_t le_temp_Request
(
    const char* LE_NONNULL sensorName
        ///< [IN] Name of the temperature sensor.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the temperature sensor's name from its reference.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_OVERFLOW      The name length exceed the maximum length.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_temp_GetSensorName
(
    le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
    char* sensorName,
        ///< [OUT] Name of the temperature sensor.
    size_t sensorNameSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_temp_ThresholdEvent'
 *
 * This event provides information on the threshold reached.
 *
 */
//--------------------------------------------------------------------------------------------------
le_temp_ThresholdEventHandlerRef_t le_temp_AddThresholdEventHandler
(
    le_temp_ThresholdEventHandlerFunc_t ThresholdEventHandlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_temp_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_temp_RemoveThresholdEventHandler
(
    le_temp_ThresholdEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the temperature in degree Celsius.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the temperature.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_temp_GetTemperature
(
    le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
    int32_t* temperaturePtr
        ///< [OUT] Temperature in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the temperature threshold in degree Celsius. This function does not start the temperature
 * monitoring, call le_temp_StartMonitoring() to start it.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_temp_SetThreshold
(
    le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
    const char* LE_NONNULL threshold,
        ///< [IN] Name of the threshold.
    int32_t temperature
        ///< [IN] Temperature threshold in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the temperature threshold in degree Celsius.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_temp_GetThreshold
(
    le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
    const char* LE_NONNULL threshold,
        ///< [IN] Name of the threshold.
    int32_t* temperaturePtr
        ///< [OUT] Temperature threshold in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Start the temperature monitoring with the temperature thresholds configured by
 * le_temp_SetThreshold() function.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_BAD_PARAMETER There is a mismatch with the configured threshold values.
 *      - LE_FAULT         The function failed to apply the thresholds.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_temp_StartMonitoring
(
    void
);

#endif // LE_TEMP_INTERFACE_H_INCLUDE_GUARD