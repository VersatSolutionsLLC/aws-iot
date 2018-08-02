/*
 * AUTO-GENERATED _componentMain.c for the PublishRadio component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _PublishRadio_aws_ServiceInstanceName;
const char** aws_ServiceInstanceNamePtr = &_PublishRadio_aws_ServiceInstanceName;
void aws_ConnectService(void);
extern const char* _PublishRadio_radio_ServiceInstanceName;
const char** radio_ServiceInstanceNamePtr = &_PublishRadio_radio_ServiceInstanceName;
void radio_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t PublishRadio_LogSession;
le_log_Level_t* PublishRadio_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _PublishRadio_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _PublishRadio_Init(void)
{
    LE_DEBUG("Initializing PublishRadio component library.");

    // Connect client-side IPC interfaces.
    aws_ConnectService();
    radio_ConnectService();

    // Register the component with the Log Daemon.
    PublishRadio_LogSession = log_RegComponent("PublishRadio", &PublishRadio_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_PublishRadio_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
