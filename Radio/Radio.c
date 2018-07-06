#include "legato.h"
#include "interfaces.h"

COMPONENT_INIT
{
}

int32_t radio_Signal() {
    le_result_t   res;
    le_onoff_t    onoff;
    res = le_mrc_GetRadioPower(&onoff);
    if ((res != LE_OK) || (onoff == LE_OFF))
    {
    	LE_INFO("Radio OFF");
    	return -1;

    }
    else if((res != LE_OK) || (onoff == LE_ON)){
    	uint32_t cell = -1;
    	res = le_mrc_GetSignalQual(&cell);
    	if(res!=LE_OK) {
    		LE_ERROR("Not able to read signal quality");
    		return -1;
    	}
    	LE_INFO("Radio ON %d", cell);
    	return cell;
    }
    return -1;

}

int32_t radio_Temperature(){
    le_result_t   res;
    const char *sensorName = "POWER_CONTROLLER";
    le_temp_SensorRef_t sensorRef = le_temp_Request	(sensorName);
    int32_t 	temperaturePtr = -1;
    res = le_temp_GetTemperature (sensorRef, &temperaturePtr);
    if(res == LE_OK){
    	LE_INFO("Temperature: %d", temperaturePtr);
    }
    else {
    	LE_ERROR("Unable to read Temperature!!");
    }
    return temperaturePtr;

}
