#include "legato.h"
#include "interfaces.h"

char previousParams[1024];
const char* _FRM_JSON =
		"{\"temp\":%s,\"%s\":%s,\"timestamp\":%d,\"loc\":%s,\"imei\":%s,\"imsi\":%s}";
const char* _FRM_JSON_GSM = "{\"rssi\":%s,\"ber\":%s}";
const char* _FRM_JSON_LTE =
		"{\"rssi\":%s,\"rsrp\":%s,\"rsrq\":%s,\"snr\":%s,\"bler\":%s}";
const char* _FRM_JSON_LOC =
		"{\"lat\":%s,\"lon\":%s,\"alt\":%s,\"altWgs84\":%s,\"hAcc\":%s,\"vAcc\":%s}";

/**
 * Split string
 */
char** _strSplit(char* a_str, const char a_delim) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
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

	if (result) {
		size_t idx = 0;
		char* token = strtok(a_str, delim);

		while (token) {
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
int _getJson(char* json, const int jsonLength) {
	char params[1024];
	char radioJson[1024];
	char locationJson[1024];

	radio_Info(params, 1024);
	//radio_NeighbourInfo();
	if (strcmp(previousParams, params) == 0) {
		strcpy(previousParams, params);
		return 0;
	}
	strcpy(previousParams, params);
	char** tokens = _strSplit(params, ',');
	int imei = 0;
	int imsi = 1;
	int ratPos = 2;
	int temperature = 3;
	int rssi = 4;

	int rsrp = 5;
	int rsrq = 6;
	int snr = 7;


	int ber = 5;int bler = 8;

	char *rat = *(tokens + ratPos);
	u_int32_t timestamp = (unsigned) time(NULL);

	int cmpGSM = strcmp(rat, "GSM");
	int cmpLTE = strcmp(rat, "LTE");

	if (cmpGSM == 0) {
		snprintf(radioJson, jsonLength, _FRM_JSON_GSM, *(tokens + rssi),
				*(tokens + ber));
	} else if (cmpLTE == 0) {
		snprintf(radioJson, jsonLength, _FRM_JSON_LTE, *(tokens + rssi),
				*(tokens + rsrp), *(tokens + rsrq), *(tokens + snr),
				*(tokens + bler));
	}

	int lat = 0;
	int lon = 1;
	int alt = 2;
	int altWgs84 = 3;
	int hAcc = 4;
	int vAcc = 5;

	gps_GetLocation(params, 1024);
	char** gpsTokens = _strSplit(params, ',');
	snprintf(locationJson, jsonLength, _FRM_JSON_LOC, *(gpsTokens + lat),
			*(gpsTokens + lon), *(gpsTokens + alt), *(gpsTokens + altWgs84),
			*(gpsTokens + hAcc), *(gpsTokens + vAcc));

	if (json != NULL)
		snprintf(json, jsonLength, _FRM_JSON, *(tokens + temperature), rat,
				radioJson, timestamp, locationJson, *(tokens + imei), *(tokens + imsi));
	free(tokens);

	free(gpsTokens);
	return strlen(json);
}

/**
 * ON successful connection or disconnection of mobile data the function called with proper
 * interface name and isConnected flag.
 */
static void _connectionHandler(char* infName, bool isConnected,
		void* contextPtr) {
	if (!isConnected) {
		LE_WARN("Device not connect to internet!");
		return;
	}
	LE_INFO("Device connected to internet with interface name %s", infName);

	const char *topic = "versat/radio";
	int32_t qosType = 1;
	int32_t topicLen = strlen(topic);
	aws_Connect();

	char json[1024];

	int rc = 0;

	/*Publish topic to MQTT client*/
	int l = 50;
	while (rc == 0) {
		int32_t jsonLen = _getJson(json, 1024);
		LE_DEBUG("Got JSON with length %d", jsonLen);
		//Skipping publish for identical data.
		if (jsonLen == 0) {
			LE_DEBUG("Nothing to publish! Skipping...");
			continue;
		}

		rc = aws_Publish(topic, topicLen, qosType, json, jsonLen);
		LE_INFO("Published, Topic:%s, Qos: %d, Payload: %s", topic, qosType,
				json);
		sleep(l);
	}

}

COMPONENT_INIT {

	LE_INFO("=========Trying to connect MQTT Host============\n");
	if (radio_IsConnected()) {
		_connectionHandler("manual", true, NULL);
	}

	radio_AddDataConnectionStateHandler(
			(radio_DataConnectionStateHandlerFunc_t) _connectionHandler, NULL);

	radio_Connect();

}
