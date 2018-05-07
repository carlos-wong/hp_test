#include "AudioService.h"
#include "audiocontrol.h"
#include <QDebug>
#include <QDBusMessage>

#define VOL_MAX_LEVEL 40
static int VOL_TABLE[]=
{
    0,  2,  3,  4,  6,  8, 10, 12, 14, 16,
    18, 20, 22, 24, 26, 28, 30, 32, 34, 36,
    39, 42, 45, 48, 51, 54, 57, 60, 63, 66,
    69, 72, 75, 78, 81, 84, 87, 90, 95, 99,
};

enum AudioCardType {
    ACT_Undefine = -1,
    ACT_Default,
    ACT_Ark169,
    ACT_Yaoxi,
};

char* painputswitch_id = "";
char* paoutputvolume_id = "";
char* pamuteswitch_id = "";
char* paoutputfl_id = "";
char* paoutputfr_id = "";
char* paoutputrl_id = "";
char* paoutputrr_id = "";
char* paoutputbass_id = "";
char* paoutputmiddle_id = "";
char* paoutputtreble_id = "";

class AudioServicePrivate
{
public:
    explicit AudioServicePrivate(AudioService* parent);
    ~AudioServicePrivate();
    void initialize();
    void initializeArk169();
    void initializeYaoxi();
    void initializeDefault();
    bool insmodKO(const QString& path);
    void SetInputAudio(AudioSource source);
    void SetOutputAudio(int volume);
    void SetOutputVolume(int volume);
    int pos2volume(int pos/* 0 ~ 40*/);
private:
    Q_DECLARE_PUBLIC(AudioService)
    AudioService* const q_ptr;
    int	m_nMinVol;
    int	m_nMaxVol;
    float m_Step;
    int m_CurrentVolume;
    int m_CurrentSoundLeft;
    int m_CustomSoundLeft;
    int m_CurrentSoundRight;
    int m_CustomSoundRight;
    int m_CurrentBass;
    int m_CustomBass;
    int m_CurrentMiddle;
    int m_CustomMiddle;
    int m_CurrentTreble;
    int m_CustomTreble;
    AudioSource m_AudioSource;
    AudioCardType m_AudioCardType;
    MuteItem m_MuteItem;
    SpeakerSoundItem m_SpeakerSoundItem;
};

AudioService::AudioService(QObject *parent)
    : QObject(parent)
    , d_ptr(new AudioServicePrivate(this))
{
}

AudioService::~AudioService()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void AudioService::requestAudioSource(const AudioSource source)
{
    Q_D(AudioService);
    if (source != d->m_AudioSource) {
        d->m_AudioSource = source;
        d->SetOutputVolume(d->m_CurrentVolume / 2);
        d->SetOutputVolume(d->m_CurrentVolume / 4);
        d->SetOutputVolume(d->m_CurrentVolume / 8);
        d->SetOutputVolume(0);
        if (SSI_PowerOn == d->m_SpeakerSoundItem) {
            if (MI_Unmute == d->m_MuteItem) {
                AudioControl control;
                control.Set(PAMUTE_AUDIO, 0);
                d->SetInputAudio(source);
                control.Set(PAMUTE_AUDIO, 1);
            }
        } else {
            d->SetInputAudio(source);
        }
        d->SetOutputVolume(d->m_CurrentVolume / 8);
        d->SetOutputVolume(d->m_CurrentVolume / 4);
        d->SetOutputVolume(d->m_CurrentVolume / 2);
        d->SetOutputVolume(d->m_CurrentVolume);
    }
}

void AudioService::releaseAudioSource(const AudioSource source)
{
    Q_D(AudioService);
    if (source == d->m_AudioSource) {
        d->m_AudioSource = AS_Idle;
        d->SetInputAudio(d->m_AudioSource);
    }
}

void AudioService::reqesetReset()
{
    AudioControl control;
    control.Set(PAMUTE_AUDIO, 1);
    AudioPersistent::reset();
}

void AudioService::requestMuteToggole()
{
    Q_D(AudioService);
    AudioControl control;
    if (MI_Unmute == d->m_MuteItem) {
        d->m_MuteItem = MI_Mute;
        if (SSI_PowerOn == d->m_SpeakerSoundItem) {
            control.Set(PAMUTE_AUDIO, 1);
        }
    } else if (MI_Mute == d->m_MuteItem) {
        d->m_MuteItem = MI_Unmute;
        if (SSI_PowerOn == d->m_SpeakerSoundItem) {
            control.Set(PAMUTE_AUDIO, 0);
        }
        onVolumeChange(d->m_CurrentVolume);
    }
    emit onMuteChange(d->m_MuteItem);
}

void AudioService::requestSpeaker(const int item)
{
    Q_D(AudioService);
    if (item != d->m_SpeakerSoundItem) {
        d->m_SpeakerSoundItem = item;
        if (SSI_PowerOff == item) {
            if (ACT_Ark169 == d->m_AudioCardType) {
                AudioControl control;
                control.Set(PAMUTE_AUDIO, 1);
            } else {
                d->SetOutputVolume(0);
            }
        } else if (SSI_PowerOn == item) {
            if (MI_Unmute == d->m_MuteItem) {
                d->SetOutputVolume(d->m_CurrentVolume);
                if (ACT_Ark169 == d->m_AudioCardType) {
                    AudioControl control;
                    control.Set(PAMUTE_AUDIO, 0);
                }
            } else {
                if (ACT_Ark169 == d->m_AudioCardType) {
                    AudioControl control;
                    control.Set(PAMUTE_AUDIO, 1);
                } else {
                    d->SetOutputVolume(0);
                }
            }
        }
    }
}

void AudioService::requestMute(const int item)
{
    Q_D(AudioService);
    if (item != d->m_MuteItem) {
        AudioControl control;
        if (MI_Unmute == item) {
            d->m_MuteItem = MI_Unmute;
            if (ACT_Ark169 == d->m_AudioCardType) {
                if (SSI_PowerOn == d->m_SpeakerSoundItem) {
                    control.Set(PAMUTE_AUDIO, 0);
                }
            } else {
                if (SSI_PowerOn == d->m_SpeakerSoundItem) {
                    d->SetOutputVolume(d->m_CurrentVolume);
                }
            }
        } else if (MI_Mute == item) {
            d->m_MuteItem = MI_Mute;
            if (ACT_Ark169 == d->m_AudioCardType) {
                if (SSI_PowerOn == d->m_SpeakerSoundItem) {
                    control.Set(PAMUTE_AUDIO, 1);
                }
            } else {
                if (SSI_PowerOn == d->m_SpeakerSoundItem) {
                    d->SetOutputVolume(0);
                }
            }
        }
    }
}

void AudioService::requestIncreaseHalfVolume()
{
    Q_D(AudioService);
    if (ACT_Default == d->m_AudioCardType) {
        d->SetOutputVolume(d->m_CurrentVolume);
    }
}

void AudioService::requestDecreaseHalfVolume()
{
    Q_D(AudioService);
    if (ACT_Default == d->m_AudioCardType) {
        d->SetOutputVolume(d->m_CurrentVolume / 2);
    }
}

void AudioService::requestIncreaseVolume()
{
    Q_D(AudioService);
    if (MI_Unmute == d->m_MuteItem) {
        if (d->m_CurrentVolume < 40) {
            ++d->m_CurrentVolume;
        }
        requestSetVolume(d->m_CurrentVolume);
    } else {
        requestMuteToggole();
    }
}

void AudioService::requestDecreaseVolume()
{
    Q_D(AudioService);
    if (MI_Unmute == d->m_MuteItem) {
        if (d->m_CurrentVolume > 0) {
            --d->m_CurrentVolume;
        }
        requestSetVolume(d->m_CurrentVolume);
    } else {
        requestMuteToggole();
    }
}

void AudioService::requestSetVolume(const int volume)
{
    Q_D(AudioService);
    if (volume > 40) {
        d->m_CurrentVolume = 40;
    } else if (volume < 0) {
        d->m_CurrentVolume = 0;
    } else {
        d->m_CurrentVolume = volume;
    }
    if (SSI_PowerOn == d->m_SpeakerSoundItem) {
        d->SetOutputVolume(d->m_CurrentVolume);
    }
    AudioPersistent::setVolume(d->m_CurrentVolume);
    emit onVolumeChange(d->m_CurrentVolume);
    if (MI_Mute == d->m_MuteItem) {
        d->m_MuteItem = MI_Unmute;
        AudioControl control;
        if (SSI_PowerOn == d->m_SpeakerSoundItem) {
            control.Set(PAMUTE_AUDIO, 0);
        }
    }
}

void AudioService::setEqualizerItem(const int item, const int bass, const int middle, const int treble)
{
    int bassValue = bass;
    int middleValue = middle;
    int trebleValue = treble;
    AudioPersistent::setEqualizerItem(item);
    switch (item) {
    case EI_Standard: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 7;
        break;
    }
    case EI_Popular: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 11;
        break;
    }
    case EI_Classic: {
        bassValue = 11;
        middleValue = 9;
        trebleValue = 2;
        break;
    }
    case EI_Jazz: {
        bassValue = 10;
        middleValue = 11;
        trebleValue = 11;
        break;
    }
    default: {
        Q_D(AudioService);
        if (-1 == bassValue) {
            bassValue = d->m_CustomBass;
        }
        AudioPersistent::setBassEqualizer(bassValue);
        if (-1 == middleValue) {
            middleValue = d->m_CustomMiddle;
        }
        AudioPersistent::setMiddleEqualizer(middleValue);
        if (-1 == trebleValue) {
            trebleValue = d->m_CustomTreble;
        }
        AudioPersistent::setTrebleEqualizer(trebleValue);
        break;
    }
    }
    Q_D(AudioService);
    bool change = false;
    if (d->m_CurrentBass != bassValue) {
        d->m_CurrentBass = bassValue;
        change = true;
    }
    if (d->m_CurrentMiddle != middleValue) {
        d->m_CurrentMiddle = middleValue;
        change = true;
    }
    if (d->m_CurrentTreble != trebleValue) {
        d->m_CurrentTreble = trebleValue;
        change = true;
    }
    if (change) {
        AudioControl control;
        Q_D(AudioService);
        switch (d->m_AudioCardType) {
        case ACT_Ark169: {
            control.Set(PAOUTPUTBASS_AUDIO, bassValue * 2);
            control.Set(PAOUTPUTMIDDLE_AUDIO, middleValue * 2);
            control.Set(PAOUTPUTTREBLE_AUDIO, trebleValue * 2);
            break;
        }
        case ACT_Yaoxi: {
            control.Set(PAOUTPUTBASS_AUDIO, bassValue);
            control.Set(PAOUTPUTMIDDLE_AUDIO, middleValue);
            control.Set(PAOUTPUTTREBLE_AUDIO, trebleValue);
            break;
        }
        default: {
            break;
        }
        }
    }
    emit onEqualizerItemChange(item, bassValue, middleValue, trebleValue);
}

void AudioService::setSoundItem(const int item, const int left, const int right)
{
    Q_D(AudioService);
    AudioPersistent::setSoundItem(item);
    int leftValue = left;
    int rightValue = right;

    switch (item) {
    case SI_Master: {
        leftValue = -3;
        rightValue = -3;
        break;
    }
    case SI_Slave: {
        leftValue = 3;
        rightValue = -3;
        break;
    }
    case SI_RearLeft: {
        leftValue = -3;
        rightValue = 3;
        break;
    }
    case SI_RearRight: {
        leftValue = 3;
        rightValue = 3;
        break;
    }
    default: {
        if (-8 == leftValue) {
            leftValue = AudioPersistent::getLeftSound();;
        }
        if (-8 == rightValue) {
            rightValue = AudioPersistent::getRightSound();
        }
        AudioPersistent::setLeftSound(leftValue);
        AudioPersistent::setRightSound(rightValue);
        break;
    }
    }

    bool change = false;
    if (d->m_CurrentSoundLeft != leftValue) {
        d->m_CurrentSoundLeft = leftValue;
        change = true;
    }
    if (d->m_CurrentSoundRight != rightValue) {
        d->m_CurrentSoundRight = rightValue;
        change = true;
    }
    if (change) {
        AudioControl control;
        // int fl = 0;
        // int fr = 0;
        // int rl = 0;
        // int rr = 0;
        // switch (d->m_AudioCardType) {
        // case ACT_Ark169: {
        //     fl = 75 + round(sqrt(pow((leftValue + 7), 2) + pow((rightValue + 7), 2)));
        //     fr = 75 + round(sqrt(pow((leftValue - 7), 2) + pow((rightValue + 7), 2)));
        //     rl = 75 + round(sqrt(pow((leftValue + 7), 2) + pow((rightValue - 7), 2)));
        //     rr = 75 + round(sqrt(pow((leftValue - 7), 2) + pow((rightValue - 7), 2)));
        //     break;
        // }
        // case ACT_Yaoxi: {
        //     fl = 11 + round(sqrt(pow((leftValue + 7), 2) + pow((rightValue + 7), 2)));
        //     fr = 11 + round(sqrt(pow((leftValue - 7), 2) + pow((rightValue + 7), 2)));
        //     rl = 11 + round(sqrt(pow((leftValue + 7), 2) + pow((rightValue - 7), 2)));
        //     rr = 11 + round(sqrt(pow((leftValue - 7), 2) + pow((rightValue - 7), 2)));
        //     break;
        // }

        // default: {
        //     break;
        // }
        // 
        int maxFaderGain = 95;
        int step = 5;
        int halfValue = 7;
        int fl_value;
        int fr_value;
        int rl_value;
        int rr_value;

        printf("leftValue:%d rightValue:%d\n", leftValue, rightValue);
        if ((-1<= leftValue && leftValue <= 1) && (-1 <= rightValue && rightValue <= 1))//x y中心 全车
        {
            fl_value = fr_value = rl_value = rr_value = maxFaderGain;
        }else if (leftValue <= -6 && rightValue <= -6)// 四个角
        {
            fl_value = maxFaderGain;
            fr_value = rl_value = rr_value = 0;
        }else if (leftValue >= 6 && rightValue <= -6)
        {
            fr_value = maxFaderGain;
            fl_value = rl_value = rr_value = 0;
        }else if (leftValue <= -6 && rightValue >= 6)
        {
            rl_value = maxFaderGain;
            fl_value = fr_value = rr_value = 0;
        }else if (leftValue >= 6 && rightValue >= 6)
        {
            rr_value = maxFaderGain;
            fl_value = fr_value = rl_value = 0;
        }else if (leftValue < -1 && rightValue < -1)// fl
        {
            fl_value = maxFaderGain;
            fr_value = maxFaderGain - (-leftValue)*step;
            rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
        }else if (leftValue > -1 && rightValue < -1) //fr
        {
            fr_value = maxFaderGain;
            fl_value = maxFaderGain - leftValue*step;
            rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
        }else if (leftValue < -1 && rightValue > 1)//rl
        {
            rl_value = maxFaderGain;
            rr_value = maxFaderGain - (-leftValue)*step;
            fl_value = fr_value = maxFaderGain - (halfValue + rightValue)*step;
        }else if (leftValue > 1 && rightValue > 1)//rr
        {
            rr_value = maxFaderGain;
            rl_value = maxFaderGain - leftValue*step;
            fl_value = fr_value = maxFaderGain - (halfValue + rightValue)*step;
        }else if (leftValue < -1 && rightValue <= 1 && rightValue >= -1) //x y 轴线周围 fl rl
        {
            fl_value = rl_value = maxFaderGain;
            fr_value = rr_value = maxFaderGain - (halfValue + (-leftValue))*step;
        }else if (leftValue <= 1 && leftValue >= -1 && rightValue < -1)//fl fr
        {
            fl_value = fr_value = maxFaderGain;
            rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
        }else if (leftValue > 1 && rightValue >= -1 && rightValue <= 1)//fr rr
        {
            fr_value = rr_value = maxFaderGain;
            fl_value = rl_value = maxFaderGain - (halfValue + (leftValue))*step;
        }else if (leftValue <= 1 && leftValue >= -1 && rightValue > 1)//rl rr
        {
            rl_value = rr_value = maxFaderGain;
            fl_value = fr_value = maxFaderGain - (halfValue + (rightValue))*step;
        }
        control.Set(PAOUTPUTFL_AUDIO, fl_value);
        control.Set(PAOUTPUTFR_AUDIO, fr_value);
        control.Set(PAOUTPUTRL_AUDIO, rl_value);
        control.Set(PAOUTPUTRR_AUDIO, rr_value);
    }
    emit onSoundItemChange(item, leftValue, rightValue);
}

AudioServicePrivate::AudioServicePrivate(AudioService *parent)
    : q_ptr(parent)
{
    m_nMinVol = 0;
    m_nMaxVol = 120;
    m_Step = 1.0f;
    m_CurrentVolume = 35;
    m_CurrentSoundLeft = 0;
    m_CustomSoundLeft = AudioPersistent::getLeftSound();
    m_CurrentSoundRight = 0;
    m_CustomSoundRight = AudioPersistent::getRightSound();
    m_CurrentBass = 0;
    m_CustomBass = AudioPersistent::getBassEqualizer();
    m_CurrentMiddle = 0;
    m_CustomMiddle = AudioPersistent::getMiddleEqualizer();
    m_CurrentTreble = 0;
    m_CustomTreble = AudioPersistent::getTrebleEqualizer();
    m_AudioSource = AS_Idle;
    m_AudioCardType = ACT_Default;
    m_MuteItem = MI_Unmute;
    m_SpeakerSoundItem = SSI_PowerOn;
    initialize();
}

AudioServicePrivate::~AudioServicePrivate()
{
}

void AudioServicePrivate::initialize()
{
    Q_Q(AudioService);
    bool ret = QDBusConnection::sessionBus().registerService(ArkMicroAudioService);
    ret = ret && QDBusConnection::sessionBus().registerObject(ArkMicroAudioPath,
                                                              q,
                                                              QDBusConnection::ExportNonScriptableSignals
                                                              | QDBusConnection::ExportNonScriptableSlots);
    if (!ret) {
        qCritical() << "AudioService Register QDbus failed!";
        exit(EXIT_FAILURE);
    }
    bool exists(QFile::exists("/tmp/audio"));
    ret = !exists;
    ret = ret && insmodKO(QString("/lib/modules/3.4.0/kernel/drivers/ark/audio/ark-cs4334-codec.ko"));
    ret = ret && insmodKO(QString("/lib/modules/3.4.0/kernel/drivers/ark/audio/ark-sddac-codec.ko"));
    ret = ret && insmodKO(QString("/lib/modules/3.4.0/kernel/drivers/ark/audio/snd-soc-ark-i2s.ko"));
    ret = ret && insmodKO(QString("/lib/modules/3.4.0/kernel/drivers/ark/audio/snd-soc-ark-pcm-dma.ko"));
    ret = ret && insmodKO(QString("/lib/modules/3.4.0/kernel/drivers/ark/audio/snd-soc-ark-sddac.ko"));
    //    rmmod -f snd-soc-ark-sddac.ko snd-soc-ark-pcm-dma.ko snd-soc-ark-i2s.ko ark-sddac-codec.ko ark-cs4334-codec.ko
    /*    if (!ret) {
        if (!exists) {
            qWarning() << "AudioService driver initialize failed!";
        } else {
            qWarning() << "AudioService driver already initialize!";
        }
    } else */{
        //        QFile file(QString("/tmp/audio"));
        //        if (file.open(QIODevice::WriteOnly)) {
        //            file.close();
        //            qWarning() << "AudioService driver initialize ok!";
        //        }
        if (QString(qgetenv("PROTOCOL_ID")).contains(QString("ark169"))) {
            m_AudioSource = AS_Radio;
            m_AudioCardType = ACT_Ark169;
            m_Step = 2.000000f;
            m_CurrentVolume = AudioPersistent::getVolume();
            painputswitch_id = "numid=13,iface=MIXER,name='PA Input Select'";
            paoutputvolume_id = "numid=23,iface=MIXER,name='PA Volume'";
            pamuteswitch_id = "numid=21,iface=MIXER,name='PA Mute'";
            paoutputfl_id = "numid=15,iface=MIXER,name='PA Fader-FL'";
            paoutputfr_id = "numid=16,iface=MIXER,name='PA Fader-FR'";
            paoutputrl_id = "numid=17,iface=MIXER,name='PA Fader-RL'";
            paoutputrr_id = "numid=18,iface=MIXER,name='PA Fader-RR'";
            paoutputbass_id = "numid=3,iface=MIXER,name='EQ Bass'";
            paoutputmiddle_id = "numid=6,iface=MIXER,name='EQ Middle'";
            paoutputtreble_id = "numid=9,iface=MIXER,name='EQ Treble'";
            initializeArk169();
        } else if (QString(qgetenv("PROTOCOL_ID")).contains(QString("yaoxi"))) {
            m_AudioSource = AS_Radio;
            m_AudioCardType = ACT_Yaoxi;
            m_Step = 3.175000f;
            m_CurrentVolume = AudioPersistent::getVolume();
            painputswitch_id = "numid=6,iface=MIXER,name='PA Input-Select'";
            paoutputvolume_id = "numid=5,iface=MIXER,name='PA Volume'";
            paoutputfl_id = "numid=8,iface=MIXER,name='Speaker FL'";
            paoutputfr_id = "numid=9,iface=MIXER,name='Speaker FR'";
            paoutputrl_id = "numid=10,iface=MIXER,name='Speaker RL'";
            paoutputrr_id = "numid=11,iface=MIXER,name='Speaker RR'";
            paoutputbass_id = "numid=3,iface=MIXER,name='EQ Bass'";
            paoutputtreble_id = "numid=4,iface=MIXER,name='EQ Treble'";
            initializeYaoxi();
        } else {
            m_Step = 3.175000f;
            m_CurrentVolume = AudioPersistent::getVolume();
            initializeDefault();
        }
    }
}

void AudioServicePrivate::initializeArk169()
{
    AudioControl control;
    SetOutputVolume(m_CurrentVolume);
    SoundItem soundItem = AudioPersistent::getSoundItem();
    int leftValue;
    int rightValue;
    switch (soundItem) {
    case SI_Master: {
        leftValue = -3;
        rightValue = -3;
        break;
    }
    case SI_Slave: {
        leftValue = 3;
        rightValue = -3;
        break;
    }
    case SI_RearLeft: {
        leftValue = -3;
        rightValue = 3;
        break;
    }
    case SI_RearRight: {
        leftValue = 3;
        rightValue = 3;
        break;
    }
    default: {//customer
        // if (-1 == leftValue) {
            leftValue = m_CustomSoundLeft;
        // }
        // if (-1 == rightValue) {
            rightValue = m_CustomSoundRight;
        // }
        break;
    }
    }

    m_CurrentSoundLeft = leftValue;
    m_CurrentSoundRight = rightValue;

    int fl_value;
    int fr_value;
    int rl_value;
    int rr_value;
    int maxFaderGain = 95;
    int step = 5;
    int halfValue = 7;

    if ((-1<= leftValue && leftValue <= 1) && (-1 <= rightValue && rightValue <= 1))//x y中心 全车
    {
        fl_value = fr_value = rl_value = rr_value = maxFaderGain;
    }else if (leftValue <= -6 && rightValue <= -6)// 四个角
    {
        fl_value = maxFaderGain;
        fr_value = rl_value = rr_value = 0;
    }else if (leftValue >= 6 && rightValue <= -6)
    {
        fr_value = maxFaderGain;
        fl_value = rl_value = rr_value = 0;
    }else if (leftValue <= -6 && rightValue >= 6)
    {
        rl_value = maxFaderGain;
        fl_value = fr_value = rr_value = 0;
    }else if (leftValue >= 6 && rightValue >= 6)
    {
        rr_value = maxFaderGain;
        fl_value = fr_value = rl_value = 0;
    }else if (leftValue < -1 && rightValue < -1)// fl
    {
        fl_value = maxFaderGain;
        fr_value = maxFaderGain - (-leftValue)*step;
        rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
    }else if (leftValue > -1 && rightValue < -1) //fr
    {
        fr_value = maxFaderGain;
        fl_value = maxFaderGain - leftValue*step;
        rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
    }else if (leftValue < -1 && rightValue > 1)//rl
    {
        rl_value = maxFaderGain;
        rr_value = maxFaderGain - (-leftValue)*step;
        fl_value = fr_value = maxFaderGain - (halfValue + rightValue)*step;
    }else if (leftValue > 1 && rightValue > 1)//rr
    {
        rr_value = maxFaderGain;
        rl_value = maxFaderGain - leftValue*step;
        fl_value = fr_value = maxFaderGain - (halfValue + rightValue)*step;
    }else if (leftValue < -1 && rightValue <= 1 && rightValue >= -1) //x y 轴线周围 fl rl
    {
        fl_value = rl_value = maxFaderGain;
        fr_value = rr_value = maxFaderGain - (halfValue + (-leftValue))*step;
    }else if (leftValue <= 1 && leftValue >= -1 && rightValue < -1)//fl fr
    {
        fl_value = fr_value = maxFaderGain;
        rl_value = rr_value = maxFaderGain - (halfValue + (-rightValue))*step;
    }else if (leftValue > 1 && rightValue >= -1 && rightValue <= 1)//fr rr
    {
        fr_value = rr_value = maxFaderGain;
        fl_value = rl_value = maxFaderGain - (halfValue + (leftValue))*step;
    }else if (leftValue <= 1 && leftValue >= -1 && rightValue > 1)//rl rr
    {
        rl_value = rr_value = maxFaderGain;
        fl_value = fr_value = maxFaderGain - (halfValue + (rightValue))*step;
    }


    AudioControl soundControl;
    soundControl.Set(PAOUTPUTFL_AUDIO, fl_value);
    soundControl.Set(PAOUTPUTFR_AUDIO, fr_value);
    soundControl.Set(PAOUTPUTRL_AUDIO, rl_value);
    soundControl.Set(PAOUTPUTRR_AUDIO, rr_value);

    EqualizerItem equalizerItem = AudioPersistent::getEqualizerItem();
    int bassValue;
    int middleValue;
    int trebleValue;
    switch (equalizerItem) {
    case EI_Standard: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 7;
        break;
    }
    case EI_Popular: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 11;
        break;
    }
    case EI_Classic: {
        bassValue = 11;
        middleValue = 9;
        trebleValue = 2;
        break;
    }
    case EI_Jazz: {
        bassValue = 10;
        middleValue = 11;
        trebleValue = 11;
        break;
    }
    default: {
        bassValue = m_CustomBass;
        middleValue = m_CustomMiddle;
        trebleValue = m_CustomTreble;
        break;
    }
    }
    m_CurrentBass = bassValue;
    m_CurrentMiddle = middleValue;
    m_CurrentTreble = trebleValue;
    AudioControl eqControl;
    eqControl.Set(PAOUTPUTBASS_AUDIO, bassValue * 2);
    eqControl.Set(PAOUTPUTMIDDLE_AUDIO, middleValue * 2);
    eqControl.Set(PAOUTPUTTREBLE_AUDIO, trebleValue * 2);
    //            eqControl.Set(PAMUTESWITCH_AUDIO, 0);
    control.Set(PAINPUTSWITCH_AUDIO, 4);
}

void AudioServicePrivate::initializeYaoxi()
{
    AudioControl control;
    SetOutputVolume(m_CurrentVolume);
    SoundItem soundItem = AudioPersistent::getSoundItem();
    int leftValue;
    int rightValue;
    switch (soundItem) {
    case SI_Master: {
        leftValue = -3;
        rightValue = -3;
        break;
    }
    case SI_Slave: {
        leftValue = 3;
        rightValue = -3;
        break;
    }
    case SI_RearLeft: {
        leftValue = -3;
        rightValue = 3;
        break;
    }
    case SI_RearRight: {
        leftValue = 3;
        rightValue = 3;
        break;
    }
    default: {
        if (-1 == leftValue) {
            leftValue = m_CustomSoundLeft;
        }
        if (-1 == rightValue) {
            rightValue = m_CustomSoundRight;
        }
        break;
    }
    }
    AudioControl soundControl;
    soundControl.Set(PAOUTPUTFL_AUDIO, 11 + leftValue + rightValue);
    soundControl.Set(PAOUTPUTFR_AUDIO, 11 + leftValue + rightValue);
    soundControl.Set(PAOUTPUTRL_AUDIO, 11 - leftValue - rightValue);
    soundControl.Set(PAOUTPUTRR_AUDIO, 11 - leftValue - rightValue);

    EqualizerItem equalizerItem = AudioPersistent::getEqualizerItem();
    int bassValue;
    int middleValue;
    int trebleValue;
    switch (equalizerItem) {
    case EI_Standard: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 7;
        break;
    }
    case EI_Popular: {
        bassValue = 7;
        middleValue = 7;
        trebleValue = 11;
        break;
    }
    case EI_Classic: {
        bassValue = 11;
        middleValue = 9;
        trebleValue = 2;
        break;
    }
    case EI_Jazz: {
        bassValue = 10;
        middleValue = 11;
        trebleValue = 11;
        break;
    }
    default: {
        bassValue = m_CustomBass;
        middleValue = m_CustomMiddle;
        trebleValue = m_CustomTreble;
        break;
    }
    }
    AudioControl eqControl;
    eqControl.Set(PAOUTPUTBASS_AUDIO, bassValue);
    eqControl.Set(PAOUTPUTMIDDLE_AUDIO, middleValue);
    eqControl.Set(PAOUTPUTTREBLE_AUDIO, trebleValue);
    //            eqControl.Set(PAMUTESWITCH_AUDIO, 0);
    control.Set(PAINPUTSWITCH_AUDIO, 1);
}

void AudioServicePrivate::initializeDefault()
{
    AudioControl control;
    int min, max;
    control.GetRange(LEFT_AUDIO,&min, &max);
    m_nMinVol = min;
    m_nMaxVol = max;
    float distance = m_nMaxVol - m_nMinVol;
    m_Step = distance / VOL_MAX_LEVEL;
    control.Set(LEFT_AUDIO, max);
    control.GetRange(RIGHT_AUDIO,&min, &max);
    control.Set(RIGHT_AUDIO, max);
    control.Set(IRSTATUS_AUDIO, 0);
    control.Set(OUTPUT_AUDIO, 0);
    control.Set(FM_AUDIO, 885);
    SetInputAudio(AS_Idle);
    SetOutputAudio(m_CurrentVolume);
}

bool AudioServicePrivate::insmodKO(const QString &path)
{
    bool flag(QFile::exists(path));
    if (flag) {
        QString cmd = QString("insmod ") + QString(" ") + path;
        if (0 != system(cmd.toLocal8Bit().data())) {
            flag = false;
        }
    }
    return flag;
}

//unused                                    Item #0 'A_SINGLE'
//unused                                    Item #1 'B_SINGLE'
//bluetooth                                 Item #2 'C_SINGLE:BT'
//music,video,carplay,carlife...     Item #3 'D_SINGLE:NAVI'
//radio                                     Item #4 'E1_SINGLE:RADIO'
//aux in                                    Item #5 'E2_SINGLE:AUX'
//unused Item #6 'D_DIFF'
//unused Item #7 'E_FULL_DIFF'

void AudioServicePrivate::SetInputAudio(int source)
{
    AudioControl control;
    switch(source)  {
    case AS_Idle: {
        if (ACT_Ark169 == m_AudioCardType) {
        } else {
            control.Set(PAMUTE_AUDIO, 1);
            control.Set(STRAM_AUDIO, 2);
        }
    }
    case AS_Aux: {
        if (ACT_Ark169 == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 5);
        } else if (ACT_Yaoxi == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 3);
        } else {
            control.Set(PAMUTE_AUDIO, 1);
            control.Set(STRAM_AUDIO, 2);
        }
        break;
    }
    case AS_Radio: {
        if (ACT_Ark169 == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 4);
        } else if (ACT_Yaoxi == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 1);
        }
        break;
    }
    case AS_Music:
    case AS_Video:
    case AS_CarplayMusic:
    case AS_CarplayPhone:
    case AS_AutoMusic:
    case AS_CarlifeMusic: {
        if (ACT_Ark169 == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 3);
        } else if (ACT_Yaoxi == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 0);
        } else {
            control.Set(PAMUTE_AUDIO, 0);
            control.Set(STRAM_AUDIO, 2);
        }
        break;
    }
    case AS_CarlifePhone:
    case AS_AutoPhone:
    case AS_BluetoothMusic:
    case AS_BluetoothPhone: {
        if (ACT_Ark169 == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 2);
        } else if (ACT_Yaoxi == m_AudioCardType) {
            control.Set(PAINPUTSWITCH_AUDIO, 2);
        } else {
            control.Set(PAMUTE_AUDIO,0);
            control.Set(STRAM_AUDIO, 3);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void AudioServicePrivate::SetOutputAudio(int volume)
{
    if (ACT_Ark169 == m_AudioCardType) {
    } else {
        AudioControl control;
        control.Set(FM_AUDIO, 0);
        control.Set(IRSTATUS_AUDIO, 0);
        control.Set(OUTPUT_AUDIO, 1);
        SetOutputVolume(volume);
    }
}

void AudioServicePrivate::SetOutputVolume(int volume)
{
#if 0
    AudioControl control;
    int nValue = pos2volume(volume);
    control.Set(AMP_AUDIO,0);
    usleep(10);
    control.Set(AMP_AUDIO,nValue/2);
    usleep(10);
    control.Set(AMP_AUDIO,nValue);
#else
    AudioControl control;
    int nValue = pos2volume(volume);
    switch (m_AudioCardType) {
    case ACT_Ark169:
    case ACT_Yaoxi: {
        control.Set(PAOUTPUTVOLUME_AUDIO, nValue);
        break;
    }
    default: {
        control.Set(LEFT_AUDIO,0);
        control.Set(RIGHT_AUDIO,0);
        usleep(10);
        control.Set(LEFT_AUDIO,nValue/2);
        control.Set(RIGHT_AUDIO,nValue/2);
        usleep(10);
        control.Set(LEFT_AUDIO,nValue);
        control.Set(RIGHT_AUDIO,nValue);
        break;
    }
    }
    //    if (ACT_Ark169 == m_AudioCardType) {
    //        //        control.Set(PAOUTPUTVOLUME_AUDIO, 0);
    //    } else {
    //        control.Set(LEFT_AUDIO,0);
    //        control.Set(RIGHT_AUDIO,0);
    //        usleep(10);
    //    }
    //    if (ACT_Ark169 == m_AudioCardType) {
    //        //        control.Set(PAOUTPUTVOLUME_AUDIO, nValue/2);
    //    } else {
    //        control.Set(LEFT_AUDIO,nValue/2);
    //        control.Set(RIGHT_AUDIO,nValue/2);
    //        usleep(10);
    //    }
    //    if (ACT_Ark169 == m_AudioCardType) {
    //        control.Set(PAOUTPUTVOLUME_AUDIO, nValue);
    //    } else {
    //        control.Set(LEFT_AUDIO,nValue);
    //        control.Set(RIGHT_AUDIO,nValue);
    //    }
#endif
}

int AudioServicePrivate::pos2volume(int pos)
{
    if (pos >= 40) {
        switch (m_AudioCardType) {
        case ACT_Yaoxi: {
            return pos;
            break;
        }
        case ACT_Ark169: {
            return pos * m_Step;
            break;
        }
        default: {
            return m_nMaxVol;
            break;
        }
        }
        //        if (ACT_Ark169 == m_AudioCardType) {
        //            return pos * m_Step;
        //        } else {
        //            return m_nMaxVol;
        //        }
    } else {
        switch (m_AudioCardType) {
        case ACT_Yaoxi: {
            return pos;
            break;
        }
        default: {
            return pos * m_Step;
            break;
        }
        }
    }
}
