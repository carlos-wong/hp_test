#include "SettingGeneralTimeWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include "ArkApplication.h"
#include "TextWidget.h"
#include "Utility.h"
#include "BusinessLogic/Setting.h"
#include <QDateTime>
#include "SettingGeneralTimeColumeWidget.h"
#include <QPainter>
#include <QDate>
#include <QTime>
#include <QDebug>

namespace SourceString {
static const QString Modify(QObject::tr("Modify"));
static const QString Conrifm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
static const QString Year(QObject::tr("Year"));
static const QString Month(QObject::tr("Month"));
static const QString Day(QObject::tr("Day"));
static const QString Hour(QObject::tr("Hour"));
static const QString Minute(QObject::tr("Minute"));
//static const QString Second(QObject::tr("Second"));
}

class SettingGeneralTimeWidgetPrivate
{
public:
    explicit SettingGeneralTimeWidgetPrivate(SettingGeneralTimeWidget* parent);
    ~SettingGeneralTimeWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void updateCurrentDateTime(const QDateTime &dateTime);
private:
    Q_DECLARE_PUBLIC(SettingGeneralTimeWidget)
    SettingGeneralTimeWidget* const q_ptr;
    SettingGeneralTimeColumeWidget* m_Year;
    SettingGeneralTimeColumeWidget* m_Month;
    SettingGeneralTimeColumeWidget* m_Day;
    SettingGeneralTimeColumeWidget* m_Hour;
    SettingGeneralTimeColumeWidget* m_Minute;
    BmpButton* m_HourClockToggle;
    BmpButton* m_Modify;
    TextWidget* m_ModifyText;
    BmpButton* m_Confirm;
    TextWidget* m_ConfirmText;
    BmpButton* m_Cancel;
    TextWidget* m_CancelText;
};

SettingGeneralTimeWidget::SettingGeneralTimeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingGeneralTimeWidgetPrivate(this))
{
}

SettingGeneralTimeWidget::~SettingGeneralTimeWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}
void SettingGeneralTimeWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingGeneralTimeWidget::onHourClockChange(const int type)
{
    Q_D(SettingGeneralTimeWidget);
    if (HCT_12HourClock == type) {
        d->m_HourClockToggle->setText(QString("12H"));
    } else {
        d->m_HourClockToggle->setText(QString("24H"));
    }
}

void SettingGeneralTimeWidget::onCurrentDateTime(const QDateTime &dateTime)
{
    Q_D(SettingGeneralTimeWidget);
    d->updateCurrentDateTime(dateTime);
}

void SettingGeneralTimeWidget::onIndexChange(const int index)
{
    Q_D(SettingGeneralTimeWidget);
    int year = 1970 + d->m_Year->currentIndex();
    int month = d->m_Month->currentIndex() + 1;
    int day = d->m_Day->currentIndex() + 1;
    int j;
    for (j = 31; j > 28; --j) {
        QDate date(year, month, j);
        if (date.isValid()) {
            break;
        }
    }
    QList<QString> list;
    list.clear();
    for (int i = 0; i < j; ++i) {
        list.append(QString("%1").arg(i + 1, 2, 10, QChar('0')));
    }
    d->m_Day->replaceListView(list);
    d->m_Day->scrollToIndex(day < j? day - 1: j - 1);
}

unsigned char DecimalToBCD(const unsigned short int number)
{
    return (((number / 10) << 4) + ((number % 10) & 0x0f));
}

void SettingGeneralTimeWidget::onBmpButtonRelease()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(SettingGeneralTimeWidget);
    if (ptr == d->m_HourClockToggle) {
        connectSignalAndSlotByNamesake(g_Setting, this, ARKRECEIVER(onHourClockChange(const int)));
        g_Setting->toggleHourClock();
        return;
    } else if (ptr == d->m_Confirm) {
        int year = 1970 + d->m_Year->currentIndex();
        int month = d->m_Month->currentIndex();
        int day = d->m_Day->currentIndex() + 1;
        int hour = d->m_Hour->currentIndex();
        int minute = d->m_Minute->currentIndex();
        int second =0;
        setDateTime(year,
                    month,
                    day,
                    hour,
                    minute,
                    second);
        ArkCar car;
        car.type = ACT_Time;
        unsigned char tmp[7] = {0};
        tmp[0] = DecimalToBCD(year / 100);
        tmp[1] = DecimalToBCD(year % 100);
        tmp[2] = DecimalToBCD(month);
        tmp[3] = DecimalToBCD(day);
        tmp[4] = DecimalToBCD(hour);
        tmp[5] = DecimalToBCD(minute);
        tmp[6] = DecimalToBCD(second);
        car.length = 7;
        car.data = tmp;
        ArkProtocol protocol;
        protocol.type = AT_Car;
        protocol.direction = AD_Setter;
        protocol.data = static_cast<void*>(&car);
        g_Setting->setArkProtocol(ArkProtocolWrapper(protocol));
        d->m_Modify->setVisible(true);
        d->m_Confirm->setVisible(false);
        d->m_Cancel->setVisible(false);
        ArkApp->onTimeout();
        d->m_HourClockToggle->setVisible(true);
    } else if (ptr == d->m_Cancel) {
        d->m_Modify->setVisible(true);
        d->m_Confirm->setVisible(false);
        d->m_Cancel->setVisible(false);
        d->m_HourClockToggle->setVisible(true);
    } else if (ptr == d->m_Modify) {
        d->m_Modify->setVisible(false);
        d->m_HourClockToggle->setVisible(false);
        d->m_Confirm->setVisible(true);
        d->m_Cancel->setVisible(true);
    }
    d->m_Year->setEnabled(d->m_Modify->isHidden());
    d->m_Month->setEnabled(d->m_Modify->isHidden());
    d->m_Day->setEnabled(d->m_Modify->isHidden());
    d->m_Hour->setEnabled(d->m_Modify->isHidden());
    d->m_Minute->setEnabled(d->m_Modify->isHidden());
}

SettingGeneralTimeWidgetPrivate::SettingGeneralTimeWidgetPrivate(SettingGeneralTimeWidget *parent)
    : q_ptr(parent)
{
    m_Year = NULL;
    m_Month = NULL;
    m_Day = NULL;
    m_Hour = NULL;
    m_Minute = NULL;
    m_HourClockToggle = NULL;
    m_Modify = NULL;
    m_ModifyText = NULL;
    m_Confirm = NULL;
    m_ConfirmText = NULL;
    m_Cancel = NULL;
    m_CancelText = NULL;
    initializeParent();
    initializeBasicWidget();
}

SettingGeneralTimeWidgetPrivate::~SettingGeneralTimeWidgetPrivate()
{
}

void SettingGeneralTimeWidgetPrivate::initializeParent()
{
    Q_Q(SettingGeneralTimeWidget);
    q->setVisible(false);
}

void SettingGeneralTimeWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingGeneralTimeWidget);
    m_HourClockToggle = new BmpButton(q);
    m_HourClockToggle->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_HourClockToggle->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    g_Widget->geometryFit(36, 277, 88, 46, m_HourClockToggle);
    m_HourClockToggle->setLanguageType(BmpButton::T_NoTranslate);
    if (HCT_12HourClock == SettingPersistent::getHourClockType()) {
        m_HourClockToggle->setText(QString("12H"));
    } else {
        m_HourClockToggle->setText(QString("24H"));
    }
    m_HourClockToggle->setVisible(true);
    m_Modify = new BmpButton(q);
    m_Modify->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_Modify->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    g_Widget->geometryFit(476, 277, 88, 46, m_Modify);
    m_ModifyText = new TextWidget(m_Modify);
    g_Widget->geometryFit(0,0, 88, 40, m_ModifyText);
    m_ModifyText->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_ModifyText->setText(SourceString::Modify);
    m_Modify->setVisible(true);
    m_Confirm = new BmpButton(q);
    m_Confirm->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_Confirm->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    g_Widget->geometryFit(368, 277, 88, 46, m_Confirm);
    m_Confirm->setVisible(false);
    m_ConfirmText = new TextWidget(m_Confirm);
    g_Widget->geometryFit(0,0, 88, 40, m_ConfirmText);
    m_ConfirmText->setText(SourceString::Conrifm);
    m_ConfirmText->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_ConfirmText->setVisible(true);

    m_Cancel = new BmpButton(q);
    m_Cancel->setNormalBmpPath(QString(":/Images/SettingWidgetBtnNormal.png"));
    m_Cancel->setPressBmpPath(QString(":/Images/SettingWidgetBtnPress.png"));
    g_Widget->geometryFit(476, 277, 88, 46, m_Cancel);
    m_Cancel->setVisible(false);
    m_CancelText = new TextWidget(m_Cancel);
    g_Widget->geometryFit(0,0, 88, 40, m_CancelText);
    m_CancelText->setText(SourceString::Cancel);
    m_CancelText->setFontPointSize(11 * g_Widget->widthScalabilityFactor());
    m_CancelText->setVisible(true);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_HourClockToggle, ARKSENDER(released()),
                     q,                 ARKRECEIVER(onBmpButtonRelease()),
                     type);
    QObject::connect(m_Modify, ARKSENDER(released()),
                     q,        ARKRECEIVER(onBmpButtonRelease()),
                     type);
    QObject::connect(m_Confirm, ARKSENDER(released()),
                     q,         ARKRECEIVER(onBmpButtonRelease()),
                     type);
    QObject::connect(m_Cancel, ARKSENDER(released()),
                     q,        ARKRECEIVER(onBmpButtonRelease()),
                     type);
    QList<QString> list;

    m_Year = new SettingGeneralTimeColumeWidget(q);
    m_Year->setText(SourceString::Year);
    g_Widget->geometryFit(31, 20, 82, 219, m_Year);
    list.clear();
    for (int i = 0; i < 100; ++i) {
        list.append(QString("%1").arg(i + 1970, 4, 10, QChar('0')));
    }
    m_Year->replaceListView(list);
    m_Year->setVisible(true);
    QObject::connect(m_Year, ARKSENDER(indexChange(const int)),
                     q,      ARKRECEIVER(onIndexChange(const int)),
                     type);

    m_Month = new SettingGeneralTimeColumeWidget(q);
    m_Month->setText(SourceString::Month);
    g_Widget->geometryFit(141, 20, 82, 219, m_Month);
    list.clear();
    for (int i = 0; i < 12; ++i) {
        list.append(QString("%1").arg(i + 1, 2, 10, QChar('0')));
    }
    m_Month->replaceListView(list);
    QObject::connect(m_Month, ARKSENDER(indexChange(const int)),
                     q,       ARKRECEIVER(onIndexChange(const int)),
                     type);
    m_Month->setVisible(true);

    QDateTime dateTime = QDateTime::currentDateTime();
    m_Day = new SettingGeneralTimeColumeWidget(q);
    m_Day->setText(SourceString::Day);
    g_Widget->geometryFit(257, 20, 82, 219, m_Day);
    int j;
    for (j = 31; j > 28; --j) {
        QDate refDate(dateTime.date().year(), dateTime.date().month(), j);
        if (!refDate.isValid()) {
            break;
        }
    }
    list.clear();
    for (int i = 0; i < j; ++i) {
        list.append(QString("%1").arg(i + 1, 2, 10, QChar('0')));
    }
    m_Day->replaceListView(list);
    m_Day->setVisible(true);

    m_Hour = new SettingGeneralTimeColumeWidget(q);
    m_Hour->setText(SourceString::Hour);
    g_Widget->geometryFit(370, 20, 82, 219, m_Hour);
    list.clear();
    for (int i = 0; i < 24; ++i) {
        list.append(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    m_Hour->replaceListView(list);
    m_Hour->setVisible(true);

    m_Minute = new SettingGeneralTimeColumeWidget(q);
    m_Minute->setText(SourceString::Minute);
    g_Widget->geometryFit(483, 20, 82, 219, m_Minute);
    list.clear();
    for (int i = 0; i < 60; ++i) {
        list.append(QString("%1").arg(i, 2, 10, QChar('0')));
    }
    m_Minute->replaceListView(list);
    m_Minute->setVisible(true);

    QObject::connect(ArkApp, ARKSENDER(currentDateTime(const QDateTime&)),
                     q,      ARKRECEIVER(onCurrentDateTime(const QDateTime&)),
                     type);
    m_Year->setEnabled(false);
    m_Month->setEnabled(false);
    m_Day->setEnabled(false);
    m_Hour->setEnabled(false);
    m_Minute->setEnabled(false);
    updateCurrentDateTime(dateTime);
}

void SettingGeneralTimeWidgetPrivate::updateCurrentDateTime(const QDateTime &dateTime)
{
    if (!m_Year->isEnabled()) {
        m_Year->scrollToIndex(dateTime.toString(QString("yyyy")).toInt() - 1970);
    }
    if (!m_Month->isEnabled()) {
        m_Month->scrollToIndex(dateTime.toString(QString("MM")).toInt() - 1);
    }
    if (!m_Day->isEnabled()) {
        m_Day->scrollToIndex(dateTime.toString(QString("dd")).toInt() - 1);
    }
    if (!m_Hour->isEnabled()) {
        m_Hour->scrollToIndex(dateTime.toString(QString("hh")).toInt());
    }
    if (!m_Minute->isEnabled()) {
        m_Minute->scrollToIndex(dateTime.toString(QString("mm")).toInt());
    }
    //    if (!m_Second->isEnabled()) {
    //        m_Second->scrollToIndex(dateTime.toString(QString("ss")).toInt());
    //    }
}
