#include "SettingFactoryWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "SettingFactoryNumberPadWidget/SettingFactoryNumberPadWidget.h"
#include "SettingFactoryLinkWidget/SettingFactoryLinkWidget.h"
#include "SettingFactoryLogoWidget/SettingFactoryLogoWidget.h"
#include <QDebug>

static const QString password("666666");

class SettingFactoryWidgetPrivate
{
public:
    explicit SettingFactoryWidgetPrivate(SettingFactoryWidget* parent);
    ~SettingFactoryWidgetPrivate();
    void initializeParent();
    void connectAllSlots();
    void initializeToolWidget();
    void initializeNumberPadWidget();
    void initializeLinkWidget();
    void initializeLogoWidget();
private:
    Q_DECLARE_PUBLIC(SettingFactoryWidget)
    SettingFactoryWidget* const q_ptr;
    SettingFactoryNumberPadWidget* m_SettingFactoryNumberPadWidget;
    SettingFactoryToolWidget* m_SettingFactoryToolWidget;
    QString m_Password;
    SettingFactoryLinkWidget* m_SettingFactoryLinkWidget;
    SettingFactoryLogoWidget* m_SettingFactoryLogoWidget;
};

SettingFactoryWidget::SettingFactoryWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingFactoryWidgetPrivate(this))
{
}

SettingFactoryWidget::~SettingFactoryWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingFactoryWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void SettingFactoryWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper1, Widget::T_Setting, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_ViewPaper0:
    case Widget::T_ViewPaper1:
    case Widget::T_SettingGeneral:
    case Widget::T_SettingStyle:
    case Widget::T_SettingSound: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingFactory: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingFactory, Widget::T_SettingFactory, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingFactoryWidget);
            d->initializeNumberPadWidget();
            d->m_Password.clear();
            d->m_SettingFactoryNumberPadWidget->setPassword(d->m_Password);
            d->m_SettingFactoryNumberPadWidget->setVisible(true);
            if (NULL != d->m_SettingFactoryLinkWidget) {
                d->m_SettingFactoryLinkWidget->setVisible(false);
                qDebug() << "visible" << __LINE__;
            }
            if (NULL != d->m_SettingFactoryLogoWidget) {
                d->m_SettingFactoryLogoWidget->setVisible(false);
            }
            if (NULL != d->m_SettingFactoryToolWidget) {
                d->m_SettingFactoryToolWidget->setVisible(false);
            }
            if (!isVisible()) {
                setVisible(true);
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingFactoryWidget::onToolBtnRelease(const SettingFactoryToolWidget::CheckBtn btn)
{
    Q_D(SettingFactoryWidget);
    switch (btn) {
    case SettingFactoryToolWidget::CB_UpdateLink: {
        d->m_SettingFactoryToolWidget->setCheckBtn(SettingFactoryToolWidget::CB_UpdateLink);
        d->initializeLinkWidget();
        d->m_SettingFactoryLinkWidget->hideDialog();
        d->m_SettingFactoryLinkWidget->setVisible(true);
        if (NULL != d->m_SettingFactoryLogoWidget) {
            d->m_SettingFactoryLogoWidget->setVisible(false);
        }
        break;
    }
    case SettingFactoryToolWidget::CB_UpdateLogo: {
        d->m_SettingFactoryToolWidget->setCheckBtn(SettingFactoryToolWidget::CB_UpdateLogo);
        d->initializeLogoWidget();
        d->m_SettingFactoryLogoWidget->hideDialog();
        d->m_SettingFactoryLogoWidget->setVisible(true);
        if (NULL != d->m_SettingFactoryLinkWidget) {
            d->m_SettingFactoryLinkWidget->setVisible(false);
        }
        break;
    }
    default: {
        return ;
        break;
    }
    }
//    qDebug() << "btn" << btn;
//    if (NULL != d->m_SettingFactoryLinkWidget) {
//        d->m_SettingFactoryLinkWidget->setVisible(btn == SettingFactoryToolWidget::CB_UpdateLink);
//        qDebug() << "visible" << __LINE__;
//    }
//    if (NULL != d->m_SettingFactoryLogoWidget) {
//        d->m_SettingFactoryLogoWidget->setVisible(btn == SettingFactoryToolWidget::CB_UpdateLogo);
//    }
    d->m_SettingFactoryToolWidget->setVisible(true);
}

void SettingFactoryWidget::onNumberPadKeyType(const QString &type)
{
    Q_D(SettingFactoryWidget);
    if (QString("Confirm") == type) {
        qDebug() << __PRETTY_FUNCTION__ << __LINE__;
        if (d->m_Password == password) {
            d->m_SettingFactoryNumberPadWidget->setVisible(false);
            d->initializeToolWidget();
            d->m_SettingFactoryToolWidget->setVisible(true);
            onToolBtnRelease(SettingFactoryToolWidget::CB_UpdateLink);
        }
    } else if (QString("Backspace") == type) {
        if (d->m_Password.length() > 0) {
            d->m_Password = d->m_Password.left(d->m_Password.length() - 1);
            QString pound;
            for (int i = 0; i < d->m_Password.length(); ++i) {
                pound.append(QChar('*'));
            }
            d->m_SettingFactoryNumberPadWidget->setPassword(pound);
        }
    } else {
        if (d->m_Password.count() < 10) {
            d->m_Password += type;
            QString pound;
            for (int i = 0; i < d->m_Password.length(); ++i) {
                pound.append(QChar('*'));
            }
            d->m_SettingFactoryNumberPadWidget->setPassword(pound);
        }
    }
}

SettingFactoryWidgetPrivate::SettingFactoryWidgetPrivate(SettingFactoryWidget *parent)
    : q_ptr(parent)
{
    m_SettingFactoryNumberPadWidget = NULL;
    m_SettingFactoryToolWidget = NULL;
    m_SettingFactoryLinkWidget = NULL;
    m_SettingFactoryLogoWidget = NULL;
    initializeParent();
    connectAllSlots();
}

SettingFactoryWidgetPrivate::~SettingFactoryWidgetPrivate()
{
}

void SettingFactoryWidgetPrivate::initializeParent()
{
    Q_Q(SettingFactoryWidget);
    q->setVisible(false);
}

void SettingFactoryWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingFactoryWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
}

void SettingFactoryWidgetPrivate::initializeToolWidget()
{
    if (NULL == m_SettingFactoryToolWidget) {
        Q_Q(SettingFactoryWidget);
        m_SettingFactoryToolWidget = new SettingFactoryToolWidget(q);
        m_SettingFactoryToolWidget->setCheckBtn(SettingFactoryToolWidget::CB_UpdateLink);
        QObject::connect(m_SettingFactoryToolWidget, ARKSENDER(toolButtonRelease(const SettingFactoryToolWidget::CheckBtn)),
                         q,                          ARKRECEIVER(onToolBtnRelease(const SettingFactoryToolWidget::CheckBtn)));
    }
}

void SettingFactoryWidgetPrivate::initializeNumberPadWidget()
{
    if (NULL == m_SettingFactoryNumberPadWidget) {
        Q_Q(SettingFactoryWidget);
        m_SettingFactoryNumberPadWidget = new SettingFactoryNumberPadWidget(q);
        m_SettingFactoryNumberPadWidget->setVisible(false);
        QObject::connect(m_SettingFactoryNumberPadWidget, ARKSENDER(numberPadKeyType(const QString&)),
                         q,                               ARKRECEIVER(onNumberPadKeyType(const QString&)));
    }
}

void SettingFactoryWidgetPrivate::initializeLinkWidget()
{

    if (NULL == m_SettingFactoryLinkWidget) {
        Q_Q(SettingFactoryWidget);
        m_SettingFactoryLinkWidget = new SettingFactoryLinkWidget(q);
        m_SettingFactoryLinkWidget->setVisible(true);
        qDebug() << "visible" << __LINE__;
    }
}

void SettingFactoryWidgetPrivate::initializeLogoWidget()
{
    if (NULL == m_SettingFactoryLogoWidget) {
        Q_Q(SettingFactoryWidget);
        m_SettingFactoryLogoWidget = new SettingFactoryLogoWidget(q);
        m_SettingFactoryLogoWidget->setVisible(true);
    }
}
