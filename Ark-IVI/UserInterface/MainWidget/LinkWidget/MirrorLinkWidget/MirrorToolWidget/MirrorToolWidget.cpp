#include "MirrorToolWidget.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include <QPainter>

class MirrorToolWidgetPrivate
{
    Q_DISABLE_COPY(MirrorToolWidgetPrivate)
public:
    explicit MirrorToolWidgetPrivate(MirrorToolWidget* parent);
    ~MirrorToolWidgetPrivate();
    void initialize();
    void connectAllSlots();
    BmpButton* m_BlueToothBtn = NULL;
    BmpButton* m_MenuBtn = NULL;
    BmpButton* m_CarBtn = NULL;
    BmpButton* m_HomeBtn = NULL;
    BmpButton* m_BackBtn = NULL;
private:
    MirrorToolWidget* m_Parent = NULL;
};

MirrorToolWidget::MirrorToolWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MirrorToolWidgetPrivate(this))
{
}

MirrorToolWidget::~MirrorToolWidget()
{
}

void MirrorToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 76, 480, this);
}

void MirrorToolWidget::timerEvent(QTimerEvent *event)
{
    QWidget::timerEvent(event);
}

bool MirrorToolWidget::event(QEvent *event)
{
    bool ret(false);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonDblClick: {
        ret = true;
        break;
    }
    default: {
        ret = QWidget::event(event);
        break;
    }
    }
    return ret;
}

void MirrorToolWidget::onToolBtnRelease()
{
    const BmpButton* ptr = dynamic_cast<const BmpButton*>(sender());
    if (ptr == m_Private->m_CarBtn) {
        emit toolBtnRelease(MirrorToolWidget::T_Car);
    } else if (ptr == m_Private->m_BackBtn) {
        emit toolBtnRelease(MirrorToolWidget::T_Back);
    } else if (ptr == m_Private->m_HomeBtn) {
        emit toolBtnRelease(MirrorToolWidget::T_Home);
    } else if (ptr == m_Private->m_MenuBtn) {
        emit toolBtnRelease(MirrorToolWidget::T_Menu);
    } else if (ptr == m_Private->m_BlueToothBtn) {
        emit toolBtnRelease(MirrorToolWidget::T_BlueTooth);
    }
}

MirrorToolWidgetPrivate::MirrorToolWidgetPrivate(MirrorToolWidget *parent)
    : m_Parent(parent)
{
    initialize();
    connectAllSlots();
}

MirrorToolWidgetPrivate::~MirrorToolWidgetPrivate()
{
}

void MirrorToolWidgetPrivate::initialize()
{
    m_BlueToothBtn = new BmpButton(m_Parent);
    m_BlueToothBtn->setNormalBmpPath(QString(":/Images/LinkWidgetMirrorBlueToothNormal.png"));
    m_BlueToothBtn->setPressBmpPath(QString(":/Images/LinkWidgetMirrorBlueToothPress.png"));
    m_BlueToothBtn->setVisible(true);
    g_Widget->geometryFit(0, 24, 76, 52, m_BlueToothBtn);
    m_MenuBtn = new BmpButton(m_Parent);
    m_MenuBtn->setNormalBmpPath(QString(":/Images/MirrorLinkWidgetMenuBtnNormal.png"));
    m_MenuBtn->setPressBmpPath(QString(":/Images/MirrorLinkWidgetMenuBtnPress.png"));
    g_Widget->geometryFit(0, 100, 76, 52, m_MenuBtn);
    m_MenuBtn->setVisible(true);
    m_HomeBtn = new BmpButton(m_Parent);
    m_HomeBtn->setNormalBmpPath(QString(":/Images/MirrorLinkWidgetHomeBtnNormal.png"));
    m_HomeBtn->setPressBmpPath(QString(":/Images/MirrorLinkWidgetHomeBtnPress.png"));
    g_Widget->geometryFit(0, 176, 76, 52, m_HomeBtn);
    m_HomeBtn->setVisible(true);
    m_BackBtn = new BmpButton(m_Parent);
    m_BackBtn->setNormalBmpPath(QString(":/Images/MirrorLinkWidgetBackBtnNormal.png"));
    m_BackBtn->setPressBmpPath(QString(":/Images/MirrorLinkWidgetBackBtnPress.png"));
    g_Widget->geometryFit(0, 252, 76, 52, m_BackBtn);
    m_BackBtn->setVisible(true);
    m_CarBtn = new BmpButton(m_Parent);
    m_CarBtn->setNormalBmpPath(QString(":/Images/MirrorLinkWidgetCarBtnNormal.png"));
    m_CarBtn->setPressBmpPath(QString(":/Images/MirrorLinkWidgetCarBtnPress.png"));
    g_Widget->geometryFit(0, 328, 76, 52, m_CarBtn);
    m_CarBtn->setVisible(true);
}

void MirrorToolWidgetPrivate::connectAllSlots()
{
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection);
    QObject::connect(m_CarBtn, SIGNAL(released()),
                     m_Parent, SLOT(onToolBtnRelease()),
                     type);
    QObject::connect(m_BackBtn, SIGNAL(released()),
                     m_Parent,  SLOT(onToolBtnRelease()),
                     type);
    QObject::connect(m_HomeBtn, SIGNAL(released()),
                     m_Parent,  SLOT(onToolBtnRelease()),
                     type);
    QObject::connect(m_MenuBtn, SIGNAL(released()),
                     m_Parent,  SLOT(onToolBtnRelease()),
                     type);
    QObject::connect(m_BlueToothBtn, SIGNAL(released()),
                     m_Parent,       SLOT(onToolBtnRelease()),
                     type);
}
