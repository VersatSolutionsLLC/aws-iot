/*
 * cversAgentHeader.h
 *
 *  Created on: Sep 28, 2018
 *      Author: Admin
 */

#ifndef VERSAGENTHEADER_H_
#define VERSAGENTHEADER_H_

#define SIZE_OF_IMEI_IMSI_NO 					20
#define ENABLE_READ_FLAG 						1
#define ENABLE_PUBLISH_FLAG 					1
#define TIME_INTERVAL_IN_SEC_TO_READ_METRIC 	2
#define TIME_INTERVAL_IN_SEC_TO_PUBLISH_DATA	4
#define TEMP_FILE_PATH 							"/data/le_fs/metrics.json.temp"
#define FILE_PATH 								"/data/le_fs/metrics.json"

#define SIZE_LIMIT_OF_PUBLISH_DATA 				256
#define SIZE_LIMIT_OF_KEY_FIELD_B				100
#define SIZE 									1024
#define PAYLOAD_SIZE 							1024

#define CONFIG_SECTION 							"agent_1"
#define CONFIG_KEY_FILE_SIZE_LIMIT 				"FileSizeLimitInBytes"
#define CONFIG_KEY_PUBLISH_DATA_LIMIT			"PublishDataLimitInBytes"
#define CONFIG_KEY_TOPIC_NAME 					"Topic"
#define CONFIG_KEY_QOS_VALUE 					"QOS"

#define CONFIG_DEFAULT_FILE_SIZE_LIMIT 			2048
#define CONFIG_DEFAULT_PUBLISH_DATA_LIMIT 		1024
#define CONFIG_DEFAULT_TOPIC_NAME 				"VersatIOT"
#define CONFIG_DEFAULT_QOS_VALUE 				1
#define MAX_NO_OF_NEIGHBOR_CELL					5

typedef struct deviceInfoStruct {
	char imei[SIZE_OF_IMEI_IMSI_NO];
	char imsi[SIZE_OF_IMEI_IMSI_NO];
	int32_t temperaturePtr;
	u_int32_t timestamp;
} _deviceInfo;

typedef struct radioInfoStruct {
		char ratS[8];
		int32_t rssi;
		uint32_t ber;
		uint32_t bler;
		float rsrq;
		float rsrp;
		float snr;
} _radioInfo;

typedef struct radioServingCellInfoStruct {
	uint32_t cellId;
	uint32_t lac;
	uint16_t tac;
} _radioServingCellInfo;

typedef struct radioNeighborInfoStruct {
	char ratS[15];
	uint32_t lac;
	uint32_t cid;
	int32_t rxLevel;
	float ecio;
	float intraRsrp;
	float intraRsrq;
	float interRsrp;
	float interRsrq;
} _radioNeighborInfo;

typedef struct gpsInfoStruct {
	float latitude;
	float longitude;
	float altitude;
	float altitudeOnWgs84;
	float hAccuracy;
	float vAccuracy;

} _gpsInfo;

LE_SHARED void device_Info(_deviceInfo*);
LE_SHARED void radio_Info(_radioInfo*);
LE_SHARED void radio_ServingCellInfo(_radioServingCellInfo*);
LE_SHARED void radio_NeighbourInfo(_radioNeighborInfo*,int*);
LE_SHARED void gps_GetLocation(_gpsInfo*);

#endif /* VERSAGENTHEADER_H_ */
