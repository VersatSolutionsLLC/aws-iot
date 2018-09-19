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

extern const char* _AwsComponent_config_ServiceInstanceName;
const char** config_ServiceInstanceNamePtr = &_AwsComponent_config_ServiceInstanceName;
void config_ConnectService(void);
extern const char* _AwsComponent_le_data_ServiceInstanceName;
const char** le_data_ServiceInstanceNamePtr = &_AwsComponent_le_data_ServiceInstanceName;
void le_data_ConnectService(void);
extern const char* _AwsComponent_aws_ServiceInstanceName;
const char** aws_ServiceInstanceNamePtr = &_AwsComponent_aws_ServiceInstanceName;
void aws_AdvertiseService(void);
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

    // Advertise server-side IPC interfaces.
    aws_AdvertiseService();

    // Connect client-side IPC interfaces.
    config_ConnectService();
    le_data_ConnectService();

    // Register the component with the Log Daemon.
    AwsComponent_LogSession = log_RegComponent("AwsComponent", &AwsComponent_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_AwsComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
