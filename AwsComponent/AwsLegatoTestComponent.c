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
static char HostAddress[HOST_ADDRESS_SIZE] = AWS_IOT_MQTT_HOST;
static uint32_t port = AWS_IOT_MQTT_PORT;





/**
 * Publish AWS IoT topic
 */
int publishTopic(AWS_IoT_Client* mqttClient, const char* topicName, const int topicLen, const char* json, const int jsonLength){
	IoT_Error_t rc = SUCCESS;
	IoT_Publish_Message_Params paramsQOS1;
	paramsQOS1.qos = QOS1;
	paramsQOS1.payload = (void *)json;
	paramsQOS1.isRetained = 0;
	paramsQOS1.payloadLen = jsonLength;
	rc = aws_iot_mqtt_publish(mqttClient, topicName, topicLen, &paramsQOS1);
	if (rc == MQTT_REQUEST_TIMEOUT_ERROR)
	{
		IOT_ERROR("QOS1 publish ack for activity record not received. Loss of connectivity.");
	}
	else if (rc != SUCCESS)
	{
		IOT_ERROR("Activity records publish failure %d", rc);
	}
	return rc;
}

/**
 * Split string
 */
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            LE_ASSERT(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        LE_ASSERT(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


//returns lenght of json
int getRadioJson(char* json, const int jsonLength){
	char params[1024];
	radio_Info(params, 1024);
    char** tokens = str_split(params, ',');
    int temperature = 1;
    int rssi = 2;

    int rsrp = 3;
    int rsrq = 4;
    int snr = 5;
    int bler = 1;

    int ber = 3;

    char *rat = *(tokens);
	u_int32_t timestamp = (unsigned)time(NULL);

    int cmpGSM = strcmp(rat,"GSM");
    int cmpLTE = strcmp(rat,"LTE");

    if(cmpGSM == 0){
    	snprintf(json, jsonLength,"{\"temp\":%s,\"rat\":\"%s\",\"rssi\":%s,\"ber\":%s,\"timestamp\":%d}" , *(tokens + temperature),rat,*(tokens + rssi),*(tokens + ber), timestamp);
    	free(*(tokens + temperature));
    	free(*(tokens + rssi));
    	free(*(tokens + ber));
    }
    else if(cmpLTE == 0){
    	snprintf(json, jsonLength,"{\"temp\":%s,\"rat\":\"%s\",\"rssi\":%s,\"rsrp\":%s,\"rsrq\":%s,\"snr\":%s,\"bler\":%s,\"timestamp\":%d}" ,*(tokens + temperature),rat,*(tokens + rssi),*(tokens + rsrp),*(tokens + rsrq),*(tokens + snr),*(tokens + bler), timestamp);
    	free(*(tokens + temperature));
    	free(*(tokens + rssi));
    	free(*(tokens + bler));
    	free(*(tokens + rsrp));
    	free(*(tokens + rsrp));
    	free(*(tokens + snr));
   }
   free(tokens);
   return strlen(json);
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}



COMPONENT_INIT
{
	IoT_Error_t rc = FAILURE;
	//int32_t i = 0;

	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];

	IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	getcwd(CurrentWD, sizeof(CurrentWD));
	snprintf(rootCA, PATH_MAX + 1, "%s", AWS_IOT_ROOT_CA_FILENAME);
	snprintf(clientCRT, PATH_MAX + 1, "%s", AWS_IOT_CERTIFICATE_FILENAME);
	snprintf(clientKey, PATH_MAX + 1, "%s", AWS_IOT_PRIVATE_KEY_FILENAME);

	IOT_DEBUG("rootCA %s", rootCA);
	IOT_DEBUG("clientCRT %s", clientCRT);
	IOT_DEBUG("clientKey %s", clientKey);


	// initialize the mqtt client
	AWS_IoT_Client mqttClient;
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	mqttInitParams.enableAutoReconnect = false; // We enable this later below
	mqttInitParams.pHostURL = HostAddress;
	mqttInitParams.port = port;
	mqttInitParams.pRootCALocation = rootCA;
	mqttInitParams.pDeviceCertLocation = clientCRT;
	mqttInitParams.pDevicePrivateKeyLocation = clientKey;
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	rc = aws_iot_mqtt_init(&mqttClient, &mqttInitParams);
	if(SUCCESS != rc) {
		IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
	}

	connectParams.keepAliveIntervalInSec = 600;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;

	IOT_INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&mqttClient, &connectParams);
	if(SUCCESS != rc) {
		IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(&mqttClient, true);
	if(SUCCESS != rc) {
		IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
	}
	// loop and publish a change in temperature
	char json[1024];
	const int radioTopicLen = strlen(RADIO_TOPIC);
	while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			sleep(1);
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}
		int radioJsonLen = getRadioJson(json, 1024);
		if(radioJsonLen > 0){
			publishTopic(&mqttClient,RADIO_TOPIC, radioTopicLen, json, radioJsonLen);
		}
		else{
			LE_ERROR("Not able to publish as radioJSONLen = %d", radioJsonLen);
		}

		sleep(30);
	}

	if(SUCCESS != rc) {
		IOT_ERROR("An error occurred in the loop %d", rc);
	}

	IOT_INFO("Disconnecting");
	rc = aws_iot_shadow_disconnect(&mqttClient);

	if(SUCCESS != rc) {
		IOT_ERROR("Disconnect error %d", rc);
	}

}


