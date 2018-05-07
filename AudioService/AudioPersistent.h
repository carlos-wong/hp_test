#ifndef AUDIOPERSISTENT_H
#define AUDIOPERSISTENT_H

#include <QtGlobal>

enum SpeakerSoundItem {
    SSI_Undefine = -1,
    SSI_PowerOn,
    SSI_PowerOff,
};
#define SpeakerSoundItem int

enum MuteItem {
    MI_Undefine = -1,
    MI_Unmute,
    MI_Mute,
};
#define MuteItem int

enum EqualizerItem {
    EI_Undefine = -1,
    EI_Standard,
    EI_Popular,
    EI_Classic,
    EI_Jazz,
    EI_Custom,
};
#define EqualizerItem int

enum SoundItem {
    SI_Undefine = -1,
    SI_Master,
    SI_Slave,
    SI_RearLeft,
    SI_RearRight,
    SI_Custom,
};
#define SoundItem int

enum AudioSource {
    AS_Undefine = -1,
    AS_Idle = 0,
    AS_Radio = 1,
    AS_Music = 2,
    AS_Video = 3,
    AS_CarplayMusic = 4,
    AS_CarplayPhone = 5,
    AS_AutoMusic = 6,
    AS_AutoPhone = 7,
    AS_CarlifeMusic = 8,
    AS_CarlifePhone = 9,
    AS_BluetoothMusic = 10,
    AS_BluetoothPhone = 11,
    AS_Aux = 12,
};
#define AudioSource int

class AudioService;
class AudioPersistent
{
    Q_DISABLE_COPY(AudioPersistent)
public:
    static void reset();
    static int getVolume(const bool reload = false);
    static int getEqualizerItem(const bool reload = false);
    static int getBassEqualizer(const bool reload = false);
    static int getMiddleEqualizer(const bool reload = false);
    static int getTrebleEqualizer(const bool reload = false);
    static void setEqualizerItem(const int item);
    static void setBassEqualizer(const int value);
    static void setMiddleEqualizer(const int value);
    static void setTrebleEqualizer(const int value);
    static int getSoundItem(const bool reload = false);
    static int getLeftSound(const bool reload = false);
    static int getRightSound(const bool reload = false);
    static void setSoundItem(const int item);
    static void setLeftSound(const int value);
    static void setRightSound(const int value);
private:
    friend class AudioService;
    static void setVolume(int value);
};

#endif // AUDIOPERSISTENT_H
