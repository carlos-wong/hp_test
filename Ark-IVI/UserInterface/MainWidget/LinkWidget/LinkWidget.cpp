#include "LinkWidget.h"
#include "CarlifeLinkWidget/CarlifeLinkWidget.h"
#include "AutoLinkWidget/AutoLinkWidget.h"
#include "CarplayLinkWidget/CarplayLinkWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "TextWidget.h"
#include "BusinessLogic/Setting.h"
#include <QVariant>
#include <QPainter>
#include <QTime>

namespace SourceString {
static const QString Select_Connect_Mode(QObject::tr("Select connect mode!"));
static const QString CarlifeTip(QObject::tr("1:Enable USB debugging options!\n"
                                            "2:Connect to bluetooth devices!\n"
                                            "3:Connect usb cable!\n"
                                            "4:Press Carlife icon!"));
static const QString CarPlayTip(QObject::tr("1:Connect usb cable!\n"
                                            "2:Press Carplay icon!\n"
                                            " "));
static const QString No_Carlife_Device(QObject::tr("No Carlife Device..."));
static const QString Conneting_Carlife_Device(QObject::tr("Conneting Carlife Device..."));
static const QString Connect_Carlife_Device_Fail = QString(QObject::tr("Connect Carlife Device Fail..."));
static const QString Remove_Carlife_Device(QObject::tr("Remove Carlife Device..."));
static const QString No_Auto_Device(QObject::tr("No Auto Device..."));
static const QString Conneting_Auto_Device(QObject::tr("Conneting Auto Device..."));
static const QString Connect_Auto_Device_Fail = QString(QObject::tr("Connect Auto Device Fail..."));
static const QString Remove_Auto_Device(QObject::tr("Remove Auto Device..."));
static const QString No_Carplay_Device(QObject::tr("No Carplay Device..."));
static const QString Conneting_Carplay_Device(QObject::tr("Conneting Carplay Device..."));
static const QString Connect_Carplay_Device_Fail = QString(QObject::tr("Connect Carplay Device Fail..."));
static const QString Remove_Carplay_Device(QObject::tr("Remove Carplay Device..."));
static const QString Unsupport_Switch_Device = QString(QObject::tr("Unsupport Switch Device..."));
}

class LinkWidgetPrivate
{
    Q_DISABLE_COPY(LinkWidgetPrivate)
public:
    explicit LinkWidgetPrivate(LinkWidget *parent);
    ~LinkWidgetPrivate();
    void initializeBasicWidget();
    void initializeMessageBoxWidget();
    void initializeCarlifeWidget();
    void initializeAutoWidget();
    void initializeCarplayWidget();
    void connectAllSlots();
    BmpWidget* m_Background;
    TextWidget* m_TitleText;
    BmpButton* m_CarlifeBtn;
    TextWidget* m_CarlifeBtnText;
    BmpButton* m_AutoBtn;
    TextWidget* m_AutoBtnText;
    BmpButton* m_CarplayBtn;
    TextWidget* m_CarplayBtnText;
    MessageBox* m_MessageBox;
    CarlifeLinkWidget* m_CarlifeLinkWidget;
    AutoLinkWidget* m_AutoLinkWidget;
    CarplayLinkWidget* m_CarplayLinkWidget;
    Link_Type m_Link_Type;
    Link_STATUS m_Link_STATUS;
    QTime m_Time;
private:
    LinkWidget* m_Parent;
};

LinkWidget::LinkWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new LinkWidgetPrivate(this))
{
}

LinkWidget::~LinkWidget()
{
}

void LinkWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void LinkWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                if ((LINK_STARTING != m_Private->m_Link_STATUS)
                        && (LINK_SUCCESS != m_Private->m_Link_STATUS)) {
                    g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Link, WidgetStatus::RequestShow);
                }
            }
        }
        break;
    }
    case Widget::T_Link: {
        if (WidgetStatus::RequestShow == status) {
            //            qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_Link_Type << m_Private->m_Link_STATUS;
            //            g_Widget->setWidgetType(Widget::T_Link, requestType, WidgetStatus::Show);
            qDebug() << m_Private->m_Link_STATUS << m_Private->m_Link_Type;
            if (((LINK_EXITING != m_Private->m_Link_STATUS)
                 && (LINK_EXITED != m_Private->m_Link_STATUS))
                    || (-1 == m_Private->m_Link_Type)) {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__;
                g_Widget->setWidgetType(Widget::T_Link, requestType, WidgetStatus::Show);
            } else {
                if (CARPLAY == m_Private->m_Link_Type) {
                    g_Widget->setWidgetType(Widget::T_Carplay, Widget::T_Link, WidgetStatus::RequestShow);
                } else if (AUTO == m_Private->m_Link_Type) {
                    g_Widget->setWidgetType(Widget::T_Auto, Widget::T_Link, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_Carlife, Widget::T_Link, WidgetStatus::RequestShow);
                }
                qDebug() << __PRETTY_FUNCTION__ << __LINE__ << "background";
            }
        } else if (WidgetStatus::Show == status) {
            m_Private->initializeBasicWidget();
            if (NULL != m_Private->m_MessageBox) {
                if (m_Private->m_MessageBox->isVisible()) {
                    m_Private->m_MessageBox->setVisible(false);
                }
            }
            m_Private->m_Background->setVisible(true);
            m_Private->m_TitleText->setVisible(true);
            if (NULL != m_Private->m_CarlifeBtn) {
                m_Private->m_CarlifeBtn->setVisible(true);
            }
            if (NULL != m_Private->m_CarlifeBtnText) {
                m_Private->m_CarlifeBtnText->setVisible(true);
            }
            if (NULL != m_Private->m_AutoBtn) {
                m_Private->m_AutoBtn->setVisible(true);
            }
            if (NULL != m_Private->m_AutoBtnText) {
                m_Private->m_AutoBtnText->setVisible(true);
            }
            m_Private->m_CarplayBtn->setVisible(true);
            m_Private->m_CarplayBtnText->setVisible(true);
            m_Private->m_Time.restart();
            setVisible(true);
//            static bool flag = true;
//            if (flag) {
//                flag = false;
//                startTimer(100);
//            }
        }
        break;
    }
    case Widget::T_Carlife:
    case Widget::T_Auto:
    case Widget::T_Carplay: {
        if (WidgetStatus::Show == status) {
            if (NULL != m_Private->m_MessageBox) {
                m_Private->m_MessageBox->setVisible(false);
            }
            m_Private->m_Background->setVisible(false);
            m_Private->m_TitleText->setVisible(false);
            if (NULL != m_Private->m_CarlifeBtn) {
                m_Private->m_CarlifeBtn->setVisible(false);
            }
            if (NULL != m_Private->m_CarlifeBtnText) {
                m_Private->m_CarlifeBtnText->setVisible(false);
            }
            if (NULL != m_Private->m_AutoBtn) {
                m_Private->m_AutoBtn->setVisible(false);
            }
            if (NULL != m_Private->m_AutoBtnText) {
                m_Private->m_AutoBtnText->setVisible(false);
            }
            m_Private->m_CarplayBtn->setVisible(false);
            m_Private->m_CarplayBtnText->setVisible(false);
            setVisible(true);
        }
        break;
    }
    default : {
        if (WidgetStatus::Show == status) {
            if (NULL != m_Private->m_MessageBox) {
                m_Private->m_MessageBox->setVisible(false);
            }
            setVisible(false);
        }
        break;
    }
    }
}

void LinkWidget::onLinkStatusChange(const Link_Type type, const Link_STATUS status)
{
    switch (type) {
    case CARPLAY: {
        m_Private->m_Time.restart();
        if (LINK_DISCONNECTED == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(true);
            m_Private->m_MessageBox->setText(SourceString::No_Carplay_Device);
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_STARTING == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(false);
            m_Private->m_MessageBox->setText(SourceString::Conneting_Carplay_Device);
            m_Private->initializeCarplayWidget();
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_FAIL == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carplay, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Connect_Carplay_Device_Fail);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_REMOVED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carplay, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Remove_Carplay_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_SUCCESS == status) {
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
            m_Private->m_Link_Type = type;
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
        } else if (LINK_EXITING == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0,  Widget::T_Carplay, WidgetStatus::RequestShow);
            }
        } else if (LINK_EXITED == status) {
            m_Private->m_Link_STATUS = status;
        }  else if (LINK_UNSUPPORTED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carplay, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Unsupport_Switch_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        }/* else if (LINK_CALL_PHONE == status) {
            g_Widget->acquireWidgetType(Widget::T_Link);
        } else if (LINK_CALL_PHONE_EXITED == status) {
            g_Widget->releaseWidgetType(Widget::T_Link);
        }*/
        break;
    }
    case AUTO: {
        if (LINK_DISCONNECTED == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(true);
            m_Private->m_MessageBox->setText(SourceString::No_Auto_Device);
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_STARTING == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(false);
            m_Private->m_MessageBox->setText(SourceString::Conneting_Auto_Device);
            m_Private->initializeCarlifeWidget();
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_FAIL == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Auto, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Connect_Auto_Device_Fail);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_REMOVED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Auto, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Remove_Auto_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_SUCCESS == status) {
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
            m_Private->m_Link_Type = type;
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
        } else if (LINK_EXITING == status) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Auto, WidgetStatus::RequestShow);
            }
        } else if (LINK_EXITED == status) {
            m_Private->m_Link_STATUS = status;
        } else if (LINK_UNSUPPORTED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Auto, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Unsupport_Switch_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        }/* else if (LINK_CALL_PHONE == status) {
            g_Widget->acquireWidgetType(Widget::T_Link);
        } else if (LINK_CALL_PHONE_EXITED == status) {
            g_Widget->releaseWidgetType(Widget::T_Link);
        }*/
        break;
    }
    case CARLIFE: {
        if (LINK_DISCONNECTED == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(true);
            m_Private->m_MessageBox->setText(SourceString::No_Carlife_Device);
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_STARTING == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setAutoHide(false);
            m_Private->m_MessageBox->setText(SourceString::Conneting_Carlife_Device);
            m_Private->initializeCarlifeWidget();
            m_Private->m_MessageBox->raise();
            m_Private->m_MessageBox->setVisible(true);
        } else if (LINK_FAIL == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carlife, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Connect_Carlife_Device_Fail);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_REMOVED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carlife, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Remove_Carlife_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        } else if (LINK_SUCCESS == status) {
            connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
            m_Private->m_Link_Type = type;
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
        } else if (LINK_EXITING == status) {
            m_Private->m_Link_STATUS = status;
            m_Private->initializeMessageBoxWidget();
            m_Private->m_MessageBox->setVisible(false);
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Carlife, WidgetStatus::RequestShow);
            }
        } else if (LINK_EXITED == status) {
            m_Private->m_Link_STATUS = status;
        } else if (LINK_UNSUPPORTED == status) {
            m_Private->m_Link_STATUS = status;
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_Link, Widget::T_Carlife, WidgetStatus::RequestShow);
                m_Private->initializeMessageBoxWidget();
                m_Private->m_MessageBox->setAutoHide(true);
                m_Private->m_MessageBox->setText(SourceString::Unsupport_Switch_Device);
                m_Private->m_MessageBox->raise();
                m_Private->m_MessageBox->setVisible(true);
            }
        }/* else if (LINK_CALL_PHONE == status) {
            g_Widget->acquireWidgetType(Widget::T_Link);
        } else if (LINK_CALL_PHONE_EXITED == status) {
            g_Widget->releaseWidgetType(Widget::T_Link);
        }*/
        break;
    }
    default: {
        break;
    }
    }
}

void LinkWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Key: {
        struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
        if ((NULL != key)
                && (!protocol.handler)
                && (isVisible())) {
            if ((AKS_Release == key->status)
                    || (AKS_Active == key->status)) {
                if ((LINK_SUCCESS == m_Private->m_Link_STATUS)
                        || (LINK_EXITING == m_Private->m_Link_STATUS)) {
                    switch (key->type) {
                    case AKT_CommonPrevious: {
                        g_Link->requestKeyValue(m_Private->m_Link_Type, MEDIA_PREVIOUS_KEY);
                        break;
                    }
                    case AKT_CommonNext: {
                        g_Link->requestKeyValue(m_Private->m_Link_Type, MEDIA_NEXT_KEY);
                        break;
                    }
                    default: {
                        break;
                    }
                    }
                }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void LinkWidget::onToolButtonRelease()
{
    static int elapsed = 0;
    elapsed += m_Private->m_Time.restart();
    if (elapsed > 500) {
        elapsed = 0;
        BmpButton* ptr = static_cast<BmpButton*>(sender());
        if (ptr == m_Private->m_CarlifeBtn) {
            g_Link->startCarlife();
            m_Private->initializeCarlifeWidget();
            g_Widget->setWidgetType(Widget::T_Carlife, Widget::T_Link, WidgetStatus::RequestShow);
        } else if (ptr == m_Private->m_AutoBtn) {
            g_Link->startAuto();
            m_Private->initializeAutoWidget();
            g_Widget->setWidgetType(Widget::T_Auto, Widget::T_Link, WidgetStatus::RequestShow);
        } else if (ptr == m_Private->m_CarplayBtn) {
            g_Link->startCarplay();
            m_Private->initializeCarplayWidget();
            g_Widget->setWidgetType(Widget::T_Carplay, Widget::T_Link, WidgetStatus::RequestShow);
        }
    }
}

LinkWidgetPrivate::LinkWidgetPrivate(LinkWidget *parent)
    : m_Parent(parent)
{
    m_Background = NULL;
    m_TitleText = NULL;
    m_CarlifeBtn = NULL;
    m_CarlifeBtnText  = NULL;
    m_AutoBtn = NULL;
    m_AutoBtnText  = NULL;
    m_CarplayBtn = NULL;
    m_CarplayBtnText = NULL;
    m_MessageBox = NULL;
    m_CarlifeLinkWidget = NULL;
    m_AutoLinkWidget = NULL;
    m_CarplayLinkWidget = NULL;
    m_Link_Type = CARPLAY;
    m_Link_STATUS = LINK_UNSUPPORTED;
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Link, m_Parent, ARKRECEIVER(onLinkStatusChange(const int, const int)));
}

LinkWidgetPrivate::~LinkWidgetPrivate()
{
}

void LinkWidgetPrivate::initializeBasicWidget()
{
    if (NULL == m_Background) {
        m_Background = new BmpWidget(m_Parent);
        m_Background->setBackgroundBmpPath(QString(":/Images/LinkWidgetBackground.png"));
        g_Widget->geometryFit(44, 72, 712, 372, m_Background);
        m_Background->setVisible(true);
    }
    if (NULL == m_TitleText) {
        m_TitleText = new TextWidget(m_Parent);
        m_TitleText->setText(SourceString::Select_Connect_Mode);
        g_Widget->geometryFit(50, 72, 700, 60, m_TitleText);
        m_TitleText->setVisible(true);
    }
    if (CLT_CarlifeCarplay == SettingPersistent::getCarLinkType()) {
        if (NULL == m_CarlifeBtn) {
            m_CarlifeBtn = new BmpButton(m_Parent);
            m_CarlifeBtn->setNormalBmpPath(QString(":/Images/LinkWidgetCarlifeBtnNormal.png"));
            m_CarlifeBtn->setPressBmpPath(QString(":/Images/LinkWidgetCarlifeBtnNormal.png"));
            g_Widget->geometryFit(150, 230, 96, 96, m_CarlifeBtn);
            m_CarlifeBtn->setVisible(true);
        }
        if (NULL == m_CarlifeBtnText) {
            m_CarlifeBtnText = new TextWidget(m_Parent);
            m_CarlifeBtnText->setLanguageType(TextWidget::T_NoTranslate);
            m_CarlifeBtnText->setText(QString("Carlife"));
            g_Widget->geometryFit(123, 326, 150, 29, m_CarlifeBtnText);
            m_CarlifeBtnText->setVisible(true);
        }
    } else {
        if (NULL == m_AutoBtn) {
            m_AutoBtn = new BmpButton(m_Parent);
            m_AutoBtn->setNormalBmpPath(QString(":/Images/LinkWidgetAutoBtnNormal.png"));
            m_AutoBtn->setPressBmpPath(QString(":/Images/LinkWidgetAutoBtnNormal.png"));
            g_Widget->geometryFit(150, 230, 96, 96, m_AutoBtn);
            m_AutoBtn->setVisible(true);
        }
        if (NULL == m_AutoBtnText) {
            m_AutoBtnText = new TextWidget(m_Parent);
            m_AutoBtnText->setLanguageType(TextWidget::T_NoTranslate);
            m_AutoBtnText->setText(QString("Auto"));
            g_Widget->geometryFit(123, 326, 150, 29, m_AutoBtnText);
            m_AutoBtnText->setVisible(true);
        }
    }
    if (NULL == m_CarplayBtn) {
        m_CarplayBtn = new BmpButton(m_Parent);
        m_CarplayBtn->setNormalBmpPath(QString(":/Images/LinkWidgetCarplayBtnNormal.png"));
        m_CarplayBtn->setPressBmpPath(QString(":/Images/LinkWidgetCarplayBtnNormal.png"));
        g_Widget->geometryFit(554, 230, 96, 96, m_CarplayBtn);
        m_CarplayBtn->setVisible(true);
    }
    if (NULL == m_CarplayBtnText) {
        m_CarplayBtnText = new TextWidget(m_Parent);
        m_CarplayBtnText->setLanguageType(TextWidget::T_NoTranslate);
        m_CarplayBtnText->setText(QString("Carplay"));
        g_Widget->geometryFit(527, 326, 150, 29, m_CarplayBtnText);
        m_CarplayBtnText->setVisible(true);
    }
    connectAllSlots();
}

void LinkWidgetPrivate::initializeMessageBoxWidget()
{
    if (NULL == m_MessageBox) {
        m_MessageBox = new MessageBox(m_Parent);
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        g_Widget->geometryFit(0, 0, 800, 480, m_MessageBox);
    }
}

void LinkWidgetPrivate::initializeCarlifeWidget()
{
    if (NULL == m_CarlifeLinkWidget) {
        m_CarlifeLinkWidget = new CarlifeLinkWidget(m_Parent);
    }
}

void LinkWidgetPrivate::initializeAutoWidget()
{
    if (NULL == m_AutoLinkWidget) {
        m_AutoLinkWidget = new AutoLinkWidget(m_Parent);
    }
}

void LinkWidgetPrivate::initializeCarplayWidget()
{
    if (NULL == m_CarplayLinkWidget) {
        m_CarplayLinkWidget = new CarplayLinkWidget(m_Parent);
    }
}

void LinkWidgetPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    if (NULL != m_CarlifeBtn) {
        QObject::connect(m_CarlifeBtn, ARKSENDER(released()),
                         m_Parent,     ARKRECEIVER(onToolButtonRelease()),
                         type);
    }
    if (NULL != m_AutoBtn) {
        QObject::connect(m_AutoBtn, ARKSENDER(released()),
                         m_Parent,  ARKRECEIVER(onToolButtonRelease()),
                         type);
    }
    if (NULL != m_CarplayBtn) {
        QObject::connect(m_CarplayBtn, ARKSENDER(released()),
                         m_Parent,     ARKRECEIVER(onToolButtonRelease()),
                         type);
    }
}
