#ifndef ARKCAR_H
#define ARKCAR_H

enum ArkCarType {
    ACT_Undefine = -1,
    ACT_Version,
    ACT_Display,
    ACT_Reversing,
    ACT_Steering,
    ACT_Camera,
    ACT_Radar,
    ACT_Track,
    ACT_AssistTrack,
    ACT_ILLLight,
    ACT_Brake,
    ACT_Speed,
    ACT_Calibrate,
    ACT_MCUUpdate,
    ACT_Time,
    ACT_ACC,
};

#define ACS_ReversingStatus ACS_ReversingOn,    \
                            ACS_ReversingOff


#define ACS_CameraStatus    ACS_CameraOriginal, \
                            ACS_CameraExtra,    \
                            ACS_Camera360

#define ACS_RadarStatus     ACS_RadarOn,        \
                            ACS_RadarOff,        \
                            ACS_RadarRearLeft,   \
                            ACS_RadarRearRight,  \
                            ACS_RadarFrontLeft,  \
                            ACS_RadarFrontRight

#define ACS_TrackStatus     ACS_TrackOn,    \
                            ACS_TrackOff

#define ACS_AssistTrackStatus     ACS_AssistTrackOn,  \
                                  ACS_AssistTrackOff

#define ACS_SteeringStatus  ACS_SteeringValue

#define ACS_ILLLightStatus  ACS_ILLLightOn, \
                            ACS_ILLLightOff

#define ACS_BrakeStatus     ACS_BrakeOn, \
                            ACS_BrakeOff

#define ACS_SpeedStatus           ACS_SpeedValue

#define ACS_CalibrateStatus     ACS_CalibrateOn,\
                                ACS_CalibrateOff

#define ACS_MCUUpdateStatus     ACS_MCUUpdateStart,\
                                ACS_MCUUpdateFileLen,\
                                ACS_MCUUpdateTransfer,\
                                ACS_MCUUpdatePercent,\
                                ACS_MCUUpdateTransferEnd,\
                                ACS_MCUUpdateSuccess,\
                                ACS_MCUUpdateFail

#define ACS_ACCStatus           ACS_ACCOff,\
                                ACS_ACCOn

enum ArkCarStatus {
    ACS_Undefine = -1,
    ACS_CarDisplay,
    ACS_NaviDisplay,
    ACS_ReversingStatus,
    ACS_CameraStatus,
    ACS_RadarStatus,
    ACS_TrackStatus,
    ACS_AssistTrackStatus,
    ACS_SteeringStatus,
    ACS_ILLLightStatus,
    ACS_BrakeStatus,
    ACS_SpeedStatus,
    ACS_CalibrateStatus,
    ACS_MCUUpdateStatus,
    ACS_ACCStatus,
};

struct ArkCar {
    enum ArkCarType type;
    enum ArkCarStatus status;
    unsigned short int length;
    unsigned char* data;
};

struct CarDeviceState {
    int reversing;
    int illlight;
    int brake;
};

#endif //ARKCAR_H
