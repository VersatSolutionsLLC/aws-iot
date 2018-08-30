/*
 * AUTO-GENERATED _componentMain.c for the Radio component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _Radio_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_Radio_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _Radio_le_info_ServiceInstanceName;
const char** le_info_ServiceInstanceNamePtr = &_Radio_le_info_ServiceInstanceName;
void le_info_ConnectService(void);
extern const char* _Radio_le_sim_ServiceInstanceName;
const char** le_sim_ServiceInstanceNamePtr = &_Radio_le_sim_ServiceInstanceName;
void le_sim_ConnectService(void);
extern const char* _Radio_le_temp_ServiceInstanceName;
const char** le_temp_ServiceInstanceNamePtr = &_Radio_le_temp_ServiceInstanceName;
void le_temp_ConnectService(void);
extern const char* _Radio_le_data_ServiceInstanceName;
const char** le_data_ServiceInstanceNamePtr = &_Radio_le_data_ServiceInstanceName;
void le_data_ConnectService(void);
extern const char* _Radio_radio_ServiceInstanceName;
const char** radio_ServiceInstanceNamePtr = &_Radio_radio_ServiceInstanceName;
void radio_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t Radio_LogSession;
le_log_Level_t* Radio_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _Radio_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _Radio_Init(void)
{
    LE_DEBUG("Initializing Radio component library.");

    // Advertise server-side IPC interfaces.
    radio_AdvertiseService();

    // Connect client-side IPC interfaces.
    le_mrc_ConnectService();
    le_info_ConnectService();
    le_sim_ConnectService();
    le_temp_ConnectService();
    le_data_ConnectService();

    // Register the component with the Log Daemon.
    Radio_LogSession = log_RegComponent("Radio", &Radio_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_Radio_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
