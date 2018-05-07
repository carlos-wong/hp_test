#include "CalibrateWidget.h"
#include "TextWidget.h"
#include "SettingPersistent.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QProcess>
#include <QWSServer>
#include <QWSMouseHandler>
#include <QDebug>

namespace SourceString {
static const QString Touch_Cursor_To_Calibrate(QObject::tr("Touch Cursor To Calibrate"));
static const QString Calibrate_Finish(QObject::tr("Calibrate Finish"));
}

class CalibrateWidgetPrivate
{
public:
    explicit CalibrateWidgetPrivate(CalibrateWidget* parent);
    ~CalibrateWidgetPrivate();
    void initialize();
    void initializeParent();
    void initializeBasicWidget();
    TextWidget* m_Title;
    QProcess* m_Process;
    QPoint m_CursorPoint;
private:
    CalibrateWidget* const q_ptr;
    Q_DECLARE_PUBLIC(CalibrateWidget)
};

CalibrateWidget::CalibrateWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CalibrateWidgetPrivate(this))
{
}

CalibrateWidget::~CalibrateWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void CalibrateWidget::startCalibrate()
{
    Q_D(CalibrateWidget);
    stopCalibrate();
    QString cmd = QString("ts_calibrate");
    QStringList arguments;
    arguments << QString("hidegui");
    d->m_Process->start(cmd, arguments);
    d->m_Process->waitForStarted();
    setVisible(true);
    raise();
}

void CalibrateWidget::stopCalibrate()
{
    Q_D(CalibrateWidget);
    if (QProcess::NotRunning != d->m_Process->state()) {
        d->m_Process->kill();
    }
    setVisible(false);
}

void CalibrateWidget::paintEvent(QPaintEvent *event)
{
    Q_D(CalibrateWidget);
    if ((d->m_CursorPoint.x() > 0)
            && (d->m_CursorPoint.y() > 0)) {
        QPainter painter(this);
        painter.fillRect(event->rect(), Qt::black);
        QPoint point = d->m_CursorPoint;
        painter.fillRect(point.x() - 6, point.y() - 1, 13, 3, Qt::white);
        painter.fillRect(point.x() - 1, point.y() - 6, 3, 13, Qt::white);
    }
}

void CalibrateWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void CalibrateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << __PRETTY_FUNCTION__;
    //    stopCalibrate();
}

void CalibrateWidget::onStarted()
{
}

void CalibrateWidget::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
}

void CalibrateWidget::onError(QProcess::ProcessError error)
{
    qDebug() << __PRETTY_FUNCTION__ << error;
}

void CalibrateWidget::onStateChanged(QProcess::ProcessState state)
{
}

void CalibrateWidget::onReadyReadStandardOutput()
{
    qDebug() << __PRETTY_FUNCTION__;
    Q_D(CalibrateWidget);
    while (d->m_Process->canReadLine()) {
        QString output(d->m_Process->readLine());
        if (output.startsWith(QString("cursor:x="))) {
            QString keyWord("cursor:x=");
            bool flag(true);
            int x = output.mid(keyWord.length()).toInt(&flag);
            if (flag) {
                d->m_CursorPoint.setX(x);
            }
        } else if (output.startsWith(QString("cursor:y="))) {
            QString keyWord("cursor:y=");
            bool flag(true);
            int y = output.mid(keyWord.length()).toInt(&flag);
            if (flag) {
                d->m_CursorPoint.setY(y);
                update();
            }
        } else if (output.startsWith(QString("Calibration success."))) {
            d->m_Title->setText(SourceString::Calibrate_Finish);
            d->m_CursorPoint.setX(0);
            d->m_CursorPoint.setY(0);
            update();
            qApp->processEvents();

#ifndef DESKTOP_AMD64
            QWSServer::instance()->openMouse();
#endif

            SettingPersistent::setCalibrate(true);
            setVisible(false);
        }
    }
}

void CalibrateWidget::onReadyReadStandardError()
{
}

CalibrateWidgetPrivate::CalibrateWidgetPrivate(CalibrateWidget *parent)
    : q_ptr(parent)
{
    m_Title = NULL;
    initialize();
    initializeParent();
    initializeBasicWidget();
}

CalibrateWidgetPrivate::~CalibrateWidgetPrivate()
{
}

void CalibrateWidgetPrivate::initialize()
{
    m_CursorPoint.setX(-1);
    m_CursorPoint.setY(-1);
    Q_Q(CalibrateWidget);
    m_Process = new QProcess(q);
    m_Process->setProcessChannelMode(QProcess::MergedChannels);
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_Process, ARKSENDER(started()),
                     q,         ARKRECEIVER(onStarted()),
                     type);
    QObject::connect(m_Process, ARKSENDER(finished(int, QProcess::ExitStatus)),
                     q,         ARKRECEIVER(onFinished(int, QProcess::ExitStatus)),
                     type);
    QObject::connect(m_Process, ARKSENDER(error(QProcess::ProcessError)),
                     q,         ARKRECEIVER(onError(QProcess::ProcessError)),
                     type);
    QObject::connect(m_Process, ARKSENDER(stateChanged(QProcess::ProcessState)),
                     q,         ARKRECEIVER(onStateChanged(QProcess::ProcessState)),
                     type);
    QObject::connect(m_Process, ARKSENDER(readyReadStandardOutput()),
                     q,         ARKRECEIVER(onReadyReadStandardOutput()),
                     type);
    QObject::connect(m_Process, ARKSENDER(readyReadStandardError()),
                     q,         ARKRECEIVER(onReadyReadStandardError()),
                     type);
}

void CalibrateWidgetPrivate::initializeParent()
{
    Q_Q(CalibrateWidget);
    q->setVisible(false);
}

void CalibrateWidgetPrivate::initializeBasicWidget()
{
    Q_Q(CalibrateWidget);
    m_Title = new TextWidget(q);
    m_Title->setText(SourceString::Touch_Cursor_To_Calibrate);
    g_Widget->geometryFit(0, 0, 800, 240, m_Title);
    m_Title->setVisible(true);
}
