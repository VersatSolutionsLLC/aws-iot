/*
 * AUTO-GENERATED _componentMain.c for the AwsComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _AwsComponent_radio_ServiceInstanceName;
const char** radio_ServiceInstanceNamePtr = &_AwsComponent_radio_ServiceInstanceName;
void radio_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t AwsComponent_LogSession;
le_log_Level_t* AwsComponent_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _AwsComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _AwsComponent_Init(void)
{
    LE_DEBUG("Initializing AwsComponent component library.");

    // Connect client-side IPC interfaces.
    radio_ConnectService();

    // Register the component with the Log Daemon.
    AwsComponent_LogSession = log_RegComponent("AwsComponent", &AwsComponent_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_AwsComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
