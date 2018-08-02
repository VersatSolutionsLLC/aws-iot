#include "legato.h"
#include "interfaces.h"
/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file shadow_sample.c
 * @brief A simple connected window example demonstrating the use of Thing Shadow
 */

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

#define HOST_ADDRESS_SIZE 255

static bool unsubscribe = false;

static le_thread_Ref_t yieldThread;
static le_thread_Ref_t MainThread;
// Mutex used to prevent races between the threads.
static le_mutex_Ref_t Mutex;
#define LOCK()      le_mutex_Lock(Mutex);
#define UNLOCK()    le_mutex_Unlock(Mutex);

AWS_IoT_Client client;

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


void _stopThread(){
	if (le_thread_GetCurrent() == MainThread)
	    {
	        // Main yieldThread joins with the child before exiting the process.
			LE_DEBUG("Yield thread joins with main thread.");
	        void* threadResult;
	        le_thread_Join(yieldThread, &threadResult);

	    }
	    else
	    {
	        // Child yieldThread just exits so the main yieldThread can join with it.
			LE_DEBUG("Yield thread exited.");
			le_thread_Exit(NULL);
	    }
}

/**
 * Initialize TLS connection parameters. If called with empty parameters, it uses the values defined in the
 * configure file.
 */
void aws_InitTlsParams(const char* certPath, const char* rootCa,
		const char* privateKey, const char* cert) {

	if (strlen(certPath) < 0 || certPath == NULL) {

		LE_DEBUG("Certificate path is not supplied. Use default one('%s')",
				AWS_IOT_CERT_PATH);
		snprintf(tlsParams.certDirectory, PATH_MAX + 1, "%s",
		AWS_IOT_CERT_PATH);
	} else {
		strcpy(tlsParams.certDirectory, certPath);
	}

	// Root CA certificate name update
	if (strlen(rootCa) < 0 || rootCa == NULL) {
		LE_DEBUG(
				"Root CA certificate name is not supplied. Use default one('%s')",
				AWS_IOT_ROOT_CA_FILENAME);
		snprintf(tlsParams.rootCA, PATH_MAX + 1, "%s/%s",
				tlsParams.certDirectory, AWS_IOT_ROOT_CA_FILENAME);

	} else {
		strcpy(tlsParams.rootCA, rootCa);
	}

	// AWS_IOT certificate file name update
	if (strlen(cert) < 0 || cert == NULL) {
		LE_DEBUG(
				"AWS_IOT Client certificate file name is not supplied. Use default one('%s')",
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
				"AWS_IOT pvt key file name is not supplied. Use default one('%s')",
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
const char* GetErrorTypeFromCode(int errorCode) {
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
		LE_ERROR("Error Type(%d) : %s\n", rc, GetErrorTypeFromCode(rc));
	} else {
		LE_DEBUG("\nSUCCESS:Disconnected from remote host");
	}
	return rc;
}

/**================================================================================
 * Task  			: Invoked as callback handler
 * Objective		:
 * Return 			: None
 * ================================================================================
 */
void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if (NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

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
		}
	}
}

/**================================================================================
 * Task  			: Set configuration parameter's values, if supplied from
 * 					  the input parameters otherwise use config file values
 * Objective		: Make provision to get configuration from user side
 * Return 			: None
 * ================================================================================
 */
void aws_InitConnectionParams(const char* hostName, const int32_t port) {

	if (strlen(hostName) < 0 || hostName == NULL) {
		LE_DEBUG("Host name not supplied('%s')", AWS_IOT_MQTT_HOST);
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
void aws_Init() {

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
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;
	IoT_Error_t rc = FAILURE;

	LOCK();
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
 * Return 			: None on success, halts execution in case of failure
 * ================================================================================
 */
int aws_Connect() {
	IoT_Error_t rc = FAILURE;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	connectParams.keepAliveIntervalInSec = 2000;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;

	LE_DEBUG("Connecting...");
	ClientState state = _getConnectionState();

	if (state >= CLIENT_STATE_CONNECTED_IDLE
			&& state <= CLIENT_STATE_DISCONNECTED_ERROR) {
		return SUCCESS;
	}
	LOCK();
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	UNLOCK();
	if (rc != SUCCESS)
		return rc;

	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	LOCK();
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	UNLOCK();
	if (SUCCESS != rc) {
		LE_ERROR("Unable to set Auto Reconnect to true - %d", rc);
	}
	return rc;
}

/**================================================================================
 * Task  			: Publish any topic to mqtt broker
 *
 * Objective		: Invokes internal API 'aws_iot_mqtt_publish' to publish any
 * 					  topic to MQTT broker
 * Return 			: None on success, halts execution in case of failure
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
	LOCK()
	;
	rc = aws_iot_mqtt_publish(&client, topic, topicLen, &paramsQOS1);
	UNLOCK()
	;
	if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
		LE_ERROR(
				"QOS1 publish ack for activity record not received. Loss of connectivity.");
	} else if (rc != SUCCESS) {
		LE_ERROR("Activity records publish failure %d", rc);
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
void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName,
		uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	LE_DEBUG("Got Published message, Topic Name : %.*s\tPayload : %.*s",
			topicNameLen, topicName, (int ) params->payloadLen,
			(char * ) params->payload);
}

int aws_Yield(int32_t timeout) {
	LE_DEBUG("====================Calling YIELD===========================");
	IoT_Error_t rc = SUCCESS;
	rc = aws_iot_mqtt_yield(&client, timeout);
	if(rc!=SUCCESS) _stopThread();
	else LE_ERROR("Failed to yield!");
	return rc;
}

static void* _Yield(void* timeout) {
	IoT_Error_t rc = SUCCESS;
	int to = (int) timeout;
	LE_DEBUG("YIELD with timeout: %d", to);
	while (rc == SUCCESS && !unsubscribe) {
		LOCK();
		rc = aws_Yield(to);
		UNLOCK();
	}
	return NULL;
}

/**================================================================================
 * Task  			:
 *
 * Objective		:
 * Return 			: None on success, halts execution in case of failure
 * ================================================================================
 */
int aws_Subscribe(const char* sTopic, int32_t topicLen, int32_t qosType,
		int32_t timeout) {
	IoT_Error_t rc = FAILURE;
	LE_DEBUG("Subscribing on topic : %s", sTopic);
	LOCK();
	rc = aws_iot_mqtt_subscribe(&client, sTopic, topicLen, qosType,
			iot_subscribe_callback_handler, NULL);
	UNLOCK();
	if (SUCCESS != rc) {
		LE_ERROR("Error subscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, GetErrorTypeFromCode(rc));
	}
	unsubscribe = false;
	yieldThread = le_thread_Create(sTopic, _Yield, (void*) timeout);
	le_thread_SetJoinable(yieldThread);
	le_thread_Start(yieldThread);
	return rc;

}

int aws_UnSubscribe(const char* sTopic, int32_t topicLen) {
	IoT_Error_t rc = FAILURE;
	LE_DEBUG("UnSubscribing on topic : %s", sTopic);
	LOCK();
	rc = aws_iot_mqtt_unsubscribe(&client, sTopic, topicLen);
	UNLOCK();
	if (SUCCESS != rc) {
		LE_ERROR("Error UnSubscribing topic %s: \nErrorType(%d) = %s ", sTopic,
				rc, GetErrorTypeFromCode(rc));
	}
	unsubscribe = true;
	_stopThread();
	return rc;
}

COMPONENT_INIT {

	MainThread = le_thread_GetCurrent();
	Mutex = le_mutex_CreateNonRecursive("mutex");
	/* ============================================================*/
	/*Initialize remote MQTT Connection*/
	/* ============================================================*/
	//Initialize connection parameters.
	aws_InitConnectionParams(NULL, -1);
	//Initialize TLS default parameters. Skip this step if TLS encryption not required
	aws_InitTlsParams(NULL, NULL, NULL, NULL);
	//Initialize connection to the MQTT broker
	aws_Init();
}

