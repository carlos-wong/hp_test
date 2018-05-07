#include "UtilityWidget.h"
#include "BusinessLogic/Widget.h"
#include "TextWidget.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QDateTime>
#include <QDate>

namespace SourceString {
static const QString Monday(QObject::tr("Monday"));
static const QString Tuesday(QObject::tr("Tuesday"));
static const QString Wednesday(QObject::tr("Wednesday"));
static const QString Thursday(QObject::tr("Thursday"));
static const QString Friday(QObject::tr("Friday"));
static const QString Saturday(QObject::tr("Saturday"));
static const QString Sunday(QObject::tr("Sunday"));
}

class UtilityWidgetPrivate
{
public:
    explicit UtilityWidgetPrivate(UtilityWidget* parent);
    ~UtilityWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void onCurrentDate(const QDate& date);
private:
    Q_DECLARE_PUBLIC(UtilityWidget)
    UtilityWidget* const q_ptr;
    TextWidget* m_Week;
};

UtilityWidget::UtilityWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new UtilityWidgetPrivate(this))
{
}

UtilityWidget::~UtilityWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void UtilityWidget::timerEvent(QTimerEvent *event)
{
    Q_D(UtilityWidget);
    QDate date = QDate::currentDate();
    if (date.isValid()) {
        d->m_Week->setText(QDate::longDayName(date.dayOfWeek()));
    }
}

void UtilityWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(57, 177, 286, 58, this);
}

void UtilityWidget::onCurrentDateTime(const QDateTime &dateTime)
{
    Q_D(UtilityWidget);
    d->onCurrentDate(dateTime.date());
}

UtilityWidgetPrivate::UtilityWidgetPrivate(UtilityWidget *parent)
    : q_ptr(parent)
{
    m_Week = NULL;
    initializeBasic();
    initializeParent();
}

UtilityWidgetPrivate::~UtilityWidgetPrivate()
{
}

void UtilityWidgetPrivate::initializeParent()
{
    Q_Q(UtilityWidget);
    q->setVisible(true);
}

void UtilityWidgetPrivate::initializeBasic()
{
    Q_Q(UtilityWidget);
    m_Week = new TextWidget(q);
    g_Widget->geometryFit(143, 0, 143, 58, m_Week);
    QObject::connect(ArkApp, ARKSENDER(currentDateTime(const QDateTime&)),
                     q,      ARKRECEIVER(onCurrentDateTime(const QDateTime&)));
    onCurrentDate(QDate::currentDate());
    m_Week->setVisible(true);
}

void UtilityWidgetPrivate::onCurrentDate(const QDate &date)
{
    switch (date.dayOfWeek()) {
    case 1: {
        m_Week->setText(SourceString::Monday);
        break;
    }
    case 2: {
        m_Week->setText(SourceString::Tuesday);
        break;
    }
    case 3: {
        m_Week->setText(SourceString::Wednesday);
        break;
    }
    case 4: {
        m_Week->setText(SourceString::Thursday);
        break;
    }
    case 5: {
        m_Week->setText(SourceString::Friday);
        break;
    }
    case 6: {
        m_Week->setText(SourceString::Saturday);
        break;
    }
    case 7: {
        m_Week->setText(SourceString::Sunday);
        break;
    }
    default: {
        break;
    }
    }
}
