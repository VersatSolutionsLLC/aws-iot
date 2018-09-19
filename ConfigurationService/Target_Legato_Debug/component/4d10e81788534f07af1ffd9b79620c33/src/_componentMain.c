/*
 * AUTO-GENERATED _componentMain.c for the Configuration component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _Configuration_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_Configuration_config_ServiceInstanceName;
void config_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t Configuration_LogSession;
le_log_Level_t* Configuration_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _Configuration_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _Configuration_Init(void)
{
    LE_DEBUG("Initializing Configuration component library.");

    // Advertise server-side IPC interfaces.
    config_AdvertiseService();

    // Register the component with the Log Daemon.
    Configuration_LogSession = log_RegComponent("Configuration", &Configuration_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_Configuration_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
