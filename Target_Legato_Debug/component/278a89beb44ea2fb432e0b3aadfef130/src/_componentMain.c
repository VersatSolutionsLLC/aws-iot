/*
 * AUTO-GENERATED _componentMain.c for the ReadConfig component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _ReadConfig_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_ReadConfig_config_ServiceInstanceName;
void config_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t ReadConfig_LogSession;
le_log_Level_t* ReadConfig_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _ReadConfig_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _ReadConfig_Init(void)
{
    LE_DEBUG("Initializing ReadConfig component library.");

    // Advertise server-side IPC interfaces.
    config_AdvertiseService();

    // Register the component with the Log Daemon.
    ReadConfig_LogSession = log_RegComponent("ReadConfig", &ReadConfig_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_ReadConfig_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
