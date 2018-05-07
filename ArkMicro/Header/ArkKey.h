#ifndef ARKKEY_H
#define ARKKEY_H

#define AKT_CommonType      AKT_CommonPower,  \
                            AKT_CommonHome,   \
                            AKT_CommonMode,   \
                            AKT_CommonUp,   \
                            AKT_CommonDown, \
                            AKT_CommonLeft, \
                            AKT_CommonRight,    \
                            AKT_CommonEnter,    \
                            AKT_CommonCancel,   \
                            AKT_CommonReturn,   \
                            AKT_CommonLeftTurn, \
                            AKT_CommonRightTurn,    \
                            AKT_CommonEnterNavi, \
                            AKT_CommonExitNavi, \
                            AKT_CommonNaviEnterExitToggle,  \
                            AKT_CommonVoice,   \
                            AKT_CommonMenu, \
                            AKT_CommonPhoneAccept,  \
                            AKT_CommonPhoneReject,  \
                            AKT_CommonPhoneAcceptRejectToggle,  \
                            AKT_CommonIdle,   \
                            AKT_CommonWork, \
                            AKT_CommonIdleWorkToggle,   \
                            AKT_CommonVolumeMute, \
                            AKT_CommonVolumeIncrement,    \
                            AKT_CommonVolumeDecrement,  \
                            AKT_CommonPrevious, \
                            AKT_CommonNext, \
                            AKT_CommonPlay, \
                            AKT_CommonPause,    \
                            AKT_CommonPlayPauseToggle

#define AKT_SteeringType
#define AKT_PanelType

enum ArkKeyType {
    AKT_Undefine = -1,
    AKT_CommonType,
    AKT_SwitchToMedia,
    AKT_SwitchToNavi,
    AKT_SwitchToRadio,
    AKT_SwitchToBt,
    AKT_SwitchToDVD,
    AKT_SwitchToTouchCal,
    AKT_BtPickUp,
    AKT_BtHangUp,
};

enum ArkKeyStatus {
    AKS_Undefine = -1,
    AKS_Press,
    AKS_Holder,
    AKS_LongPress,
    AKS_Release,
    AKS_Active,
    AKS_Value,
    AKS_Leave,
    AKS_Hover,
};

struct ArkKey {
    enum ArkKeyType type;
    enum ArkKeyStatus status;
};

#endif //ARKKEY_H
