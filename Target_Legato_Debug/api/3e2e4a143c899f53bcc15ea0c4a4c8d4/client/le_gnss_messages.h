
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_GNSS_MESSAGES_H_INCLUDE_GUARD
#define LE_GNSS_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "fd6e07b1cbe4a22fc7c64f2ac7a7fb6d"

#ifdef MK_TOOLS_BUILD
    extern const char** le_gnss_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_gnss_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_gnss"
#endif


#define _MAX_MSG_SIZE 2019

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_gnss_SetConstellation 0
#define _MSGID_le_gnss_GetConstellation 1
#define _MSGID_le_gnss_SetConstellationArea 2
#define _MSGID_le_gnss_GetConstellationArea 3
#define _MSGID_le_gnss_EnableExtendedEphemerisFile 4
#define _MSGID_le_gnss_DisableExtendedEphemerisFile 5
#define _MSGID_le_gnss_LoadExtendedEphemerisFile 6
#define _MSGID_le_gnss_GetExtendedEphemerisValidity 7
#define _MSGID_le_gnss_InjectUtcTime 8
#define _MSGID_le_gnss_Start 9
#define _MSGID_le_gnss_Stop 10
#define _MSGID_le_gnss_ForceHotRestart 11
#define _MSGID_le_gnss_ForceWarmRestart 12
#define _MSGID_le_gnss_ForceColdRestart 13
#define _MSGID_le_gnss_ForceFactoryRestart 14
#define _MSGID_le_gnss_GetTtff 15
#define _MSGID_le_gnss_Enable 16
#define _MSGID_le_gnss_Disable 17
#define _MSGID_le_gnss_SetAcquisitionRate 18
#define _MSGID_le_gnss_GetAcquisitionRate 19
#define _MSGID_le_gnss_AddPositionHandler 20
#define _MSGID_le_gnss_RemovePositionHandler 21
#define _MSGID_le_gnss_GetPositionState 22
#define _MSGID_le_gnss_GetLocation 23
#define _MSGID_le_gnss_GetAltitude 24
#define _MSGID_le_gnss_GetTime 25
#define _MSGID_le_gnss_GetGpsTime 26
#define _MSGID_le_gnss_GetEpochTime 27
#define _MSGID_le_gnss_GetTimeAccuracy 28
#define _MSGID_le_gnss_GetGpsLeapSeconds 29
#define _MSGID_le_gnss_GetDate 30
#define _MSGID_le_gnss_GetHorizontalSpeed 31
#define _MSGID_le_gnss_GetVerticalSpeed 32
#define _MSGID_le_gnss_GetDirection 33
#define _MSGID_le_gnss_GetSatellitesInfo 34
#define _MSGID_le_gnss_GetSbasConstellationCategory 35
#define _MSGID_le_gnss_GetSatellitesStatus 36
#define _MSGID_le_gnss_GetDop 37
#define _MSGID_le_gnss_GetDilutionOfPrecision 38
#define _MSGID_le_gnss_GetAltitudeOnWgs84 39
#define _MSGID_le_gnss_GetMagneticDeviation 40
#define _MSGID_le_gnss_GetLastSampleRef 41
#define _MSGID_le_gnss_ReleaseSampleRef 42
#define _MSGID_le_gnss_SetSuplAssistedMode 43
#define _MSGID_le_gnss_GetSuplAssistedMode 44
#define _MSGID_le_gnss_SetSuplServerUrl 45
#define _MSGID_le_gnss_InjectSuplCertificate 46
#define _MSGID_le_gnss_DeleteSuplCertificate 47
#define _MSGID_le_gnss_SetNmeaSentences 48
#define _MSGID_le_gnss_GetNmeaSentences 49
#define _MSGID_le_gnss_GetState 50
#define _MSGID_le_gnss_SetMinElevation 51
#define _MSGID_le_gnss_GetMinElevation 52
#define _MSGID_le_gnss_SetDopResolution 53


#endif // LE_GNSS_MESSAGES_H_INCLUDE_GUARD