/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#include "le_sim_messages.h"
#include "le_sim_interface.h"


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
    le_sim_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
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
                "le_sim_ConnectService() not called for current thread");

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
    _ClientDataPool = le_mem_CreatePool("le_sim_ClientData", sizeof(_ClientData_t));

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_CreatePool("le_sim_ClientThreadData", sizeof(_ClientThreadData_t));

    // Create the thread-local data key to be used to store a pointer to each thread object.
    LE_ASSERT(pthread_key_create(&_ThreadDataKey, NULL) == 0);

    // Create safe reference map for handler references.
    // The size of the map should be based on the number of handlers defined multiplied by
    // the number of client threads.  Since this number can't be completely determined at
    // build time, just make a reasonable guess.
    _HandlerRefMap = le_ref_CreateMap("le_sim_ClientHandlers", 5);
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
void le_sim_ConnectService
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
le_result_t le_sim_TryConnectService
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

    LE_FATAL("Component for le_sim disconnected\n");
}

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
void le_sim_DisconnectService
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
static void _Handle_le_sim_AddNewStateHandler
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
    le_sim_NewStateHandlerFunc_t _handlerRef_le_sim_AddNewStateHandler = (le_sim_NewStateHandlerFunc_t)_clientDataPtr->handlerPtr;
    void* contextPtr = _clientDataPtr->contextPtr;

    // Unpack the remaining parameters
    le_sim_Id_t simId;
    if (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_States_t simState;
    if (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               &simState ))
    {
        goto error_unpack;
    }

    // Call the registered handler
    if ( _handlerRef_le_sim_AddNewStateHandler != NULL )
    {
        _handlerRef_le_sim_AddNewStateHandler(simId, simState, contextPtr );
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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_sim_NewStateHandlerRef_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddNewStateHandler;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
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
 * Remove handler function for EVENT 'le_sim_NewState'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveNewStateHandler
(
    le_sim_NewStateHandlerRef_t handlerRef
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
    _msgPtr->id = _MSGID_le_sim_RemoveNewStateHandler;
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
    handlerRef = (le_sim_NewStateHandlerRef_t)clientDataPtr->handlerRef;
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


// This function parses the message buffer received from the server, and then calls the user
// registered handler, which is stored in a client data object.
static void _Handle_le_sim_AddSimToolkitEventHandler
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
    le_sim_SimToolkitEventHandlerFunc_t _handlerRef_le_sim_AddSimToolkitEventHandler = (le_sim_SimToolkitEventHandlerFunc_t)_clientDataPtr->handlerPtr;
    void* contextPtr = _clientDataPtr->contextPtr;

    // Unpack the remaining parameters
    le_sim_Id_t simId;
    if (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_StkEvent_t stkEvent;
    if (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               &stkEvent ))
    {
        goto error_unpack;
    }

    // Call the registered handler
    if ( _handlerRef_le_sim_AddSimToolkitEventHandler != NULL )
    {
        _handlerRef_le_sim_AddSimToolkitEventHandler(simId, stkEvent, contextPtr );
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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_sim_SimToolkitEventHandlerRef_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddSimToolkitEventHandler;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
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
 * Remove handler function for EVENT 'le_sim_SimToolkitEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveSimToolkitEventHandler
(
    le_sim_SimToolkitEventHandlerRef_t handlerRef
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
    _msgPtr->id = _MSGID_le_sim_RemoveSimToolkitEventHandler;
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
    handlerRef = (le_sim_SimToolkitEventHandlerRef_t)clientDataPtr->handlerRef;
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
 * Get the current selected card.
 *
 * @return Number of the current selected SIM card.
 */
//--------------------------------------------------------------------------------------------------
le_sim_Id_t le_sim_GetSelectedCard
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

    le_sim_Id_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_GetSelectedCard;
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
    _msgPtr->id = _MSGID_le_sim_SelectCard;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    _msgPtr->id = _MSGID_le_sim_GetICCID;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(iccid)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (iccid)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (iccidSize-1) ));
    }

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
    if (iccid &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               iccid,
                               iccidSize,
                               20 )))
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
    _msgPtr->id = _MSGID_le_sim_GetEID;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(eid)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (eid)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (eidSize-1) ));
    }

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
    if (eid &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               eid,
                               eidSize,
                               32 )))
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
    _msgPtr->id = _MSGID_le_sim_GetIMSI;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(imsi)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (imsi)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (imsiSize-1) ));
    }

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
    if (imsi &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               imsi,
                               imsiSize,
                               15 )))
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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    bool _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_IsPresent;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (!le_pack_UnpackBool( &_msgBufPtr, &_msgBufSize, &_result ))
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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    bool _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_IsReady;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (!le_pack_UnpackBool( &_msgBufPtr, &_msgBufSize, &_result ))
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
    if ( strnlen(pin, 8) > 8 )
    {
        LE_FATAL("strnlen(pin, 8) > 8");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_EnterPIN;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  pin, 8 ));

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
    if ( strnlen(oldpin, 8) > 8 )
    {
        LE_FATAL("strnlen(oldpin, 8) > 8");
    }
    if ( strnlen(newpin, 8) > 8 )
    {
        LE_FATAL("strnlen(newpin, 8) > 8");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_ChangePIN;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  oldpin, 8 ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  newpin, 8 ));

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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    int32_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_GetRemainingPINTries;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (!le_pack_UnpackInt32( &_msgBufPtr, &_msgBufSize, &_result ))
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
    _msgPtr->id = _MSGID_le_sim_GetRemainingPUKTries;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(remainingPukTriesPtrPtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (remainingPukTriesPtrPtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               remainingPukTriesPtrPtr )))
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
    if ( strnlen(pin, 8) > 8 )
    {
        LE_FATAL("strnlen(pin, 8) > 8");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_Unlock;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  pin, 8 ));

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
    if ( strnlen(pin, 8) > 8 )
    {
        LE_FATAL("strnlen(pin, 8) > 8");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_Lock;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  pin, 8 ));

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
    if ( strnlen(puk, 8) > 8 )
    {
        LE_FATAL("strnlen(puk, 8) > 8");
    }
    if ( strnlen(newpin, 8) > 8 )
    {
        LE_FATAL("strnlen(newpin, 8) > 8");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_Unblock;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  puk, 8 ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  newpin, 8 ));

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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_sim_States_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_GetState;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    _msgPtr->id = _MSGID_le_sim_GetSubscriberPhoneNumber;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(phoneNumberStr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (phoneNumberStr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (phoneNumberStrSize-1) ));
    }

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
    if (phoneNumberStr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               phoneNumberStr,
                               phoneNumberStrSize,
                               17 )))
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
    _msgPtr->id = _MSGID_le_sim_GetHomeNetworkOperator;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(nameStr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (nameStr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (nameStrSize-1) ));
    }

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
    if (nameStr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               nameStr,
                               nameStrSize,
                               100 )))
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
    _msgPtr->id = _MSGID_le_sim_GetHomeNetworkMccMnc;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(mccPtr)) << 0);
    _requiredOutputs |= ((!!(mncPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mccPtrSize-1) ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mncPtrSize-1) ));
    }

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
    if (mccPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mccPtr,
                               mccPtrSize,
                               3 )))
    {
        goto error_unpack;
    }
    if (mncPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mncPtr,
                               mncPtrSize,
                               3 )))
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
    _msgPtr->id = _MSGID_le_sim_LocalSwapToEmergencyCallSubscription;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  manufacturer ));

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
    _msgPtr->id = _MSGID_le_sim_LocalSwapToCommercialSubscription;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  manufacturer ));

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
    _msgPtr->id = _MSGID_le_sim_IsEmergencyCallSubscriptionSelected;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(isEcsPtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (isEcsPtr &&
        (!le_pack_UnpackBool( &_msgBufPtr, &_msgBufSize,
                                               isEcsPtr )))
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
    _msgPtr->id = _MSGID_le_sim_AcceptSimToolkitCommand;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    _msgPtr->id = _MSGID_le_sim_RejectSimToolkitCommand;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    _msgPtr->id = _MSGID_le_sim_GetSimToolkitRefreshMode;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(refreshModePtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (refreshModePtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               refreshModePtr )))
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
    _msgPtr->id = _MSGID_le_sim_GetSimToolkitRefreshStage;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(refreshStagePtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
    if (refreshStagePtr &&
        (!le_pack_UnpackUint32( &_msgBufPtr, &_msgBufSize,
                                               refreshStagePtr )))
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
    if ( (NULL == commandApduPtr) &&
         (0 != commandApduSize) )
    {
        LE_FATAL("If commandApduPtr is NULL "
                 "commandApduSize must be zero");
    }
    if ( commandApduSize > 255 )
    {
        LE_FATAL("commandApduSize > 255");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_SendApdu;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(responseApduPtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    bool commandApduResult;
    LE_PACK_PACKARRAY( &_msgBufPtr, &_msgBufSize,
                       commandApduPtr, commandApduSize,
                       255, le_pack_PackUint8,
                       &commandApduResult );
    LE_ASSERT(commandApduResult);
    if (responseApduPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (*responseApduSizePtr) ));
    }

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
    bool responseApduResult;
    if (responseApduPtr)
    {
        LE_PACK_UNPACKARRAY( &_msgBufPtr, &_msgBufSize,
                             responseApduPtr, responseApduSizePtr,
                             256, le_pack_UnpackUint8,
                             &responseApduResult );
        if (!responseApduResult)
        {
        goto error_unpack;
        }
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


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
    if ( strnlen(fileIdentifier, 4) > 4 )
    {
        LE_FATAL("strnlen(fileIdentifier, 4) > 4");
    }
    if ( (NULL == dataPtr) &&
         (0 != dataSize) )
    {
        LE_FATAL("If dataPtr is NULL "
                 "dataSize must be zero");
    }
    if ( dataSize > 100 )
    {
        LE_FATAL("dataSize > 100");
    }
    if ( strnlen(path, 100) > 100 )
    {
        LE_FATAL("strnlen(path, 100) > 100");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_SendCommand;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(sw1Ptr)) << 0);
    _requiredOutputs |= ((!!(sw2Ptr)) << 1);
    _requiredOutputs |= ((!!(responsePtr)) << 2);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  command ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  fileIdentifier, 4 ));
    LE_ASSERT(le_pack_PackUint8( &_msgBufPtr, &_msgBufSize,
                                                  p1 ));
    LE_ASSERT(le_pack_PackUint8( &_msgBufPtr, &_msgBufSize,
                                                  p2 ));
    LE_ASSERT(le_pack_PackUint8( &_msgBufPtr, &_msgBufSize,
                                                  p3 ));
    bool dataResult;
    LE_PACK_PACKARRAY( &_msgBufPtr, &_msgBufSize,
                       dataPtr, dataSize,
                       100, le_pack_PackUint8,
                       &dataResult );
    LE_ASSERT(dataResult);
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  path, 100 ));
    if (responsePtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (*responseSizePtr) ));
    }

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
    if (sw1Ptr &&
        (!le_pack_UnpackUint8( &_msgBufPtr, &_msgBufSize,
                                               sw1Ptr )))
    {
        goto error_unpack;
    }
    if (sw2Ptr &&
        (!le_pack_UnpackUint8( &_msgBufPtr, &_msgBufSize,
                                               sw2Ptr )))
    {
        goto error_unpack;
    }
    bool responseResult;
    if (responsePtr)
    {
        LE_PACK_UNPACKARRAY( &_msgBufPtr, &_msgBufSize,
                             responsePtr, responseSizePtr,
                             256, le_pack_UnpackUint8,
                             &responseResult );
        if (!responseResult)
        {
        goto error_unpack;
        }
    }


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


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
    _msgPtr->id = _MSGID_le_sim_Reset;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_sim_FPLMNListRef_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_CreateFPLMNList;
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
    if (!le_pack_UnpackReference( &_msgBufPtr, &_msgBufSize, &_result ))
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
    if ( strnlen(mcc, 3) > 3 )
    {
        LE_FATAL("strnlen(mcc, 3) > 3");
    }
    if ( strnlen(mnc, 3) > 3 )
    {
        LE_FATAL("strnlen(mnc, 3) > 3");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddFPLMNOperator;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  FPLMNListRef ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  mcc, 3 ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr, &_msgBufSize,
                                  mnc, 3 ));

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
    _msgPtr->id = _MSGID_le_sim_WriteFPLMNList;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  FPLMNListRef ));

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
)
{
    le_msg_MessageRef_t _msgRef;
    le_msg_MessageRef_t _responseMsgRef;
    _Message_t* _msgPtr;

    // Will not be used if no data is sent/received from server.
    __attribute__((unused)) uint8_t* _msgBufPtr;
    __attribute__((unused)) size_t _msgBufSize;

    le_sim_FPLMNListRef_t _result;

    // Range check values, if appropriate


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_ReadFPLMNList;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));

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

    // Unpack any "out" parameters


    // Release the message object, now that all results/output has been copied.
    le_msg_ReleaseMsg(_responseMsgRef);


    return _result;

error_unpack:
    LE_FATAL("Unexpected response from server.");
}


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
    _msgPtr->id = _MSGID_le_sim_GetFirstFPLMNOperator;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(mccPtr)) << 0);
    _requiredOutputs |= ((!!(mncPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  FPLMNListRef ));
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mccPtrSize-1) ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mncPtrSize-1) ));
    }

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
    if (mccPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mccPtr,
                               mccPtrSize,
                               3 )))
    {
        goto error_unpack;
    }
    if (mncPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mncPtr,
                               mncPtrSize,
                               3 )))
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
    _msgPtr->id = _MSGID_le_sim_GetNextFPLMNOperator;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(mccPtr)) << 0);
    _requiredOutputs |= ((!!(mncPtr)) << 1);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  FPLMNListRef ));
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mccPtrSize-1) ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (mncPtrSize-1) ));
    }

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
    if (mccPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mccPtr,
                               mccPtrSize,
                               3 )))
    {
        goto error_unpack;
    }
    if (mncPtr &&
        (!le_pack_UnpackString( &_msgBufPtr, &_msgBufSize,
                               mncPtr,
                               mncPtrSize,
                               3 )))
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
    _msgPtr->id = _MSGID_le_sim_DeleteFPLMNList;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, &_msgBufSize,
                                                  FPLMNListRef ));

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
    _msgPtr->id = _MSGID_le_sim_OpenLogicalChannel;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(channelPtrPtr)) << 0);
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
    if (channelPtrPtr &&
        (!le_pack_UnpackUint8( &_msgBufPtr, &_msgBufSize,
                                               channelPtrPtr )))
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
    _msgPtr->id = _MSGID_le_sim_CloseLogicalChannel;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint8( &_msgBufPtr, &_msgBufSize,
                                                  channel ));

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
    if ( (NULL == commandApduPtr) &&
         (0 != commandApduSize) )
    {
        LE_FATAL("If commandApduPtr is NULL "
                 "commandApduSize must be zero");
    }
    if ( commandApduSize > 255 )
    {
        LE_FATAL("commandApduSize > 255");
    }


    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(GetCurrentSessionRef());
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_SendApduOnChannel;
    _msgBufPtr = _msgPtr->buffer;
    _msgBufSize = _MAX_MSG_SIZE;

    // Pack a list of outputs requested by the client.
    uint32_t _requiredOutputs = 0;
    _requiredOutputs |= ((!!(responseApduPtr)) << 0);
    LE_ASSERT(le_pack_PackUint32(&_msgBufPtr, &_msgBufSize, _requiredOutputs));

    // Pack the input parameters
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, &_msgBufSize,
                                                  simId ));
    LE_ASSERT(le_pack_PackUint8( &_msgBufPtr, &_msgBufSize,
                                                  channel ));
    bool commandApduResult;
    LE_PACK_PACKARRAY( &_msgBufPtr, &_msgBufSize,
                       commandApduPtr, commandApduSize,
                       255, le_pack_PackUint8,
                       &commandApduResult );
    LE_ASSERT(commandApduResult);
    if (responseApduPtr)
    {
        LE_ASSERT(le_pack_PackSize( &_msgBufPtr, &_msgBufSize, (*responseApduSizePtr) ));
    }

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
    bool responseApduResult;
    if (responseApduPtr)
    {
        LE_PACK_UNPACKARRAY( &_msgBufPtr, &_msgBufSize,
                             responseApduPtr, responseApduSizePtr,
                             256, le_pack_UnpackUint8,
                             &responseApduResult );
        if (!responseApduResult)
        {
        goto error_unpack;
        }
    }


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
        case _MSGID_le_sim_AddNewStateHandler :
            le_event_QueueFunctionToThread(callersThreadRef, _Handle_le_sim_AddNewStateHandler, msgRef, clientDataPtr);
            break;
        case _MSGID_le_sim_AddSimToolkitEventHandler :
            le_event_QueueFunctionToThread(callersThreadRef, _Handle_le_sim_AddSimToolkitEventHandler, msgRef, clientDataPtr);
            break;

        default:
            LE_FATAL("Unknowm msg id = %i for client thread = %p", msgPtr->id, callersThreadRef);
    }
}