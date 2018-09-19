#include "legato.h"
#include "interfaces.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "aws_iot_config_defn.h"
#include "test_versAwsIot.h"

#define MAX_SIZE 								100
#define HOST_ADDRESS_SIZE 						255
#define EVENT_NAME_SUBSCRIBE 					"aws_subs_event"
#define THREAD_NAME_SUBSCRIBE					"aws_subs_thread"
#define WAIT_FOR_MQTT_CONNECT_SEC 				60
#define WAIT_FOR_DATA_CONNECTION_RELEASE_SEC 	5

static int _numSubcribes;

static le_mutex_Ref_t _awsMqttMutex;
#define LOCK()      le_mutex_Lock(_awsMqttMutex)
#define UNLOCK()    le_mutex_Unlock(_awsMqttMutex)

static le_mutex_Ref_t _subUnsubMutex;

static AWS_IoT_Client _client;

IoT_Client_Connect_Params _connectParams;
typedef struct {
	char rootCA[PATH_MAX + 1];
	char clientCrt[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char currentWd[PATH_MAX + 1];
	char certDirectory[PATH_MAX + 1];
} TLSParams;

typedef struct {
	char host[HOST_ADDRESS_SIZE];
	int32_t port;
} ConnectionParams;

static le_event_Id_t _subEventId;
static le_thread_Ref_t _awsPollThread;

static TLSParams _tlsParams;
static ConnectionParams _connectionParams;

static bool _forceStopAWSPoll = false;
/**
 * Is always used within LOCKed scopes. The
 * only UNLOCKed scope where this is modified
 * is when aws_Hibernate() is called.
 */
static bool _isInHibernation = false;
static bool _preferredCellularData = true;
static le_data_RequestObjRef_t _lastDataRequest = NULL;

/**
 * Initialize TLS connection parameters.
 */
void _initTlsParams
(
		const char* rootCa, 	///< [IN]  Root CA certificate file name
		const char* privateKey, ///< [IN]  Pvt key file name
		const char* cert		///< [IN]  Client certificate file name
		) {

	if ((strlen(rootCa) < 0 || rootCa == NULL)
			|| (strlen(cert) < 0 || cert == NULL)
			|| (strlen(cert) < 0 || cert == NULL)) {
		LE_WARN(
				"One or more AWS_IOT client configuration file names(/Root CA certificate name/ Client certificate file/ PVT key file) are not supplied");
	}
	strcpy(_tlsParams.rootCA, rootCa);
	strcpy(_tlsParams.clientCrt, cert);
	strcpy(_tlsParams.clientKey, privateKey);
}
/**
 * Utility function to provide meaningful names against
 * relevant return codes from various AWS APIs (defined
 * in aws_iot_err.h).
 *
 * @return string
 */
const char* _getErrorTypeFromCode(int errorCode	///< [IN]  Error code defined at aws_iot_error.h
		) {
	switch (errorCode) {
	case 6:
		return "NETWORK_PHYSICAL_LAYER_CONNECTED";
	case 5:
		return "NETWORK_MANUALLY_DISCONNECTED";
	case 4:
		return "NETWORK_ATTEMPTING_RECONNECT";
	case 3:
		return "NETWORK_RECONNECTED";
	case 2:
		return "MQTT_NOTHING_TO_READ";
	case 1:
		return "MQTT_CONNACK_CONNECTION_ACCEPTED";
	case 0:
		return "SUCCESS";
	case -1:
		return "FAILURE";
	case -2:
		return "NULL_VALUE_ERROR";
	case -4:
		return "SSL_CONNECTION_ERROR";
	case -5:
		return "TCP_SETUP_ERROR";
	case -6:
		return "NETWORK_SSL_CONNECT_TIMEOUT_ERROR";
	case -7:
		return "NETWORK_SSL_WRITE_ERROR";
	case -8:
		return "NETWORK_SSL_INIT_ERROR";
	case -9:
		return "NETWORK_SSL_CERT_ERROR";
	case -10:
		return "NETWORK_SSL_WRITE_TIMEOUT_ERROR";
	case -11:
		return "NETWORK_SSL_READ_TIMEOUT_ERROR";
	case -12:
		return "NETWORK_SSL_READ_ERROR";
	case -13:
		return "NETWORK_DISCONNECTED_ERROR";
	case -14:
		return "NETWORK_RECONNECT_TIMED_OUT_ERROR";
	case -15:
		return "NETWORK_ALREADY_CONNECTED_ERROR";
	case -16:
		return "NETWORK_MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED";
	case -17:
		return "NETWORK_SSL_UNKNOWN_ERROR";
	case -18:
		return "NETWORK_PHYSICAL_LAYER_DISCONNECTED";
	case -19:
		return "NETWORK_X509_ROOT_CRT_PARSE_ERROR";
	case -20:
		return "NETWORK_X509_DEVICE_CRT_PARSE_ERROR";
	case -21:
		return "NETWORK_PK_PRIVATE_KEY_PARSE_ERROR";
	case -22:
		return "NETWORK_ERR_NET_SOCKET_FAILED";
	case -23:
		return "NETWORK_ERR_NET_UNKNOWN_HOST";
	case -24:
		return "NETWORK_ERR_NET_CONNECT_FAILED";
	case -25:
		return "NETWORK_SSL_NOTHING_TO_READ";
	case -26:
		return "MQTT_CONNECTION_ERROR";
	default:
		return "Invalid choice";
	}
}

/**
 * Initialize 'ConnectionParams values by 'host name' and 'port number'
 * i.e. required to initialize MQTT broker in AWS_IOT service
 *
 * @return 	None
 * */
void _initConnectionParams(const char* hostName,	///< [IN]  MQTT host name
		const int32_t port		///< [IN]  MQTT service listening port
		) {

	if (strlen(hostName) == 0 || port <= 0) {
		LE_WARN(
				"Invalid 'Host name'(%s) and/or 'Port Number'(%d) is supplied in this context",
				hostName, port);
	}
	strcpy(_connectionParams.host, hostName);
	_connectionParams.port = port;
}

/**
 * Initialize connection to MQTT broker
 * @return None on success, halts execution in case of failure
 */
IoT_Error_t _initConnection() {

	LE_DEBUG("=============================================\n");
	LE_DEBUG("Root CA 		= %s\n", _tlsParams.rootCA);
	LE_DEBUG("Client CRT 	= %s\n", _tlsParams.clientCrt);
	LE_DEBUG("Client Key 	= %s\n", _tlsParams.clientKey);
	LE_DEBUG("=============================================\n");

	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;

	LE_DEBUG("\nAWS IoT SDK Version");
	LE_DEBUG("\n=============================================\n");
	LE_DEBUG("\nVERSION_MAJOR = %d,", VERSION_MAJOR);
	LE_DEBUG("\nVERSION_MINOR = %d", VERSION_MINOR);
	LE_DEBUG("\nVERSION_PATCH = %d", VERSION_PATCH);
	LE_DEBUG("\nVERSION_TAG = %s\n", VERSION_TAG);

	/* Configure init parameters*/

	mqttInitParams.enableAutoReconnect = false; // We enable this later below
	mqttInitParams.pHostURL = _connectionParams.host;
	mqttInitParams.port = _connectionParams.port;
	// If TLS parameters not defined. Try to connect the MQTT host without TLS encryption.
	//TODO:Test Pending
	if (_tlsParams.clientCrt != NULL || strlen(_tlsParams.clientCrt) != 0) {
		mqttInitParams.pRootCALocation = _tlsParams.rootCA;
		mqttInitParams.pDeviceCertLocation = _tlsParams.clientCrt;
		mqttInitParams.pDevicePrivateKeyLocation = _tlsParams.clientKey;
	}
	mqttInitParams.isSSLHostnameVerify = (_tlsParams.clientCrt != NULL
			|| strlen(_tlsParams.clientCrt) != 0);
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.disconnectHandlerData = NULL;

	return aws_iot_mqtt_init(&_client, &mqttInitParams);
}

/**
 * Attempts to connect to the MQTT broker - infinitely
 * till the connection is established or a hibernation
 * has been requested.
 *
 * Checking _isInHibernation after this function returns
 * will indicate whether a Hibernate() was called while
 * establishing the connection or not - it will be false
 * if Hibernate() was called.
 *
 * NOTE 1: This private function is called from within
 * LOCKed scopes, so in that sense, it is guaranteed
 * to be running only once at any instance of time.
 *
 * @return true, if the connection was established.
 */
bool _connect() {
	bool isConnected = false;
	_isInHibernation = false;
	time_t sTime, eTime;
	LE_DEBUG("Connecting....");
	while (!_isInHibernation && !isConnected) {
		le_data_ConnectService();
		if (_preferredCellularData
				&& le_data_GetTechnology() != LE_DATA_CELLULAR
				&& le_data_SetTechnologyRank(1, LE_DATA_CELLULAR) != LE_OK) {
			continue;
		}

		// Release data connection in case any connection interrupt occurs
		if ( NULL != _lastDataRequest) {
			LE_DEBUG("Releasing existing data connection before requesting..");
			le_data_Release(_lastDataRequest);
			sleep(WAIT_FOR_DATA_CONNECTION_RELEASE_SEC);
		}

		// Let the le_data service set the connection. Meanwhile try setting
		// up connection with MQTT host until connection is established successfully
		// or time limit is reached
		_lastDataRequest = le_data_Request();

		time(&sTime);
		time(&eTime);
		while (difftime(eTime, sTime) <= WAIT_FOR_MQTT_CONNECT_SEC
				&& !isConnected) {

			isConnected = aws_iot_mqtt_connect(&_client, &_connectParams)
					== SUCCESS;
			time(&eTime);
		}
	}
	if (isConnected) {
		LE_DEBUG("Connected successfully! ");
	} else {
		LE_DEBUG("Connection interrupted.");
	}
	return isConnected;
}

/**
 * Publish the MQTT payload against the topic name.
 * If the service is in hibernation, it is re-connected automatically.
 *
 * @return SUCCESS (0) if the publish succeeds, else an AWS IoT Error Type
 * defining the failure reason.
 */
int32_t aws_Publish(const char* topic,	///< [IN]  The MQTT Topic name
		int32_t topicLen,				///< [IN]  Length of the MQTT Topic name
		int32_t qosType,				///< [IN]  MQTT QoS Type
		const char* payload,			///< [IN]  The MQTT payload
		int32_t payloadLen				///< [IN]  Length of the MQTT payload
		) {

	LOCK();

	IoT_Publish_Message_Params paramsQOS1;
	paramsQOS1.qos = qosType;
	paramsQOS1.payload = (void*) payload;
	paramsQOS1.isRetained = 0;
	paramsQOS1.payloadLen = payloadLen;

	int rc = aws_iot_mqtt_publish(&_client, topic, topicLen, &paramsQOS1);

	switch (rc) {
	case SUCCESS:
		if (_isInHibernation) {
			LE_WARN(
					"DEV ERROR! _isInHibernation should be false - probably set to true outside a LOCK()/UNLOCK() scope!");
			_isInHibernation = false;
		}
		break;
	case NETWORK_DISCONNECTED_ERROR:
	case MQTT_REQUEST_TIMEOUT_ERROR:
		_connect();
		break;
	}
	if (!_isInHibernation && rc != SUCCESS) {
		rc = aws_iot_mqtt_publish(&_client, topic, topicLen, &paramsQOS1);
	}

	UNLOCK();
	return rc;
}

/**
 * AWS MQTT Subscribe callback. Relays any received message as an event.
 */
void _subscribeCallback(AWS_IoT_Client *pClient, char *topicName,
		uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	LE_DEBUG(
			"Received new MQTT message: Topic[%.*s], Payload[%.*s]\nWill relay as event...",
			topicNameLen, topicName, (int ) params->payloadLen,
			(char * ) params->payload);

	// Dispatch the event
	le_event_Report(_subEventId, params->payload, strlen(params->payload));
}

/**
 * Subscribe event handler
 */
static void _onSubscribeEventReceive(void* reportPtr, ///< [in] Reporter pointer value.
		void* secondLayerHandlerFunc	///< [in] Handler reference.
		) {

	aws_SubscribeEventHandlerFunc_t clientHandlerFuncRef =
			secondLayerHandlerFunc;
	clientHandlerFuncRef(reportPtr, le_event_GetContextPtr());
}

/**
 * Add Subscribe event handler
 *
 * @return A reference to the handler
 */
aws_SubscribeEventHandlerRef_t aws_AddSubscribeEventHandler(
		aws_SubscribeEventHandlerFunc_t clientHandlerFunc, ///< [in] Handler reference.
		void* contextPtr ///< [in] Context pointer value.
		) {
	le_event_HandlerRef_t handlerRef = le_event_AddLayeredHandler(
			"SubscribeHandler", _subEventId, _onSubscribeEventReceive,
			(le_event_HandlerFunc_t) clientHandlerFunc);
	if (handlerRef != NULL) {
		LE_DEBUG("Successfully added subscribe layered handler.");
		le_event_SetContextPtr(handlerRef, contextPtr);
	} else {
		LE_ERROR("Failed to add subscribe handler!");
	}
	return (aws_SubscribeEventHandlerRef_t) (handlerRef);
}

/**
 * Remove subscribe event handler.
 */
void aws_RemoveSubscribeEventHandler(aws_SubscribeEventHandlerRef_t handlerRef ///< [in] Handler reference.
		) {
	le_event_RemoveHandler((le_event_HandlerRef_t) handlerRef);
}

/**
 * Runs as a separate thread to continuously ping and poll AWS MQTT for subscribed messages.
 * Does not exit in case of an MQTT failure, and keeps on checking as long as at least one
 * subscriber exists.
 */
static void* _awsPoll(void* timeout	///< [IN]  Maximum timeout (ms) for AWS Yield
		) {

	IoT_Error_t rc = SUCCESS;
	LE_DEBUG("Starting AWS Polling...");
	while (!_forceStopAWSPoll && _numSubcribes > 0) {
		LOCK();
		rc = aws_iot_mqtt_yield(&_client, (int) timeout);
		UNLOCK();

		if (rc != SUCCESS) {
			LE_ERROR("Failed to ping/poll AWS MQTT! Error (%d) %s ", rc,
					_getErrorTypeFromCode(rc));
		}
	}
	_forceStopAWSPoll = false;
	LE_DEBUG("Stopped AWS Polling!");
	return NULL;
}

/**
 * Terminates the AWS polling thread.
 */
void _stopAwsPoll() {

	if (_awsPollThread != NULL ) {
		// wait for the poll thread to exit
		le_thread_Join(_awsPollThread, (void*) NULL);
		LE_DEBUG("AWS polling thread is joined and terminated.");
	}
}

/**
 * Disconnects the service from the MQTT broker, and the network
 * (internet) as well, till a Publish() is requested.
 * WARNING: All topic subscriptions will stop working as long as
 * this service is in hibernation. (TODO: may change in future.)
 */
void aws_Hibernate() {

	// TODO: shall we allow when there are still subscribers? Probably yes.
	/*if (_numSubcribes) {
	 LE_DEBUG("Cannot hibernate as long as there are subscribers!");
	 return false;
	 }*/

	LE_DEBUG("Hibernating...");
	_isInHibernation = true;

	LOCK();
	// disconnect MQTT, then le_data
	IoT_Error_t rc = aws_iot_mqtt_disconnect(&_client);
	if (SUCCESS != rc) {
		LE_WARN("Ignoring MQTT disconnect error (%d): %s", rc,
				_getErrorTypeFromCode(rc));
	}

	// Force stop the polling, irrespective of whether there
	// are any subscribers or not
	_forceStopAWSPoll = true;
	_stopAwsPoll();

	// TODO Periodically (1 min) _connect(), check once if there are
	// any pending Subscribe messages, then go back to sleep.
	// TODO provided that works! else ..?? prevent hibernation??

	if ( NULL != _lastDataRequest) {
		LE_DEBUG("Releasing data connection ...");
		le_data_Release(_lastDataRequest);
		LE_DEBUG("Data connection is released now");
	}
	UNLOCK();
}

/**
 * Subscribe to an MQTT topic.
 *
 * If the service is in hibernation, it is temporarily reconnected to
 * perform the MQTT Subscribe, and then sent back to hibernation.
 *
 * @return SUCCESS (0) if the subscribe succeeds, else an AWS IoT Error Type
 * defining the failure reason.
 */
int aws_Subscribe(const char* sTopic,	///< [IN]  The MQTT Topic name
		int32_t topicLen,				///< [IN]  Length of the MQTT Topic name
		int32_t qosType					///< [IN]  MQTT QoS Type
		) {
	LOCK();

	bool wasInHibernation = _isInHibernation;
	int rc = aws_iot_mqtt_subscribe(&_client, sTopic, topicLen, qosType,
			_subscribeCallback, NULL);

	switch (rc) {
	case NETWORK_DISCONNECTED_ERROR:
	case MQTT_REQUEST_TIMEOUT_ERROR:
		_connect();
		if (!_isInHibernation) {
			rc = aws_iot_mqtt_subscribe(&_client, sTopic, topicLen, qosType,
					_subscribeCallback, NULL);
		}
		break;
	}

	if (SUCCESS != rc) {
		LE_ERROR("Error subscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, _getErrorTypeFromCode(rc));
	} else {

		// Mutex to avoid subscribe-unsubscribe race condition
		le_mutex_Lock(_subUnsubMutex);

		// Create AWS Polling thread for the first subscription
		if (++_numSubcribes == 1) {
			_awsPollThread = le_thread_Create(THREAD_NAME_SUBSCRIBE, _awsPoll,
					(void*) YIELD_TIMEOUT);
			le_thread_SetJoinable(_awsPollThread);
			le_thread_Start(_awsPollThread);
		}

		le_mutex_Unlock(_subUnsubMutex);
	}

	UNLOCK();

	// Resume hibernation, if needed
	if (wasInHibernation && !_isInHibernation) {
		aws_Hibernate();
	}
	return rc;
}

/**
 * Unsubscribe from an MQTT topic.
 *
 * If the service is in hibernation, it is temporarily reconnected to
 * perform the MQTT Subscribe, and then sent back to hibernation.
 *
 * @return SUCCESS (0) if the subscribe succeeds, else an AWS IoT Error Type
 * defining the failure reason.
 */
int aws_Unsubscribe(const char* sTopic, ///< [IN]  The MQTT Topic name
		int32_t topicLen	///< [IN]  Topic Length
		) {
	LE_DEBUG("Unsubscribing from topic : %s.", sTopic);

	LOCK();

	bool wasInHibernation = _isInHibernation;
	IoT_Error_t rc = aws_iot_mqtt_unsubscribe(&_client, sTopic, topicLen);

	switch (rc) {
	case NETWORK_DISCONNECTED_ERROR:
	case MQTT_REQUEST_TIMEOUT_ERROR:
		_connect();
		if (!_isInHibernation) {
			rc = aws_iot_mqtt_unsubscribe(&_client, sTopic, topicLen);
		}
		break;
	default:
		break;
	}

	UNLOCK();

	// Resume hibernation, if needed
	if (wasInHibernation && !_isInHibernation) {
		aws_Hibernate();
	}

	// Mutex to avoid subscribe unsubscribe race condition
	le_mutex_Lock(_subUnsubMutex);
	if (SUCCESS != rc) {
		LE_ERROR("Error Unsubscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, _getErrorTypeFromCode(rc));
		LE_DEBUG("Total no of subscribers = %d ", _numSubcribes);
	} else if (--_numSubcribes <= 0) {
		_numSubcribes = 0;
		_stopAwsPoll();
	}
	le_mutex_Unlock(_subUnsubMutex);

	return rc;
}

/**
 * Whether the service is in hibernation or not.
 *
 * @return Whether the service is in hibernation or not.
 */
bool aws_IsInHibernation() {
	return _isInHibernation;
}

COMPONENT_INIT {

	char aws_host_name[MAX_SIZE];
	char aws_client_id[MAX_SIZE];
	char aws_thing_name[MAX_SIZE];
	char aws_root_CA_filename[MAX_SIZE];
	char aws_certificate_filename[MAX_SIZE];
	char aws_private_key_filename[MAX_SIZE];
	int aws_port_number = 0;
	int isMandatoryConfigMissing = 0;

	_awsMqttMutex = le_mutex_CreateNonRecursive("yieldMutex");
	_subUnsubMutex = le_mutex_CreateNonRecursive("subMutex");
	_numSubcribes = 0;

	LOCK();

	if (!config_GetString(CONFIG_SECTION, CONFIG_KEY_HOST_NAME, "",
			aws_host_name,
			MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_HOST_NAME, CONFIG_SECTION, "Configuration file");
		isMandatoryConfigMissing = 1;
	}
	if (!config_GetString(CONFIG_SECTION, CONFIG_KEY_CLIENT_ID, "",
			aws_client_id,
			MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_CLIENT_ID, CONFIG_SECTION, "Configuration file");
		isMandatoryConfigMissing = 1;
	}
	if (!config_GetString(CONFIG_SECTION, CONFIG_KEY_THING_NAME, "",
			aws_thing_name,
			MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_THING_NAME, CONFIG_SECTION, "Configuration file");
		isMandatoryConfigMissing = 1;
	}
	if (!config_GetString(CONFIG_SECTION, CONFIG_KEY_ROOT_CA_FILE_NAME, "",
			aws_root_CA_filename, MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_ROOT_CA_FILE_NAME, CONFIG_SECTION,
				"Configuration file");
		isMandatoryConfigMissing = 1;
	}
	if (!config_GetString(CONFIG_SECTION,
	CONFIG_KEY_CERTIFICATE_FILE_NAME, "", aws_certificate_filename, MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_CERTIFICATE_FILE_NAME, CONFIG_SECTION,
				"Configuration file");
		isMandatoryConfigMissing = 1;
	}
	if (!config_GetString(CONFIG_SECTION,
	CONFIG_KEY_PRIVATE_KEY_FILE_NAME, "", aws_private_key_filename, MAX_SIZE)) {
		LE_ERROR("Missing mandatory configuration [%s]%s in '%s'",
				CONFIG_KEY_PRIVATE_KEY_FILE_NAME, CONFIG_SECTION,
				"Configuration file");
		isMandatoryConfigMissing = 1;
	}

	if (!config_GetInteger(CONFIG_SECTION, CONFIG_KEY_PORT_NUMBER,
	CONFIG_DEFAULT_PORT_NUMBER, &aws_port_number)) {
		// default to 443
		LE_DEBUG("Using default value %d for '%s' key",
				CONFIG_DEFAULT_PORT_NUMBER, CONFIG_KEY_PORT_NUMBER);
	}

	if (!config_GetBoolean(CONFIG_SECTION, CONFIG_KEY_CELLULAR_PREFERRED,
	CONFIG_DEFAULT_CELLULAR_PREFERRED, &_preferredCellularData)) {
		LE_DEBUG("Using default value %d for '%s' key",
				CONFIG_DEFAULT_CELLULAR_PREFERRED,
				CONFIG_KEY_CELLULAR_PREFERRED);
	}

	if (isMandatoryConfigMissing) {
		UNLOCK();
		LE_FATAL("Failed to load configuration for AWS_IOT");
	}
	LE_DEBUG("================== AWS_IOT PARAMS =================");
	LE_DEBUG(" HOST NAME = %s", aws_host_name);
	LE_DEBUG(" CLIENT ID = %s", aws_client_id);
	LE_DEBUG(" THING NAME = %s", aws_thing_name);
	LE_DEBUG(" CERT FILE = %s", aws_certificate_filename);
	LE_DEBUG(" PVT KEY FILE = %s", aws_private_key_filename);
	LE_DEBUG(" PORT NUMBER = %d", aws_port_number);
	LE_DEBUG("==============================================================");
	/*============================================================*/
	/* Initialize remote MQTT Connection*/
	/*============================================================*/
	// Initialize connection parameters.
	_initConnectionParams(aws_host_name, aws_port_number);
	// Initialize TLS default parameters. Skip this step if TLS encryption not required
	_initTlsParams(aws_root_CA_filename, aws_private_key_filename,
			aws_certificate_filename);
	// Initialize connection to the MQTT broker
	int rc = _initConnection();
	if (SUCCESS != rc) {
		UNLOCK();
		LE_FATAL("aws_iot_mqtt_init returned error : %d ", rc);
	}
	LE_DEBUG("\nSUCCESS: AWS_IOT has been initialized.");

	// reusable connect parameters
	_connectParams = iotClientConnectParamsDefault;
	_connectParams.keepAliveIntervalInSec = 2000;
	_connectParams.isCleanSession = true;
	_connectParams.MQTTVersion = MQTT_3_1_1;
	_connectParams.pClientID = aws_client_id;
	_connectParams.clientIDLen = (uint16_t) strlen(aws_client_id);
	_connectParams.isWillMsgPresent = false;

	// Event ID to dispatch MQTT Subscribe events
	_subEventId = le_event_CreateId(EVENT_NAME_SUBSCRIBE, 512); // Expected payload size: <512 bytes

	UNLOCK();

	// Start unit test of AWS_IOT functions
	IOT_TEST();
}
