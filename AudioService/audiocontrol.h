#ifndef AUDIOCONTROL_H
#define AUDIOCONTROL_H

#include "alsa/asoundlib.h"
#include "AudioService.h"

#define DEFAULT     "default"

#define LEFT_ID     "numid=1,iface=MIXER,name='Left Playback Volume'"
#define RIGHT_ID    "numid=2,iface=MIXER,name='Right Playback Volume'"
#define STRAM_ID    "numid=3,iface=MIXER,name='Stream Select'"
#define OUTPUT_ID   "numid=4,iface=MIXER,name='Output Select'"
#define AMP_ID      "numid=5,iface=MIXER,name='AMP Volume'"
#define FM_ID       "numid=6,iface=MIXER,name='FM Freq'"
#define IRSTATUS_ID "numid=7,iface=MIXER,name='IR Status'"
#define IRCHANNEL_ID "numid=8,iface=MIXER,name='IR Channel'"
#define PAMUTE_ID   "numid=21,iface=MIXER,name='PA Mute'"
#if 0
#define PAINPUTSWITCH_ID            "numid=3,iface=MIXER,name='PA-Input Switch'"
#define PAOUTPUTVOLUME_ID           "numid=10,iface=MIXER,name='PA-Output Volume'"
#define PAMUTESWITCH_ID             "numid=29,iface=MIXER,name='PA-Mute Switch'"
#define PAOUTPUTFL_ID               "numid=11,iface=MIXER,name='PA-Output-FL Select'"
#define PAOUTPUTFR_ID               "numid=12,iface=MIXER,name='PA-Output-FR Select'"
#define PAOUTPUTRL_ID               "numid=13,iface=MIXER,name='PA-Output-RL Select'"
#define PAOUTPUTRR_ID               "numid=14,iface=MIXER,name='PA-Output-RR Select'"
//numid=23,iface=MIXER,name='EQ-Bass Gain'
//numid=26,iface=MIXER,name='EQ-Bass(Boost/Cut) Switch'
//numid=20,iface=MIXER,name='EQ-Bass-F0 Switch'
//numid=17,iface=MIXER,name='EQ-Bass-Q Switch'
//numid=24,iface=MIXER,name='EQ-Middle Gain'
//numid=27,iface=MIXER,name='EQ-Middle(Boost/Cut) Switch'
//numid=21,iface=MIXER,name='EQ-Middle-F0 Switch'
//numid=18,iface=MIXER,name='EQ-Middle-Q Switch'
//numid=25,iface=MIXER,name='EQ-Treble Gain'
//numid=28,iface=MIXER,name='EQ-Treble(Boost/Cut) Switch'
//numid=22,iface=MIXER,name='EQ-Treble-F0 Switch'
//numid=19,iface=MIXER,name='EQ-Treble-Q Switch'
//numid=1,iface=MIXER,name='Left Playback Volume'
//numid=30,iface=MIXER,name='PA Reset'
//numid=5,iface=MIXER,name='PA-Input Gain'
//numid=3,iface=MIXER,name='PA-Input Switch'
//numid=4,iface=MIXER,name='PA-Input-Full_Diff-Type Switch'
//numid=31,iface=MIXER,name='PA-Loudness Gain'
//numid=9,iface=MIXER,name='PA-Loudness-F0 Switch'
//numid=6,iface=MIXER,name='PA-Mixing-Channel1 Switch'
//numid=7,iface=MIXER,name='PA-Mixing-Channel2 Switch'
//numid=8,iface=MIXER,name='PA-Mixing-input Select'
//numid=29,iface=MIXER,name='PA-Mute Switch'
//numid=10,iface=MIXER,name='PA-Output Volume'
//numid=11,iface=MIXER,name='PA-Output-FL Select'
//numid=12,iface=MIXER,name='PA-Output-FR Select'
//numid=13,iface=MIXER,name='PA-Output-RL Select'
//numid=14,iface=MIXER,name='PA-Output-RR Select'
//numid=15,iface=MIXER,name='PA-Output-Sub1 Select'
//numid=16,iface=MIXER,name='PA-Output-Sub2 Select'
//numid=2,iface=MIXER,name='Right Playback Volume'
//numid=33,iface=MIXER,name='Subwoofer-Input select'
//numid=32,iface=MIXER,name='Subwoofer-LPF fc'
//numid=35,iface=MIXER,name='Subwoofer-Lpf Parse'
//numid=34,iface=MIXER,name='Subwoofer-Output select'
#else
extern char* painputswitch_id;
//#define PAINPUTSWITCH_ID            "numid=13,iface=MIXER,name='PA Input Select'"
extern char* paoutputvolume_id;
//#define PAOUTPUTVOLUME_ID           "numid=23,iface=MIXER,name='PA Volume'"
extern char* pamuteswitch_id;
//#define PAMUTESWITCH_ID             "numid=21,iface=MIXER,name='PA Mute'"
extern char* paoutputfl_id;
//#define PAOUTPUTFL_ID               "numid=15,iface=MIXER,name='PA Fader-FL'"
extern char* paoutputfr_id;
//#define PAOUTPUTFR_ID               "numid=16,iface=MIXER,name='PA Fader-FR'"
extern char* paoutputrl_id;
//#define PAOUTPUTRL_ID               "numid=17,iface=MIXER,name='PA Fader-RL'"
extern char* paoutputrr_id;
//#define PAOUTPUTRR_ID               "numid=18,iface=MIXER,name='PA Fader-RR'"
extern char* paoutputbass_id;
//#define PAOUTPUTBASS_ID             "numid=3,iface=MIXER,name='EQ Bass'"
extern char* paoutputmiddle_id;
//#define PAOUTPUTMIDDLE_ID           "numid=6,iface=MIXER,name='EQ Middle'"
extern char* paoutputtreble_id;
//#define PAOUTPUTTREBLE_ID           "numid=9,iface=MIXER,name='EQ Treble'"
//numid=3,iface=MIXER,name='EQ Bass'
//numid=4,iface=MIXER,name='EQ BassF0'
//numid=5,iface=MIXER,name='EQ BassQ'
//numid=6,iface=MIXER,name='EQ Middle'
//numid=7,iface=MIXER,name='EQ MiddleF0'
//numid=8,iface=MIXER,name='EQ MiddleQ'
//numid=9,iface=MIXER,name='EQ Treble'
//numid=10,iface=MIXER,name='EQ TrebleF0'
//numid=11,iface=MIXER,name='EQ TrebleQ'
//numid=1,iface=MIXER,name='Left Playback Volume'
//numid=15,iface=MIXER,name='PA Fader-FL '
//numid=16,iface=MIXER,name='PA Fader-FR'
//numid=17,iface=MIXER,name='PA Fader-RL'
//numid=18,iface=MIXER,name='PA Fader-RR'
//numid=19,iface=MIXER,name='PA Fader-Sub1'
//numid=20,iface=MIXER,name='PA Fader-Sub2'
//numid=13,iface=MIXER,name='PA Input Select'
//numid=14,iface=MIXER,name='PA Input-Full-Diff-Type Select'
//numid=12,iface=MIXER,name='PA Input-Gain'
//numid=24,iface=MIXER,name='PA Loudness'
//numid=26,iface=MIXER,name='PA Loudness-F0 Select'
//numid=25,iface=MIXER,name='PA Loudness-HiCut Select'
//numid=27,iface=MIXER,name='PA Mixing-CH1 Switch'
//numid=28,iface=MIXER,name='PA Mixing-CH2 Switch'
//numid=29,iface=MIXER,name='PA Mixing-Input Select'
//numid=21,iface=MIXER,name='PA Mute'
//numid=22,iface=MIXER,name='PA Reset'
//numid=30,iface=MIXER,name='PA Sub-Input Select'
//numid=31,iface=MIXER,name='PA Sub-LPF-FC Select'
//numid=32,iface=MIXER,name='PA Sub-LPF-Parse Select'
//numid=33,iface=MIXER,name='PA Sub-Output Select'
//numid=23,iface=MIXER,name='PA Volume'
//numid=2,iface=MIXER,name='Right Playback Volume'
#endif


//front rear
//amixer cset numid=15,iface=MIXER,name='PA Fader-FL '
//75  80 95
//-15 0  15

//EQ
//amixer cset numid=9,iface=MIXER,name='EQ Treble'
//0   15 30
//-15 0  15

typedef enum
{
    LEFT_AUDIO = 0x01,
    RIGHT_AUDIO,
    STRAM_AUDIO,
    OUTPUT_AUDIO,
    AMP_AUDIO,
    FM_AUDIO,
    IRSTATUS_AUDIO,
    IRCHANNEL_AUDIO,
    PAMUTE_AUDIO,
    PAINPUTSWITCH_AUDIO,
    PAOUTPUTVOLUME_AUDIO,
    PAMUTESWITCH_AUDIO,
    PAOUTPUTFL_AUDIO,
    PAOUTPUTFR_AUDIO,
    PAOUTPUTRL_AUDIO,
    PAOUTPUTRR_AUDIO,
    PAOUTPUTBASS_AUDIO,
    PAOUTPUTMIDDLE_AUDIO,
    PAOUTPUTTREBLE_AUDIO,
}TYPE_AUDIO;

typedef struct Packet
{
    int count;
    int type;
} Packet_t;

typedef struct Data
{
    int max;
    int min;
    int cur;
    int step;
}Data_t;


class AudioControl
{
public:
    AudioControl();

public:

    //get control value
    virtual int Get(int Audio_Type ,int *nValue);

    //set control value
    virtual int Set(int Audio_Type, int nValue);

    //get control range
    virtual int GetRange(int Audio_Type, int *pMin ,int *pMax);

protected:
    //get string from enum's
    char* getstring(int type);

    //get id from audio's string
    int getpid(int audio_type, snd_ctl_elem_id_t **ppid);

    //get info from id
    int getpinfo(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t **ppinfo);

    //get elem from id
    int getpelem(snd_ctl_elem_id_t *pid, snd_hctl_elem_t **ppelem);

    //get control from id and info
    int getpcontrol(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t *pinfo ,snd_ctl_elem_value_t **ppcontrol);

    //get type
    int get_type(snd_ctl_elem_info_t *pinfo, int *ptype);

    //get size
    int get_size(snd_ctl_elem_info_t *pinfo, int *psize);

    //get range from elem and info
    int get_range(int type, snd_hctl_elem_t *pelem, snd_ctl_elem_info_t *pinfo, Data *pdata);

    //get value
    int get_value(int type, int count, snd_ctl_elem_value_t *pcontrol, int *pvalue);

    //set value
    int set_value(snd_ctl_elem_id_t *pid, snd_ctl_elem_info_t *pinfo, snd_ctl_elem_value_t *pcontrol, int value);
};

#endif // AUDIOCONTROL_H
