#include "legato.h"
#include "interfaces.h"
#include "versAgentHeader.h"

/**
 * GNSS state
 */
typedef enum {
	READY = 0,		///< Device is now Ready state
	ACTIVE = 1,		///< Device is now active state
	DISABLED = 2,	///< Device is now disabled state
	UNINITILIZED = -1	///< Device is now disabled state
} gnss_state;

static gnss_state state = UNINITILIZED;

/**
 * Get 3D location (latitude, longitude, altitude, wgs84Altitude, horizontal accuracy, vertical accuracy)
 */
void gps_GetLocation
(
		_gpsInfo *gpsInfo	///< [OUT] 	Structure to store gps information
)
{

	if (UNINITILIZED == state) {
		le_result_t rc = le_gnss_Enable();
		switch (rc) {
		case LE_OK:
		case LE_DUPLICATE:
			state = READY;
			rc = le_gnss_Start();
			if (rc != LE_OK && rc != LE_DUPLICATE) {
				LE_FATAL("Failed to start GNSS!");
			}
			LE_DEBUG("Now the GNSS device state is ACTIVE");
			break;
		default:
			LE_FATAL("Failed to enable GNSS!")
			;
		}
	}


	int32_t lat, lon, vAcc, hAcc, alt, altWgs84;
	le_gnss_SampleRef_t positionSampleRef = le_gnss_GetLastSampleRef();
	le_result_t result = le_gnss_GetLocation(positionSampleRef,&lat, &lon, &hAcc);
	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));

	gpsInfo->latitude = (float)lat/1000000.0;
	gpsInfo->longitude = (float)lon/1000000.0;
	gpsInfo->hAccuracy = (float)hAcc/100.0;


	result = le_gnss_GetAltitude( positionSampleRef, &alt, &vAcc);

	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));
	gpsInfo->altitude = (float)alt/1000.0;
	gpsInfo->vAccuracy = (float)vAcc/10.0;

	result = le_gnss_GetAltitudeOnWgs84(positionSampleRef, &altWgs84);
	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));
	gpsInfo->altitudeOnWgs84 = altWgs84/1000;
}

