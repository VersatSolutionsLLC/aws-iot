#include "legato.h"
#include "interfaces.h"
#include "versAgentHeader.h"

static char payloadData[PAYLOAD_SIZE];
void _deleteFile(char*);


/**
 * Read metric info from the file metrics.json and copy the content into metrics.json.temp.
 * On successful copy operation, the file metrics.json is deleted. Afterwards on successful
 * publish , the file metrics.json.temp will be deleted
 *
 * @return 1 on success and 0 in case of any error
 */
int _renameMetricFileAndReadPayload
(
		char *payloadData			///< [OUT] Content of the file
) {

	int fd1 = le_atomFile_Open(FILE_PATH, LE_FLOCK_READ);
	if (LE_NOT_FOUND == fd1 || LE_FAULT == fd1) {
		LE_WARN("Publishing data to AWS-IOT is not possible");
		if (LE_NOT_FOUND == fd1)
			LE_WARN("Payload file %s does not exist", FILE_PATH);
		else
			LE_ERROR("Error occurred while opening payload file %s", FILE_PATH);
		return 0;
	}
	LE_DEBUG("Payload file %s is opened", FILE_PATH);

	// Read file to get the metric info as publish payload
	LE_DEBUG("Reading metric info from %s file as publish payload", FILE_PATH);

	//Create temporary file
	LE_DEBUG("Creating temporary file %s to copy payload", TEMP_FILE_PATH);
	int fd2 = le_atomFile_Create(TEMP_FILE_PATH, LE_FLOCK_WRITE,
			LE_FLOCK_REPLACE_IF_EXIST, S_IRWXU);

	// Handle error and write content in the file
	if (LE_NOT_FOUND == fd2 || LE_FAULT == fd2) {
		LE_ERROR(
				"Publishing data is not possible.Failed to create temp payload file %s",
				TEMP_FILE_PATH);
		return 0;
	}
	LE_DEBUG("Temp file %s is opened now.", TEMP_FILE_PATH);
	int n = read(fd1, payloadData, PAYLOAD_SIZE);
	payloadData[n] = '\0';

	n = write(fd2,payloadData,strlen(payloadData));
	LE_DEBUG("No of bytes written = %d",n);

	if ( LE_OK != le_atomFile_Close(fd2)) {
		LE_ERROR("Unable to close temp payload file %s",TEMP_FILE_PATH);
	}
	if ( LE_OK != le_atomFile_Close(fd1)) {
			LE_ERROR("Unable to close payload file %s",FILE_PATH);
	}
	_deleteFile(FILE_PATH);
	return 1;
}

/**
 * Invoke aws_Publish of versAwsIotService application to publish topic to MQTT host.
 * Publish operation takes place by extracting each entry from payload data and then
 * send to MQTT host. Overall success case is determined on each successful publish.
 *
 * @return none on success. Print error string in case of unsuccessful publish.
 * NOTE: In both the cases the file metrics.json.temp will be deleted
 */
void _publish
(
		char *topic,			///< [IN] Name of the topic to publish
		int qosType,			///< [IN] QOS level(0/1) to send data
		char *payloadData		///< [IN] Payload data as message
) {
	int rc = -1;
	int i = 0,j = 0;
	char tempStr[CONFIG_DEFAULT_PUBLISH_DATA_LIMIT];


	if (0 == strlen(payloadData)) {
		return;
	}

	LE_DEBUG("==========================================");
	LE_DEBUG("Topic 				= %s", topic);
	LE_DEBUG("Topic Length 			= %d", strlen(topic));
	LE_DEBUG("QOS Type 				= %d", qosType);
	LE_DEBUG("Payload 				= %s", payloadData);
	LE_DEBUG("Payload Length 		= %d", strlen(payloadData));
	LE_DEBUG("==========================================");

	// Publish data
	int fragmentCnt = 0;
	while (payloadData[i] != '\0') {
		if (payloadData[i] == '\n') {
			++fragmentCnt;
			tempStr[j] = '\0';
			LE_DEBUG(
					"------------------------------------------------------------------------------------");
			LE_DEBUG("Publishing payload [Fragment=%d size=%d bytes) = %s.... ", fragmentCnt, strlen(tempStr), tempStr);
			LE_DEBUG(
					"------------------------------------------------------------------------------------");
			rc = aws_Publish(topic, strlen(topic), qosType, tempStr,
					strlen(tempStr));

			if (rc == 0)
				LE_INFO("SUCCESS: Published payload[%d] to AWS-IOT",
						fragmentCnt);
			else
				LE_ERROR(
						"FAILURE: Error(code = %d) occurred while publishing payload[%d] to AWS-IOT",
						rc, fragmentCnt);
			j = 0; // Reinitialize position index for next publish
		}
		tempStr[j++] = payloadData[i++];
	}
	strcpy(payloadData, ""); // Emptying payload content after publish
}

/**
 * Delete files metrics.json and metrics.json.temp respectively after reading
 * the contents and after publishing the data to MQTT host.
 *
 * @return none on success. Print error string in case of any error occurs.
 */
void _deleteFile
(
		char *file				///< [IN] Name of the file to be deleted
)
{

	LE_DEBUG("Going to delete file %s", file);
	le_result_t res = le_atomFile_Delete(file);
	switch (res) {
	case LE_NOT_FOUND:
		LE_ERROR("File %s does not exist", file);
		break;
	case LE_FAULT:
		LE_ERROR("Failed to delete file %s.Error occurred during operation",
				file);
		break;
	default:
		LE_DEBUG("File %s is deleted", file);
	}
}

/**
 * Entry point
 */
COMPONENT_INIT {
	char topic[100];
	int qosType;

	// Get agent information (Topic name,QOS value) from configuration file
	if (!config_GetString(CONFIG_SECTION, CONFIG_KEY_TOPIC_NAME,
	CONFIG_DEFAULT_TOPIC_NAME, topic,
	SIZE_LIMIT_OF_KEY_FIELD_B)) {
		LE_ERROR(
				"Missing mandatory configuration [%s]%s in '%s'.Considering default value %s",
				CONFIG_KEY_TOPIC_NAME, CONFIG_SECTION, "Configuration file",
				CONFIG_DEFAULT_TOPIC_NAME);
	}
	if (!config_GetInteger(CONFIG_SECTION, CONFIG_KEY_QOS_VALUE,
	CONFIG_DEFAULT_QOS_VALUE, &qosType)) {
		LE_ERROR(
				"Missing mandatory configuration [%s]%s in '%s'.Considering default value %d",
				CONFIG_KEY_QOS_VALUE, CONFIG_SECTION, "Configuration file",
				CONFIG_DEFAULT_QOS_VALUE);
	}

	while ( ENABLE_PUBLISH_FLAG ) {
		if (_renameMetricFileAndReadPayload(payloadData)) {
			_publish(topic, qosType, payloadData);
			_deleteFile(TEMP_FILE_PATH);
		}
		sleep(TIME_INTERVAL_IN_SEC_TO_PUBLISH_DATA);
	}
}
