#include "legato.h"
#include "interfaces.h"

COMPONENT_INIT {
}

void radio_Rat(char* ratS, size_t size) {
	le_result_t res;
	le_mrc_Rat_t rat;

	res = le_mrc_GetRadioAccessTechInUse(&rat);
	if (res == LE_OK) {
		if ((rat >= LE_MRC_RAT_UNKNOWN) && (rat <= LE_MRC_RAT_LTE)) {
			switch (rat) {
			case LE_MRC_RAT_GSM:
				//ratS = (char *)malloc(3);
				strcpy(ratS,"GSM");
				break;
			default:
				//ratS = (char *)malloc(7);
				strcpy(ratS , "Unknown");

			}
			LE_INFO("le_mrc_GetRadioAccessTechInUse return rat 0x%02X", rat);
		}
	} else {
		LE_ERROR("An error occurred during rat fetch!");
		//ratS = (char *)malloc(7);
		strcpy(ratS , "Unknown");
	}
	//ratS = "Unknown";
}

int32_t radio_Rssi() {
	le_result_t res;
	int32_t rssi = 0;
	uint32_t ber = 0;

	le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();
	if (metricsRef != NULL) {

		res = le_mrc_GetGsmSignalMetrics(metricsRef, &rssi, &ber);
		if (res == LE_OK){

			LE_INFO("GSM metrics rxLevel.%ddBm, er.%d", rssi, ber);
			return rssi;
		}
		else{
			LE_ERROR("Unable to read RSSI!");
			return -1;
		}
	}
	else{
		LE_ERROR("Returns NULL metric reference!");
		return -1;
	}
}

uint32_t radio_Ber() {
	le_result_t res;
	int32_t rssi = 0;
	uint32_t ber = 0;

	le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();
	if (metricsRef != NULL) {

		res = le_mrc_GetGsmSignalMetrics(metricsRef, &rssi, &ber);
		if (res == LE_OK){

			LE_INFO("GSM metrics rxLevel.%ddBm, er.%d", rssi, ber);
			return ber;
		}
		else{
			LE_ERROR("Unable to read RSSI!");
			return -1;
		}
	}
	else{
		LE_ERROR("Returns NULL metric reference!");
		return -1;
	}
}




int32_t radio_Signal() {
	le_result_t res;
	le_onoff_t onoff;

	int32_t rssi = 0;
	uint32_t ber = 0;

	le_mrc_MetricsRef_t metricsRef = le_mrc_MeasureSignalMetrics();
	if (metricsRef != NULL) {

		res = le_mrc_GetGsmSignalMetrics(metricsRef, &rssi, &ber);
		if (res == LE_OK)
			LE_INFO("GSM metrics rxLevel.%ddBm, er.%d", rssi, ber);
		else
			LE_ERROR("Unable to read RSSI!");
	}

	res = le_mrc_GetRadioPower(&onoff);
	if ((res != LE_OK) || (onoff == LE_OFF)) {
		LE_INFO("Radio OFF");
		return -1;

	} else if ((res != LE_OK) || (onoff == LE_ON)) {
		uint32_t cell = -1;
		res = le_mrc_GetSignalQual(&cell);
		if (res != LE_OK) {
			LE_ERROR("Not able to read signal quality");
			return -1;
		}
		LE_INFO("Radio ON %d", cell);
		return cell;
	}
	return -1;

}

int32_t radio_Temperature() {
	le_result_t res;
	const char *sensorName = "POWER_CONTROLLER";
	le_temp_SensorRef_t sensorRef = le_temp_Request(sensorName);
	int32_t temperaturePtr = -1;
	res = le_temp_GetTemperature(sensorRef, &temperaturePtr);
	if (res == LE_OK) {
		LE_INFO("Temperature: %d", temperaturePtr);
	} else {
		LE_ERROR("Unable to read Temperature!!");
	}
	return temperaturePtr;

}
