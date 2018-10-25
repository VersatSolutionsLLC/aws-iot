/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#include "le_pos_messages.h"
#include "le_pos_interface.h"


//--------------------------------------------------------------------------------------------------
// Generic Client Types, Variables and Functions
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
/**
 * Client Data Objects
 *
 * This object is used for each registered handler.  This is needed since we are not using
 * events, but are instead queueing functions directly with the event loop.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_event_HandlerFunc_t handlerPtr;          ///< Registered handler function
    void*                  contextPtr;          ///< ContextPtr registered with handler
    le_event_HandlerRef_t  handlerRef;          ///< HandlerRef for the registered handler
    le_thread_Ref_t        callersThreadRef;    ///< Caller's thread.
}
_ClientData_t;


//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for client data objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ClientDataPool;


//--------------------------------------------------------------------------------------------------
/**
 * Client Thread Objects
 *
 * This object is used to contain thread specific data for each IPC client.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_msg_SessionRef_t sessionRef;     ///< Client Session Reference
    int                 clientCount;    ///< Number of clients sharing this thread
    le_pos_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for client thread objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ClientThreadDataPool;


//--------------------------------------------------------------------------------------------------
/**
 * Key under which the pointer to the Thread Object (_ClientThreadData_t) will be kept in
 * thread-local storage.  This allows a thread to quickly get a pointer to its own Thread Object.
 */
//--------------------------------------------------------------------------------------------------
static pthread_key_t _ThreadDataKey;


//--------------------------------------------------------------------------------------------------
/**
 * Safe Reference Map for use with Add/Remove handler references
 *
 * @warning Use _Mutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static le_ref_MapRef_t _HandlerRefMap;


//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above HandlerRefMap.
 *
 * Unused attribute is needed because this variable may not always get used.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static pthread_mutex_t _Mutex = PTHREAD_MUTEX_INITIALIZER;   // POSIX "Fast" mutex.

/// Locks the mutex.
#define _LOCK    LE_ASSERT(pthread_mutex_lock(&_Mutex) == 0);

/// Unlocks the mutex.
#define _UNLOCK  LE_ASSERT(pthread_mutex_unlock(&_Mutex) == 0);


//--------------------------------------------------------------------------------------------------
/**
 * This global flag is shared by all client threads, and is used to indicate whether the common
 * data has been initialized.
 *
 * @warning Use InitMutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static bool CommonDataInitialized = false;


//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above CommonDataInitialized.
 */
//--------------------------------------------------------------------------------------------------
static pthread_mutex_t InitMutex = PTHREAD_MUTEX_INITIALIZER;   // POSIX "Fast" mutex.

/// Locks the mutex.
#define LOCK_INIT    LE_ASSERT(pthread_mutex_lock(&InitMutex) == 0);

/// Unlocks the mutex.
#define UNLOCK_INIT  LE_ASSERT(pthread_mutex_unlock(&InitMutex) == 0);

//--------------------------------------------------------------------------------------------------
/**
 * Trace reference used for controlling tracing in this module.
 */
//--------------------------------------------------------------------------------------------------
#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)

static le_log_TraceRef_t TraceRef;

/// Macro used to generate trace output in this module.
/// Takes the same parameters as LE_DEBUG() et. al.
#define TRACE(...) LE_TRACE(TraceRef, ##__VA_ARGS__)

/// Macro used to query current trace state in this module
#define IS_TRACE_ENABLED LE_IS_TRACE_ENABLED(TraceRef)

#else

#define TRACE(...)
#define IS_TRACE_ENABLED 0

#endif

//--------------------------------------------------------------------------------------------------
/**
 * Forward declaration needed by InitClientForThread
 */
//--------------------------------------------------------------------------------------------------
static void ClientIndicationRecvHandler
(
    le_msg_MessageRef_t  msgRef,
    void*                contextPtr
);


//--------------------------------------------------------------------------------------------------
/**
 * Initialize thread specific data, and connect to the service for the current thread.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t InitClientForThread
(
    bool isBlocking
)
{
#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)
    // Get a reference to the trace keyword that is used to control tracing in this module.
    TraceRef = le_log_GetTraceRef("ipc");
#endif

    // Open a session.
    le_msg_ProtocolRef_t protocolRef;
    le_msg_SessionRef_t sessionRef;

    protocolRef = le_msg_GetProtocolRef(PROTOCOL_ID_STR, sizeof(_Message_t));
    sessionRef = le_msg_CreateSession(protocolRef, SERVICE_INSTANCE_NAME);
    le_msg_SetSessionRecvHandler(sessionRef, ClientIndicationRecvHandler, NULL);

    if ( isBlocking )
    {
        le_msg_OpenSessionSync(sessionRef);
    }
    else
    {
        le_result_t result;

        result = le_msg_TryOpenSessionSync(sessionRef);
        if ( result != LE_OK )
        {
            LE_DEBUG("Could not connect to '%s' service", SERVICE_INSTANCE_NAME);

            le_msg_DeleteSession(sessionRef);

            switch (result)
            {
                case LE_UNAVAILABLE:
                    LE_DEBUG("Service not offered");
                    break;

                case LE_NOT_PERMITTED:
                    LE_DEBUG("Missing binding");
                    break;

                case LE_COMM_ERROR:
                    LE_DEBUG("Can't reach ServiceDirectory");
                    break;

                default:
                    LE_CRIT("le_msg_TryOpenSessionSync() returned unexpected result code %d (%s)",
                            result,
                            LE_RESULT_TXT(result));
                    break;
            }

            return result;
        }
    }

    // Store the client sessionRef in thread-local storage, since each thread requires
    // its own sessionRef.
    _ClientThreadData_t* clientThreadPtr = le_mem_ForceAlloc(_ClientThreadDataPool);
    memset(clientThreadPtr, 0, sizeof(_ClientThreadData_t));
    clientThreadPtr->sessionRef = sessionRef;
    if (pthread_setspecific(_ThreadDataKey, clientThreadPtr) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    // This is the first client for the current thread
    clientThreadPtr->clientCount = 1;

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the client thread data for the current thread.
 *
 * If the current thread does not have client data, then NULL is returned
 */
//--------------------------------------------------------------------------------------------------
static _ClientThreadData_t* GetClientThreadDataPtr
(
    void
)
{
    return pthread_getspecific(_ThreadDataKey);
}


//--------------------------------------------------------------------------------------------------
/**
 * Return the sessionRef for the current thread.
 *
 * If the current thread does not have a session ref, then this is a fatal error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static le_msg_SessionRef_t GetCurrentSessionRef
(
    void
)
{
    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then the session ref has not been created.
    LE_FATAL_IF(clientThreadPtr==NULL,
                "le_pos_ConnectService() not called for current thread");

    return clientThreadPtr->sessionRef;
}


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads.
 */
//--------------------------------------------------------------------------------------------------
static void InitCommonData(void)
{
    // Allocate the client data pool
    _ClientDataPool = le_mem_CreatePool("le_pos_ClientData", sizeof(_ClientData_t));

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_CreatePool("le_pos_ClientThreadData", sizeof(_ClientThreadData_t));

    // Create the thread-local data key to be used to store a pointer to each thread object.
    LE_ASSERT(pthread_key_create(&_ThreadDataKey, NULL) == 0);

    // Create safe reference map for handler references.
    // The size of the map should be based on the number of handlers defined multiplied by
    // the number of client threads.  Since this number can't be completely determined at
    // build time, just make a reasonable guess.
    _HandlerRefMap = le_ref_CreateMap("le_pos_ClientHandlers", 5);
}


//--------------------------------------------------------------------------------------------------
/**
 * Connect to the service, using either blocking or non-blocking calls.
 *
 * This function implements the details of the public ConnectService functions.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t DoConnectService
(
    bool isBlocking
)
{
    // If this is the first time the function is called, init the client common data.
    LOCK_INIT
    if ( ! CommonDataInitialized )
    {
        InitCommonData();
        CommonDataInitialized = true;
    }
    UNLOCK_INIT

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        le_result_t result;

        result = InitClientForThread(isBlocking);
        if ( result != LE_OK )
        {
            // Note that the blocking call will always return LE_OK
            return result;
        }

        LE_DEBUG("======= Starting client for '%s' service ========", SERVICE_INSTANCE_NAME);
    }
    else
    {
        // Keep track of the number of clients for the current thread.  There is only one
        // connection per thread, and it is shared by all clients.
        clientThreadPtr->clientCount++;
        LE_DEBUG("======= Starting another client for '%s' service ========",
                 SERVICE_INSTANCE_NAME);
    }

    return LE_OK;
}


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
void le_pos_ConnectService
(
    void
)
{
    // Connect to the service; block until connected.
    DoConnectService(true);
}

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
le_result_t le_pos_TryConnectService
(
    void
)
{
    // Connect to the service; return with an error if not connected.
    return DoConnectService(false);
}

//--------------------------------------------------------------------------------------------------
// Session close handler.
//
// Dispatches session close notifications to the registered client handler function (if any)
//--------------------------------------------------------------------------------------------------
static void SessionCloseHandler
(
    le_msg_SessionRef_t sessionRef,
    void *contextPtr
)
{
    _ClientThreadData_t* clientThreadPtr = contextPtr;

    le_msg_DeleteSession( clientThreadPtr->sessionRef );

    // Need to delete the thread specific data, since it is no longer valid.  If a new
    // client session is started, new thread specific data will be allocated.
    le_mem_Release(clientThreadPtr);
    if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    LE_DEBUG("======= '%s' service spontaneously disconnected ========", SERVICE_INSTANCE_NAME);

    if (clientThreadPtr->disconnectHandler)
    {
        clientThreadPtr->disconnectHandler(clientThreadPtr->contextPtr);
    }

    LE_FATAL("Component for le_pos disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_pos_SetServerDisconnectHandler
(
    le_pos_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    if (NULL == clientThreadPtr)
    {
        LE_CRIT("Trying to set disconnect handler for non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        clientThreadPtr->disconnectHandler = disconnectHandler;
        clientThreadPtr->contextPtr = contextPtr;

        if (disconnectHandler)
        {
            le_msg_SetSessionCloseHandler(clientThreadPtr->sessionRef,
                                          SessionCloseHandler,
                                          clientThreadPtr);
        }
    }
}

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
void le_pos_DisconnectService
(
    void
)
{
    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        LE_CRIT("Trying to stop non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        // This is the last client for this thread, so close the session.
        if ( clientThreadPtr->clientCount == 1 )
        {
            le_msg_DeleteSession( clientThreadPtr->sessionRef );

            // Need to delete the thread specific data, since it is no longer valid.  If a new
            // client session is started, new thread specific data will be allocated.
            le_mem_Release(clientThreadPtr);
            if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
            {
                LE_FATAL("pthread_setspecific() failed!");
            }

            LE_DEBUG("======= Stopping client for '%s' service ========", SERVICE_INSTANCE_NAME);
        }
        else
        {
            // There is one less client sharing this thread's connection.
            clientThreadPtr->clientCount--;

            LE_DEBUG("======= Stopping another client for '%s' service ========",
                     SERVICE_INSTANCE_NAME);
        }
    }
}


//--------------------------------------------------------------------------------------------------
// Client Specific Client Code
//--------------------------------------------------------------------------------------------------


// This function parses the message buffer received from the server, and then calls the user
// registered handler, which is stored in a client data object.
static void _Handle_le_pos_AddMovementHandler
(
    void* _reportPtr,
    void* _dataPtr
)
{
    le_msg_MessageRef_t _msgRef = _reportPtr;
    _Message_t* _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    uint8_t* _msgBufPtr = _msgPtr->buffer;
    size_t _msgBufSize = _MAX_MSG_SIZE;

    // The clientContextPtr always exists and is always first. It is a safe reference to the client
    // data object, but we already get the pointer to the client data object through the _dataPtr
    // parameter, so we don't need to do anything with clientContextPtr, other than unpacking it.
    void* _clientContextPtr;
    if (!le_pack_UnpackReference( &_msgBufPtr, &_msgBufSize,
                                  &_clientContextPtr ))
    {
        goto error_unpack;
    }

    // The client data pointer is passed in as a parameter, since the lookup in the safe ref map
    // and check for NULL has already been done when this function is queued.
    _ClientData_t* _clientDataPtr = _dataPtr;

    // Pull out additional data from the client data pointer
    le_pos_MovementHandlerFunc_t _handlerRef_le_pos_AddMovementHandler = (le_pos_MovementHandlerFunc_t)_clientDataPtr->handlerPtr;
    void* contextPtr = _clientDataPtr->contextPtr;

    // Unpack the remaining parameters
    le_pos_SampleRef_t positionSampleRef;
    if (!le_pack_UnpackReference( &_msgBufPtr, &_msgBufSize,
                                               &positionSampleRef ))
    {
        goto error_unpack;
    }

    // Call the registered handler
    if ( _handlerRef_le_pos_AddMovementHandler != NULL )
    {
        _handlerRef_le_pos_AddMovementHandler(positionSampleRef, contextPtr );
    }
    else
    {
        LE_FATAL("Error in client data: no registered handler");
    }

    // Release the message, now that we are finished with it.
    le_msg_ReleaseMsg(_msgRef);

    return;

error_unpack:
    // Handle any unpack errors by dying -- server should not be sending invalid data; if it is
    // something is seriously wrong.
    LE_FATAL("Error unpacking message");
}


//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_pos_Movement'
 *
 * This event provides information on movement changes.
 *
 */
//--------------------------------------------------------------------------------------------------
le_pos_MovementHandlerRef_t le_pos_AddMovementHandler
(
    uint32_t horizontalMagnitude,
        ///< [IN] Horizontal magnitude in meters.
        ///<       0 means that I don't care about
        ///<      changes in the latitude and longitude.
    uint32_t verticalMagnitude,
        ///< [IN] Vertical magnitude in meters.
        ///<       0 means that I don't care about
        ///<       changes in the altitude.
    le_pos_MovementHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_pos_MovementHandlerRef_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_AddMovementHandler;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  horizontalMagnitude ));
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  verticalMagnitude ));
    // The handlerPtr and contextPtr input parameters are stored in the client
    // data object, and it is a safe reference to this object that is passed down
    // as the context pointer.  The handlerPtr is not passed down.
    // Create a new client data object and fill it in
    _ClientData_t* _clientDataPtr = le_mem_ForceAlloc(_ClientDataPool);
    _clientDataPtr->handlerPtr = (le_event_HandlerFunc_t)handlerPtr;
    _clientDataPtr->contextPtr = contextPtr;
    _clientDataPtr->callersThreadRef = le_thread_GetCurrent();
    // Create a safeRef to be passed down as the contextPtr
    _LOCK
    contextPtr = le_ref_CreateRef(_HandlerRefMap, _clientDataPtr);
    _UNLOCK
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize, contextPtr ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackReference( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    if (_result)
    {
        // Put the handler reference result into the client data object, and
        // then return a safe reference to the client data object as the reference;
        // this safe reference is contained in the contextPtr, which was assigned
        // when the client data object was created.
        _clientDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        _result = contextPtr;
    }
    else
    {
        // Add failed, release the client data.
        le_mem_Release(_clientDataPtr);
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_pos_Movement'
 */
//--------------------------------------------------------------------------------------------------
void le_pos_RemoveMovementHandler
(
    le_pos_MovementHandlerRef_t handlerRef
        ///< [IN]
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_RemoveMovementHandler;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    // The passed in handlerRef is a safe reference for the client data object.  Need to get the
    // real handlerRef from the client data object and then delete both the safe reference and
    // the object since they are no longer needed.
    _LOCK
    _ClientData_t* clientDataPtr = le_ref_Lookup(_HandlerRefMap, handlerRef);
    LE_FATAL_IF(clientDataPtr==NULL, "Invalid reference");
    le_ref_DeleteRef(_HandlerRefMap, handlerRef);
    _UNLOCK
    handlerRef = (le_pos_MovementHandlerRef_t)clientDataPtr->handlerRef;
    le_mem_Release(clientDataPtr);
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                     handlerRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);

    return;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the 2D location's data (Latitude, Longitude, Horizontal
 * accuracy).
 *
 * @return LE_FAULT         Function failed to get the 2D location's data
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note latitudePtr, longitudePtr, hAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_Get2DLocation
(
    int32_t* latitudePtr,
        ///< [OUT] WGS84 Latitude in degrees, positive North [resolution 1e-6].
    int32_t* longitudePtr,
        ///< [OUT] WGS84 Longitude in degrees, positive East [resolution 1e-6].
    int32_t* hAccuracyPtr
        ///< [OUT] Horizontal position's accuracy in meters by default.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_Get2DLocation;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(latitudePtr)) << 0);
    _requiredOutputs |= ((!!(longitudePtr)) << 1);
    _requiredOutputs |= ((!!(hAccuracyPtr)) << 2);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (latitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               latitudePtr )))
    {
        goto error_unpack;
    }
    if (longitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               longitudePtr )))
    {
        goto error_unpack;
    }
    if (hAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               hAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the 3D location's data (Latitude, Longitude, Altitude,
 * Horizontal accuracy, Vertical accuracy).
 *
 * @return LE_FAULT         Function failed to get the 3D location's data
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note latitudePtr, longitudePtr,hAccuracyPtr, altitudePtr, vAccuracyPtr can be set to NULL
 *       if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_Get3DLocation
(
    int32_t* latitudePtr,
        ///< [OUT] WGS84 Latitude in degrees, positive North [resolution 1e-6].
    int32_t* longitudePtr,
        ///< [OUT] WGS84 Longitude in degrees, positive East [resolution 1e-6].
    int32_t* hAccuracyPtr,
        ///< [OUT] Horizontal position's accuracy in meters by default.
    int32_t* altitudePtr,
        ///< [OUT] Altitude above Mean Sea Level in meters by default.
    int32_t* vAccuracyPtr
        ///< [OUT] Vertical position's accuracy in meters by default.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_Get3DLocation;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(latitudePtr)) << 0);
    _requiredOutputs |= ((!!(longitudePtr)) << 1);
    _requiredOutputs |= ((!!(hAccuracyPtr)) << 2);
    _requiredOutputs |= ((!!(altitudePtr)) << 3);
    _requiredOutputs |= ((!!(vAccuracyPtr)) << 4);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (latitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               latitudePtr )))
    {
        goto error_unpack;
    }
    if (longitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               longitudePtr )))
    {
        goto error_unpack;
    }
    if (hAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               hAccuracyPtr )))
    {
        goto error_unpack;
    }
    if (altitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               altitudePtr )))
    {
        goto error_unpack;
    }
    if (vAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               vAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the time of the last updated location
 *
 * @return LE_FAULT         Function failed to get the time.
 * @return LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetTime
(
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetTime;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(hoursPtr)) << 0);
    _requiredOutputs |= ((!!(minutesPtr)) << 1);
    _requiredOutputs |= ((!!(secondsPtr)) << 2);
    _requiredOutputs |= ((!!(millisecondsPtr)) << 3);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (hoursPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               hoursPtr )))
    {
        goto error_unpack;
    }
    if (minutesPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               minutesPtr )))
    {
        goto error_unpack;
    }
    if (secondsPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               secondsPtr )))
    {
        goto error_unpack;
    }
    if (millisecondsPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               millisecondsPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the date of the last updated location
 *
 * @return LE_FAULT         Function failed to get the date.
 * @return LE_OUT_OF_RANGE  The retrieved date is invalid (all fields are set to 0).
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetDate
(
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2014].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetDate;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(yearPtr)) << 0);
    _requiredOutputs |= ((!!(monthPtr)) << 1);
    _requiredOutputs |= ((!!(dayPtr)) << 2);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (yearPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               yearPtr )))
    {
        goto error_unpack;
    }
    if (monthPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               monthPtr )))
    {
        goto error_unpack;
    }
    if (dayPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               dayPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the motion's data (Horizontal Speed, Horizontal Speed's
 * accuracy, Vertical Speed, Vertical Speed's accuracy).
 *
 * @return LE_FAULT         Function failed to get the motion's data.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX,
 *                          UINT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note hSpeedPtr, hSpeedAccuracyPtr, vSpeedPtr, vSpeedAccuracyPtr can be set to NULL if not
 *       needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetMotion
(
    uint32_t* hSpeedPtr,
        ///< [OUT] Horizontal Speed in m/sec.
    uint32_t* hSpeedAccuracyPtr,
        ///< [OUT] Horizontal Speed's accuracy in m/sec.
    int32_t* vSpeedPtr,
        ///< [OUT] Vertical Speed in m/sec, positive up.
    int32_t* vSpeedAccuracyPtr
        ///< [OUT] Vertical Speed's accuracy in m/sec.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetMotion;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(hSpeedPtr)) << 0);
    _requiredOutputs |= ((!!(hSpeedAccuracyPtr)) << 1);
    _requiredOutputs |= ((!!(vSpeedPtr)) << 2);
    _requiredOutputs |= ((!!(vSpeedAccuracyPtr)) << 3);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (hSpeedPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               hSpeedPtr )))
    {
        goto error_unpack;
    }
    if (hSpeedAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               hSpeedAccuracyPtr )))
    {
        goto error_unpack;
    }
    if (vSpeedPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               vSpeedPtr )))
    {
        goto error_unpack;
    }
    if (vSpeedAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               vSpeedAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the heading indication.
 *
 * @return LE_FAULT         Function failed to get the heading indication.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note Heading is given in degrees.
 *       Heading ranges from 0 to 359 degrees, where 0 is True North.
 *
 * @note headingPtr, headingAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetHeading
(
    uint32_t* headingPtr,
        ///< [OUT] Heading in degrees.
        ///< Range: 0 to 359, where 0 is True North.
    uint32_t* headingAccuracyPtr
        ///< [OUT] Heading's accuracy in degrees.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetHeading;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(headingPtr)) << 0);
    _requiredOutputs |= ((!!(headingAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (headingPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               headingPtr )))
    {
        goto error_unpack;
    }
    if (headingAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               headingAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the direction indication. Direction of movement is the direction that the vehicle or person
 * is actually moving.
 *
 * @return LE_FAULT         Function failed to get the direction indication.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note Direction is given in degrees.
 *       Direction ranges from 0 to 359 degrees, where 0 is True North.
 *
 * @note directionPtr, directionAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetDirection
(
    uint32_t* directionPtr,
        ///< [OUT] Direction indication in degrees.
        ///< Range: 0 to 359, where 0 is True North.
    uint32_t* directionAccuracyPtr
        ///< [OUT] Direction's accuracy estimate in degrees.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetDirection;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(directionPtr)) << 0);
    _requiredOutputs |= ((!!(directionAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (directionPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               directionPtr )))
    {
        goto error_unpack;
    }
    if (directionAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               directionAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position fix state
 *
 * @return LE_FAULT         Function failed to get the position fix state.
 * @return LE_OK            Function succeeded.
 *
 * @note In case the function fails to get the position fix state, a fatal error occurs,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_GetFixState
(
    le_pos_FixState_t* statePtr
        ///< [OUT] Position fix state.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetFixState;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(statePtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (statePtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               statePtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's 2D location (latitude, longitude,
 * horizontal accuracy).
 *
 * @return LE_FAULT         Function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note latitudePtr, longitudePtr, horizontalAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_Get2DLocation
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* latitudePtr,
        ///< [OUT] WGS84 Latitude in degrees, positive North [resolution 1e-6].
    int32_t* longitudePtr,
        ///< [OUT] WGS84 Longitude in degrees, positive East [resolution 1e-6].
    int32_t* horizontalAccuracyPtr
        ///< [OUT] Horizontal position's accuracy in meters by default.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_Get2DLocation;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(latitudePtr)) << 0);
    _requiredOutputs |= ((!!(longitudePtr)) << 1);
    _requiredOutputs |= ((!!(horizontalAccuracyPtr)) << 2);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (latitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               latitudePtr )))
    {
        goto error_unpack;
    }
    if (longitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               longitudePtr )))
    {
        goto error_unpack;
    }
    if (horizontalAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               horizontalAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's time.
 *
 * @return LE_FAULT         Function failed to get the time.
 * @return LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetTime
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetTime;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(hoursPtr)) << 0);
    _requiredOutputs |= ((!!(minutesPtr)) << 1);
    _requiredOutputs |= ((!!(secondsPtr)) << 2);
    _requiredOutputs |= ((!!(millisecondsPtr)) << 3);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (hoursPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               hoursPtr )))
    {
        goto error_unpack;
    }
    if (minutesPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               minutesPtr )))
    {
        goto error_unpack;
    }
    if (secondsPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               secondsPtr )))
    {
        goto error_unpack;
    }
    if (millisecondsPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               millisecondsPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's date.
 *
 * @return LE_FAULT         Function failed to get the date.
 * @return LE_OUT_OF_RANGE  The retrieved date is invalid (all fields are set to 0).
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetDate
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2014].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetDate;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(yearPtr)) << 0);
    _requiredOutputs |= ((!!(monthPtr)) << 1);
    _requiredOutputs |= ((!!(dayPtr)) << 2);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (yearPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               yearPtr )))
    {
        goto error_unpack;
    }
    if (monthPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               monthPtr )))
    {
        goto error_unpack;
    }
    if (dayPtr &&
        (!le_pack_UnpackUint16( &_msgBufPtr, &_msgBufSize,
                                               dayPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's altitude.
 *
 * @return LE_FAULT         Function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note altitudePtr, altitudeAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetAltitude
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* altitudePtr,
        ///< [OUT] Altitude above Mean Sea Level in meters by default.
    int32_t* altitudeAccuracyPtr
        ///< [OUT] Vertical position's accuracy in meters by default.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetAltitude;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(altitudePtr)) << 0);
    _requiredOutputs |= ((!!(altitudeAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (altitudePtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               altitudePtr )))
    {
        goto error_unpack;
    }
    if (altitudeAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               altitudeAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's horizontal speed.
 *
 * @return LE_FAULT         Function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to INT32_MAX,
 *                          UINT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note hSpeedPtr, hSpeedAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetHorizontalSpeed
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* hSpeedPtr,
        ///< [OUT] The Horizontal Speed in m/sec.
    uint32_t* hSpeedAccuracyPtr
        ///< [OUT] The Horizontal Speed's accuracy in m/sec.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetHorizontalSpeed;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(hSpeedPtr)) << 0);
    _requiredOutputs |= ((!!(hSpeedAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (hSpeedPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               hSpeedPtr )))
    {
        goto error_unpack;
    }
    if (hSpeedAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               hSpeedAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's vertical speed.
 *
 * @return LE_FAULT         The function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is not valid (set to INT32_MAX).
 * @return LE_OK            The function succeeded.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note vSpeedPtr, vSpeedAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetVerticalSpeed
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* vspeedPtr,
        ///< [OUT] The Vertical Speed in m/sec, positive up.
    int32_t* vspeedAccuracyPtr
        ///< [OUT] The Vertical Speed's accuracy in m/sec.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetVerticalSpeed;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(vspeedPtr)) << 0);
    _requiredOutputs |= ((!!(vspeedAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (vspeedPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               vspeedPtr )))
    {
        goto error_unpack;
    }
    if (vspeedAccuracyPtr &&
        (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize,
                                               vspeedAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's heading. Heading is the direction that the vehicle or person is facing.
 *
 * @return LE_FAULT         Function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 * @return LE_OK            TFunction succeeded.
 *
 * @note Heading is given in degrees.
 *       Heading ranges from 0 to 359 degrees, where 0 is True North.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note headingPtr, headingAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetHeading
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* headingPtr,
        ///< [OUT] Heading in degrees.
        ///< Range: 0 to 359, where 0 is True North.
    uint32_t* headingAccuracyPtr
        ///< [OUT] Heading's accuracy estimate in degrees.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetHeading;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(headingPtr)) << 0);
    _requiredOutputs |= ((!!(headingAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (headingPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               headingPtr )))
    {
        goto error_unpack;
    }
    if (headingAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               headingAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's direction. Direction of movement is the direction that the vehicle or
 * person is actually moving.
 *
 * @return LE_FAULT         Function failed to find the positionSample.
 * @return LE_OUT_OF_RANGE  One of the retrieved parameter is invalid (set to UINT32_MAX).
 * @return LE_OK            Function succeeded.
 *
 * @note Direction is given in degrees.
 *       Direction ranges from 0 to 359 degrees, where 0 is True North.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note directionPtr, directionAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetDirection
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* directionPtr,
        ///< [OUT] Direction indication in degrees.
        ///< Range: 0 to 359, where 0 is True North.
    uint32_t* directionAccuracyPtr
        ///< [OUT] Direction's accuracy estimate in degrees.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetDirection;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(directionPtr)) << 0);
    _requiredOutputs |= ((!!(directionAccuracyPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (directionPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               directionPtr )))
    {
        goto error_unpack;
    }
    if (directionAccuracyPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               directionAccuracyPtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's fix state.
 *
 * @return LE_FAULT         Function failed to get the position sample's fix state.
 * @return LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_sample_GetFixState
(
    le_pos_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    le_pos_FixState_t* statePtr
        ///< [OUT] Position fix state.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_GetFixState;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(statePtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters
    if (statePtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               statePtr )))
    {
        goto error_unpack;
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Release the position sample.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_pos_sample_Release
(
    le_pos_SampleRef_t positionSampleRef
        ///< [IN] Position sample's reference.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_sample_Release;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  positionSampleRef ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);

    return;
}


//--------------------------------------------------------------------------------------------------
/**
 * Set the acquisition rate.
 *
 * @return
 *    LE_OUT_OF_RANGE    Invalid acquisition rate.
 *    LE_OK              The function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_SetAcquisitionRate
(
    uint32_t acquisitionRate
        ///< [IN] Acquisition rate in milliseconds.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_SetAcquisitionRate;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  acquisitionRate ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the acquisition rate.
 *
 * @return
 *    Acquisition rate in milliseconds.
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_pos_GetAcquisitionRate
(
    void
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    uint32_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_GetAcquisitionRate;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


//--------------------------------------------------------------------------------------------------
/**
 * Set the resolution for the positioning distance values.
 *
 * @return LE_OK               Function succeeded.
 * @return LE_BAD_PARAMETER    Invalid parameter provided.
 *
 * @note The positioning distance values are: the altitude above sea level, the horizontal
 *       position accuracy and the vertical position accuracy. The API sets the same resolution to
 *       all distance values. The resolution change request takes effect immediately.
 *
 * @warning The positioning distance values resolutions are platform dependent. Please refer to
 *          @ref platformConstraintsPositioning_SettingResolution section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_pos_SetDistanceResolution
(
    le_pos_Resolution_t resolution
        ///< [IN] Resolution.
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_result_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_pos_SetDistanceResolution;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  resolution ));

    // Send a request to the server and get the response.
    TRACE("Sending message to server and waiting for response : %ti bytes sent",
          _msgBufPtr-_msgPtr->buffer);

    _responseMsgRef = le_msg_RequestSyncResponse(_msgRef);
    // It is a serious error if we don't get a valid response from the server.  Call disconnect
    // handler (if one is defined) to allow cleanup
    if (_responseMsgRef == NULL)
    {
        SessionCloseHandler(GetCurrentSessionRef(), GetClientThreadDataPtr());
    }

    // Process the result and/or output parameters, if there are any.
    _msgPtr = le_msg_GetPayloadPtr(_responseMsgRef);
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Unpack the result first
    if (!le_pack_UnpackResult( &_msgBufPtr, &_msgBufSize, &_result ))
    {
        goto error_unpack;
    }

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


static void ClientIndicationRecvHandler
(
    le_msg_MessageRef_t  msgRef,
    void*                contextPtr
)
{
    // Get the message payload
    _Message_t* msgPtr = le_msg_GetPayloadPtr(msgRef);
    uint8_t* _msgBufPtr = msgPtr->buffer;
    size_t _msgBufSize = _MAX_MSG_SIZE;

    // Have to partially unpack the received message in order to know which thread
    // the queued function should actually go to.
    void* clientContextPtr;
    if (!le_pack_UnpackReference( &_msgBufPtr, &_msgBufSize, &clientContextPtr ))
    {
        LE_FATAL("Failed to unpack message from server.");
        return;
    }

    // The clientContextPtr is a safe reference for the client data object.  If the client data
    // pointer is NULL, this means the handler was removed before the event was reported to the
    // client. This is valid, and the event will be dropped.
    _LOCK
    _ClientData_t* clientDataPtr = le_ref_Lookup(_HandlerRefMap, clientContextPtr);
    _UNLOCK
    if ( clientDataPtr == NULL )
    {
        LE_DEBUG("Ignore reported event after handler removed");
        return;
    }

    // Pull out the callers thread
    le_thread_Ref_t callersThreadRef = clientDataPtr->callersThreadRef;

    // Trigger the appropriate event
    switch (msgPtr->id)
    {
        case _MSGID_le_pos_AddMovementHandler :
            le_event_QueueFunctionToThread(callersThreadRef, _Handle_le_pos_AddMovementHandler, msgRef, clientDataPtr);
            break;

        default:
            LE_FATAL("Unknowm msg id = %i for client thread = %p", msgPtr->id, callersThreadRef);
    }
}