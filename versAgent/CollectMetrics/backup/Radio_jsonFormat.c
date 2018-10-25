/*
 * readRadioParam.c
 *
 *  Created on: Sep 20, 2018
 *      Author: Admin
 */

#include "legato.h"
#include "versAgentHeader.h"
#include "interfaces.h"

#define NULL_VALUE -999
#define _INTERFACE_NAME_MAX_BYTES 100
#define SIZE 1024

char previousParams[1024];
const char* _FRM_JSON =
		"{\"temp\":%s,\"%s\":%s,\"timestamp\":%d,\"loc\":%s,\"imei\":%s,\"imsi\":%s}";
const char* _FRM_JSON_GSM = "{\"rssi\":%s,\"ber\":%s}";
const char* _FRM_JSON_LTE =
		"{\"rssi\":%s,\"rsrp\":%s,\"rsrq\":%s,\"snr\":%s,\"bler\":%s}";
const char* _FRM_JSON_LOC =
		"{\"lat\":%s,\"lon\":%s,\"alt\":%s,\"altWgs84\":%s,\"hAcc\":%s,\"vAcc\":%s}";

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
	//char locationJson[1024];

	radio_Info(params);
	//radio_NeighbourInfo();
	if (strcmp(previousParams, params) == 0) {
		strcpy(previousParams, params);
		return 0;
	}
	strcpy(previousParams, params);
	char** tokens = _strSplit(params, ',');
//	int imei = 0;
//	int imsi = 1;
	int ratPos = 2;
//	int temperature = 3;
	int rssi = 4;

	int rsrp = 5;
	int rsrq = 6;
	int snr = 7;


	int ber = 5;int bler = 8;

	char *rat = *(tokens + ratPos);
//	u_int32_t timestamp = (unsigned) time(NULL);

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

//	int lat = 0;
//	int lon = 1;
//	int alt = 2;
//	int altWgs84 = 3;
//	int hAcc = 4;
//	int vAcc = 5;

//	gps_GetLocation(params, 1024);
//	char** gpsTokens = _strSplit(params, ',');
//	snprintf(locationJson, jsonLength, _FRM_JSON_LOC, *(gpsTokens + lat),
//			*(gpsTokens + lon), *(gpsTokens + alt), *(gpsTokens + altWgs84),
//			*(gpsTokens + hAcc), *(gpsTokens + vAcc));
//
//	if (json != NULL)
//		snprintf(json, jsonLength, _FRM_JSON, *(tokens + temperature), rat,
//				radioJson, timestamp, locationJson, *(tokens + imei), *(tokens + imsi));
//	free(tokens);
//
//	free(gpsTokens);
	return strlen(json);
}
void other_Info(char* params) {
	le_result_t res;

	const int32_t imeiImsiSIZE = 20;
	char imei[imeiImsiSIZE];
	char imsi[imeiImsiSIZE];

	//Temperature
	const char *sensorName = "POWER_CONTROLLER";
	le_temp_SensorRef_t sensorRef = le_temp_Request(sensorName);
	int32_t temperaturePtr = -1;
	res = le_temp_GetTemperature(sensorRef, &temperaturePtr);
	if (res == LE_OK) {
		LE_DEBUG("Temperature: %d", temperaturePtr);
	} else {
		LE_ERROR("Unable to read Temperature!!");
	}

	//IMEI
	res = le_info_GetImei(imei, imeiImsiSIZE);
	if (res == LE_OK)
		LE_DEBUG("IMEI: %s", imei);
	res = le_sim_GetIMSI(LE_SIM_EXTERNAL_SLOT_1, imsi, imeiImsiSIZE);
	if (res == LE_OK)
		LE_DEBUG("IMSI: %s", imsi);
	snprintf(params, SIZE, "%s,%s,%d", imei, imsi, temperaturePtr);
}
void radio_Info(char* params) {
	le_result_t res;
	le_mrc_Rat_t rat;

	char ratS[8];
	int32_t rssi = 0;
	uint32_t ber = 0;
	uint32_t bler = 0;
	int32_t rsrq = 0;
	int32_t rsrp = 0;
	int32_t snr = 0;


	//Signal Matrices
	le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();

	//RAT
	res = le_mrc_GetRadioAccessTechInUse(&rat);
	if (res == LE_OK) {
		if ((rat >= LE_MRC_RAT_UNKNOWN) && (rat <= LE_MRC_RAT_LTE)) {
			switch (rat) {
			case LE_MRC_RAT_GSM:
				strcpy(ratS, "GSM");
				if (metricsRef != NULL) {
					res = le_mrc_GetGsmSignalMetrics(metricsRef, &rssi, &ber);
					if (res == LE_OK) {

						LE_DEBUG("GSM metrics rxLevel.%ddBm, er.%d", rssi, ber);
					} else {
						LE_ERROR("Unable to read RSSI!");
						rssi = NULL_VALUE;
						ber = NULL_VALUE;
					}
				} else {
					LE_ERROR("Returns NULL metric reference!");
					rssi = NULL_VALUE;
					ber = NULL_VALUE;
				}
				snprintf(params, SIZE, "%s,%d,%d", ratS, rssi, ber);
//				snprintf(params, SIZE,
//						"\"IMEI\":%s,\"IMSI\":%s,\"RATS\":\"%s\",\"TEMP\":,%d,\"RSSI\":%d,\"BER\":%d",
//						imei, imsi, ratS, temperaturePtr, rssi, ber);
				break;
			case LE_MRC_RAT_LTE:
				strcpy(ratS, "LTE");
				if (metricsRef != NULL) {
					res = le_mrc_GetLteSignalMetrics(metricsRef, &rssi, &bler,
							&rsrq, &rsrp, &snr);
					if (res == LE_OK) {

						LE_DEBUG(
								"LTE metrics rxLevel.%ddBm, er.%d, RSRQ:%d, RSRP:%d, SNR:%d",
								rssi, bler, rsrq, rsrp, snr);
					} else {
						LE_ERROR("Unable to read RSSI!");
						rssi = NULL_VALUE;
						bler = NULL_VALUE;
						rsrq = NULL_VALUE;
						rsrp = NULL_VALUE;
						snr = NULL_VALUE;
					}
				} else {
					LE_ERROR("Returns NULL metric reference!");
					rssi = NULL_VALUE;
					bler = NULL_VALUE;
					rsrq = NULL_VALUE;
					rsrp = NULL_VALUE;
					snr = NULL_VALUE;
				}
				snprintf(params, SIZE, "%s,%d,%d,%d,%d,%d", ratS, rssi, rsrp,
						rsrq, snr, bler);
//				snprintf(params, SIZE, "\"IMEI\":%s,\"IMSI\":%s,\"RATS\":\"%s\",\"TEMP\":%d,\"RSSI\":%d,\"RSRP\":%d,\"RSRQ\":%d,\"SNR\":%d,\"BLER\":%d", imei, imsi,
//									ratS, temperaturePtr, rssi, rsrp, rsrq, snr, bler);
				break;
			default:
				strcpy(ratS, "Unknown");
				rssi = NULL_VALUE;
				bler = NULL_VALUE;
				rsrq = NULL_VALUE;
				rsrp = NULL_VALUE;
				snr = NULL_VALUE;
				rssi = NULL_VALUE;
				ber = NULL_VALUE;
//				snprintf(params, SIZE, "\"IMEI\":%s,\"IMSI\":%s,\"RATS\":\"%s\",\"TEMP\":%d,\"RSSI\":%d,\"BER\":%d,\"RSRP\":%d,\"RSRQ\":%d,\"SNR\":%d,\"BLER\":%d", imei,
//						imsi, ratS, temperaturePtr, rssi, ber, rsrp, rsrq, snr,
//						bler);
				snprintf(params, SIZE, ":%s,%d,%d,%d,%d,%d,%d",ratS,rssi, ber, rsrp, rsrq, snr,
										bler);

			}
			LE_DEBUG("le_mrc_GetRadioAccessTechInUse return rat 0x%02X", rat);
		}
	} else {
		LE_ERROR("An error occurred during rat fetch!");
		strcpy(ratS, "Unknown");
		rssi = NULL_VALUE;
		bler = NULL_VALUE;
		rsrq = NULL_VALUE;
		rsrp = NULL_VALUE;
		snr = NULL_VALUE;
		ber = NULL_VALUE;
		snprintf(params, SIZE, "%s,%d,%d,%d,%d,%d,%d",
				ratS,rssi, ber, rsrp, rsrq, snr, bler);
//		snprintf(params, SIZE, "\"IMEI:\"%s,\"IMSI:\"%s,\"RATS\":\"%s\",\"TEMP:\"%d,\"RSSI:\"%d,\"BER:\"%d,\"RSRP:\"%d,\"RSRQ:\"%d,\"SNR:\"%d,\"BLER:\"%d", imei, imsi,
//						ratS, temperaturePtr, rssi, ber, rsrp, rsrq, snr, bler);
	}

}

void radio_ServingCellInfo(char* params) {
	uint32_t cellId, lac;
	uint16_t tac;

	cellId = le_mrc_GetServingCellId();
	LE_DEBUG("le_mrc_GetServingCellId returns cellId.%d", cellId);
	lac = le_mrc_GetServingCellLocAreaCode();
	LE_DEBUG("le_mrc_GetServingCellLocAreaCode returns lac.%d", lac);
	tac = le_mrc_GetServingCellLteTracAreaCode();
	LE_DEBUG("le_mrc_GetServingCellLteTracAreaCode returns Tac.0x%X (%d)", tac,
			tac);
	snprintf(params, SIZE, "\"CELLID\":%d,\"LAC\":%d,\"TAC\":%d", cellId, lac,	tac);
}

/**
 * Concatenate two string
 */
void _concatenate(
		char* s1,	///< [IN]  String
		char* s2	///< [IN]  String
		)
{
	int i, j;

	i = 0;
	while( s1[i]!='\0')
	{
		i++;
	}

  	j = 0;
  	while( s2[j]!='\0')
  	{
  		s1[i] = s2[j];
  		i++;
  		j++;
  	}
  	s1[i] = '\0';
}


void radio_NeighbourInfo(char* params) {

	uint32_t lac;

	le_result_t res;
	le_mrc_NeighborCellsRef_t ngbrRef;
	le_mrc_CellInfoRef_t cellRef;
	uint32_t i = 0;
	uint32_t cid = 0;
	lac = 0;
	int32_t rxLevel = 0;
	le_mrc_Rat_t rat = 0;
	int32_t ecio = 0;
	int32_t intraRsrp = 0;
	int32_t intraRsrq = 0;
	int32_t interRsrp = 0;
	int32_t interRsrq = 0;

	LE_DEBUG("Start Testle_mrc_GetNeighborCellsInfo");

	ngbrRef = le_mrc_GetNeighborCellsInfo();

	if (ngbrRef) {
		i = 0;

		cellRef = le_mrc_GetFirstNeighborCellInfo(ngbrRef);
		LE_ASSERT(cellRef);
		cid = le_mrc_GetNeighborCellId(cellRef);
		lac = le_mrc_GetNeighborCellLocAreaCode(cellRef);
		rxLevel = le_mrc_GetNeighborCellRxLevel(cellRef);
		rat = le_mrc_GetNeighborCellRat(cellRef);
		LE_DEBUG("Cell #%d, cid.%d, lac.%d, rxLevel.%ddBm, RAT.%d", i, cid, lac,
				rxLevel, rat);
		// Specific values for UMTS and LTE
		switch (rat) {
		case LE_MRC_RAT_UMTS:
			ecio = le_mrc_GetNeighborCellUmtsEcIo(cellRef);
			LE_DEBUG("Cell #%d, UMTS EcIo.%010.1fdB", i, ((double )ecio / 10));
			break;

		case LE_MRC_RAT_LTE:
			res = le_mrc_GetNeighborCellLteIntraFreq(cellRef, &intraRsrq,
					&intraRsrp);
			//snprintf(params, SIZE, "%d,LTE,%d,%d,%d", i,cid, lac, rxLevel);
			snprintf(params, SIZE, "\"CellNo\":%d,\"NetworkType\":\"LTE\",\"CID\":%d,\"LAC\":%d,\"RXLevel\":%d", i,cid, lac, rxLevel);
			LE_ASSERT(res == LE_OK)
			;
			res = le_mrc_GetNeighborCellLteInterFreq(cellRef, &interRsrq,
					&interRsrp);
			LE_ASSERT(res == LE_OK)
			;

			LE_DEBUG(
					"Cell #%d, LTE Intra-RSRQ.%010.1fdB, Intra-RSRP.%010.1fdBm, "
							"Inter-RSRQ.%010.1fdB, Inter-RSRP.%010.1fdBm", i,
					((double )intraRsrq / 10), ((double )intraRsrp / 10),
					((double )interRsrq / 10), ((double )interRsrp / 10));
			//snprintf(params,SIZE, "%d,%d,%d,%d",i,cid, lac, rxLevel);
			snprintf(params,SIZE, "\"CellNo\":%d,\"CID\":%d,\"LAC\":%d,\"RXLevel\":%d",i,cid, lac, rxLevel);
			break;

		default:
			LE_DEBUG("Nothing more to display");
			break;
		}

		while ((cellRef = le_mrc_GetNextNeighborCellInfo(ngbrRef)) != NULL) {
			i++;
			cid = le_mrc_GetNeighborCellId(cellRef);
			lac = le_mrc_GetNeighborCellLocAreaCode(cellRef);
			rxLevel = le_mrc_GetNeighborCellRxLevel(cellRef);
			rat = le_mrc_GetNeighborCellRat(cellRef);
			LE_DEBUG("Cell #%d, cid.%d, lac.%d, rxLevel.%ddBm, RAT.%d", i, cid,
					lac, rxLevel, rat);
			// Specific values for UMTS and LTE
			switch (rat) {
			case LE_MRC_RAT_UMTS:
				ecio = le_mrc_GetNeighborCellUmtsEcIo(cellRef);
				LE_DEBUG("Cell #%d, UMTS EcIo.%010.1fdB", i,
						((double )ecio / 10));
				break;

			case LE_MRC_RAT_LTE:
				res = le_mrc_GetNeighborCellLteIntraFreq(cellRef, &intraRsrq,
						&intraRsrp);
				LE_ASSERT(res == LE_OK)
				;
				res = le_mrc_GetNeighborCellLteInterFreq(cellRef, &interRsrq,
						&interRsrp);
				LE_ASSERT(res == LE_OK)
				;

				LE_DEBUG(
						"Cell #%d, LTE Intra-RSRQ.%010.1fdB, Intra-RSRP.%010.1fdBm, "
								"Inter-RSRQ.%010.1fdB, Inter-RSRP.%010.1fdBm",
						i, ((double )intraRsrq / 10), ((double )intraRsrp / 10),
						((double )interRsrq / 10), ((double )interRsrp / 10));
				break;

			default:
				LE_DEBUG("Nothing more to display");
				break;
			}
		}

		le_mrc_DeleteNeighborCellsInfo(ngbrRef);
	}
}

