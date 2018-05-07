#ifndef ARKLINK_H
#define ARKLINK_H

enum ArkLinkType {
    ALT_Undefine = -1,
    ALT_LinkCarplay,
    ALT_LinkCarlife,
    ALT_LinkAndroidMirror,
    ALT_LinkAndroidCarlife,
    ALT_LinkIOSCarlife
};

#define ALS_LinkStatus  ALS_LinkUnsuppored, \
                        ALS_LinkConnected,  \
                        ALS_LinkDisconnected,  \
                        ALS_LinkStarting,  \
                        ALS_LinkSuccess,  \
                        ALS_LinkFail,  \
                        ALS_LinkExiting,  \
                        ALT_LinkExited,  \
                        ALS_LinkRemoved,    \
                        ALT_LinkInserted,  \
                        ALT_LinkNotInserted,  \
                        ALT_LinkNotInstall, \
                        ALT_LinkCallPhone,  \
                        ALT_LinkCallPhoneExited


enum ArkLinkStatus {
    ALS_Undefine = -1,
    ALS_LinkStatus
};

struct ArkLink {
    enum ArkLinkType type;
    enum ArkLinkStatus status;
};

#endif // ARKLINK_H
