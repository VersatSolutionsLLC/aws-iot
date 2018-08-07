#include "legato.h"
#include "interfaces.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "test_AwsIot.h"

#define HOST_ADDRESS_SIZE 255
#define AWS_IOT_SUBSCRIBE_THREAD_NAME "aws_subs_thread"

void _stopYieldThread();
static int _numSubcribes;

static bool unsubscribe = false;

static le_thread_Ref_t yieldThread;
//static le_log_TraceRef_t TraceRef;

// Mutex used to prevent races between the threads.
static le_mutex_Ref_t Mutex;
static le_mutex_Ref_t MutexSubscribe;
#define LOCK()      le_mutex_Lock(Mutex)
#define UNLOCK()    le_mutex_Unlock(Mutex)

static AWS_IoT_Client client;
void _stopYieldThread();
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

TLSParams tlsParams;
ConnectionParams connectionParams;

/**
 * Initialize TLS connection parameters. If called with empty parameters, it uses the values defined in the
 * configure file.
 */
void _initTlsParams(const char* certPath, const char* rootCa,
		const char* privateKey, const char* cert) {

	if (strlen(certPath) < 0 || certPath == NULL) {

		LE_DEBUG("Certificate path is not supplied. Using default ('%s')",
				AWS_IOT_CERT_PATH);
		snprintf(tlsParams.certDirectory, PATH_MAX + 1, "%s",
		AWS_IOT_CERT_PATH);
	} else {
		strcpy(tlsParams.certDirectory, certPath);
	}

	// Root CA certificate name update
	if (strlen(rootCa) < 0 || rootCa == NULL) {
		LE_DEBUG(
				"Root CA certificate name is not supplied. Using default ('%s')",
				AWS_IOT_ROOT_CA_FILENAME);
		snprintf(tlsParams.rootCA, PATH_MAX + 1, "%s/%s",
				tlsParams.certDirectory, AWS_IOT_ROOT_CA_FILENAME);

	} else {
		strcpy(tlsParams.rootCA, rootCa);
	}

	// AWS_IOT certificate file name update
	if (strlen(cert) < 0 || cert == NULL) {
		LE_DEBUG(
				"AWS_IOT Client certificate file name is not supplied. Using default ('%s')",
				AWS_IOT_CERTIFICATE_FILENAME);
		snprintf(tlsParams.clientCrt, PATH_MAX + 1, "%s/%s",
				tlsParams.certDirectory,
				AWS_IOT_CERTIFICATE_FILENAME);

	} else {
		strcpy(tlsParams.clientCrt, cert);
	}

	// AWS_IOT Pvt Key file name update
	if (strlen(privateKey) < 0 || privateKey == NULL) {
		LE_DEBUG(
				"AWS_IOT pvt key file name is not supplied. Using default ('%s')",
				AWS_IOT_PRIVATE_KEY_FILENAME);
		snprintf(tlsParams.clientKey, PATH_MAX + 1, "%s/%s",
				tlsParams.certDirectory, AWS_IOT_PRIVATE_KEY_FILENAME);

	} else {
		strcpy(tlsParams.clientKey, privateKey);

	}

}

/**================================================================================
 * Task  			: Return string from enumerated data type value
 * Objective		: Get error type corresponding to a specific error code
 * Return 			: string
 * ================================================================================
 */
const char* _getErrorTypeFromCode(int errorCode) {
	switch (errorCode) {
	case 6: {
		return "NETWORK_PHYSICAL_LAYER_CONNECTED";
	}
	case 5: {
		return "NETWORK_MANUALLY_DISCONNECTED";
	}
	case 4: {
		return "NETWORK_ATTEMPTING_RECONNECT";
	}
	case 3: {
		return "NETWORK_RECONNECTED";
	}
	case 2: {
		return "MQTT_NOTHING_TO_READ";
	}
	case 1: {
		return "MQTT_CONNACK_CONNECTION_ACCEPTED";
	}
	case 0: {
		return "SUCCESS";
	}
	case -1: {
		return "FAILURE";
	}
	case -2: {
		return "NULL_VALUE_ERROR";
	}
	case -4: {
		return "SSL_CONNECTION_ERROR";
	}
	case -5: {
		return "TCP_SETUP_ERROR";
	}
	case -6: {
		return "NETWORK_SSL_CONNECT_TIMEOUT_ERROR";
	}
	case -7: {
		return "NETWORK_SSL_WRITE_ERROR";
	}
	case -8: {
		return "NETWORK_SSL_INIT_ERROR";
	}
	case -9: {
		return "NETWORK_SSL_CERT_ERROR";
	}
	case -10: {
		return "NETWORK_SSL_WRITE_TIMEOUT_ERROR";
	}
	case -11: {
		return "NETWORK_SSL_READ_TIMEOUT_ERROR";
	}
	case -12: {
		return "NETWORK_SSL_READ_ERROR";
	}
	case -13: {
		return "NETWORK_DISCONNECTED_ERROR";
	}
	case -14: {
		return "NETWORK_RECONNECT_TIMED_OUT_ERROR";
	}
	case -15: {
		return "NETWORK_ALREADY_CONNECTED_ERROR";
	}
	case -16: {
		return "NETWORK_MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED";
	}
	case -17: {
		return "NETWORK_SSL_UNKNOWN_ERROR";
	}
	case -18: {
		return "NETWORK_PHYSICAL_LAYER_DISCONNECTED";
	}
	case -19: {
		return "NETWORK_X509_ROOT_CRT_PARSE_ERROR";
	}
	case -20: {
		return "NETWORK_X509_DEVICE_CRT_PARSE_ERROR";
	}
	case -21: {
		return "NETWORK_PK_PRIVATE_KEY_PARSE_ERROR";
	}
	case -22: {
		return "NETWORK_ERR_NET_SOCKET_FAILED";
	}
	case -23: {
		return "NETWORK_ERR_NET_UNKNOWN_HOST";
	}
	case -24: {
		return "NETWORK_ERR_NET_CONNECT_FAILED";
	}
	case -25: {
		return "NETWORK_SSL_NOTHING_TO_READ";
	}
	case -26: {
		return "MQTT_CONNECTION_ERROR";
	}
	default: {
		return "Invalid choice";
	}
	}
}

/**================================================================================
 * Task  			: Return client's connection state
 * Objective		: Get client's state to verify current connection status
 * Return 			: enumerated data
 * ================================================================================
 */
ClientState _getConnectionState() {
	ClientState clientState;
	LOCK();
	clientState = aws_iot_mqtt_get_client_state(&client);
	//LE_DEBUG("\nIn aws_getConnState: Client State = %s \(%d\)",ClientState,clientState);
	UNLOCK();
	FUNC_EXIT_RC(clientState);
}
/**================================================================================
 * Task  			: Disconnect client from MQTT broker
 * Objective		: Invokes internal API aws_iot_mqtt_disconnect to initiate
 * 					  disconnection from MQTT broker
 * Return 			: 0 on success .Numeric error code in case of failure
 * ================================================================================
 */
int aws_disconnect() {
	IoT_Error_t rc = FAILURE;
	LOCK();
	rc = aws_iot_mqtt_disconnect(&client);
	UNLOCK();
	if (SUCCESS != rc) {
		LE_ERROR("Unable to disconnect to MQTT host");
		LE_ERROR("Error Type(%d) : %s\n", rc, _getErrorTypeFromCode(rc));
	} else {
		LE_DEBUG("\nClient is now disconnected from remote MQTT host");
	}
	return rc;
}

/**================================================================================
 * Task  			: Invoked as callback handler
 * Objective		:
 * Return 			: None
 * ================================================================================
 */
void _disconnectCallback(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if (NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	LOCK();
	if (aws_iot_is_autoreconnect_enabled(pClient)) {
		LE_DEBUG(
				"Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if (NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
			_stopYieldThread();
		}
	}
	UNLOCK();
}

/**================================================================================
 * Task  			: Set configuration parameter's values, if supplied from
 * 					  the input parameters otherwise use config file values
 * Objective		: Make provision to get configuration from user side
 * Return 			: None
 * ================================================================================
 */
void _initConnectionParams(const char* hostName, const int32_t port) {

	if (strlen(hostName) < 0 || hostName == NULL) {
		LE_DEBUG("Host name not supplied. Using ('%s')", AWS_IOT_MQTT_HOST);
		snprintf(connectionParams.host, PATH_MAX + 1, "%s", AWS_IOT_MQTT_HOST);

	} else {
		strcpy(connectionParams.host, hostName);
	}
	//Port number update
	if (port <= 0) {
		LE_DEBUG("Port number is not supplied, using the default one: '%d'\n",
				AWS_IOT_MQTT_PORT);
		connectionParams.port = AWS_IOT_MQTT_PORT;
	} else {
		connectionParams.port = port;
	}
}

/**================================================================================
 * Task  			: Initialize connection to MQTT broker
 *
 * Objective		: Invokes internal API 'aws_iot_mqtt_init' to initiate
 * 				      client connection to MQTT broker
 * Return 			: None on success, halts execution in case of failure
 * ================================================================================
 */
void _initConnection() {

	ClientState state = _getConnectionState();

	LOCK();
	if (state == CLIENT_STATE_INITIALIZED) {
			UNLOCK();
			return;
	}
	// if already Inited, return.

	LE_DEBUG("=============================================\n");
	LE_DEBUG("Root CA 		= %s\n", tlsParams.rootCA);
	LE_DEBUG("Client CRT 	= %s\n", tlsParams.clientCrt);
	LE_DEBUG("Client Key 	= %s\n", tlsParams.clientKey);
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
	mqttInitParams.pHostURL = connectionParams.host;
	mqttInitParams.port = connectionParams.port;
	// If TLS parameters not defined. Try to connect the MQTT host without TLS encryption.
	if (tlsParams.clientCrt != NULL || strlen(tlsParams.clientCrt) != 0) {
		mqttInitParams.pRootCALocation = tlsParams.rootCA;
		mqttInitParams.pDeviceCertLocation = tlsParams.clientCrt;
		mqttInitParams.pDevicePrivateKeyLocation = tlsParams.clientKey;
	}
	mqttInitParams.isSSLHostnameVerify = (tlsParams.clientCrt != NULL
			|| strlen(tlsParams.clientCrt) != 0);
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.disconnectHandler = _disconnectCallback;
	mqttInitParams.disconnectHandlerData = NULL;
	IoT_Error_t rc = FAILURE;

	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
	UNLOCK();
	if (SUCCESS != rc) {
		LE_FATAL("aws_iot_mqtt_init returned error : %d ", rc);
	}
	LE_DEBUG("\nSUCCESS: AWS_IOT has been initialized.");
}
/**================================================================================
 * Task  			: Connect client to MQTT broker
 *
 * Objective		: Invokes internal API 'aws_iot_mqtt_connect' to make connection
 * 				      to MQTT broker
 * Return 			:
 * ================================================================================
 */
int aws_Connect() {
	ClientState state = _getConnectionState();
	LOCK();

	if (state >= CLIENT_STATE_CONNECTED_IDLE
				&& state <= CLIENT_STATE_CONNECTED_WAIT_FOR_CB_RETURN) {
			UNLOCK();
			LE_DEBUG("MQTT host is already connected to client");
			return SUCCESS;
	}
	IoT_Error_t rc = FAILURE;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	connectParams.keepAliveIntervalInSec = 2000;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;
	LE_DEBUG("Connecting to MQTT host '%s'...",AWS_IOT_MQTT_HOST);
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if (rc != SUCCESS) {
		UNLOCK();
		state = _getConnectionState();
		LE_DEBUG("Failed to connect to MQTT host.. Network error(%d) and connection error(%d) is identified",rc,state);
		return rc;
	}
	LE_DEBUG("MQTT client '%s' is now connected to MQTT host",connectParams.pClientID);
	// Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	//  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	//  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if (SUCCESS != rc) {
		LE_ERROR("Unable to set Auto Reconnect to true - %d", rc);
	}
	UNLOCK();
	return rc;
}

/**================================================================================
 * Task  			: Publish any topic to mqtt broker
 *
 * Objective		: Invokes internal API 'aws_iot_mqtt_publish' to publish any
 * 					  topic to MQTT broker
 * Return 			:
 * ================================================================================
 */
int aws_Publish(const char* topic, const int topicLen, int32_t qosType,
		const char* payload, const int payloadLen) {

	IoT_Error_t rc = FAILURE;
	IoT_Publish_Message_Params paramsQOS1;
	paramsQOS1.qos = qosType;
	paramsQOS1.payload = (void *) payload;
	paramsQOS1.isRetained = 0;
	paramsQOS1.payloadLen = payloadLen;
	LOCK();
	rc = aws_iot_mqtt_publish(&client, topic, topicLen, &paramsQOS1);
	UNLOCK();
	if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
		LE_ERROR(
				"QOS1 publish ack for activity record not received. Loss of connectivity.");
	} else if (rc == -13 ) {
		LE_ERROR("Unable to publish topic. Network is disconnected");
	} else if ( rc != SUCCESS ) {
		LE_ERROR("Activity records publish failure %d", rc);
	} else {
		LE_DEBUG("Published message (%s) with payload (%s) to host",topic,payload);
	}

	return rc;

}

/**================================================================================
 * Task  			:
 *
 * Objective		:
 * Return 			: None on success, halts execution in case of failure
 * ================================================================================
 */
void _subscribeCallback(AWS_IoT_Client *pClient, char *topicName,
		uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	LE_DEBUG("=====================================================================");
	LE_DEBUG("Got Published message, Topic Name : %.*s\tPayload : %.*s",
			topicNameLen, topicName, (int ) params->payloadLen,
			(char * ) params->payload);
	LE_DEBUG("======================================================================");
}

/**
 *
 */
static void* _yield(void* timeout) {

	//TraceRef = le_log_GetTraceRef("Func _yield()");
	IoT_Error_t rc = SUCCESS;
	int to = (int) timeout;
	LE_DEBUG("YIELD with timeout: %d", to);
	while (rc == SUCCESS && !unsubscribe) {
		//LE_TRACE(TraceRef, "====================Calling YIELD===========================");
		LE_DEBUG("Calling YIELD...");
		IoT_Error_t rc = SUCCESS;
		LOCK();
		rc = aws_iot_mqtt_yield(&client, to);
		UNLOCK();
		if (rc != SUCCESS) {
			// Child yieldThread just exits so the main yieldThread can join with it.
			LE_DEBUG("Yield thread exited.");
			le_thread_Exit(NULL);
		}
	}
	return NULL;
}

/**================================================================================
 * Task  			:
 *
 * Objective		:
 * Return 			: None on success
 * ================================================================================
 */
int aws_Subscribe(const char* sTopic, int32_t topicLen, int32_t qosType) {
	IoT_Error_t rc = FAILURE;
	LE_DEBUG("Subscribing on topic : %s", sTopic);
	LOCK();
	rc = aws_iot_mqtt_subscribe(&client, sTopic, topicLen, qosType,
			_subscribeCallback, NULL);
	UNLOCK();

	// Mutex to avoid subscribe unsubscribe race condition
	le_mutex_Lock(MutexSubscribe);
	if (SUCCESS != rc) {
		LE_ERROR("Error subscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, _getErrorTypeFromCode(rc));
	} else if (_numSubcribes == 0) {

		// Create thread to call yield function for first subscription
		unsubscribe = false;
		yieldThread = le_thread_Create(AWS_IOT_SUBSCRIBE_THREAD_NAME, _yield, (void*) YIELD_TIMEOUT);
		le_thread_SetJoinable(yieldThread);
		le_thread_Start(yieldThread);
		_numSubcribes++;
	} else {
		_numSubcribes++;
	}
	le_mutex_Unlock(MutexSubscribe);
	return rc;
}

/**
 * Stop yield thread
 */
void _stopYieldThread() {
	unsubscribe = true;
	// Main yieldThread joins with the child before exiting the process.
	LE_DEBUG("Waiting for Yield thread to finish..");
	void* threadResult;
	le_thread_Join(yieldThread, &threadResult);
	LE_DEBUG("Yield thread is joined and terminated");
}

/**
 * Unsubscribe to a particular topic
 */
int aws_UnSubscribe(const char* sTopic, int32_t topicLen) {
	IoT_Error_t rc = FAILURE;
	LE_DEBUG("UnSubscribing on topic : %s", sTopic);
	LOCK();
	rc = aws_iot_mqtt_unsubscribe(&client, sTopic, topicLen);
	UNLOCK();

	// Mutex to avoid subscribe unsubscribe race condition
	le_mutex_Lock(MutexSubscribe);
	if (SUCCESS != rc) {
		LE_ERROR("Error UnSubscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, _getErrorTypeFromCode(rc));
		LE_DEBUG("Total no of subscription = %d ",_numSubcribes);
	} else if (--_numSubcribes <= 0) {
		_stopYieldThread();
		_numSubcribes = 0;
	}
	le_mutex_Unlock(MutexSubscribe);
	return rc;
}

COMPONENT_INIT {
	Mutex = le_mutex_CreateNonRecursive("yieldMutex");
	MutexSubscribe = le_mutex_CreateNonRecursive("subMutex");
	_numSubcribes = 0;
	/*============================================================*/
	/* Initialize remote MQTT Connection*/
	/*============================================================*/
	// Initialize connection parameters.
	_initConnectionParams(NULL, -1);
	// Initialize TLS default parameters. Skip this step if TLS encryption not required
	_initTlsParams(NULL, NULL, NULL, NULL);
	// Initialize connection to the MQTT broker
	_initConnection();

	LE_DEBUG("================ S T A R T   T E S T ====================");
	IOT_TEST();
}

