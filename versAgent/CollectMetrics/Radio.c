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


/**
 * Get device information (IMEI, IMSI, Temperature and timestamp info
 */
void device_Info
(
		_deviceInfo *deviceInfo 	///< [OUT] 	Structure to store device information
)
{

	deviceInfo->timestamp = (unsigned) time(NULL);
	le_result_t res;
	deviceInfo->temperaturePtr = -1;

	//Temperature
	const char *sensorName = "POWER_CONTROLLER";
	le_temp_SensorRef_t sensorRef = le_temp_Request(sensorName);
	res = le_temp_GetTemperature(sensorRef, &deviceInfo->temperaturePtr);
	res = le_temp_GetTemperature(sensorRef, &deviceInfo->temperaturePtr);
	if (res == LE_OK) {
		LE_DEBUG("Temperature: %d", deviceInfo->temperaturePtr);
	} else {
		LE_ERROR("Unable to read Temperature!!");
	}

	//IMEI
	res = le_info_GetImei(deviceInfo->imei, SIZE_OF_IMEI_IMSI_NO);
	if (res == LE_OK)
		LE_DEBUG("IMEI: %s", deviceInfo->imei);
	res = le_sim_GetIMSI(LE_SIM_EXTERNAL_SLOT_1, deviceInfo->imsi,
			SIZE_OF_IMEI_IMSI_NO);
	if (res == LE_OK)
		LE_DEBUG("IMSI: %s", deviceInfo->imsi);
}

/**
 * Get radio information (RATS, BLER, BER, RSSI, RSRP, RSRQ, SNR)
 */
void radio_Info
(
		_radioInfo *radioInfo			///< [OUT] 	Structure to store radio information
)
{

	le_result_t res;
	le_mrc_Rat_t rat;
	int32_t valRsrq = 0;
	int32_t valRsrp = 0;
	int32_t valSnr = 0;

	radioInfo->rssi = 0;
	radioInfo->ber = 0;
	radioInfo->bler = 0;
	radioInfo->rsrp = 0;
	radioInfo->rsrq = 0;
	radioInfo->snr = 0;

	//Signal Matrices
	le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();

	//RAT
	res = le_mrc_GetRadioAccessTechInUse(&rat);
	if (res == LE_OK) {
		if ((rat >= LE_MRC_RAT_UNKNOWN) && (rat <= LE_MRC_RAT_LTE)) {
			switch (rat) {
			case LE_MRC_RAT_GSM:
				strcpy(radioInfo->ratS, "GSM");
				if (metricsRef != NULL) {
					res = le_mrc_GetGsmSignalMetrics(metricsRef,
							&radioInfo->rssi, &radioInfo->ber);
					if (res == LE_OK) {

						LE_DEBUG("GSM metrics rxLevel.%ddBm, er.%d",
								radioInfo->rssi, radioInfo->ber);
					} else {
						LE_ERROR("Unable to read RSSI!");
						radioInfo->rssi = NULL_VALUE;
						radioInfo->ber = NULL_VALUE;
					}
				} else {
					LE_ERROR("Returns NULL metric reference!");
					radioInfo->rssi = NULL_VALUE;
					radioInfo->ber = NULL_VALUE;
				}
				break;
			case LE_MRC_RAT_LTE:
				strcpy(radioInfo->ratS, "LTE");
				if (metricsRef != NULL) {
					res = le_mrc_GetLteSignalMetrics(metricsRef,
							&radioInfo->rssi, &radioInfo->bler, &valRsrq,
							&valRsrp, &valSnr);

					radioInfo->rsrq = (double) valRsrq / 10.0;
					radioInfo->rsrp = (double) valRsrp / 10.0;
					radioInfo->snr = (double) valSnr / 10.0;

					if (res == LE_OK) {

						LE_DEBUG(
								"LTE metrics rxLevel.%ddBm, er.%d, RSRQ:%lf, RSRP:%lf, SNR:%lf",
								radioInfo->rssi, radioInfo->bler,
								radioInfo->rsrq, radioInfo->rsrp,
								radioInfo->snr);
					} else {
						LE_ERROR("Unable to read RSSI!");
						radioInfo->rssi = NULL_VALUE;
						radioInfo->bler = NULL_VALUE;
						radioInfo->rsrq = NULL_VALUE;
						radioInfo->rsrp = NULL_VALUE;
						radioInfo->snr = NULL_VALUE;
					}
				} else {
					LE_ERROR("Returns NULL metric reference!");
					radioInfo->rssi = NULL_VALUE;
					radioInfo->bler = NULL_VALUE;
					radioInfo->rsrq = NULL_VALUE;
					radioInfo->rsrp = NULL_VALUE;
					radioInfo->snr = NULL_VALUE;
				}
				break;
			default:
				strcpy(radioInfo->ratS, "Unknown");
				radioInfo->rssi = NULL_VALUE;
				radioInfo->bler = NULL_VALUE;
				radioInfo->rsrq = NULL_VALUE;
				radioInfo->rsrp = NULL_VALUE;
				radioInfo->snr = NULL_VALUE;
				radioInfo->rssi = NULL_VALUE;
				radioInfo->ber = NULL_VALUE;

			}
			LE_DEBUG("le_mrc_GetRadioAccessTechInUse return rat 0x%02X", rat);
		}
	} else {
		LE_ERROR("An error occurred during rat fetch!");
		strcpy(radioInfo->ratS, "Unknown");
		radioInfo->rssi = NULL_VALUE;
		radioInfo->bler = NULL_VALUE;
		radioInfo->rsrq = NULL_VALUE;
		radioInfo->rsrp = NULL_VALUE;
		radioInfo->snr = NULL_VALUE;
		radioInfo->ber = NULL_VALUE;
	}
}

/**
 * Get serving cell information (LAC, TAC, CellID)
 */
void radio_ServingCellInfo
(
		_radioServingCellInfo *radioServingCellInfo		///< [OUT] 	Structure to store serving cell information
)
{

	radioServingCellInfo->cellId = le_mrc_GetServingCellId();
	LE_DEBUG("le_mrc_GetServingCellId returns cellId.%d",
			radioServingCellInfo->cellId);
	radioServingCellInfo->lac = le_mrc_GetServingCellLocAreaCode();
	LE_DEBUG("le_mrc_GetServingCellLocAreaCode returns lac.%d",
			radioServingCellInfo->lac);
	radioServingCellInfo->tac = le_mrc_GetServingCellLteTracAreaCode();
	LE_DEBUG("le_mrc_GetServingCellLteTracAreaCode returns Tac.0x%X (%d)",
			radioServingCellInfo->tac, radioServingCellInfo->tac);
}

/**
 * Get neighbor cell information (LAC, CID, RxLevel, ECIO, IntraRSRP, IntraRSRQ, InterRSRP, InterRSRQ)
 */
void radio_NeighbourInfo
(
		_radioNeighborInfo *radioNeighborInfo,	///< [OUT] 	Structure pointer to store all neighbor cell information
		int *numNeighbor						///< [OUT] 	Number of neighbor cells
)
{

	uint32_t i = 0;
	le_result_t res;
	le_mrc_NeighborCellsRef_t ngbrRef;
	le_mrc_CellInfoRef_t cellRef;
	le_mrc_Rat_t rat = 0;
	int32_t valRsrp = 0;
	int32_t valRsrq = 0;
	int32_t valEcio = 0;

	radioNeighborInfo[0].lac = 0;
	radioNeighborInfo[0].rxLevel = 0;
	radioNeighborInfo[0].ecio = 0;
	radioNeighborInfo[0].intraRsrp = 0;
	radioNeighborInfo[0].intraRsrq = 0;
	radioNeighborInfo[0].interRsrp = 0;
	radioNeighborInfo[0].interRsrq = 0;

	LE_DEBUG("Start Testle_mrc_GetNeighborCellsInfo");

	ngbrRef = le_mrc_GetNeighborCellsInfo();

	if (ngbrRef) {
		i = 0;
		cellRef = le_mrc_GetFirstNeighborCellInfo(ngbrRef);
		LE_ASSERT(cellRef);
		radioNeighborInfo[0].cid = le_mrc_GetNeighborCellId(cellRef);
		radioNeighborInfo[0].lac = le_mrc_GetNeighborCellLocAreaCode(
				cellRef);
		radioNeighborInfo[0].rxLevel = le_mrc_GetNeighborCellRxLevel(
				cellRef);
		rat = le_mrc_GetNeighborCellRat(cellRef);
		LE_DEBUG("Cell #%d, cid.%d, lac.%d, rxLevel.%ddBm, RAT.%d", i,
				radioNeighborInfo[0].cid, radioNeighborInfo[0].lac,
				radioNeighborInfo[0].rxLevel, rat);
		// Specific values for UMTS and LTE
		switch (rat) {
		case LE_MRC_RAT_UMTS:
			strcpy(radioNeighborInfo->ratS, "RAT_UMTS");
			valEcio = le_mrc_GetNeighborCellUmtsEcIo(cellRef);
			radioNeighborInfo[0].ecio = (float) valEcio / 10.0;
			LE_DEBUG("Cell #%d, UMTS EcIo.%010.1fdB", i,
					radioNeighborInfo[0].ecio);
			break;

		case LE_MRC_RAT_LTE:
			strcpy(radioNeighborInfo[0].ratS, "LTE");
			res = le_mrc_GetNeighborCellLteIntraFreq(cellRef, &valRsrq,
					&valRsrp);
			LE_ASSERT(res == LE_OK)
			;
			radioNeighborInfo[0].intraRsrq = (float) valRsrq / 10.0;
			radioNeighborInfo[0].intraRsrp = (float) valRsrp / 10.0;

			res = le_mrc_GetNeighborCellLteInterFreq(cellRef, &valRsrq,
					&valRsrp);
			LE_ASSERT(res == LE_OK)
			;
			radioNeighborInfo[0].interRsrq = (float) valRsrq / 10;
			radioNeighborInfo[0].interRsrp = (float) valRsrp / 10;
			LE_DEBUG(
					"Cell #%d, Test LTE Intra-RSRQ.%010.1fdB, Intra-RSRP.%010.1fdBm, "
							"Inter-RSRQ.%010.1fdB, Inter-RSRP.%010.1fdBm", i,
					radioNeighborInfo[0].intraRsrq,
					radioNeighborInfo[0].intraRsrp,
					radioNeighborInfo[0].interRsrq,
					radioNeighborInfo[0].interRsrp);
			break;
		default:
			LE_DEBUG("Nothing more to display");
			break;
		}

		while ((cellRef = le_mrc_GetNextNeighborCellInfo(ngbrRef)) != NULL) {
			i++;
			radioNeighborInfo[i].lac = 0;
			radioNeighborInfo[i].rxLevel = 0;
			radioNeighborInfo[i].ecio = 0;
			radioNeighborInfo[i].intraRsrp = 0;
			radioNeighborInfo[i].intraRsrq = 0;
			radioNeighborInfo[i].interRsrp = 0;
			radioNeighborInfo[i].interRsrq = 0;
			valRsrq = 0;
			valRsrp = 0;

			radioNeighborInfo[i].cid = le_mrc_GetNeighborCellId(cellRef);
			radioNeighborInfo[i].lac = le_mrc_GetNeighborCellLocAreaCode(
					cellRef);
			radioNeighborInfo[i].rxLevel = le_mrc_GetNeighborCellRxLevel(
					cellRef);

			rat = le_mrc_GetNeighborCellRat(cellRef);

			LE_DEBUG("Cell #%d, cid.%d, lac.%d, rxLevel.%ddBm, RAT.%d", i,
					radioNeighborInfo[i].cid, radioNeighborInfo[i].lac,
					radioNeighborInfo[i].rxLevel, rat);
			// Specific values for UMTS and LTE

			switch (rat) {
			case LE_MRC_RAT_UMTS:
				strcpy(radioNeighborInfo[i].ratS, "RAT_UMTS");
				valEcio = le_mrc_GetNeighborCellUmtsEcIo(cellRef);
				LE_DEBUG("Cell #%d, UMTS EcIo.%010.1fdB", i,
						radioNeighborInfo[i].ecio);
				radioNeighborInfo[i].ecio = (float) valEcio / 10.0;
				break;

			case LE_MRC_RAT_LTE:
				strcpy(radioNeighborInfo[i].ratS, "LTE");
				res = le_mrc_GetNeighborCellLteIntraFreq(cellRef, &valRsrq,
						&valRsrp);
				LE_ASSERT(res == LE_OK)
				;
				radioNeighborInfo[i].intraRsrq = (float) valRsrq / 10.0;
				radioNeighborInfo[i].intraRsrp = (float) valRsrp / 10.0;
				res = le_mrc_GetNeighborCellLteInterFreq(cellRef, &valRsrq,
						&valRsrp);
				LE_ASSERT(res == LE_OK)
				;
				radioNeighborInfo[i].interRsrq = (float) valRsrq / 10.0;
				radioNeighborInfo[i].interRsrp = (float) valRsrp / 10.0;
				LE_DEBUG(
						"Cell #%d, LTE Intra-RSRQ.%010.1fdB, Intra-RSRP.%010.1fdBm, "
								"Inter-RSRQ.%010.1fdB, Inter-RSRP.%010.1fdBm",
						i, radioNeighborInfo[i].intraRsrq,
						radioNeighborInfo[i].intraRsrp,
						radioNeighborInfo[i].interRsrq,
						radioNeighborInfo[i].interRsrp);

				break;

			default:
				strcpy(radioNeighborInfo[i].ratS, "Default");
				LE_DEBUG("Nothing more to display");
				break;
			}

		}
		//le_mrc_DeleteNeighborCellsInfo(ngbrRef);
	}
	*numNeighbor = i + 1;
}

