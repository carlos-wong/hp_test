#include "SteeringWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include <QDebug>

namespace SourceString {
static const QString Reset(QObject::tr("Reset"));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Start_Studying_Long_Press_Steering_Key_Then_Press_UI_Key(QObject::tr("Start Studying, Long Press Steering Key, Then Press UI Key!"));
static const QString Finish_Study(QObject::tr("Finish Study!"));
}

class SteeringWidgetPrivate
{
public:
    explicit SteeringWidgetPrivate(SteeringWidget* parent);
    ~SteeringWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(SteeringWidget)
    SteeringWidget* const q_ptr;
    TextWidget* m_Title;
    BmpButton* m_NextBtn;
    BmpButton* m_PreviousBtn;
    BmpButton* m_PowerBtn;
    BmpButton* m_MuteBtn;
    BmpButton* m_VolumeAddBtn;
    BmpButton* m_VolumeSubBtn;
    BmpButton* m_AnswerBtn;
    BmpButton* m_ModeBtn;
    TextWidget* m_ModeText;
    BmpButton* m_RejectBtn;
    BmpButton* m_ResetBtn;
    TextWidget* m_ResetText;
    BmpButton* m_ConfirmBtn;
    TextWidget* m_ConfirmText;
};

SteeringWidget::SteeringWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SteeringWidgetPrivate(this))
{
    onSteeringStudyChange(g_Setting->getSteeringStudy());
}

SteeringWidget::~SteeringWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SteeringWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void SteeringWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(SteeringWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_Steering, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_Steering: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_Steering, Widget::T_ViewPaper1, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
            d->m_Title->setText(SourceString::Start_Studying_Long_Press_Steering_Key_Then_Press_UI_Key);
            g_Setting->steeringStudy(SSWT_StartSample, SSWS_Undefine);
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                g_Setting->steeringStudy(SSWT_ExitSample, SSWS_Undefine);
                setVisible(false);
            }
        }
        break;
    }
    }
}

void SteeringWidget::onSteeringStudyChange(const QMap<StudySteeringWheelStatus, struct StudySteeringWheel>& map)
{
    qDebug() << __PRETTY_FUNCTION__ << map.size();
    Q_D(SteeringWidget);
    if (SSWS_Ok == map.value(SSWS_Previous).status) {
        d->m_PreviousBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_PreviousBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_PreviousBtn->setDisabled(SSWS_Ok == map.value(SSWS_Previous).status);
    if (SSWS_Ok == map.value(SSWS_Next).status) {
        d->m_NextBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_NextBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_NextBtn->setDisabled(SSWS_Ok == map.value(SSWS_Next).status);
    if (SSWS_Ok == map.value(SSWS_Power).status) {
        d->m_PowerBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_PowerBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_PowerBtn->setDisabled(SSWS_Ok == map.value(SSWS_Power).status);
    if (SSWS_Ok == map.value(SSWS_Mute).status) {
        d->m_MuteBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_MuteBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_MuteBtn->setDisabled(SSWS_Ok == map.value(SSWS_Mute).status);
    if (SSWS_Ok == map.value(SSWS_VolumeAdd).status) {
        d->m_VolumeAddBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_VolumeAddBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_VolumeAddBtn->setDisabled(SSWS_Ok == map.value(SSWS_VolumeAdd).status);
    if (SSWS_Ok == map.value(SSWS_VolumeSub).status) {
        d->m_VolumeSubBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_VolumeSubBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_VolumeSubBtn->setDisabled(SSWS_Ok == map.value(SSWS_VolumeSub).status);
    if (SSWS_Ok == map.value(SSWS_Answer).status) {
        d->m_AnswerBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_AnswerBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_AnswerBtn->setDisabled(SSWS_Ok == map.value(SSWS_Answer).status);
    if (SSWS_Ok == map.value(SSWS_Mode).status) {
        d->m_ModeBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_ModeBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_ModeBtn->setDisabled(SSWS_Ok == map.value(SSWS_Mode).status);
    if (SSWS_Ok == map.value(SSWS_Handup).status) {
        d->m_RejectBtn->setStatus(BmpButton::B_Check);
    } else {
        d->m_RejectBtn->setStatus(BmpButton::B_Normal);
    }
    d->m_RejectBtn->setDisabled(SSWS_Ok == map.value(SSWS_Handup).status);
}

void SteeringWidget::onToolButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(SteeringWidget);
    if (ptr == d->m_PreviousBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Previous);
    } else if (ptr == d->m_NextBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Next);
    } else if (ptr == d->m_PowerBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Power);
    } else if (ptr == d->m_MuteBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Mute);
    } else if (ptr == d->m_VolumeAddBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_VolumeAdd);
    } else if (ptr == d->m_VolumeSubBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_VolumeSub);
    } else if (ptr == d->m_AnswerBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Answer);
    } else if (ptr == d->m_ModeBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Mode);
    } else if (ptr == d->m_RejectBtn) {
        g_Setting->steeringStudy(SSWT_StudyCode, SSWS_Handup);
    } else if (ptr == d->m_ResetBtn) {
        g_Setting->steeringStudy(SSWT_ResetSample, SSWS_Undefine);
        d->m_Title->setText(SourceString::Start_Studying_Long_Press_Steering_Key_Then_Press_UI_Key);
        g_Setting->steeringStudy(SSWT_StartSample, SSWS_Undefine);
    } else if (ptr == d->m_ConfirmBtn) {
        g_Setting->steeringStudy(SSWT_SaveSample, SSWS_Undefine);
        d->m_Title->setText(SourceString::Finish_Study);
    }
}

SteeringWidgetPrivate::SteeringWidgetPrivate(SteeringWidget *parent)
    : q_ptr(parent)
{
    m_Title = NULL;
    m_NextBtn = NULL;
    m_PreviousBtn = NULL;
    m_PowerBtn = NULL;
    m_MuteBtn = NULL;
    m_VolumeAddBtn = NULL;
    m_VolumeSubBtn = NULL;
    m_AnswerBtn = NULL;
    m_ModeBtn = NULL;
    m_ModeText = NULL;
    m_RejectBtn = NULL;
    m_ResetBtn = NULL;
    m_ResetText = NULL;
    m_ConfirmBtn = NULL;
    m_ConfirmText = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SteeringWidgetPrivate::~SteeringWidgetPrivate()
{
}

void SteeringWidgetPrivate::initializeParent()
{
    Q_Q(SteeringWidget);
    q->setVisible(false);
}

void SteeringWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SteeringWidget);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);

    m_Title = new TextWidget(q);
    m_Title->setText(SourceString::Start_Studying_Long_Press_Steering_Key_Then_Press_UI_Key);
    g_Widget->geometryFit(0, 52, 800, 28, m_Title);
    m_Title->setVisible(true);

    m_PreviousBtn = new BmpButton(q);
    m_PreviousBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetPreviousNormal.png"));
    m_PreviousBtn->setPressBmpPath(QString(":/Images/SteeringWidgetPreviousPress.png"));
    m_PreviousBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetPreviousPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 0, 80, 210, 70, m_PreviousBtn);
    m_PreviousBtn->setVisible(true);
    QObject::connect(m_PreviousBtn, ARKSENDER(released()),
                     q,             ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_NextBtn = new BmpButton(q);
    m_NextBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetNextNormal.png"));
    m_NextBtn->setPressBmpPath(QString(":/Images/SteeringWidgetNextPress.png"));
    m_NextBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetNextPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 1, 80, 210, 70, m_NextBtn);
    m_NextBtn->setVisible(true);
    QObject::connect(m_NextBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_PowerBtn = new BmpButton(q);
    m_PowerBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetPowerNormal.png"));
    m_PowerBtn->setPressBmpPath(QString(":/Images/SteeringWidgetPowerPress.png"));
    m_PowerBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetPowerPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 2, 80, 210, 70, m_PowerBtn);
    m_PowerBtn->setVisible(true);
    QObject::connect(m_PowerBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_MuteBtn = new BmpButton(q);
    m_MuteBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetMuteNormal.png"));
    m_MuteBtn->setPressBmpPath(QString(":/Images/SteeringWidgetMutePress.png"));
    m_MuteBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetMutePress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 0, 177, 210, 70, m_MuteBtn);
    m_MuteBtn->setVisible(true);
    QObject::connect(m_MuteBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_VolumeAddBtn = new BmpButton(q);
    m_VolumeAddBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetVolumeAddNormal.png"));
    m_VolumeAddBtn->setPressBmpPath(QString(":/Images/SteeringWidgetVolumeAddPress.png"));
    m_VolumeAddBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetVolumeAddPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 1, 177, 210, 70, m_VolumeAddBtn);
    m_VolumeAddBtn->setVisible(true);
    QObject::connect(m_VolumeAddBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_VolumeSubBtn = new BmpButton(q);
    m_VolumeSubBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetVolumeSubNormal.png"));
    m_VolumeSubBtn->setPressBmpPath(QString(":/Images/SteeringWidgetVolumeSubPress.png"));
    m_VolumeSubBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetVolumeSubPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 2, 177, 210, 70, m_VolumeSubBtn);
    m_VolumeSubBtn->setVisible(true);
    QObject::connect(m_VolumeSubBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_AnswerBtn = new BmpButton(q);
    m_AnswerBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetAnswerNormal.png"));
    m_AnswerBtn->setPressBmpPath(QString(":/Images/SteeringWidgetAnswerPress.png"));
    m_AnswerBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetAnswerPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 0, 273, 210, 70, m_AnswerBtn);
    m_AnswerBtn->setVisible(true);
    QObject::connect(m_AnswerBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_ModeBtn = new BmpButton(q);
    m_ModeBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetBlankNormal.png"));
    m_ModeBtn->setPressBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    m_ModeBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 1, 273, 210, 70, m_ModeBtn);
    m_ModeBtn->setVisible(true);
    QObject::connect(m_ModeBtn, ARKSENDER(released()),
                     q,         ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_ModeText = new TextWidget(m_ModeBtn);
    m_ModeText->setLanguageType(TextWidget::T_NoTranslate);
    g_Widget->geometryFit(0, 0, 210, 70, m_ModeText);
    m_ModeText->setText(QString("Mode"));
    m_ModeText->setVisible(true);

    m_RejectBtn = new BmpButton(q);
    m_RejectBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetRejectNormal.png"));
    m_RejectBtn->setPressBmpPath(QString(":/Images/SteeringWidgetRejectPress.png"));
    m_RejectBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetRejectPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 2, 273, 210, 70, m_RejectBtn);
    m_RejectBtn->setVisible(true);
    QObject::connect(m_RejectBtn, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()),
                     type);

    m_ResetBtn = new BmpButton(q);
    m_ResetBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetBlankNormal.png"));
    m_ResetBtn->setPressBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    m_ResetBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 0, 369, 210, 70, m_ResetBtn);
    m_ResetBtn->setVisible(true);
    QObject::connect(m_ResetBtn, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_ResetText = new TextWidget(m_ResetBtn);
    g_Widget->geometryFit(0, 0, 210, 70, m_ResetText);
    m_ResetText->setText(SourceString::Reset);
    m_ResetText->setVisible(true);

    m_ConfirmBtn = new BmpButton(q);
    m_ConfirmBtn->setNormalBmpPath(QString(":/Images/SteeringWidgetBlankNormal.png"));
    m_ConfirmBtn->setPressBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    m_ConfirmBtn->setCheckBmpPath(QString(":/Images/SteeringWidgetBlankPress.png"));
    g_Widget->geometryFit(42 + (42 + 210) * 2, 369, 210, 70, m_ConfirmBtn);
    m_ConfirmBtn->setVisible(true);
    QObject::connect(m_ConfirmBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()),
                     type);
    m_ConfirmText = new TextWidget(m_ConfirmBtn);
    g_Widget->geometryFit(0, 0, 210, 70, m_ConfirmText);
    m_ConfirmText->setText(SourceString::Confirm);
    m_ConfirmText->setVisible(true);
}

void SteeringWidgetPrivate::connectAllSlots()
{
    Q_Q(SteeringWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onSteeringStudyChange(const QMap<StudySteeringWheelStatus, struct StudySteeringWheel>&)));
}

