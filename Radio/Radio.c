#include "legato.h"
#include "interfaces.h"

#define NULL_VALUE -999

COMPONENT_INIT {
}



void radio_Info(char* params, size_t size) {
	le_result_t res;
	le_mrc_Rat_t rat;
	char ratS[8];
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
				snprintf(params, size, "%s,%d,%d,%d", ratS, temperaturePtr,
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
				snprintf(params, size, "%s,%d,%d,%d,%d,%d,%d", ratS,
						temperaturePtr,rssi, rsrp, rsrq, snr, bler);
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
				snprintf(params, size, "%s,%d,%d,%d,%d,%d,%d,%d", ratS,
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
		snprintf(params, size, "%s,%d,%d,%d,%d,%d,%d,%d", ratS, temperaturePtr,
				rssi, ber, rsrp, rsrq, snr, bler);
	}

}
