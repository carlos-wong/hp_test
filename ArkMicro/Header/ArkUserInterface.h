#ifndef ARKUSERINTERFACE_H
#define ARKUSERINTERFACE_H

enum ArkUserInterfaceType {
    AUIT_Undefine = -1,
    AUIT_Launcher,
};

#define AUIS_ArkUserInterfaceStatus  AUIS_ArkUserInterfaceInit,   \
                                     AUIS_ArkUserInterfaceForeground, \
                                     AUIS_ArkUserInterfaceBackground

enum ArkUserInterfaceStatus {
    AUIS_Undefine = -1,
    AUIS_ArkUserInterfaceStatus,
};

struct ArkUserInterface {
    enum ArkUserInterfaceType type;
    enum ArkUserInterfaceStatus status;
};


#endif // ARKUSERINTERFACE_H
