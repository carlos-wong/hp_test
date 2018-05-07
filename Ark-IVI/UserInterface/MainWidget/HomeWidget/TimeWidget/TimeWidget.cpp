#include "TimeWidget.h"
#include "BusinessLogic/Widget.h"
#include "ArkApplication.h"
#include "BusinessLogic/Setting.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QDateTime>
#include "TextWidget.h"

namespace SourceString {
static const QString Monday(QObject::tr("Monday"));
static const QString Tuesday(QObject::tr("Tuesday"));
static const QString Wednesday(QObject::tr("Wednesday"));
static const QString Thursday(QObject::tr("Thursday"));
static const QString Friday(QObject::tr("Friday"));
static const QString Saturday(QObject::tr("Saturday"));
static const QString Sunday(QObject::tr("Sunday"));
}

class TimeWidgetPrivate
{
public:
    explicit TimeWidgetPrivate(TimeWidget* parent);
    ~TimeWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void setWeek(int num);
    QStringList parseDateTime(const QDateTime &dateTime);
private:
    Q_DECLARE_PUBLIC(TimeWidget)
    TimeWidget* const q_ptr;
    TextWidget* m_TimeLabel;
    TextWidget* m_DateLabel;
    TextWidget* m_WeekLabel;
    QStringList m_WeekStrList;
};

TimeWidget::TimeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new TimeWidgetPrivate(this))
{
}

TimeWidget::~TimeWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void TimeWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(510, 80, 250, 150, this);
}

void TimeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(this->rect(), QPixmap(":/Images/hw_01_small_box_home.png"));
}

void TimeWidget::onCurrentDateTime(const QDateTime &dateTime)
{
    Q_D(TimeWidget);
    QStringList timeList = d->parseDateTime(dateTime);
    d->m_TimeLabel->setText(timeList[1]);
    d->m_DateLabel->setText(timeList[0]);
    d->setWeek(dateTime.date().dayOfWeek());
}

void TimeWidget::onHourClockChange(const int type)
{
    Q_D(TimeWidget);
    QDateTime dateTime = QDateTime::currentDateTime();
    QString timeText;
    if (HCT_12HourClock == type) {
        timeText = dateTime.toString(QString("yyyy/MM/dd hh:mm ap"));
    } else {
        timeText = dateTime.toString(QString("yyyy/MM/dd hh:mm"));
    }

    QStringList timeList = timeText.split(" ");
    if  (timeList.length() > 1) {
        d->m_TimeLabel->setText(timeList[1]);
        d->m_DateLabel->setText(timeList[0]);
        d->setWeek(dateTime.date().dayOfWeek());
    }
}

TimeWidgetPrivate::TimeWidgetPrivate(TimeWidget *parent)
    : q_ptr(parent)
{
    m_TimeLabel = NULL;
    m_DateLabel = NULL;
    m_WeekLabel = NULL;
    m_WeekStrList << SourceString::Monday << SourceString::Tuesday << SourceString::Wednesday
                  << SourceString::Thursday << SourceString::Friday << SourceString::Saturday
                  << SourceString::Sunday;
    initializeParent();
    initializeBasic();
}

TimeWidgetPrivate::~TimeWidgetPrivate()
{
}

void TimeWidgetPrivate::initializeParent()
{
    Q_Q(TimeWidget);
    q->setVisible(true);
}

void TimeWidgetPrivate::initializeBasic()
{
    Q_Q(TimeWidget);
    QDateTime dateTime = QDateTime::currentDateTime();
    QStringList timeList = parseDateTime(dateTime);

    m_TimeLabel = new TextWidget(q);
    m_TimeLabel->setText(timeList[1]);
    m_TimeLabel->setFontPointSize(28 * g_Widget->widthScalabilityFactor());
    g_Widget->geometryFit(62, 28, 125, 60, m_TimeLabel);

    m_DateLabel = new TextWidget(q);
    m_DateLabel->setText(timeList[0]);
    m_DateLabel->setFontPointSize(12 * g_Widget->widthScalabilityFactor());
    g_Widget->geometryFit(7, 105, 130, 40, m_DateLabel);

    m_WeekLabel = new TextWidget(q);
    m_WeekLabel->setFontPointSize(12 * g_Widget->widthScalabilityFactor());
    g_Widget->geometryFit(175, 105, 60,40, m_WeekLabel);

    setWeek(dateTime.date().dayOfWeek());

    QObject::connect(ArkApp, ARKSENDER(currentDateTime(const QDateTime&)),
                     q,      ARKRECEIVER(onCurrentDateTime(const QDateTime&)));
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onHourClockChange(const int)));
}

QStringList TimeWidgetPrivate::parseDateTime(const QDateTime &dateTime)
{
    QString timeText;
    if (HCT_12HourClock == SettingPersistent::getHourClockType(true)) {
        timeText = dateTime.toString(QString("yyyy/MM/dd hh:mm ap"));
    } else {
        timeText = dateTime.toString(QString("yyyy/MM/dd hh:mm"));
    }

    QStringList timeList = timeText.split(" ");
    if  (timeList.length() < 2) {
        qDebug()<< "get current data time error" << __PRETTY_FUNCTION__ << __LINE__;
        timeText = QString("1970/01/01 10:00");
        timeList = timeText.split(" ");
    }

    return timeList;
}

void TimeWidgetPrivate::setWeek(int num)
{
    if (num < 1 || num > 7) return;

    m_WeekLabel->setText(m_WeekStrList[num-1]);
}
