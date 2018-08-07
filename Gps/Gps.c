#include "legato.h"
#include "interfaces.h"

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
COMPONENT_INIT {
	le_result_t rc = le_gnss_Enable();
	if (rc == LE_OK || rc == LE_DUPLICATE) {
		state = READY;
		rc = le_gnss_Start();
		if (rc == LE_OK || rc == LE_DUPLICATE) {
			state = ACTIVE;
			LE_DEBUG("Now the GNSS device state is");
		} else {
			LE_FATAL("Failed to start GNSS!");
		}
	} else {
		LE_FATAL("Failed to enable GNSS!");
	}
}

/**
 * Get 3D location (latitude, longitude, altitude, wgs84Altitude, horizontal accuracy, vertical accuracy)
 */
void gps_GetLocation(char* position, size_t size){
	int32_t     latitude;
	int32_t     longitude;
	int32_t     altitude;
	int32_t     altitudeOnWgs84;
	int32_t     hAccuracy;
	int32_t     vAccuracy;

	float lat, lon, vAcc, hAcc, alt;
	le_gnss_SampleRef_t positionSampleRef = le_gnss_GetLastSampleRef();
	le_result_t result = le_gnss_GetLocation(positionSampleRef, &latitude, &longitude, &hAccuracy);
	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));

	lat = (float)latitude/1000000.0;
	lon = (float)longitude/1000000.0;
	hAcc = (float)hAccuracy/100.0;

	LE_DEBUG("Latitude:%f Longitude:%f hAccuracy:%f", lat, lon, hAcc);

	result = le_gnss_GetAltitude( positionSampleRef, &altitude, &vAccuracy);

	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));
	alt = (float)altitude/1000.0;
	vAcc = (float)vAccuracy/10.0;
	LE_DEBUG("Altitude:%f vAccuracy:%f", alt, vAcc);

	result = le_gnss_GetAltitudeOnWgs84(positionSampleRef, &altitudeOnWgs84);
	LE_ASSERT((LE_OK == result)||(LE_OUT_OF_RANGE == result));
	altitudeOnWgs84 = altitudeOnWgs84/1000;

	snprintf(position, size, "%f,%f,%f,%d,%f,%f", lat, lon,
			alt, altitudeOnWgs84, hAcc, vAcc);
}
