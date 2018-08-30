/*
 * AUTO-GENERATED _componentMain.c for the subscribe component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _subscribe_radio_ServiceInstanceName;
const char** radio_ServiceInstanceNamePtr = &_subscribe_radio_ServiceInstanceName;
void radio_ConnectService(void);
extern const char* _subscribe_aws_ServiceInstanceName;
const char** aws_ServiceInstanceNamePtr = &_subscribe_aws_ServiceInstanceName;
void aws_ConnectService(void);
extern const char* _subscribe_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_subscribe_config_ServiceInstanceName;
void config_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t subscribe_LogSession;
le_log_Level_t* subscribe_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _subscribe_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _subscribe_Init(void)
{
    LE_DEBUG("Initializing subscribe component library.");

    // Connect client-side IPC interfaces.
    radio_ConnectService();
    aws_ConnectService();
    config_ConnectService();

    // Register the component with the Log Daemon.
    subscribe_LogSession = log_RegComponent("subscribe", &subscribe_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_subscribe_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
