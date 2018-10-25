/*
 * AUTO-GENERATED _componentMain.c for the PublishMetrics component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _PublishMetrics_aws_ServiceInstanceName;
const char** aws_ServiceInstanceNamePtr = &_PublishMetrics_aws_ServiceInstanceName;
void aws_ConnectService(void);
extern const char* _PublishMetrics_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_PublishMetrics_config_ServiceInstanceName;
void config_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t PublishMetrics_LogSession;
le_log_Level_t* PublishMetrics_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _PublishMetrics_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _PublishMetrics_Init(void)
{
    LE_DEBUG("Initializing PublishMetrics component library.");

    // Connect client-side IPC interfaces.
    aws_ConnectService();
    config_ConnectService();

    // Register the component with the Log Daemon.
    PublishMetrics_LogSession = log_RegComponent("PublishMetrics", &PublishMetrics_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_PublishMetrics_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
