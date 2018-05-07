#include "Widget.h"
#include "UserInterfaceUtility.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QResource>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <QFont>

class WidgetPrivate
{
    Q_DISABLE_COPY(WidgetPrivate)
public:
    explicit WidgetPrivate(Widget* parent);
    ~WidgetPrivate();
    void initialize();
    Widget::Type m_PreemptiveType;
    Widget::Type m_HolderType;
    Widget::Type M_Type;
    QString m_Rcc;
    float m_WidthScalabilityFactor;
    float m_HeightScalabilityFactor;
private:
    Widget* m_Parent;
};

void Widget::reset()
{
}

QRect Widget::geometryFit(const int x, const int y, const int w, const int h, QWidget *widget)
{
    initializePrivate();
    int ax = x * m_Private->m_WidthScalabilityFactor;
    int ay = y * m_Private->m_HeightScalabilityFactor;
    int aw = w * m_Private->m_WidthScalabilityFactor;
    int ah = h * m_Private->m_HeightScalabilityFactor;
    if (NULL != widget) {
        widget->move(ax, ay);
        widget->setFixedSize(QSize(aw, ah));
    }
    QRect rect(ax, ay, aw, ah);
    return rect;
}

void Widget::MoveToPoint(int x, int y, QWidget *widget)
{
    initializePrivate();
    int ax = x * m_Private->m_WidthScalabilityFactor;
    int ay = y * m_Private->m_HeightScalabilityFactor;
    if (NULL != widget) {
        widget->move(ax, ay);
    }
}

float Widget::widthScalabilityFactor()
{
    initializePrivate();
    return m_Private->m_WidthScalabilityFactor;
}

float Widget::heightScalabilityFactor()
{
    initializePrivate();
    return m_Private->m_HeightScalabilityFactor;
}

void Widget::acquireWidgetType(const Widget::Type type)
{
    initializePrivate();
    if (Widget::T_Undefine == m_Private->m_PreemptiveType) {
        if (type != m_Private->m_PreemptiveType) {
            m_Private->m_HolderType = m_Private->M_Type;
            m_Private->m_PreemptiveType = type;
            onWidgetTypeChange(type, m_Private->M_Type, WidgetStatus::RequestShow);
        }
    }
}

void Widget::releaseWidgetType(const Widget::Type type)
{
    initializePrivate();
    if (Widget::T_Undefine != m_Private->m_PreemptiveType) {
        if (type == m_Private->m_PreemptiveType) {
            m_Private->m_PreemptiveType = Widget::T_Undefine;
            //            setWidgetType(m_Private->m_HolderType, m_Private->M_Type, WidgetStatus::RequestShow);
            onWidgetTypeChange(m_Private->m_HolderType, m_Private->M_Type,  WidgetStatus::RequestShow);
        }
    }
}

void Widget::setWidgetType(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    initializePrivate();
    if (WidgetStatus::Show == status) {
        m_Private->M_Type = destinationType;
    }
    if ((Widget::T_Undefine == m_Private->m_PreemptiveType)
            || (destinationType == m_Private->m_PreemptiveType)) {
        onWidgetTypeChange(destinationType, requestType,  status);
    } else {
        m_Private->m_HolderType = destinationType;
    }
}

const Widget::Type Widget::currentWidgetType() const
{
    return m_Private->M_Type;
}

Widget::Widget(QObject *parent)
    : QObject(parent)
    , m_Private(new WidgetPrivate(this))
{
}

Widget::~Widget()
{
}

void Widget::initializePrivate()
{
    if (NULL == m_Private) {
        m_Private.reset(new WidgetPrivate(this));
    }
}

WidgetPrivate::WidgetPrivate(Widget* parent)
    : m_Parent(parent)
{
    m_PreemptiveType = Widget::T_Undefine;
    m_HolderType = Widget::T_Undefine;
    M_Type = Widget::T_Radio;
    m_Rcc = QString("/usr/share/Launcher/WSVGA/WSVGA.rcc");
//    m_Rcc = QString("/usr/bin/WSVGA.rcc");
    m_WidthScalabilityFactor = 1.000000f;
    m_HeightScalabilityFactor = 1.000000f;
    initialize();
}

WidgetPrivate::~WidgetPrivate()
{
    QResource::unregisterResource(m_Rcc);
}

void WidgetPrivate::initialize()
{
    int fd = open("/dev/fb0", O_RDONLY);
    int pixelSize(20);
    if (fd) {
        struct fb_var_screeninfo fb_var;
        if (-1 != ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)) {
            if (600 == fb_var.yres) {
                m_WidthScalabilityFactor = 1.28f;
                m_HeightScalabilityFactor = 1.25f;
                pixelSize = 25;
            } else {
                m_Rcc = QString("/usr/share/Launcher/WVGA/WVGA.rcc");
            }
            close(fd);
        }
    }

#ifndef DESKTOP_AMD64
    QFont font(QString("DejaVu Sans"));
    font.setPixelSize(pixelSize);
    ArkApp->setFont(font);
    QResource::registerResource(m_Rcc);
#else
    m_WidthScalabilityFactor = 1.28f;
    m_HeightScalabilityFactor = 1.25f;
    pixelSize = 26;
    QFont font;
    font.setPixelSize(pixelSize);
    ArkApp->setFont(font);
    QResource::registerResource(m_Rcc);
#endif
}
