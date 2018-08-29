#include "legato.h"
#include "interfaces.h"

#define NULL_VALUE -999
#define _INTERFACE_NAME_MAX_BYTES 100

typedef struct {
	bool isConnected;
	char interfaceName[_INTERFACE_NAME_MAX_BYTES];
} ConnStateData_t;

static le_data_RequestObjRef_t requestObj;

COMPONENT_INIT {
}


bool radio_IsConnected(){
	return requestObj!=NULL;
}

/**
 * Request for data connection using modem service(LE_DATA_CELLULAR).
 */
void radio_Connect() {
	le_result_t res;
	LE_DEBUG("Setting technology rank");
	le_data_Technology_t technology = le_data_GetTechnology();
	if (technology == LE_DATA_CELLULAR) {
		res = le_data_SetTechnologyRank(1, LE_DATA_CELLULAR);
		if (res != LE_OK) {
			LE_ERROR("Unable to connect data service!");
		}
		LE_DEBUG("Successfully set technology rank CELLULAR_DATA to 1");
	}
	LE_DEBUG("Attempt to connect to data");
	requestObj = le_data_Request();
}

/**
 * Request for disconnection of data service.
 */
void radio_Disconnect() {
	if (requestObj != NULL)
		le_data_Release(requestObj);
	requestObj = NULL;
}

/**
 * Remove data connection handler
 */
void radio_RemoveDataConnectionStateHandler(
		radio_DataConnectionStateHandlerRef_t handlerRef) {
	le_event_RemoveHandler((le_event_HandlerRef_t) handlerRef);
}

/**
 * Add new data connection handler
 */
radio_DataConnectionStateHandlerRef_t radio_AddDataConnectionStateHandler(
		radio_DataConnectionStateHandlerFunc_t handlerPtr, void* contextPtr) {
	LE_DEBUG("Adding data connection/disconnection handler");
	le_data_ConnectionStateHandlerRef_t handler =
			le_data_AddConnectionStateHandler(
					(le_data_ConnectionStateHandlerFunc_t) handlerPtr,
					contextPtr);
	if (handler != NULL) {
		LE_DEBUG("Successfully added data connection/disconnection handler");
	} else {
		LE_ERROR("Failed to add handler!");
	}
	return (radio_DataConnectionStateHandlerRef_t) (handler);
}

/**
 * Get radio info of current time for GSM or LTE
 */
void radio_Info(char* params, size_t size) {
	le_result_t res;
	le_mrc_Rat_t rat;

	const int32_t imeiImsiSize = 20;
	char ratS[8];
	char imei[imeiImsiSize];
	char imsi[imeiImsiSize];
	int32_t rssi = 0;
	uint32_t ber = 0;
	uint32_t bler = 0;
	int32_t rsrq = 0;
	int32_t rsrp = 0;
	int32_t snr = 0;

	//Temperature
	const char *sensorName = "POWER_CONTROLLER";
	le_temp_SensorRef_t sensorRef = le_temp_Request(sensorName);
	int32_t temperaturePtr = -1;
	res = le_temp_GetTemperature(sensorRef, &temperaturePtr);
	if (res == LE_OK) {
		LE_INFO("Temperature: %d", temperaturePtr);
	} else {
		LE_ERROR("Unable to read Temperature!!");
	}

	//IMEI

	res = le_info_GetImei(imei, imeiImsiSize);
	if (res == LE_OK)
		LE_DEBUG("IMEI: %s", imei);
	res = le_sim_GetIMSI(LE_SIM_EXTERNAL_SLOT_1 , imsi, imeiImsiSize);
	if (res == LE_OK)
			LE_DEBUG("IMSI: %s", imsi);

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

						LE_INFO("GSM metrics rxLevel.%ddBm, er.%d", rssi, ber);
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
				snprintf(params, size, "%s,%s,%s,%d,%d,%d", imei, imsi, ratS, temperaturePtr,
						rssi, ber);
				break;
			case LE_MRC_RAT_LTE:
				strcpy(ratS, "LTE");
				if (metricsRef != NULL) {
					res = le_mrc_GetLteSignalMetrics(metricsRef, &rssi, &bler,
							&rsrq, &rsrp, &snr);
					if (res == LE_OK) {

						LE_INFO(
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
				snprintf(params, size, "%s,%s,%s,%d,%d,%d,%d,%d,%d", imei, imsi, ratS,
						temperaturePtr, rssi, rsrp, rsrq, snr, bler);
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
				snprintf(params, size, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d", imei, imsi, ratS,
						temperaturePtr, rssi, ber, rsrp, rsrq, snr, bler);

			}
			LE_INFO("le_mrc_GetRadioAccessTechInUse return rat 0x%02X", rat);
		}
	} else {
		LE_ERROR("An error occurred during rat fetch!");
		strcpy(ratS, "Unknown");
		rssi = NULL_VALUE;
		bler = NULL_VALUE;
		rsrq = NULL_VALUE;
		rsrp = NULL_VALUE;
		snr = NULL_VALUE;
		rssi = NULL_VALUE;
		ber = NULL_VALUE;
		snprintf(params, size, "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d", imei, imsi, ratS, temperaturePtr,
				rssi, ber, rsrp, rsrq, snr, bler);
	}

}

void radio_NeighbourInfo() {
	uint32_t cellId, lac;
	uint16_t tac;

	cellId = le_mrc_GetServingCellId();
	LE_INFO("le_mrc_GetServingCellId returns cellId.%d", cellId);
	lac = le_mrc_GetServingCellLocAreaCode();
	LE_INFO("le_mrc_GetServingCellLocAreaCode returns lac.%d", lac);
	tac = le_mrc_GetServingCellLteTracAreaCode();
	LE_INFO("le_mrc_GetServingCellLteTracAreaCode returns Tac.0x%X (%d)", tac,
			tac);
	le_result_t res;
	le_mrc_NeighborCellsRef_t ngbrRef = le_mrc_GetNeighborCellsInfo();
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
	cellRef = le_mrc_GetNextNeighborCellInfo(ngbrRef);
	cid = le_mrc_GetNeighborCellId(cellRef);
	lac = le_mrc_GetNeighborCellLocAreaCode(cellRef);
	rxLevel = le_mrc_GetNeighborCellRxLevel(cellRef);
	rat = le_mrc_GetNeighborCellRat(cellRef);
	LE_INFO("Cell #%d, cid.%d, lac.%d, rxLevel.%ddBm, RAT.%d", i, cid, lac,
			rxLevel, rat);
	// Specific values for UMTS and LTE
	switch (rat) {
	case LE_MRC_RAT_UMTS:
		ecio = le_mrc_GetNeighborCellUmtsEcIo(cellRef);
		LE_INFO("Cell #%d, UMTS EcIo.%010.1fdB", i, ((double )ecio / 10));
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

		LE_INFO("Cell #%d, LTE Intra-RSRQ.%010.1fdB, Intra-RSRP.%010.1fdBm, "
				"Inter-RSRQ.%010.1fdB, Inter-RSRP.%010.1fdBm", i,
				((double )intraRsrq / 10), ((double )intraRsrp / 10),
				((double )interRsrq / 10), ((double )interRsrp / 10));
		break;

	default:
		LE_INFO("Nothing more to display");
		break;
	}
	le_mrc_DeleteNeighborCellsInfo(ngbrRef);
}
