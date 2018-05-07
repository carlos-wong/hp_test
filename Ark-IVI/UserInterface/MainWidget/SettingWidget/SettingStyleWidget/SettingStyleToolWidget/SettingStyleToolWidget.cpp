#include "SettingStyleToolWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"

class SettingStyleToolWidgetPrivate
{
public:
    explicit SettingStyleToolWidgetPrivate(SettingStyleToolWidget* parent);
    ~SettingStyleToolWidgetPrivate();
    void initializeParent();
    void connectAllSlots();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(SettingStyleToolWidget)
    SettingStyleToolWidget* const q_ptr;
    //    BmpButton* m_LogoBtn = NULL;
    BmpButton* m_WallpaperBtn;
};

SettingStyleToolWidget::SettingStyleToolWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingStyleToolWidgetPrivate(this))
{
}

SettingStyleToolWidget::~SettingStyleToolWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingStyleToolWidget::setCheckBtn(const SettingStyleToolWidget::CheckBtn btn)
{
    Q_D(SettingStyleToolWidget);
    switch (btn) {
    case CB_Wallpaper: {
        d->m_WallpaperBtn->setStatus(BmpButton::B_Check);
        break;
    }
    default: {
        break;
    }
    }
}

void SettingStyleToolWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 334, this);
}

void SettingStyleToolWidget::onToolButtonRelease()
{
    Q_D(SettingStyleToolWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_WallpaperBtn) {
        toolButtonRelease(SettingStyleToolWidget::CB_Wallpaper);
    }
}

SettingStyleToolWidgetPrivate::SettingStyleToolWidgetPrivate(SettingStyleToolWidget *parent)
    : q_ptr(parent)
{
    //    BmpButton* m_LogoBtn = NULL;
    m_WallpaperBtn = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingStyleToolWidgetPrivate::~SettingStyleToolWidgetPrivate()
{
}

void SettingStyleToolWidgetPrivate::initializeParent()
{
    Q_Q(SettingStyleToolWidget);
    q->setVisible(false);
}

void SettingStyleToolWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingStyleToolWidget);
}

void SettingStyleToolWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingStyleToolWidget);
    //    m_LogoBtn = new BmpButton(q);
    //    m_LogoBtn->setNormalBmpPath(QString(":/Images/SettingStyleLogoNormal.png"));
    //    m_LogoBtn->setPressBmpPath(QString(":/Images/SettingStyleLogoPress.png"));
    //    m_LogoBtn->setCheckBmpPath(QString(":/Images/SettingStyleLogoPress.png"));
    //    g_Widget->geometryFit(24, 0, 138, 66, m_LogoBtn);
    //    m_LogoBtn->setVisible(true);
    //    QObject::connect(m_LogoBtn, ARKSENDER(released()),
    //                     q,         ARKRECEIVER(onBtnReleased()));

    m_WallpaperBtn = new BmpButton(q);
    m_WallpaperBtn->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperNormal.png"));
    m_WallpaperBtn->setPressBmpPath(QString(":/Images/SettingStyleWallpaperPress.png"));
    m_WallpaperBtn->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperPress.png"));
    g_Widget->geometryFit(24, 0, 138, 66, m_WallpaperBtn);
    m_WallpaperBtn->setVisible(true);
    QObject::connect(m_WallpaperBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onToolButtonRelease()));
}

