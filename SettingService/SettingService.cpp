#include "SettingService.h"
#include "ark_api.h"
#include "SettingPersistent.h"
#include "RunnableThread.h"
#include "Utility.h"
#include "UserInterfaceUtility.h"
#include <QTimer>
#include <unistd.h>
#include <QFile>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <QCoreApplication>

//OSD1_LAYER,//ui
//OSD2_LAYER,//video
//OSD3_LAYER,
//VIDEO_LAYER,//scale
//VIDEO2_LAYER,

// static const unsigned char brightnessStep(14);
// static const unsigned char constrasStep(14);
// static const unsigned char saturationStep(14);
// static const unsigned char maximum(255 - 14);
// static const unsigned char minimum(255 - 14 * 15);

static const unsigned char brightnessStep(3);
static const unsigned char maxbrightness(43);
static const unsigned char minbrightness(0);
// static const unsigned char brightnessStep(14);
static const unsigned char constrasStep(12);
static const unsigned char maxConstrass(228);
static const unsigned char minimumConstrass(59);// 默认拖动条默认7， 默认对比度59 + 12*7 = 143

static const unsigned char saturationStep(10);
static const unsigned char maxSaturation(241);
static const unsigned char minimumSaturation(101);// 默认拖动条默认7， 默认饱和度101 + 10*7 = 171

// static const unsigned char maximum(255 - 14);
// static const unsigned char minimum(255 - 14 * 15);

static void initializeRunnableCallback(void *paramater);

class SettingServicePrivate
{
    Q_DISABLE_COPY(SettingServicePrivate)
public:
    explicit SettingServicePrivate(SettingService* parent);
    ~SettingServicePrivate();
    void initialize();
    void connectAllSlots();
    bool openDVRFileFileDescriptor();
    bool switchDVRChannel(const dvr_source type);
    bool configDVR(const dvr_source type, const int width, const int height);
    bool closeDVRFileFileDescriptor();
    bool startDVRData();
    bool stopDVRData();
    bool openDVRFrameBufferFileDescriptor();
    bool closeDVRFrameBufferFileDescriptor();
    bool detectSignal();
    int m_DVRFileDescriptor;
    int m_FrameBufferFileDescriptor;
    DisplayScreenType m_DisplayScreenType;
    AVType m_AVType;
    AVStatus m_AVStatus;
    bool m_Preemptive;
    int m_Width;
    int m_Height;
    bool m_DetactFlag;
    QTimer* m_Timer;
    LanguageType m_Language;
    unsigned char m_Brightness;
    unsigned char m_Contrast;
    unsigned char m_Saturation;
    HourClockType m_HourClockType;
    ThemeType m_ThemeType;
private:
    SettingService* m_Parent;
};

SettingService::SettingService(QObject *parent)
    : QObject(parent)
    , m_Private(new SettingServicePrivate(this))
{
}

SettingService::~SettingService()
{
}

void SettingService::synchronize()
{
    emit onLanguageChange(m_Private->m_Language);
    emit onBrightnessChange(m_Private->m_Brightness / brightnessStep);
    emit onContrastChange(m_Private->m_Contrast / constrasStep);
    emit onSaturationChange(m_Private->m_Saturation / saturationStep);
}

void SettingService::reset()
{
    SettingPersistent::reset();
}

void SettingService::setLanguage(const LanguageType language)
{
    if (language != m_Private->m_Language) {
        m_Private->m_Language = language;
        SettingPersistent::setLanguage(m_Private->m_Language);
    }
    emit onLanguageChange(m_Private->m_Language);
}

void SettingService::setBrightness(const SettingType type, const int value)
{
    switch (type) {
    case ST_Value: {
        m_Private->m_Brightness = minbrightness + value * brightnessStep;
        break;
    }
    case ST_Plus: {
        if (m_Private->m_Brightness < (maxbrightness - brightnessStep)) {
            m_Private->m_Brightness += value * brightnessStep;
        } else {
            m_Private->m_Brightness = maxbrightness;
        }
        break;
    }
    case ST_Minus:
    default: {
        if (m_Private->m_Brightness >= (minbrightness + brightnessStep)) {
            m_Private->m_Brightness -= value * brightnessStep;
        } else {
            m_Private->m_Brightness = minbrightness;
        }
        break;
    }
    }
    // int ret = arkapi_set_layer_brightness(OSD1_LAYER, m_Private->m_Brightness);
    // if (0 == ret) {
    //     int tempValue = (m_Private->m_Brightness - minimum) / brightnessStep;
    //     if ((value + 7) != tempValue) {
    //         SettingPersistent::setBrightness(tempValue);
    //     }
    //     emit onBrightnessChange(tempValue);
    // }
    SettingPersistent::setBrightness(m_Private->m_Brightness);
    emit onBrightnessChange(m_Private->m_Brightness/brightnessStep);
}

void SettingService::setContrast(const SettingType type, const int value)
{
    switch (type) {
    case ST_Value: {
        m_Private->m_Contrast = minimumConstrass + value * constrasStep;
        break;
    }
    case ST_Plus: {
        if (m_Private->m_Contrast < (maxConstrass - constrasStep)) {
            m_Private->m_Contrast += value * constrasStep;
        } else {
            m_Private->m_Contrast = maxConstrass;
        }
        break;
    }
    case ST_Minus:
    default: {
        if (m_Private->m_Contrast >= (minimumConstrass + constrasStep)) {
            m_Private->m_Contrast -= value * constrasStep;
        } else {
            m_Private->m_Contrast = minimumConstrass;
        }
        break;
    }
    }
   // int ret = arkapi_set_layer_contrast(OSD1_LAYER, m_Private->m_Contrast);
    int ret = 0;
    if (0 == ret) {
        int tempValue = (m_Private->m_Contrast - minimumConstrass) / constrasStep;
        if ((value + 7) != tempValue) {
            SettingPersistent::setContrast(tempValue);
        }
        emit onContrastChange(tempValue);
    }
}

void SettingService::setSaturation(const SettingType type, const int value)
{
    switch (type) {
    case ST_Value: {
        m_Private->m_Saturation = minimumSaturation + value * saturationStep;
        break;
    }
    case ST_Plus: {
        if (m_Private->m_Saturation < (maxSaturation - saturationStep)) {
            m_Private->m_Saturation += value * saturationStep;
        } else {
            m_Private->m_Saturation = maxSaturation;
        }
        break;
    }
    case ST_Minus:
    default: {
        if (m_Private->m_Saturation >= (minimumSaturation + saturationStep)) {
            m_Private->m_Saturation -= value * saturationStep;
        } else {
            m_Private->m_Saturation = minimumSaturation;
        }
        break;
    }
    }
   // int ret = arkapi_set_layer_saturation(OSD1_LAYER, m_Private->m_Saturation);
    int ret = 0;
    if (0 == ret) {
        int tempValue = (m_Private->m_Saturation - minimumSaturation) / saturationStep;
        if ((value + 7) != tempValue) {
            SettingPersistent::setSaturation(tempValue);
        }
        emit onSaturationChange(tempValue);
    }
}

void SettingService::setDisplayScreen(const DisplayScreenType type)
{
    int ret(0);
    switch (type) {
    case DST_On: {
        SettingPersistent::restoreBrightness();
        // ret = system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
        break;
    }
    case DST_Off: {
        ret = system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
        break;
    }
    default: {
        break;
    }
    }
    if (0 == ret) {
        emit onDisplayScreenChange(type);
    }
}

void SettingService::toggleHourClock()
{
    if (HCT_12HourClock == m_Private->m_HourClockType) {
        m_Private->m_HourClockType = HCT_24HourClock;
    } else {
        m_Private->m_HourClockType = HCT_12HourClock;
    }
    SettingPersistent::setHourClockType(m_Private->m_HourClockType);
    emit onHourClockChange(m_Private->m_HourClockType);
}

void SettingService::startAV(const AVType type, const int width, const int height)
{
    dvr_source avType(DVR_SOURCE_CAMERA);
    if (AVT_0 == type) {
        if (AVT_Undefine != m_Private->m_AVType) {
            m_Private->m_Timer->stop();
            m_Private->closeDVRFrameBufferFileDescriptor();
            m_Private->stopDVRData();
            m_Private->closeDVRFileFileDescriptor();
        }
        m_Private->m_Preemptive = true;
        avType = DVR_SOURCE_CAMERA;
    } else if ((AVT_1 == type)
               || (AVT_2 == type)) {
        if (AVT_1 == type) {
            avType = DVR_SOURCE_DVD;
        } else if (AVT_2 == type) {
            avType = DVR_SOURCE_AUX;
        }
        m_Private->m_Width = width;
        m_Private->m_Height = height;
        if (AVT_Undefine == m_Private->m_AVType) {
            m_Private->m_AVType = type;
        }
        if (m_Private->m_Preemptive) {
            emit onAVStatus(type, AVS_Detacted);
            return;
        }
    } else {
        return;
    }
    if (-1 == m_Private->m_DVRFileDescriptor) {
        if (m_Private->openDVRFileFileDescriptor()) {
            if (m_Private->switchDVRChannel(avType)) {
                m_Private->configDVR(avType, width, height);
                //                m_Private->startDVRData();
                //                if (m_Private->openDVRFrameBufferFileDescriptor()) {
                if (DVR_SOURCE_CAMERA != avType) {
                    m_Private->detectSignal();
                    usleep(155555);
                }
                if (m_Private->detectSignal()) {
                    m_Private->m_AVStatus = AVS_Detacted;
                } else {
                    m_Private->m_AVStatus = AVS_NoDetact;
                }
                m_Private->m_DetactFlag = false;
                m_Private->m_Timer->start();
                //                } else {
                ////                    m_Private->stopDVRData();
                //                    m_Private->m_AVStatus = AVS_NoDetact;
                //                }
            } else {
                m_Private->closeDVRFileFileDescriptor();
            }
        } else {
            m_Private->m_AVStatus = AVS_NoDetact;
        }
    }
    emit onAVStatus(type, m_Private->m_AVStatus);
}

void SettingService::stopAV(const AVType type)
{
    if (AVT_0 == type) {
        m_Private->m_Timer->stop();
        m_Private->closeDVRFrameBufferFileDescriptor();
        m_Private->stopDVRData();
        m_Private->closeDVRFileFileDescriptor();
        m_Private->m_AVStatus = AVS_NoDetact;
        m_Private->m_Preemptive = false;
        if (AVT_Undefine != m_Private->m_AVType) {
            startAV(m_Private->m_AVType, m_Private->m_Width, m_Private->m_Height);
        }
    } else if ((AVT_1 == type)
               || (AVT_2 == type)) {
        if (m_Private->m_AVType == type) {
            m_Private->m_AVType = AVT_Undefine;
            if (!m_Private->m_Preemptive) {
                m_Private->m_Timer->stop();
                m_Private->closeDVRFrameBufferFileDescriptor();
                m_Private->stopDVRData();
                m_Private->closeDVRFileFileDescriptor();
                m_Private->m_AVStatus = AVS_NoDetact;
            }
            m_Private->m_DetactFlag = false;
        }
        //        emit onAVStatus(type, m_Private->m_AVStatus);
    }
}

void SettingService::setThemeType(const ThemeType type)
{
    m_Private->m_ThemeType = type;
    SettingPersistent::setThemeType(m_Private->m_ThemeType);
    emit onThemeChange(m_Private->m_ThemeType);
}

void SettingService::onTimeout()
{
    if (-1 != m_Private->m_DVRFileDescriptor) {
        if (m_Private->detectSignal()) {
            QFile file(QString("/tmp/video"));
            if (!file.exists()
                    && file.open(QIODevice::WriteOnly)) {
                file.close();
            }
            if (AVS_Detacted != m_Private->m_AVStatus) {
                m_Private->m_AVStatus = AVS_Detacted;
                if (m_Private->m_Preemptive) {
                    emit onAVStatus(AVT_0, m_Private->m_AVStatus);
                } else {
                    emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
                }
            }
            m_Private->openDVRFrameBufferFileDescriptor();
            if (!m_Private->m_DetactFlag) {
                //arkapi_gui_tvout(0);
                m_Private->m_DetactFlag = m_Private->startDVRData();
            }
        } else {
            if (m_Private->m_DetactFlag) {
                m_Private->stopDVRData();
            }
            if (AVS_NoDetact != m_Private->m_AVStatus) {
                m_Private->m_AVStatus = AVS_NoDetact;
                if (m_Private->m_Preemptive) {
                    emit onAVStatus(AVT_0, m_Private->m_AVStatus);
                } else {
                    emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
                }
            }
        }
        //        qWarning() << "SettingService::onTimeout";
    }
    //    if (-1 != m_Private->m_DVRFileDescriptor) {
    //        static bool ret = false;
    //        //        ret = m_Private->detectSignal();
    //        if (ret != m_Private->detectSignal()) {
    //            ret = !ret;
    //            qWarning() << __PRETTY_FUNCTION__ << ret;
    //        }
    //        switch (m_Private->m_Timer->interval()) {
    //        case 0: {
    //            if (m_Private->m_DetactFlag) {
    //                m_Private->m_DetactFlag = false;
    //                m_Private->m_AVStatus = AVS_Detacted;
    //                if (ret) {
    //                    emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
    //                }
    //                m_Private->m_Timer->start(11);
    //            } else {
    //                if (!ret) {
    //                    m_Private->m_DetactFlag = false;
    //                    bool filter(true);
    //                    if (AVS_Play == m_Private->m_AVStatus) {
    //                        m_Private->m_DetactFlag = true;
    //                        m_Private->m_AVStatus = AVS_NoDetact;
    //                        m_Private->closeDVRFrameBufferFileDescriptor();
    //                        m_Private->stopDVRData();
    //                        filter = false;
    //                    } else if (AVS_Detacted == m_Private->m_AVStatus) {
    //                        m_Private->m_AVStatus = AVS_NoDetact;
    //                        filter = false;
    //                    }
    //                    if (!filter) {
    //                        emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
    //                    }
    //                } else {
    //                    if (AVS_NoDetact == m_Private->m_AVStatus) {
    //                        m_Private->m_DetactFlag = true;
    //                    }
    //                }
    //            }
    //            break;
    //        }
    //        case 11: {
    //            if (ret) {
    //                bool filter(true);
    //                if (AVS_Play != m_Private->m_AVStatus) {
    //                    if (AVS_Detacted == m_Private->m_AVStatus) {
    //                        emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
    //                        if (AVT_0 == m_Private->m_AVType) {
    //                            m_Private->configDVR(DVR_SOURCE_CAMERA, m_Private->m_Width, m_Private->m_Height);
    //                        } else if (AVT_1 == m_Private->m_AVType) {
    //                            m_Private->configDVR(DVR_SOURCE_AUX, m_Private->m_Width, m_Private->m_Height);
    //                        } else {
    //                            m_Private->configDVR(DVR_SOURCE_DVD, m_Private->m_Width, m_Private->m_Height);
    //                        }
    //                        m_Private->startDVRData();
    //                        filter = false;
    //                        if (m_Private->openDVRFrameBufferFileDescriptor()) {
    //                            m_Private->m_AVStatus = AVS_Play;
    //                        } else {
    //                            m_Private->stopDVRData();
    //                            m_Private->m_AVStatus = AVS_NoDetact;
    //                        }
    //                    }
    //                }
    //                if (!filter) {
    //                    emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
    //                }
    //            } else {
    //                m_Private->m_AVStatus = AVS_NoDetact;
    //                emit onAVStatus(m_Private->m_AVType, m_Private->m_AVStatus);
    //            }
    //            m_Private->m_Timer->start(0);
    //            break;
    //        }
    //        default: {
    //            break;
    //        }
    //        }
    //    }
}

SettingServicePrivate::SettingServicePrivate(SettingService *parent)
    : m_Parent(parent)
{
    m_DVRFileDescriptor = -1;
    m_FrameBufferFileDescriptor = -1;
    m_DisplayScreenType = DST_On;
    m_AVType = AVT_Undefine;
    m_AVStatus = AVS_NoDetact;
    m_Preemptive = false;
    m_Width = 0;
    m_Height = 0;
    m_DetactFlag = false;
    m_Timer = NULL;
    m_Language = SettingPersistent::getLanguage();
    m_Brightness = SettingPersistent::getBrightness();
    m_Contrast =  minimumConstrass + constrasStep * SettingPersistent::getContrast();
    m_Saturation = minimumSaturation + saturationStep * SettingPersistent::getSaturation();
    m_HourClockType = SettingPersistent::getHourClockType();
    m_ThemeType = SettingPersistent::getThemeType();
    initialize();
    connectAllSlots();
}

SettingServicePrivate::~SettingServicePrivate()
{
}

void SettingServicePrivate::initialize()
{
    m_Timer = new QTimer(m_Parent);
    m_Timer->setSingleShot(false);
    m_Timer->setInterval(100);
}

void SettingServicePrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_Timer,  SIGNAL(timeout()),
                     m_Parent, SLOT(onTimeout()),
                     type);
}

bool SettingServicePrivate::openDVRFileFileDescriptor()
{
    bool ret(false);
    if (-1 == m_DVRFileDescriptor) {
      //  m_DVRFileDescriptor = arkapi_open_dvr();
        if (m_DVRFileDescriptor < 0) {
            printf("open dvr device failure.\n");
        } else {
            ret = true;
        }
    }
    return ret;
}

bool SettingServicePrivate::switchDVRChannel(const dvr_source type)
{
    bool ret(false);
    if (-1 != m_DVRFileDescriptor) {
      //  if (arkapi_dvr_switch_channel(m_DVRFileDescriptor, type) < 0) {
      //      printf("switch dvr failure.\n");
      //  } else {
      //      ret = true;
      //  }
    }
    return ret;
}

bool SettingServicePrivate::configDVR(const dvr_source type, const int width, const int height)
{
    bool ret(false);
    if (-1 != m_DVRFileDescriptor) {
        int tvout = 0;
        if (!qgetenv("ARKTVOUT").isEmpty()) {
            tvout = 1;
        }
     //   if (arkapi_dvr_config(m_DVRFileDescriptor, type, width, height, tvout) < 0) {
     //      printf("config dvr failure.\n");
     //   } else {
     //       ret = true;
      //  }
    }
    return ret;
}

bool SettingServicePrivate::closeDVRFileFileDescriptor()
{
    bool ret(false);
    if (-1 != m_DVRFileDescriptor) {
      //  arkapi_close_dvr(m_DVRFileDescriptor);
        m_DVRFileDescriptor = -1;
        ret = true;
    }
    return ret;
}

bool SettingServicePrivate::startDVRData()
{
    bool ret(false);
    if (-1 != m_DVRFileDescriptor) {
      //  if (arkapi_dvr_start(m_DVRFileDescriptor) < 0) {
      //      printf("open start dvr failure.\n");
       // } else {
       //     qDebug() << __PRETTY_FUNCTION__ << __LINE__;
       //     ret = true;
       // }
    }
    return ret;
}

bool SettingServicePrivate::stopDVRData()
{
    bool ret(false);
    if (-1 != m_DVRFileDescriptor) {
        if (m_DetactFlag) {
            m_DetactFlag = false;
           // arkapi_dvr_stop(m_DVRFileDescriptor);
        }
        ret = true;
    }
    return ret;
}

bool SettingServicePrivate::openDVRFrameBufferFileDescriptor()
{
    bool ret(false);
    if (-1 == m_FrameBufferFileDescriptor) {
       // m_FrameBufferFileDescriptor = arkapi_open_video_fb();
        if (m_FrameBufferFileDescriptor < 0) {
            printf("open video fb device failure.\n");
        } else {
            ret = true;
        }
    }
    return ret;
}

bool SettingServicePrivate::closeDVRFrameBufferFileDescriptor()
{
    bool ret(false);
    if (-1 != m_FrameBufferFileDescriptor) {
       // arkapi_close_video_fb(m_FrameBufferFileDescriptor);
        m_FrameBufferFileDescriptor = -1;
        ret = true;
    }
    return ret;
}

bool SettingServicePrivate::detectSignal()
{
    //int ret = arkapi_dvr_detect_signal(m_DVRFileDescriptor);
   // return (0 != ret);
    return false;
}
