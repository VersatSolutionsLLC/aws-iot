

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_posCtrl Positioning Control API
 *
 * @ref le_posCtrl_interface.h "API Reference"
 *
 * <HR>
 *
 * This API is used to enable and disable the positioning service.
 *
 * All other positioning functionality is provided by the @ref c_pos.
 *
 * @section le_posCtrl_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b positioningService application service.
 *
 * Here's a code sample binding to Positioning services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_posCtrl -> positioningService.le_posCtrl
   }
   @endverbatim
 *
 * @section le_posCtrl_ctrl Positioning control
 *
 * To request that the positioning service be activated, call le_posCtrl_Request().
 *
 * To release the positioning service when finished with it, call le_posCtrl_Release().
 *
 * If the positioning service is already active when le_posCtrl_Request() is called, it will remain
 * active.  This happens if another application has already requested that the positioning service
 * be activated and not yet released the positioning service.
 *
 * The positioning service (and associated devices) will not necessarily be switched off when
 * an application calls le_posCtrl_Release(). The positioning service will be switched off only
 * after all applications that have requested that the positioning service be active have also
 * released the positioning service.
 *
 * @note    Control of the positioning service is done through a separate API than the retrieval
 *          of positioning information to allow access to positioning information by less-trusted
 *          applications, while only allowing control of the positioning system by more-trusted
 *          applications.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_posCtrl_interface.h
 *
 * Legato @ref c_posCtrl include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_POSCTRL_INTERFACE_H_INCLUDE_GUARD
#define LE_POSCTRL_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_posCtrl_DisconnectHandler_t)(void *);

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
void le_posCtrl_ConnectService
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
le_result_t le_posCtrl_TryConnectService
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
void le_posCtrl_SetServerDisconnectHandler
(
    le_posCtrl_DisconnectHandler_t disconnectHandler,
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
void le_posCtrl_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type for dealing with Position Service management.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_posCtrl_Activation* le_posCtrl_ActivationRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Request activation of the positioning service.
 *
 * @return
 *      - Reference to the service activation request (to be used later for releasing the request).
 *      - NULL if the service request could not be processed.
 *
 */
//--------------------------------------------------------------------------------------------------
le_posCtrl_ActivationRef_t le_posCtrl_Request
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Release the positioning service.
 *
 */
//--------------------------------------------------------------------------------------------------
void le_posCtrl_Release
(
    le_posCtrl_ActivationRef_t ref
        ///< [IN] Reference to a positioning service activation request.
);

#endif // LE_POSCTRL_INTERFACE_H_INCLUDE_GUARD