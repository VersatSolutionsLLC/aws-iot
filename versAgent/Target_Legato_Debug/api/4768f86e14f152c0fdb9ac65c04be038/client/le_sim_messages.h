
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_SIM_MESSAGES_H_INCLUDE_GUARD
#define LE_SIM_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "bda7edf239888ed6aebddee884543696"

#ifdef MK_TOOLS_BUILD
    extern const char** le_sim_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_sim_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_sim"
#endif


#define _MAX_MSG_SIZE 536

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_sim_AddNewStateHandler 0
#define _MSGID_le_sim_RemoveNewStateHandler 1
#define _MSGID_le_sim_AddSimToolkitEventHandler 2
#define _MSGID_le_sim_RemoveSimToolkitEventHandler 3
#define _MSGID_le_sim_GetSelectedCard 4
#define _MSGID_le_sim_SelectCard 5
#define _MSGID_le_sim_GetICCID 6
#define _MSGID_le_sim_GetEID 7
#define _MSGID_le_sim_GetIMSI 8
#define _MSGID_le_sim_IsPresent 9
#define _MSGID_le_sim_IsReady 10
#define _MSGID_le_sim_EnterPIN 11
#define _MSGID_le_sim_ChangePIN 12
#define _MSGID_le_sim_GetRemainingPINTries 13
#define _MSGID_le_sim_GetRemainingPUKTries 14
#define _MSGID_le_sim_Unlock 15
#define _MSGID_le_sim_Lock 16
#define _MSGID_le_sim_Unblock 17
#define _MSGID_le_sim_GetState 18
#define _MSGID_le_sim_GetSubscriberPhoneNumber 19
#define _MSGID_le_sim_GetHomeNetworkOperator 20
#define _MSGID_le_sim_GetHomeNetworkMccMnc 21
#define _MSGID_le_sim_LocalSwapToEmergencyCallSubscription 22
#define _MSGID_le_sim_LocalSwapToCommercialSubscription 23
#define _MSGID_le_sim_IsEmergencyCallSubscriptionSelected 24
#define _MSGID_le_sim_AcceptSimToolkitCommand 25
#define _MSGID_le_sim_RejectSimToolkitCommand 26
#define _MSGID_le_sim_GetSimToolkitRefreshMode 27
#define _MSGID_le_sim_GetSimToolkitRefreshStage 28
#define _MSGID_le_sim_SendApdu 29
#define _MSGID_le_sim_SendCommand 30
#define _MSGID_le_sim_Reset 31
#define _MSGID_le_sim_CreateFPLMNList 32
#define _MSGID_le_sim_AddFPLMNOperator 33
#define _MSGID_le_sim_WriteFPLMNList 34
#define _MSGID_le_sim_ReadFPLMNList 35
#define _MSGID_le_sim_GetFirstFPLMNOperator 36
#define _MSGID_le_sim_GetNextFPLMNOperator 37
#define _MSGID_le_sim_DeleteFPLMNList 38
#define _MSGID_le_sim_OpenLogicalChannel 39
#define _MSGID_le_sim_CloseLogicalChannel 40
#define _MSGID_le_sim_SendApduOnChannel 41


#endif // LE_SIM_MESSAGES_H_INCLUDE_GUARD