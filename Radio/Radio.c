#include "legato.h"
#include "interfaces.h"

COMPONENT_INIT
{
}

void printer_Print() {
    le_result_t   res;
    le_onoff_t    onoff;
    res = le_mrc_GetRadioPower(&onoff);
   /* const char *sensorName = "modem";
    le_temp_SensorRef_t sensorRef = le_temp_Request	(sensorName);
    int32_t 	temperaturePtr = -1;
    res = le_temp_GetTemperature (sensorRef, &temperaturePtr);
    if(res == LE_OK){
    	LE_INFO("Temperature: %d", temperaturePtr);
    }
    else {
    	LE_ERROR("Unable to read Temperature!!");
    }*/
    if ((res != LE_OK) || (onoff == LE_OFF))
    {
    	LE_INFO("Radio OFF");

    }
    else if((res != LE_OK) || (onoff == LE_ON)){
    	uint32_t cell = -1;
    	res = le_mrc_GetSignalQual(&cell);
    	if(res!=LE_OK) {
    		LE_ERROR("Not able to read signal quality");
    		return;
    	}
    	LE_INFO("Radio ON %d", cell);
    }

}
