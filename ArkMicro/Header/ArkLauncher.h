#ifndef ARKLAUNCHER_H
#define ARKLAUNCHER_H

enum ArkLauncherType {
    ALT_Undefine = -1,
    ALT_Launcher,
    ALT_LinkCarplay,
    ALT_LinkCarlife,
    ALT_LinkAndroidMirror,
    ALT_LinkAndroidCarlife,
    ALT_LinkIOSCarlife,
    ALT_SDDisk,
    ALT_USBDisk,
    ALT_AVMusic,
    ALT_AVVideo,
};

#define ALS_LauncherStatus  ALS_LauncherInit,   \
                            ALS_LauncherForeground, \
                            ALS_LauncherBackground


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

#define ALS_DiskStatus  ALS_DiskEmpty,   \
                        ALS_DiskUnsupport, \
                        ALS_DiskBusy,   \
                        ALS_DiskReady,  \
                        ALS_DiskRemove

#define ALS_AVStatus    ALS_AVStart, \
                        ALS_AVUnsupport, \
                        ALS_AVStop,  \
                        ALS_AVPause, \
                        ALS_AVPlay,  \
                        ALS_AVExit

enum ArkLauncherStatus {
    ALS_Undefine = -1,
    ALS_LauncherStatus,
    ALS_LinkStatus,
    ALS_DiskStatus,
    ALS_AVStatus
};

struct ArkLauncher {
    enum ArkLauncherType type;
    enum ArkLauncherStatus status;
};


#endif // ARKLAUNCHER_H
