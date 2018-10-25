#include "legato.h"
#include "versAgentHeader.h"
#include "interfaces.h"


static char content[SIZE];
static char lastContent[SIZE];
static int 	fileSizeLimitInBytes;
static int 	numNeighborCell = 0;

int _adjustMetricFileContent(char*);

/**
 * Check whether metrics.json file (one that contains device,radio,serving cell and
 * neighbor cell information) exists in the namespace specified path.
 *
 * @return 1 if the file exists and 0 otherwise
 */
bool _isMetricsFileExist
(
)
{
	int fd = le_atomFile_Open(FILE_PATH, LE_FLOCK_READ);
	if ( LE_NOT_FOUND == fd ) {
		return 0;
	}
	le_atomFile_Close(fd);
	return 1;
}

/**
 * Write device, radio, serving cell and neighbor cell information as JSON string into
 * metrics.json file
 *
 * @return none on success. Print error string on failure
 */
void _writeMetricToFile
(
		char *content	// [IN] buffer to write into metrics.json file
) {

	if (_isMetricsFileExist()) {
		if (!_adjustMetricFileContent(content)) {
			LE_WARN("Content adjustment failed!!");
			return;
		}
	}

// If file size exceeded the highest limit
// then delete the oldest entry and add the
// latest one in the file

	int fd = le_atomFile_Create(FILE_PATH, LE_FLOCK_READ_AND_APPEND,
			LE_FLOCK_OPEN_IF_EXIST, S_IRWXU);
	// Handle error and write content in the file
	switch (fd) {
	case LE_DUPLICATE:
		LE_ERROR(
				"Unable to open file!! Invalid access_mode 'LE_FLOCK_FAIL_IF_EXIST' is specified while opening file.Use 'LE_FLOCK_READ_AND_APPEND' and try again.");
		return;
	case LE_FAULT:
		LE_ERROR("Unable to open file!! Error occurred");
		return;
	default:
		LE_INFO("File %s is opened successfully", FILE_PATH);
	}

	LE_DEBUG("Writing content %s[%d] in the file", content, sizeof(content));
	int size = strlen(content);
	write(fd, content, size);
	content[size] = '\0';

	if (LE_OK != le_atomFile_Close(fd)) {
		LE_ERROR(
				"Unable to write content in file.Error occurred while closing file %s",
				FILE_PATH);
		return;
	}

	LE_DEBUG("Content is successfully written into the file %s", FILE_PATH);
}

/**
 * Create JSON string with the help of device ,radio, serving cell, neighbor cell and
 * gps information. Values are stored within structure members that is declared in the
 * header file "versAgentHeader.h"
 *
 * @return none
 */
void _getMetricJSON
(
		char *content,								///< [OUT] to store metrics as JSON string
		_deviceInfo deviceInfo,						///< [IN]  structure variable that contains device parameters
		_radioInfo radioInfo,						///< [IN]  structure variable that contains radio parameters
		_radioServingCellInfo radioServingCellInfo, ///< [IN]  structure variable that contains serving cell parameters
		_radioNeighborInfo *radioNeighborInfo,		///< [IN]  structure variable that contains neighbor cell parameters
		_gpsInfo gpsInfo							///< [IN]  structure variable that contains gps parameters
		) {

	int i = 0 ;
	char radioParam[SIZE / 4];
	char tempBuffer[SIZE / 4];
	char radioNeighborParam[SIZE];
	char gpsParam[SIZE / 4];

	// Store radio and serving cell information
	sprintf(radioParam,
			"{\"IsSc\":\"True\",\"RATS\":\"%s\",\"RSSI\":%d,\"BLER\":%d,\"BER\":%d,\"RSRP\":%f,\"RSRQ\":%f,\"SNR\":%f,\"LAC\":%d,\"TAC\":%d,\"CellID\":%d}",
			radioInfo.ratS, radioInfo.rssi, radioInfo.bler, radioInfo.ber,
			radioInfo.rsrp, radioInfo.rsrq, radioInfo.snr,
			radioServingCellInfo.lac, radioServingCellInfo.tac,
			radioServingCellInfo.cellId);

	// Store each neighbor cell information
	while (i < numNeighborCell) {
		sprintf(tempBuffer,
				"{\"IsSc\":\"False\",\"RATS\":\"%s\",\"RSSI\":%d,\"LAC\":%d,\"CellID\":%d,\"ECIO\":%f,\"RSRP\":%f,\"RSRQ\":%f}",
				(radioNeighborInfo + i)->ratS, (radioNeighborInfo + i)->rxLevel,
				(radioNeighborInfo + i)->lac, (radioNeighborInfo + i)->cid,
				(radioNeighborInfo + i)->ecio,
				(radioNeighborInfo + i)->intraRsrp,
				(radioNeighborInfo + i)->intraRsrq);

	// Separate each neighbor cell information as JSON string with ','
	// delimiter
		if ( 0 == i) {
			strcpy(radioNeighborParam,tempBuffer);
		} else {
			strcat(radioNeighborParam, tempBuffer);
		}
		if (i++ < numNeighborCell - 1) {
			strcat(radioNeighborParam, ",");
		}
	}
	sprintf(gpsParam,
			"{\"Lat\":%f,\"Long\":%f,\"Alt\":%f,\"AltOnWgs84\":%f,\"HAcc\":%f,\"VAcc\":%f}",
			gpsInfo.latitude, gpsInfo.longitude, gpsInfo.altitude,
			gpsInfo.altitudeOnWgs84, gpsInfo.hAccuracy, gpsInfo.vAccuracy);

	snprintf(content, SIZE,
					"{\"timestamp\":%d,\"imei\":%s,\"imsi\":%s,\"Temp\":%d,\"Cells\":[%s,%s],\"Gps\":%s}\n",
					deviceInfo.timestamp,deviceInfo.imei, deviceInfo.imsi,
					deviceInfo.temperaturePtr, radioParam,
					radioNeighborParam,gpsParam);

}

/**
 * Get metrics readings from device. It contains device data (imei,imsi,temperature,timestamp), radio information
 * (rats,rssi,rsrp,rsrq,bler,snr,ber), serving cell information (lac, tac, cellId), neighbor cell information
 * (rat, lac, cid, rxLevel, ecio, intraRsrp, intraRsrq, interRsrq, interRsrp) and gps information
 * (latitude, longitude, altitude, altitudeOnWgs84, hAccuracy,
 * vAccuracy)
 *
 * @return none
 */
void _getMetricInfo
(
		char *content			///< [OUT] to store metrics as JSON string
		)
{


	_deviceInfo deviceInfo;
	_radioInfo radioInfo;
	_radioServingCellInfo radioServingCellInfo;
	_radioNeighborInfo radioNeighborInfo[MAX_NO_OF_NEIGHBOR_CELL];
	_gpsInfo gpsInfo;

	// Get device information
	device_Info(&deviceInfo);
	LE_DEBUG("IMEI = %s",deviceInfo.imei);
	LE_DEBUG("IMSI = %s",deviceInfo.imsi);
	LE_DEBUG("TEMP = %d",deviceInfo.temperaturePtr);
	LE_DEBUG("TIME_STAMP = %d", deviceInfo.timestamp);

	// Get radio information
	radio_Info(&radioInfo);
	LE_DEBUG("RATS = %s", radioInfo.ratS);
	LE_DEBUG("RSSI = %d", radioInfo.rssi);
	LE_DEBUG("BLER = %d", radioInfo.bler);
	LE_DEBUG("BER = %d", radioInfo.ber);
	LE_DEBUG("RSRP = %f", radioInfo.rsrp);
	LE_DEBUG("RSRQ = %f", radioInfo.rsrq);
	LE_DEBUG("SNR = %f", radioInfo.snr);

	// Get serving cell information
	radio_ServingCellInfo(&radioServingCellInfo);
	LE_DEBUG("LAC = %d", radioServingCellInfo.lac);
	LE_DEBUG("TAC = %d", radioServingCellInfo.tac);
	LE_DEBUG("CELLId = %d", radioServingCellInfo.cellId);

	// Get neighbor cell information
	radio_NeighbourInfo(radioNeighborInfo,&numNeighborCell);

	int i = 0;
	while ( i < numNeighborCell ) {

		LE_DEBUG("Neighbor Cell-%d ------------------------",i+1);
		LE_DEBUG("LAC = %d", radioNeighborInfo[i].lac);
		LE_DEBUG("CellID = %d", radioNeighborInfo[i].cid);
		LE_DEBUG("RxLevel = %d", radioNeighborInfo[i].rxLevel);
		LE_DEBUG("Ecio = %f", radioNeighborInfo[i].ecio);
		LE_DEBUG("IntraRSRP = %f", radioNeighborInfo[i].intraRsrp);
		LE_DEBUG("IntraRSRQ = %f", radioNeighborInfo[i].intraRsrq);
		LE_DEBUG("InterRSRP = %f", radioNeighborInfo[i].interRsrp);
		LE_DEBUG("InterRSRQ = %f", radioNeighborInfo[i].interRsrq);
		i++;
	}

	gps_GetLocation(&gpsInfo);

	LE_DEBUG("Latitude= %f", gpsInfo.latitude);
	LE_DEBUG("Longitude = %f", gpsInfo.longitude);
	LE_DEBUG("AltitudeLevel = %f", gpsInfo.altitude);
	LE_DEBUG("AltitudeOnWgs84 = %f", gpsInfo.altitudeOnWgs84);
	LE_DEBUG("HAccuracy = %f", gpsInfo.hAccuracy);
	LE_DEBUG("VAccuracy = %f", gpsInfo.vAccuracy);

	// Get information as JSON format
	_getMetricJSON(content,deviceInfo,radioInfo,radioServingCellInfo,radioNeighborInfo,gpsInfo);
}

/**
 * Check whether two contiguous metrics readings are same or different
 *
 * @return boolean value - true if current and previous readings are different and false otherwise.
 */
bool _isDifferentMetricFromPrev
(
) {
	if (0 == strcmp(content, lastContent)) {
		strcpy(lastContent, content);
		return false;
	}
	strcpy(lastContent, content);
	return true;
}

/**
 * Obtain both size (in bytes) and content of the file metrics.json.
 *
 * @return none. Out parameters size is updated with '0' if the
 * designated file does not exist or in case any other error occurs.
 */
void _getFileSizeAndContent
(
		char *fileName,			///< [IN] Name of the metrics file
		size_t *size,			///< [OUT] size of the file
		char *buffer			///< [OUT] contents of the file
) {
	int fd = le_atomFile_Open(fileName, LE_FLOCK_READ);
	switch (fd) {
	case LE_NOT_FOUND:
	case LE_FAULT:
		LE_ERROR(
				"Unable to obtain file size and content. Either file doesn't exist / error occurred while opening file.");
		*size = 0;
		break;
	default:
		*size = read(fd, buffer, SIZE);
		buffer[*size] = '\0';
		le_atomFile_Close(fd); // Transfers all changes to disk
	}
}

/**
 * Used to manipulate content of the metrics file (metrics.json) if the file
 * size exceeds the maximum size limit (defined at versAgent.h). If it occurs,
 * the very first entry is discarded from the file and the new content is added
 * into it.
 *
 * @return 1 on success and 0 in case of any failure
 */
int _adjustMetricFileContent
(
		char *newContent	///< [IN] current reading to be added into the file
		)
{
	int i = 0, j = 0;
	int lastLineEndPos;
	int nextLineStartPos;
	size_t prevFileSize;

	char tempStr[SIZE];
	char prevContent[SIZE];

	// Return if file does not exist
	LE_DEBUG("Receiving existing file content......");
	_getFileSizeAndContent(FILE_PATH, &prevFileSize, prevContent);
	if (0 == prevFileSize || 0 == strlen(prevContent)) {
		return 0;
	}

	// Return if size is within limit
	if ((prevFileSize + strlen(newContent)) < fileSizeLimitInBytes) {
		LE_DEBUG(
				"Content adjustment is not required in the file %s.File size (%d bytes) is within highest limit (%d bytes)",
				FILE_PATH, prevFileSize, fileSizeLimitInBytes);
		return 1;
	}
	LE_WARN(
			"Total Size ( prev = %d bytes, current = %d bytes) of the payload file (%s) has exceeded the highest limit (%d bytes).Clearing oldest content from file",
			prevFileSize, strlen(newContent), FILE_PATH, fileSizeLimitInBytes);

	// Find position of the first  new line character ('\n')
	for (i = 0; prevContent[i] != '\n'; i++) {
		continue;
	}

	nextLineStartPos = i + 1;
	lastLineEndPos = prevFileSize - 1;
	j = 0;
	i = nextLineStartPos;

	// Exclude first line from the file content and
	// store it in the temporary buffer
	while (i <= lastLineEndPos) {
		tempStr[j++] = prevContent[i++];
	}
	tempStr[j] = '\0';

	// Open file again to manipulate file content
	int fd = le_atomFile_Create(FILE_PATH, LE_FLOCK_WRITE,
			LE_FLOCK_REPLACE_IF_EXIST, S_IRWXU);

	// Return if error occurred
	if (fd < 0) {
		LE_ERROR(
				"Content can not be modified in %s.Error occurred while opening file in LE_FLOCK_REPLACE_IF_EXIST mode",
				FILE_PATH);
		return 0;
	}

	// Write content in the file
	write(fd, tempStr, strlen(tempStr));

	if (LE_OK != le_atomFile_Close(fd)) {
		LE_ERROR(
				"Unable to commit changes while adjusting file content. Error faced while closing file.");
		return 0;
	}
	LE_DEBUG("Oldest content is deleted from the file %s", FILE_PATH);
	return 1;
}

/**
 * It is a TEST function and is not invoked by any other function in this application.
 * Purpose: It is used to check the contents of the file metrics.json and count the
 * the number of entries (separated by '\n') in it.
 *
 * @return 0 on success and 1 in case of any error
 */
int _readContentFromFile
(
		char *fileName		///< [IN] Name of the file to read the content from
) {

	char buffer[SIZE];

	LE_INFO("Trying to open file %s ", fileName);
	int fd = le_atomFile_Open(fileName, LE_FLOCK_READ);
	switch (fd) {
	case LE_NOT_FOUND:
		LE_ERROR("Unable to open file %s (Mode = LE_FLOCK_READ)", fileName);
		return 1;
	case LE_FAULT:
		LE_ERROR("Unable to open file %s (Mode = LE_FLOCK_READ).Error occurred",
				fileName);
		return 1;
	default:
		LE_INFO("File %s is opened (Mode = LE_FLOCK_READ)", fileName);
		int n = read(fd, buffer,SIZE);
		buffer[n] = '\0';

		LE_DEBUG("===========================Current content========================");
		LE_DEBUG("Buffer [%d bytes] = %s",n,buffer);
		LE_DEBUG("===================================================================");

		le_atomFile_Close(fd); // Transfers all changes to disk
	}

	int i = 0, numEntries = 0, numNull = 0;
	while (buffer[i++] != '\0') {
		if (buffer[i] == '\n') {
			numEntries++;
		}
		if (buffer[i] == '\0') {
			numNull++;
		}
	}
	LE_INFO("Total no of entries = %d Number of Nulls = %d", numEntries, numNull);
	return 0;
}


/**
 * Entry point
 */
COMPONENT_INIT {

	// Get configuration from agent configuration file
	config_GetInteger(CONFIG_SECTION, CONFIG_KEY_FILE_SIZE_LIMIT,
	CONFIG_DEFAULT_FILE_SIZE_LIMIT, &fileSizeLimitInBytes);

	while ( ENABLE_READ_FLAG) {
		_getMetricInfo(content);
		if (_isDifferentMetricFromPrev()) {
			_writeMetricToFile(content);
			continue;
		}
		sleep( TIME_INTERVAL_IN_SEC_TO_READ_METRIC);
	}
}
