
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_MRC_MESSAGES_H_INCLUDE_GUARD
#define LE_MRC_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "81a3a47c7cf1d1100eb275e46676f390"

#ifdef MK_TOOLS_BUILD
    extern const char** le_mrc_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_mrc_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_mrc"
#endif


#define _MAX_MSG_SIZE 120

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_mrc_AddNetRegStateEventHandler 0
#define _MSGID_le_mrc_RemoveNetRegStateEventHandler 1
#define _MSGID_le_mrc_AddRatChangeHandler 2
#define _MSGID_le_mrc_RemoveRatChangeHandler 3
#define _MSGID_le_mrc_AddPacketSwitchedChangeHandler 4
#define _MSGID_le_mrc_RemovePacketSwitchedChangeHandler 5
#define _MSGID_le_mrc_AddSignalStrengthChangeHandler 6
#define _MSGID_le_mrc_RemoveSignalStrengthChangeHandler 7
#define _MSGID_le_mrc_SetSignalStrengthIndThresholds 8
#define _MSGID_le_mrc_SetSignalStrengthIndDelta 9
#define _MSGID_le_mrc_SetAutomaticRegisterMode 10
#define _MSGID_le_mrc_SetManualRegisterMode 11
#define _MSGID_le_mrc_SetManualRegisterModeAsync 12
#define _MSGID_le_mrc_GetRegisterMode 13
#define _MSGID_le_mrc_GetPacketSwitchedState 14
#define _MSGID_le_mrc_GetPlatformSpecificRegistrationErrorCode 15
#define _MSGID_le_mrc_SetRatPreferences 16
#define _MSGID_le_mrc_GetRatPreferences 17
#define _MSGID_le_mrc_SetBandPreferences 18
#define _MSGID_le_mrc_GetBandPreferences 19
#define _MSGID_le_mrc_SetLteBandPreferences 20
#define _MSGID_le_mrc_GetLteBandPreferences 21
#define _MSGID_le_mrc_SetTdScdmaBandPreferences 22
#define _MSGID_le_mrc_GetTdScdmaBandPreferences 23
#define _MSGID_le_mrc_AddPreferredOperator 24
#define _MSGID_le_mrc_RemovePreferredOperator 25
#define _MSGID_le_mrc_GetPreferredOperatorsList 26
#define _MSGID_le_mrc_GetFirstPreferredOperator 27
#define _MSGID_le_mrc_GetNextPreferredOperator 28
#define _MSGID_le_mrc_DeletePreferredOperatorsList 29
#define _MSGID_le_mrc_GetPreferredOperatorDetails 30
#define _MSGID_le_mrc_GetNetRegState 31
#define _MSGID_le_mrc_GetSignalQual 32
#define _MSGID_le_mrc_SetRadioPower 33
#define _MSGID_le_mrc_GetRadioPower 34
#define _MSGID_le_mrc_PerformCellularNetworkScan 35
#define _MSGID_le_mrc_PerformCellularNetworkScanAsync 36
#define _MSGID_le_mrc_GetFirstCellularNetworkScan 37
#define _MSGID_le_mrc_GetNextCellularNetworkScan 38
#define _MSGID_le_mrc_DeleteCellularNetworkScan 39
#define _MSGID_le_mrc_GetCellularNetworkMccMnc 40
#define _MSGID_le_mrc_GetCellularNetworkName 41
#define _MSGID_le_mrc_GetCellularNetworkRat 42
#define _MSGID_le_mrc_IsCellularNetworkInUse 43
#define _MSGID_le_mrc_IsCellularNetworkAvailable 44
#define _MSGID_le_mrc_IsCellularNetworkHome 45
#define _MSGID_le_mrc_IsCellularNetworkForbidden 46
#define _MSGID_le_mrc_GetCurrentNetworkName 47
#define _MSGID_le_mrc_GetCurrentNetworkMccMnc 48
#define _MSGID_le_mrc_GetRadioAccessTechInUse 49
#define _MSGID_le_mrc_GetNeighborCellsInfo 50
#define _MSGID_le_mrc_DeleteNeighborCellsInfo 51
#define _MSGID_le_mrc_GetFirstNeighborCellInfo 52
#define _MSGID_le_mrc_GetNextNeighborCellInfo 53
#define _MSGID_le_mrc_GetNeighborCellId 54
#define _MSGID_le_mrc_GetNeighborCellLocAreaCode 55
#define _MSGID_le_mrc_GetNeighborCellRxLevel 56
#define _MSGID_le_mrc_GetNeighborCellRat 57
#define _MSGID_le_mrc_GetNeighborCellUmtsEcIo 58
#define _MSGID_le_mrc_GetNeighborCellLteIntraFreq 59
#define _MSGID_le_mrc_GetNeighborCellLteInterFreq 60
#define _MSGID_le_mrc_MeasureSignalMetrics 61
#define _MSGID_le_mrc_DeleteSignalMetrics 62
#define _MSGID_le_mrc_GetRatOfSignalMetrics 63
#define _MSGID_le_mrc_GetGsmSignalMetrics 64
#define _MSGID_le_mrc_GetUmtsSignalMetrics 65
#define _MSGID_le_mrc_GetLteSignalMetrics 66
#define _MSGID_le_mrc_GetCdmaSignalMetrics 67
#define _MSGID_le_mrc_GetServingCellId 68
#define _MSGID_le_mrc_GetServingCellLocAreaCode 69
#define _MSGID_le_mrc_GetServingCellLteTracAreaCode 70
#define _MSGID_le_mrc_GetBandCapabilities 71
#define _MSGID_le_mrc_GetLteBandCapabilities 72
#define _MSGID_le_mrc_GetTdScdmaBandCapabilities 73
#define _MSGID_le_mrc_AddNetworkRejectHandler 74
#define _MSGID_le_mrc_RemoveNetworkRejectHandler 75
#define _MSGID_le_mrc_AddJammingDetectionEventHandler 76
#define _MSGID_le_mrc_RemoveJammingDetectionEventHandler 77
#define _MSGID_le_mrc_StartJammingDetection 78
#define _MSGID_le_mrc_StopJammingDetection 79


#endif // LE_MRC_MESSAGES_H_INCLUDE_GUARD