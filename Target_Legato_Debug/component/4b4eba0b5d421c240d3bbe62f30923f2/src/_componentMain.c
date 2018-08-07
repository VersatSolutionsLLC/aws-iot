/*
 * AUTO-GENERATED _componentMain.c for the Gps component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _Gps_le_gnss_ServiceInstanceName;
const char** le_gnss_ServiceInstanceNamePtr = &_Gps_le_gnss_ServiceInstanceName;
void le_gnss_ConnectService(void);
extern const char* _Gps_gps_ServiceInstanceName;
const char** gps_ServiceInstanceNamePtr = &_Gps_gps_ServiceInstanceName;
void gps_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t Gps_LogSession;
le_log_Level_t* Gps_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _Gps_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _Gps_Init(void)
{
    LE_DEBUG("Initializing Gps component library.");

    // Advertise server-side IPC interfaces.
    gps_AdvertiseService();

    // Connect client-side IPC interfaces.
    le_gnss_ConnectService();

    // Register the component with the Log Daemon.
    Gps_LogSession = log_RegComponent("Gps", &Gps_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_Gps_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
