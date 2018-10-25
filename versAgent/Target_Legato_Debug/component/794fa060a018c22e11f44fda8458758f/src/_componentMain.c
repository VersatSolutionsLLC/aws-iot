/*
 * AUTO-GENERATED _componentMain.c for the CollectMetrics component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _CollectMetrics_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_CollectMetrics_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _CollectMetrics_le_info_ServiceInstanceName;
const char** le_info_ServiceInstanceNamePtr = &_CollectMetrics_le_info_ServiceInstanceName;
void le_info_ConnectService(void);
extern const char* _CollectMetrics_le_sim_ServiceInstanceName;
const char** le_sim_ServiceInstanceNamePtr = &_CollectMetrics_le_sim_ServiceInstanceName;
void le_sim_ConnectService(void);
extern const char* _CollectMetrics_le_temp_ServiceInstanceName;
const char** le_temp_ServiceInstanceNamePtr = &_CollectMetrics_le_temp_ServiceInstanceName;
void le_temp_ConnectService(void);
extern const char* _CollectMetrics_le_gnss_ServiceInstanceName;
const char** le_gnss_ServiceInstanceNamePtr = &_CollectMetrics_le_gnss_ServiceInstanceName;
void le_gnss_ConnectService(void);
extern const char* _CollectMetrics_le_pos_ServiceInstanceName;
const char** le_pos_ServiceInstanceNamePtr = &_CollectMetrics_le_pos_ServiceInstanceName;
void le_pos_ConnectService(void);
extern const char* _CollectMetrics_le_posCtrl_ServiceInstanceName;
const char** le_posCtrl_ServiceInstanceNamePtr = &_CollectMetrics_le_posCtrl_ServiceInstanceName;
void le_posCtrl_ConnectService(void);
extern const char* _CollectMetrics_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_CollectMetrics_config_ServiceInstanceName;
void config_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t CollectMetrics_LogSession;
le_log_Level_t* CollectMetrics_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _CollectMetrics_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _CollectMetrics_Init(void)
{
    LE_DEBUG("Initializing CollectMetrics component library.");

    // Connect client-side IPC interfaces.
    le_mrc_ConnectService();
    le_info_ConnectService();
    le_sim_ConnectService();
    le_temp_ConnectService();
    le_gnss_ConnectService();
    le_pos_ConnectService();
    le_posCtrl_ConnectService();
    config_ConnectService();

    // Register the component with the Log Daemon.
    CollectMetrics_LogSession = log_RegComponent("CollectMetrics", &CollectMetrics_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_CollectMetrics_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
