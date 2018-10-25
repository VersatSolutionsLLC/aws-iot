#include "legato.h"
#include "versAgentHeader.h"
#include "interfaces.h"

int _adjustMetricFileContent(char*);
static char content[SIZE];
static char lastContent[SIZE];
static int fileSizeLimitInBytes;


bool _isMetricsFileExist( ) {
	int fd = le_atomFile_Open(FILE_PATH, LE_FLOCK_READ);
	if ( LE_NOT_FOUND == fd ) {
		return 0;
	}
	le_atomFile_Close(fd);
	return 1;
}

void _writeMetricToFile
(
		char *fileContent
) {

	// If file size exceeded the highest limit
	// then delete the oldest entry and add the
	// latest one in the file
	if ( _isMetricsFileExist() ) {
		if (!_adjustMetricFileContent(fileContent)) {
			LE_ERROR("Content adjustment failed!!");
			return;
		}
	}
	// Create file before writing
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

		int size = strlen(fileContent);
		write(fd, fileContent,size);
	//	fileContent[size] = '\0';

		if (LE_OK != le_atomFile_Close(fd)) {
			LE_ERROR("Unable to write content in file.Error occurred while closing file %s",FILE_PATH);
			return;
		}
		LE_INFO("Content is successfully written into the file %s",FILE_PATH);
		LE_INFO("============================================================");
		LE_INFO("Content = %s",fileContent);
		LE_INFO("strlen Size = %d ,sizeof size = %d",strlen(fileContent),sizeof(fileContent));
		LE_INFO("============================================================");
	}
}

void _formJSON( char *content ) {
	char *sJSONForOthers = "{\"IMEI\":%s,\"IMSI\":%s,\"TEMP\":%d}";
	char *
}
void _getMetricInfo( char *content) {

	char paramOthersInfo[SIZE];
	char paramRadioInfo[SIZE];
//	char paramServingCellInfo[SIZE];
//	char paramNeighborCellInfo[SIZE];
//	char paramGPSInfo[SIZE];
	int size;

//	char temp[500];

	device_Info(paramOthersInfo);
	radio_Info(paramRadioInfo);
//	radio_ServingCellInfo(paramServingCellInfo);
//	radio_NeighbourInfo(paramNeighborCellInfo);
//	gps_GetLocation(paramGPSInfo);

	LE_INFO("========================================================");
	LE_DEBUG("Others Info		:: Size=%d ,Content = %s",strlen(paramOthersInfo),paramOthersInfo);
	LE_DEBUG("Radio Info		:: Size=%d ,Content = %s",strlen(paramRadioInfo),paramRadioInfo);
//	LE_DEBUG("Serving Cell Info :: Size=%d ,Content = %s",strlen(paramServingCellInfo),paramServingCellInfo);
//	LE_DEBUG("Neighbour Info	:: Size=%d ,Content = %s",strlen(paramNeighborCellInfo),paramNeighborCellInfo);
//	LE_DEBUG("GPS Info			:: Size=%d ,Content = %s",strlen(paramGPSInfo),paramGPSInfo);
	LE_INFO("========================================================");

	size = strlen(paramRadioInfo) + strlen(paramOthersInfo);
	snprintf(content,size+2+1,"%s,%s\n",paramRadioInfo,paramOthersInfo);

	LE_INFO("=================CONTENT=================");
	LE_INFO("%s",content);
	LE_INFO("=========================================");
//	size = (strlen(paramRadioInfo) + strlen(paramServingCellInfo)
//					+ strlen(paramNeighborCellInfo) +strlen(paramGPSInfo));
//	snprintf(content, size + 2+3+2, "{%s,%s,%s,%s}\n", paramRadioInfo,
//			paramServingCellInfo, paramNeighborCellInfo, paramGPSInfo);
//	snprintf(content, size + 2+3+15, "{\"radio\":{%s,%s,%s},\"loc\":{%s}}\n", paramRadioInfo,
//				paramServingCellInfo, paramNeighborCellInfo, paramGPSInfo);

//	snprintf(temp,500, "{%s,\"radio\":{%s,%s},\"loc\":{%s}}\n", paramRadioInfo,
//					paramServingCellInfo, paramNeighborCellInfo, paramGPSInfo);
//
//	snprintf(content,strlen(temp)+1,"%s",temp);
//	LE_INFO("content = %s",content);
//	LE_INFO("size = %d tempSize = %d",size,strlen(temp));
//	LE_INFO("temp = %s",temp);
}

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

void _getFileSizeAndContent
(
		char *fileName,
		size_t *size,
		char *buffer
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

int _adjustMetricFileContent
(
		char *newContent
) {
	int i = 0;
	int lastLineEndPos;
	int nextLineStartPos;
	size_t prevFileSize;

	char tempStr[SIZE];
	char prevContent[SIZE];


	// Return if file does not exist
	_getFileSizeAndContent(FILE_PATH, &prevFileSize,prevContent);
	if ( 0 == prevFileSize || 0 == strlen(prevContent) ) {
		return 0;
	}

	// Return if size is within limit
	if ( (prevFileSize + strlen(newContent)) < fileSizeLimitInBytes ) {
		LE_DEBUG(
				"Content adjustment is not required in the file %s.File size (%d bytes) is within highest limit (%d bytes)",
				FILE_PATH,prevFileSize, fileSizeLimitInBytes);
		return 1;
	}
	LE_WARN(
			"Total Size ( prev = %d bytes, current = %d bytes) of the payload file (%s) has exceeded the highest limit (%d bytes).Clearing oldest content from file",
			prevFileSize,strlen(newContent) , FILE_PATH, fileSizeLimitInBytes);

	// Find position of the first  new line character ('\n')
	for (i = 0; prevContent[i] != '\n'; i++) {
		continue;
	}

	nextLineStartPos = i + 1;
	lastLineEndPos = prevFileSize -1;
	int j = 0;
	i = nextLineStartPos;

	// Exclude first line from the file content and
	// store it in the temporary buffer
	while ( i <= lastLineEndPos ) {
		tempStr[j++] = prevContent[i++];
	}
	tempStr[j] = '\0';
	//LE_DEBUG("Temp string = %s length = %d",tempStr,strlen(tempStr));
	//LE_DEBUG("============================================================");

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
	LE_DEBUG("Oldest content is deleted from the file %s",FILE_PATH);
	LE_DEBUG("Current content of the file");
	LE_DEBUG("==================================================================================");
	LE_DEBUG("%s",tempStr);
	LE_DEBUG("==================================================================================");
	return 1;
}
int _readContentFromFile
(
		char *fileName
) {

	char buffer[SIZE];

	LE_INFO("Trying to open file %s ", fileName);
	int fd = le_atomFile_Open(fileName, LE_FLOCK_READ);
	switch (fd) {
	case LE_NOT_FOUND:
		LE_ERROR("Unable to open file %s (Mode = LE_FLOCK_READ)", fileName);
		return -1;
	case LE_FAULT:
		LE_ERROR("Unable to open file %s (Mode = LE_FLOCK_READ).Error occurred",
				fileName);
		return -1;
	default:
		LE_INFO("File %s is opened (Mode = LE_FLOCK_READ)", fileName);
		int n = read(fd, buffer,SIZE);
		buffer[SIZE-1] = '\0';
		buffer[n] = '\0';

		LE_INFO("===================================================");
		LE_INFO("No of bytes read n = %d",n);
		LE_INFO("%s", buffer);
		LE_INFO("===================================================");

		le_atomFile_Close(fd); // Transfers all changes to disk
	}

	int i = 0, numEntries = 0, numNull = 0;
	while (buffer[i++] != '\0') {
		LE_INFO(" ch = %d ",buffer[i]);
		if (buffer[i] == '\n') {
			numEntries++;
		}
		if (buffer[i] == '\0') {
			numNull++;
		}
	}
	LE_INFO("Number of entries = %d Number of Nulls = %d", numEntries, numNull);
	return 0;
}

COMPONENT_INIT {
	LE_INFO("============================================");
	LE_INFO("=============READ METRIC=================");
	LE_INFO("============================================");
	int numReadings = 0;

	if (0) {
	// Get total file size limit from agent configuration
	config_GetInteger(CONFIG_SECTION, CONFIG_KEY_FILE_SIZE_LIMIT,
	CONFIG_DEFAULT_FILE_SIZE_LIMIT, &fileSizeLimitInBytes);

	while ( ENABLE_READ_FLAG) {

		LE_DEBUG("---------------------Taking reading [%d]:", ++numReadings);
		_getMetricInfo(content);
		if (_isDifferentMetricFromPrev()) {

			LE_DEBUG(
					"---------------------Adding Entry (reading [%d]) in the file",
					numReadings);
			_writeMetricToFile(content);
			continue;
		}
		sleep( TIME_INTERVAL_IN_SEC_TO_READ_METRIC);
		if ( numReadings > 5 ) {
			break;
		}
	}
	}
	_getMetricInfo(content);

}
