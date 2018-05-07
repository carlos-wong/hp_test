#ifndef ARKAUDIO_H
#define ARKAUDIO_H

enum ArkAudioType {
    AA_Undefine = -1,
    AAT_QueryVolume,
    AAT_SetVolume,
    AAT_GetVolume,
    AAT_QueryEQ,
    AAT_SetEQ,
    AAT_GetEQ,
    AAT_Idle,
    AAT_Radio,
    AAT_Music,
    AAT_Video,
    AAT_Carplay,
    AAT_CarlifeMusic,
    AAT_CarlifePhone,
};


struct ArkAudio {
    enum ArkAudioType type;
    unsigned char length;
    unsigned char* data;
};

#endif //ARKAUDIO_H
