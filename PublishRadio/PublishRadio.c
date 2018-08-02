#include "legato.h"
#include "interfaces.h"

char previousParams[1024];
/**
 * Split string
 */
char** str_split(char* a_str, const char a_delim) {
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
int getRadioJson(char* json, const int jsonLength) {
	char params[1024];
	radio_Info(params, 1024);
	if (strcmp(previousParams, params) == 0) {
		strcpy(previousParams, params);
		return 0;
	}
	strcpy(previousParams, params);
	char** tokens = str_split(params, ',');
	int temperature = 1;
	int rssi = 2;

	int rsrp = 3;
	int rsrq = 4;
	int snr = 5;
	int bler = 1;

	int ber = 3;

	char *rat = *(tokens);
	u_int32_t timestamp = (unsigned) time(NULL);

	int cmpGSM = strcmp(rat, "GSM");
	int cmpLTE = strcmp(rat, "LTE");

	if (cmpGSM == 0) {
		snprintf(json, jsonLength,
				"{\"temp\":%s,\"rat\":\"%s\",\"rssi\":%s,\"ber\":%s,\"timestamp\":%d}",
				*(tokens + temperature), rat, *(tokens + rssi), *(tokens + ber),
				timestamp);
		free(*(tokens + temperature));
		free(*(tokens + rssi));
		free(*(tokens + ber));
	} else if (cmpLTE == 0) {
		snprintf(json, jsonLength,
				"{\"temp\":%s,\"rat\":\"%s\",\"rssi\":%s,\"rsrp\":%s,\"rsrq\":%s,\"snr\":%s,\"bler\":%s,\"timestamp\":%d}",
				*(tokens + temperature), rat, *(tokens + rssi),
				*(tokens + rsrp), *(tokens + rsrq), *(tokens + snr),
				*(tokens + bler), timestamp);
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

COMPONENT_INIT {
	const char *topic = "versat/radio";
	int32_t qosType = 1;
	int32_t topicLen = strlen(topic);

	LE_INFO("=========Trying to connect MQTT Host============\n");
	/*Connect to remote client*/

	aws_Connect();
	char json[1024];
	char prevJson[1024];

	int rc = 0;

	/*Publish topic to MQTT client*/
	int l = 5;
	while (rc == 0) {
		int32_t jsonLen = getRadioJson(json, 1024);
		//Skipping publish for identical data.
		if (jsonLen == 0) {
			LE_DEBUG("Nothing to publish! Skipping...");
			continue;
		}

		rc = aws_Publish(topic, topicLen, qosType, json, jsonLen);
		LE_INFO("Published, Topic:%s, Qos: %d, Payload: %s", topic, qosType,
				json);
		strcpy(prevJson, json);
		sleep(l);
	}
}
