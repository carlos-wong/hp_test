#include "Setting.h"
#include "BusinessLogicUtility.h"
#include "ArkApplication.h"
#include "UserInterfaceUtility.h"
#include "ark_api.h"
#include "display.h"
#include "Bluetooth.h"
#include "Radio.h"
#include "Audio.h"
#include "Utility.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <QSocketNotifier>
#include <QTranslator>
#include <QTimer>
#include <QFile>
#include <unistd.h>
#include <QMetaType>
#include <QThread>
#include <linux/fb.h>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>

static unsigned short int MCUUPDATETRANSFERBYTE = 512;
//#define MCUUPDATETRANSFERBYTE   256

class SettingPrivate
{
    Q_DISABLE_COPY(SettingPrivate)
public:
    explicit SettingPrivate(Setting* parent);
    ~SettingPrivate();
    void initialize();
    void connectAllSlots();
    void onLanguageChange(const LanguageType language);
    void onArkProtocol(const ArkProtocol* protocol);
    void onArkControlHandler(struct ArkControl* control);
    void onArkCarHandler(struct ArkCar* control);
    void onStartCompleteHandler();
    SettingService* m_SettingService;
    enum ArkCustomer m_ArkCustomer;
    QMap<StudySteeringWheelStatus, struct StudySteeringWheel> m_StudySteeringWheel;
    ArkCarStatus m_Reversing;
    DisplayScreenType m_DisplayScreenType;
    QTimer* m_HeartBeat;
    volatile bool m_Complete;
    QMutex m_Mutex;
    QByteArray m_MCUUpdateArray;
    bool m_LauncherTVOUT;
private:
    Setting* m_Parent;
};

static void ark_protocol_notify(const struct ArkProtocol* protocol, const void* callback_arg)
{
    if (NULL != protocol) {
        const SettingPrivate* ptr = static_cast<const SettingPrivate*>(callback_arg);
        if (NULL != ptr) {
            //            if (ptr->m_Complete) {
            const_cast<SettingPrivate*>(ptr)->onArkProtocol(protocol);
            //            }
        }
    }
}

void Setting::reset()
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->reset();
    }
    ArkControl control;
    control.type = ACLT_SetReset;
    control.length = 0;
    control.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&control);
    setArkProtocol(ArkProtocolWrapper(protocol));
}

void Setting::initializeArkProtocol()
{   
#ifndef DESKTOP_AMD64
    static bool flag(false);
    if (!flag) {
        flag = true;
        m_Private->m_ArkCustomer = ark_protocol_initialize(ark_protocol_notify, static_cast<void*>(m_Private.data()));
        switch (m_Private->m_ArkCustomer) {
        case AC_YAOXI:
        case AC_ARK169: {
            struct ArkControl ctrl;
            ctrl.type = ACLT_Handshake;
            struct ArkProtocol handshake;
            handshake.type = AT_Control;
            handshake.data = (void*)&ctrl;
            handshake.prio = AP_PRIO_NORMAL;
            ark_protocol_send(&handshake);
            break;
        }
        default: {
            break;
        }
        }
    }
#endif
}

void Setting::setStartComplete()
{
    QMutexLocker lock(&m_Private->m_Mutex);
    if (!m_Private->m_Complete) {
        m_Private->m_Complete = true;
        int fd = open("/dev/ark_display", O_RDONLY);
        if (fd >= 0) {
            int data = 0;
            if (ioctl(fd, ARKDISP_GET_BACKCAR_STATUS, &data) != 0) {
                LOG_FFLDEBUG("ARKDISP_GET_BACKCAR_STATUS fail.\n");
                data = 0;
            }
            if (data) {
                enum ArkCarStatus status = (data > 0)? ACS_ReversingOn : ACS_ReversingOff;
                if (status > 0) {
                    LOG_DEBUG("REVERSING ON\n");
                    m_Private->m_Reversing = ACS_ReversingOn;
                    if (-1 == creat("/tmp/video", 0755)) {
                        LOG_DEBUG("create /tmp/video failed!\n");
                    }
                }
            }
        } else {
            LOG_DEBUG("open /dev/ark_display fail!\n");
        }
        if (0 != system("echo 23 > /proc/display")) {
            qDebug() << "notify kernel application handler reverse failed!";
        }
        onStartComplete();
        m_Private->onStartCompleteHandler();
    }
}

bool Setting::startComplete()
{
    return m_Private->m_Complete;
}

void Setting::setLanguage(const LanguageType language)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setLanguage(language);
    }
}

void Setting::setBrightness(const SettingType type, const int value)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setBrightness(type, value);
    }
}

void Setting::setContrast(const SettingType type, const int value)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setContrast(type, value);
    }
}

void Setting::setSaturation(const SettingType type, const int value)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setSaturation(type, value);
    }
}

void Setting::setDisplayScreen(const DisplayScreenType type)
{
    qDebug() << __PRETTY_FUNCTION__ << type;
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setDisplayScreen(type);
    }
}

void Setting::startAV(const AVType type, const int width, const int height)
{
    initializePrivate();
    static bool flag = true;
    if (flag) {
        flag = false;
        if (0 != system("/etc/itu656_load.sh")) {
            flag = true;
            qDebug() << "execute itu656 /etc/itu656_load.sh failed!";
        }
    }
    //    QFile file(QString("/tmp/video"));
    //    if (!file.exists()
    //            && file.open(QIODevice::WriteOnly)) {
    //        file.close();
    //    }
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->startAV(type, width, height);
    }
}

void Setting::stopAV(const AVType type)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->stopAV(type);
    }
}

void Setting::toggleHourClock()
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->toggleHourClock();
    }
}

void Setting::setThemeType(const int type)
{
    initializePrivate();
    if (NULL != m_Private->m_SettingService) {
        m_Private->m_SettingService->setThemeType(type);
    }
}

void Setting::synchronizeSteeringWheelStatus()
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    //    static bool flag(true);
    //    if (flag) {
    m_Private->m_StudySteeringWheel.clear();
    struct StudySteeringWheel status;
    status.type = SSWS_Previous;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Previous, status);
    status.type = SSWS_Next;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Next, status);
    status.type = SSWS_Power;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Power, status);
    status.type = SSWS_Mute;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Mute, status);
    status.type = SSWS_VolumeAdd;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_VolumeAdd, status);
    status.type = SSWS_VolumeSub;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_VolumeSub, status);
    status.type = SSWS_Answer;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Answer, status);
    status.type = SSWS_Mode;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Mode, status);
    status.type = SSWS_Handup;
    status.status = SSWS_Cancel;
    m_Private->m_StudySteeringWheel.insert(SSWS_Handup, status);
    //    flag = false;
    ArkControl control;
    control.type = ACLT_SWCGetTable;
    control.length = 0;
    control.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Control;
    protocol.direction = AD_Setter;
    protocol.data = static_cast<void*>(&control);
    qDebug() << __PRETTY_FUNCTION__ << __LINE__;
    setArkProtocol(ArkProtocolWrapper(protocol));
    //    }
}

void Setting::steeringStudy(const StudySteeringWheelType type, const StudySteeringWheelStatus status)
{
    switch (type) {
    case SSWT_StartSample: {
        ArkControl control;
        control.type = ACLT_SWCStartSampling;
        qDebug() << __PRETTY_FUNCTION__ << __LINE__ << control.type;
        control.length = 0;
        control.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&control);
        setArkProtocol(ArkProtocolWrapper(protocol));
        break;
    }
    case SSWT_ExitSample: {
        ArkControl control;
        control.type = ACLT_SWCExitSampling;
        control.length = 0;
        control.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&control);
        setArkProtocol(ArkProtocolWrapper(protocol));
        break;
    }
    case SSWT_SaveSample: {
        ArkControl control;
        control.type = ACLT_SWCSave;
        control.length = 0;
        control.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&control);
        setArkProtocol(ArkProtocolWrapper(protocol));
        break;
    }
    case SSWT_ResetSample: {
        ArkControl control;
        control.type = ACLT_SWCReset;
        control.length = 0;
        control.data = NULL;
        ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&control);
        setArkProtocol(ArkProtocolWrapper(protocol));
        break;
    }
    case SSWT_StudyCode: {
        ArkControl control;
        control.type = ACLT_SWCStudy;
        control.length = 1;
        unsigned char data[1] = {0x00};
        switch (status) {
        case SSWS_Previous: {
            data[0] = 0x01;
            break;
        }
        case SSWS_Next: {
            data[0] = 0x02;
            break;
        }
        case SSWS_Power: {
            data[0] = 0x04;
            break;
        }
        case SSWS_Mute: {
            data[0] = 0x06;
            break;
        }
        case SSWS_VolumeAdd: {
            data[0] = 0x03;
            break;
        }
        case SSWS_VolumeSub: {
            data[0] = 0x05;
            break;
        }
        case SSWS_Answer: {
            data[0] = 0x07;
            break;
        }
        case SSWS_Mode: {
            data[0] = 0x09;
            break;
        }
        case SSWS_Handup: {
            data[0] = 0x08;
            break;
        }
        default: {
            return;
            break;
        }
        }
        control.data = data;
        ArkProtocol protocol;
        protocol.type = AT_Control;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&control);
        setArkProtocol(ArkProtocolWrapper(protocol));
        break;
    }
    default: {
        break;
    }
    }
}

const QMap<StudySteeringWheelStatus, StudySteeringWheel> &Setting::getSteeringStudy()
{
    return m_Private->m_StudySteeringWheel;
}

ArkCarStatus Setting::getReversing()
{
    return m_Private->m_Reversing;
}

DisplayScreenType Setting::getDisplayScreenType()
{
    return m_Private->m_DisplayScreenType;
}

void Setting::setArkProtocol(const ArkProtocolWrapper &protocol)
{
#ifndef DESKTOP_AMD64
    if ((AC_Undefine != m_Private->m_ArkCustomer)
            && (AC_Error != m_Private->m_ArkCustomer)) {
        struct ArkProtocol source;
        source.type = protocol.type;
        source.direction = protocol.direction;
        source.data = protocol.data;
        source.prio = AP_PRIO_NORMAL;
        ark_protocol_send(&source);
    }
#endif
}

bool Setting::startMCUUpgrade()
{
    bool flag = false;
    if (QFile::exists(QString("/media/udisk/mcu.bin"))) {
        QFile file(QString("/media/udisk/mcu.bin"));
        if (QFile::exists("/tmp/mcu.bin")) {
            if (0 != system("rm /tmp/mcu.bin")) {
                qDebug() << "rm /tmp/mcu.bin fail!";
                return flag;
            }
        }
        int ret = system("cp /media/udisk/mcu.bin /tmp/");
        if (0 != ret) {
            qDebug() << "mcu update file copy fail!";
        } else {
            if (file.open(QFile::ReadOnly)) {
                m_Private->m_MCUUpdateArray = file.readAll();
                if (m_Private->m_MCUUpdateArray.size() > 0) {
                    printf("%x\n", m_Private->m_MCUUpdateArray.at(0));
                }
                if (m_Private->m_MCUUpdateArray.size() > 1) {
                    printf("%x\n", m_Private->m_MCUUpdateArray.at(1));
                }
                if (m_Private->m_MCUUpdateArray.size() > 2) {
                    printf("%x\n", m_Private->m_MCUUpdateArray.at(2));
                }
                if (m_Private->m_MCUUpdateArray.size() > 3) {
                    printf("%x\n", m_Private->m_MCUUpdateArray.at(3));
                }
                if ((m_Private->m_MCUUpdateArray.size() > 4)
                        && (0x0E == m_Private->m_MCUUpdateArray.at(0))
                        && (0xE1 == m_Private->m_MCUUpdateArray.at(1))
                        && (0xEB == m_Private->m_MCUUpdateArray.at(2))
                        && (0xB5 == m_Private->m_MCUUpdateArray.at(3))) {
                    if (QFile::exists(QString("/tmp/128"))) {
                        MCUUPDATETRANSFERBYTE = 128;
                    } else if (QFile::exists(QString("/tmp/256"))) {
                        MCUUPDATETRANSFERBYTE = 256;
                    }
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "MCUUPDATETRANSFERBYTE" << MCUUPDATETRANSFERBYTE;
                    int char_length = m_Private->m_MCUUpdateArray.length();
                    int package_length = char_length / MCUUPDATETRANSFERBYTE;
                    if (char_length % MCUUPDATETRANSFERBYTE) {
                        ++package_length;
                    }
                    m_Private->m_MCUUpdateArray.resize(package_length * MCUUPDATETRANSFERBYTE);
                    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_MCUUpdateArray.size();
                    ArkCar car;
                    car.type = ACT_MCUUpdate;
                    car.status = ACS_MCUUpdateStart;
                    car.length = 0;
                    car.data = NULL;
                    ArkProtocol protocol;
                    protocol.type = AT_Car;
                    protocol.direction = AD_Setter;
                    protocol.data = static_cast<void*>(&car);
                    setArkProtocol(ArkProtocolWrapper(protocol));
                    flag = true;
                } else {
                    qDebug() << "mcu update file header error!";
                }
            } else {
                qDebug() << "mcu update file open error!";
            }
        }
    } else {
        qDebug() << "mcu update file not exists!";
    }
    return flag;
}

void Setting::requestMCUVersion()
{
    ArkCar car;
    car.type = ACT_Version;
    car.status = ACS_Undefine;
    car.length = 0;
    car.data = NULL;
    ArkProtocol protocol;
    protocol.type = AT_Car;
    protocol.direction = AD_Getter;
    protocol.data = static_cast<void*>(&car);
    setArkProtocol(ArkProtocolWrapper(protocol));
}

bool Setting::setLauncherTVOUT(const bool flag)
{
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_LauncherTVOUT << flag;
    bool ret(false);
    if ((flag != m_Private->m_LauncherTVOUT)
            && (ACS_ReversingOff == m_Private->m_Reversing)) {
        m_Private->m_LauncherTVOUT = flag;
        if (m_Private->m_LauncherTVOUT) {
            QFile file(QString("/tmp/video"));
            if (!file.exists()
                    && file.open(QIODevice::WriteOnly)) {
                file.close();
            }

#ifndef DESKTOP_AMD64
            int fd = arkapi_open_video_fb_timeout(1);
            if (fd >= 0) {
                close(fd);
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "arkapi_open_video_fb fail! maybe timeout";
            }
            if (!qgetenv("ARKTVOUT").isEmpty()) {
                if (0 != arkapi_gui_tvout(1)) {
                    qDebug() << "enable arkapi_gui_tvout failed!";
                } else {
                    qDebug() << "enable arkapi_gui_tvout success!";
                    ret = true;
                }
            }
#endif

        }/* else {
            if (!qgetenv("ARKTVOUT").isEmpty()) {
                if (0 != arkapi_gui_tvout(0)) {
                    qDebug() << "disable arkapi_gui_tvout failed!";
                } else {
                    qDebug() << "disable arkapi_gui_tvout success!";
                    ret = true;
                }
            }
        }*/
    }
    return ret;
}

void Setting::onLanguageChange(const LanguageType language)
{
    m_Private->onLanguageChange(language);
}

void Setting::onDisplayScreenHandler(const DisplayScreenType type)
{
    m_Private->m_DisplayScreenType = type;
    onDisplayScreenChange(m_Private->m_DisplayScreenType);
}

void Setting::onArkProtocolHandler(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Control: {
        m_Private->onArkControlHandler(arkProtocolWrapperToArkControl(protocol));
        break;
    }
    case AT_Car: {
        m_Private->onArkCarHandler(arkProtocolWrapperToArkCar(protocol));
        break;
    }
    default: {
        break;
    }
    }
}

void Setting::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    m_Private->initialize();
    m_Private->connectAllSlots();
}

void Setting::onTimeout()
{
#ifndef DESKTOP_AMD64
        struct ArkControl control;
        memset((void*)&control, 0, sizeof(struct ArkCar));
        struct ArkProtocol protocol;
        memset((void*)&protocol, 0, sizeof(struct ArkProtocol));
        protocol.type = AT_Control;
        protocol.direction = AD_Notify;
        protocol.data = &control;
        control.type = ACLT_Heartbeat;
        ark_protocol_send(&protocol);
#endif
}

Setting::Setting(QObject *parent)
    : QObject(parent)
    , m_Private(new SettingPrivate(this))
{
}

Setting::~Setting()
{
}

void Setting::initializePrivate()
{
    if (NULL == m_Private) {
        m_Private.reset(new SettingPrivate(this));
    }
}

SettingPrivate::SettingPrivate(Setting *parent)
    : m_Parent(parent)
{
    m_SettingService = NULL;
    m_ArkCustomer = AC_ARK169;
    m_Reversing = ACS_ReversingOff;
    m_DisplayScreenType = DST_On;
    m_HeartBeat = NULL;
    m_Complete = false;
    m_LauncherTVOUT = false;
    qRegisterMetaType<ArkProtocolWrapper>("ArkProtocolWrapper");
    QObject::connect(m_Parent, ARKSENDER(onArkProtocol(const ArkProtocolWrapper&)),
                     m_Parent, ARKRECEIVER(onArkProtocolHandler(const ArkProtocolWrapper&)),
                     Qt::QueuedConnection);
    m_Parent->startTimer(0);
}

SettingPrivate::~SettingPrivate()
{
}

void SettingPrivate::initialize()
{
    if (NULL == m_SettingService) {
        m_SettingService = new SettingService(m_Parent);
    }
}

void SettingPrivate::connectAllSlots()
{
    if (NULL != m_SettingService) {
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onBrightnessChange(const int)));
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onContrastChange(const int)));
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onSaturationChange(const int)));
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onAVStatus(const int, const int)));
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onHourClockChange(const int)));
        connectSignalAndSignalByNamesake(m_SettingService, m_Parent, ARKSENDER(onThemeChange(const int)));
        Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
        QObject::connect(m_SettingService, ARKSENDER(onDisplayScreenChange(const int)),
                         m_Parent,         ARKRECEIVER(onDisplayScreenHandler(const int)),
                         type);
        QObject::connect(m_SettingService, ARKSENDER(onLanguageChange(const int)),
                         m_Parent,         ARKRECEIVER(onLanguageChange(const int)),
                         type);
    }
}

void SettingPrivate::onLanguageChange(const LanguageType language)
{
    QString languagePath;
    languagePath.clear();
    switch (language) {
    case LT_Chinese: {
        languagePath = QString(":/Languages/Launcher_CN.qm");
        break;
    }
    case LT_Japaness: {
        languagePath = QString(":/Languages/Launcher_JP.qm");
        break;
    }
    case LT_Korean: {
        languagePath = QString(":/Languages/Launcher_KO.qm");
        break;
    }
    case LT_Spanish: {
        languagePath = QString(":/Languages/Launcher_SP.qm");
        break;
    }
    case LT_Portuguese: {
        languagePath = QString(":/Languages/Launcher_PO.qm");
        break;
    }
    case LT_Russian: {
        languagePath = QString(":/Languages/Launcher_RU.qm");
        break;
    }
    case LT_German: {
        languagePath = QString(":/Languages/Launcher_DE.qm");
        break;
    }
    case LT_French: {
        languagePath = QString(":/Languages/Launcher_FR.qm");
        break;
    }
    case LT_English:
    default: {
        languagePath = QString(":/Languages/Launcher_EN.qm");
        break;
    }
    }
    if (ArkApp->installTranslatorPath(languagePath)) {
        qDebug() << "onLanguageTranslateChange" << __LINE__ << language;
        m_Parent->onLanguageTranslateChange(language);
    }
}

void SettingPrivate::onArkProtocol(const ArkProtocol *protocol)
{
    if ((NULL != protocol)
            || (NULL != protocol->data)) {
        if (AT_Car == protocol->type) {
            ArkCar* car = static_cast<ArkCar*>(protocol->data);
            if (NULL != car) {
                if (ACT_Time == car->type) {
                    if (NULL != car) {
                        if (7 == car->length) {
                            unsigned short int year = car->data[0] * 100 + car->data[1];
                            unsigned short int month = car->data[2];
                            unsigned short int day = car->data[3];
                            unsigned short int hour = car->data[4];
                            unsigned short int minute = car->data[5];
                            unsigned short int second = car->data[6];
                            setDateTime(year, month, day, hour, minute, second);
                        }
                    }
                    return;
                } else if (ACT_Version == car->type) {
                    if (AC_ARK169 == m_ArkCustomer) {
                        if ((0 != car->length)
                                && (NULL != car->data)) {
                            SettingPersistent::setMcuVersion(QString::fromLocal8Bit((const char*)car->data, car->length));
                        }
                    }
                    return;
                }else if(ACT_ILLLight == car->type){//UI 启动未完成，仍然需要上报大灯和手刹状态
                    if (ACS_ILLLightOn == car->status)
                    {
                        SettingPersistent::illDetectSetBrightness(true);
                    }else if(ACS_ILLLightOff == car->status){
                        SettingPersistent::illDetectSetBrightness(false);
                    }
                    return;
                }else if(ACT_Brake == car->type){
                    SettingPersistent::setCurrentBrakeStatus(car->status == ACS_BrakeOn?true:false);
                    return;
                }

                QMutexLocker lock(&m_Mutex);
                if (!m_Complete) {
                    return ;
                }
                if (ACT_Reversing == car->type) {
                    if (ACS_ReversingOn == car->status) {
                        int fd = open("/dev/fb0", O_RDONLY);
                        int width(800);
                        int height(480);
                        if (fd) {
                            struct fb_var_screeninfo fb_var;
                            if (-1 != ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)) {
                                width = fb_var.xres;
                                height = fb_var.yres;
                                close(fd);
                            }
                        }
                        if (m_LauncherTVOUT) {
                            m_LauncherTVOUT = false;

#ifndef DESKTOP_AMD64
                                if (!qgetenv("ARKTVOUT").isEmpty()) {
                                    if (0 != arkapi_gui_tvout(0)) {
                                        qDebug() << "disable arkapi_gui_tvout failed!";
                                    } else {
                                        qDebug() << "disable arkapi_gui_tvout success!";
                                    }
                                }
#endif

                        }
                        if (SettingPersistent::getReverseMute())
                        {
                            g_Audio->requestMute(MI_Mute);
                        }

                        m_Parent->startAV(AVT_0, width, height);
                        m_Reversing = ACS_ReversingOn;
                        if (0 != system("echo 24 0 > /proc/display")) {
                            qDebug() << "hide osd1 fail!";
                        }
                        if (DST_Off == g_Setting->getDisplayScreenType()) {
                            usleep(66666);
                            system("echo 30 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
                        }
                    } else if (ACS_ReversingOff == car->status) {
                        if (DST_Off == g_Setting->getDisplayScreenType()) {
                            switch (g_Bluetooth->connectStatus()) {
                            case BCS_Idle:
                            case BCS_Connecting:
                            case BCS_Connected: {
                                system("echo 100 > /sys/devices/platform/pwm-backlight/backlight/pwm-backlight/brightness");
                                break;
                            }
                            default: {
                                break;
                            }
                            }
                        }
                        if (SettingPersistent::getReverseMute())
                        {
                            g_Audio->requestMute(MI_Unmute);
                        }
                        m_Parent->stopAV(AVT_0);
                        m_Reversing = ACS_ReversingOff;
                        //                        if (QFile::exists(QString("/tmp/video"))) {
                        //                            QFile::remove(QString("/tmp/video"));
                        //                        }
                        m_Parent->onArkProtocol(ArkProtocolWrapper(*protocol));
                        if (0 != system("echo 24 1 > /proc/display")) {
                            qDebug() << "show osd1 fail";
                        }
                        return;
                    }
                }
            }
        }
        if (m_Complete) {
            switch (protocol->type) {
            default: {
                m_Parent->onArkProtocol(ArkProtocolWrapper(*protocol));
                break;
            }
            }
        } else {
            switch (protocol->type) {
            case AT_Radio: {
                g_Radio->setInnerRadio(static_cast<struct ArkRadio*>(protocol->data));
                break;
            }
            case AT_Bluetooth: {
                g_Bluetooth->setInnerBluetooth(static_cast<struct ArkBlueTooth*>(protocol->data));
                break;
            }
            default: {
                break;
            }
            }
        }
    }
}

void SettingPrivate::onArkControlHandler(struct ArkControl* control)
{
    if (NULL != control) {
        switch (control->type) {
        case ACLT_Heartbeat: {
            if (NULL == m_HeartBeat) {
                m_HeartBeat = new QTimer(m_Parent);
                QObject::connect(m_HeartBeat, ARKSENDER(timeout()),
                                 m_Parent,    ARKRECEIVER(onTimeout()));
                m_HeartBeat->setSingleShot(false);
                m_HeartBeat->setInterval(1000 * int(*(control->data)));
            }
            break;
        }
        case ACLT_SWCStudyOK:
        case ACLT_SWCStudyCancel:{
            struct StudySteeringWheel status;
            status.type = SSWS_Undefine;
            if (ACLT_SWCStudyOK == control->type) {
                status.status = SSWS_Ok;
            } else {
                status.status = SSWS_Cancel;
            }
            switch (*(control->data)) {
            case 1: {
                status.type = SSWS_Previous;
                break;
            }
            case 2: {
                status.type = SSWS_Next;
                break;
            }
            case 3: {
                status.type = SSWS_VolumeAdd;
                break;
            }
            case 4: {
                status.type = SSWS_Power;
                break;
            }
            case 5: {
                status.type = SSWS_VolumeSub;
                break;
            }
            case 6: {
                status.type = SSWS_Mute;
                break;
            }
            case 7: {
                status.type = SSWS_Answer;
                break;
            }
            case 8: {
                status.type = SSWS_Handup;
                break;
            }
            case 9: {
                status.type = SSWS_Mode;
                break;
            }
            default: {
                return;
                break;
            }
            }
            m_StudySteeringWheel.insert(status.type, status);
            m_Parent->onSteeringStudyChange(m_StudySteeringWheel);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SettingPrivate::onArkCarHandler(ArkCar *car)
{
    if (NULL != car) {
        switch (car->type) {
        case ACT_MCUUpdate: {
            switch (car->status) {
            case ACS_MCUUpdateFileLen: {
                unsigned short int file_length = m_MCUUpdateArray.size() / MCUUPDATETRANSFERBYTE;
                unsigned char data[3] = {0};
                //data[0]             : 0x01(128byte);0x02(256byte)
                if (128 == MCUUPDATETRANSFERBYTE) {
                    data[0] = 0x01;
                } else if (256 == MCUUPDATETRANSFERBYTE) {
                    data[0] = 0x02;
                } else if (512 == MCUUPDATETRANSFERBYTE) {
                    data[0] = 0x03;
                }
                data[1] = (file_length & 0xff00) >> 8;
                data[2] = (file_length & 0x00ff);
                ArkCar car;
                car.type = ACT_MCUUpdate;
                car.status = ACS_MCUUpdateFileLen;
                car.data = data;
                car.length = 3;
                ArkProtocol protocol;
                protocol.type = AT_Car;
                protocol.direction = AD_Setter;
                protocol.data = static_cast<void*>(&car);
                m_Parent->setArkProtocol(ArkProtocolWrapper(protocol));
                break;
            }
            case ACS_MCUUpdatePercent: {
                if (1 != car->length) {
                    qDebug() << "ACS_MCUUpdatePercent length error!!!" << car->length;
                }
                break;
            }
            case ACS_MCUUpdateTransfer: {
                if (3 == car->length) {
                    unsigned short hight = car->data[0];
                    hight = (hight << 8);
                    unsigned short int package_offset = hight + car->data[1];
                    unsigned char data[512] = {0};
                    memcpy((void*)data, (void*)(m_MCUUpdateArray.data() + MCUUPDATETRANSFERBYTE * package_offset), sizeof(char) * MCUUPDATETRANSFERBYTE);
                    ArkCar send_car;
                    send_car.type = ACT_MCUUpdate;
                    send_car.status = ACS_MCUUpdateTransfer;
                    send_car.data = data;
                    send_car.length = MCUUPDATETRANSFERBYTE;
                    ArkProtocol protocol;
                    protocol.type = AT_Car;
                    protocol.direction = AD_Setter;
                    protocol.data = static_cast<void*>(&send_car);
                    m_Parent->setArkProtocol(ArkProtocolWrapper(protocol));
                } else {
                    qDebug() << "ACS_MCUUpdateTransfer length error!!!" << car->length;
                }
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SettingPrivate::onStartCompleteHandler()
{
}
