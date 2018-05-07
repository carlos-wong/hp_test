#include "SettingStyleWallpaperWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "BusinessLogic/Setting.h"

class SettingStyleWallpaperWidgetPrivate
{
public:
    explicit SettingStyleWallpaperWidgetPrivate(SettingStyleWallpaperWidget* parent);
    ~SettingStyleWallpaperWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void connectAllSlots();
    void onThemeChange(const int type);
private:
    Q_DECLARE_PUBLIC(SettingStyleWallpaperWidget)
    SettingStyleWallpaperWidget* const q_ptr;
    BmpButton* m_ThemeOne;
    BmpButton* m_ThemeTwo;
    BmpButton* m_ThemeThree;
    BmpButton* m_ThemeFour;
    BmpButton* m_ThemeFive;
    BmpButton* m_ThemeSix;
};

SettingStyleWallpaperWidget::SettingStyleWallpaperWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingStyleWallpaperWidgetPrivate(this))
{
}

SettingStyleWallpaperWidget::~SettingStyleWallpaperWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingStyleWallpaperWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 0, 600, 334, this);
}

void SettingStyleWallpaperWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
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
    case Widget::T_SettingSound:
    case Widget::T_SettingFactory: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    case Widget::T_SettingStyle: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_SettingStyle, Widget::T_SettingStyle, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            Q_D(SettingStyleWallpaperWidget);
            setVisible(true);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void SettingStyleWallpaperWidget::onThemeChange(const int type)
{
    Q_D(SettingStyleWallpaperWidget);
    d->onThemeChange(type);
}

void SettingStyleWallpaperWidget::onToolButtonRelease()
{
    Q_D(SettingStyleWallpaperWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_ThemeOne) {
        g_Setting->setThemeType(TT_ThemeOne);
    } else if (ptr == d->m_ThemeTwo) {
        g_Setting->setThemeType(TT_ThemeTwo);
    } else if (ptr == d->m_ThemeThree) {
        g_Setting->setThemeType(TT_ThemeThree);
    } else if (ptr == d->m_ThemeFour) {
        g_Setting->setThemeType(TT_ThemeFour);
    } else if (ptr == d->m_ThemeFive) {
        g_Setting->setThemeType(TT_ThemeFive);
    } else if (ptr == d->m_ThemeSix) {
        g_Setting->setThemeType(TT_ThemeSix);
    }
}

SettingStyleWallpaperWidgetPrivate::SettingStyleWallpaperWidgetPrivate(SettingStyleWallpaperWidget *parent)
    : q_ptr(parent)
{
    m_ThemeOne = NULL;
    m_ThemeTwo = NULL;
    m_ThemeThree = NULL;
    m_ThemeFour = NULL;
    m_ThemeFive = NULL;
    m_ThemeSix = NULL;
    initializeParent();
    initializeBasicWidget();
    connectAllSlots();
}

SettingStyleWallpaperWidgetPrivate::~SettingStyleWallpaperWidgetPrivate()
{
}

void SettingStyleWallpaperWidgetPrivate::initializeParent()
{
    Q_Q(SettingStyleWallpaperWidget);
    q->setVisible(false);
}

void SettingStyleWallpaperWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingStyleWallpaperWidget);
    m_ThemeOne = new BmpButton(q);
    m_ThemeOne->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeOneNormal.png"));
    m_ThemeOne->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeOnePress.png"));
    m_ThemeOne->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeOnePress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 0, 51 + (51 + 90) * 0, 150, 90, m_ThemeOne);
    m_ThemeOne->setVisible(true);
    QObject::connect(m_ThemeOne, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()));
    m_ThemeTwo = new BmpButton(q);
    m_ThemeTwo->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeTwoNormal.png"));
    m_ThemeTwo->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeTwoPress.png"));
    m_ThemeTwo->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeTwoPress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 1, 51 + (51 + 90) * 0, 150, 90, m_ThemeTwo);
    m_ThemeTwo->setVisible(true);
    QObject::connect(m_ThemeTwo, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()));

    m_ThemeThree = new BmpButton(q);
    m_ThemeThree->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeThreeNormal.png"));
    m_ThemeThree->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeThreePress.png"));
    m_ThemeThree->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeThreePress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 2, 51 + (51 + 90) * 0, 150, 90, m_ThemeThree);
    m_ThemeThree->setVisible(true);
    QObject::connect(m_ThemeThree, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()));

    m_ThemeFour = new BmpButton(q);
    m_ThemeFour->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeFourNormal.png"));
    m_ThemeFour->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeFourPress.png"));
    m_ThemeFour->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeFourPress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 0, 51 + (51 + 90) * 1, 150, 90, m_ThemeFour);
    m_ThemeFour->setVisible(true);
    QObject::connect(m_ThemeFour, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()));

    m_ThemeFive = new BmpButton(q);
    m_ThemeFive->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeFiveNormal.png"));
    m_ThemeFive->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeFivePress.png"));
    m_ThemeFive->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeFivePress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 1, 51 + (51 + 90) * 1, 150, 90, m_ThemeFive);
    m_ThemeFive->setVisible(true);
    QObject::connect(m_ThemeFive, ARKSENDER(released()),
                     q,           ARKRECEIVER(onToolButtonRelease()));

    m_ThemeSix = new BmpButton(q);
    m_ThemeSix->setNormalBmpPath(QString(":/Images/SettingStyleWallpaperThemeSixNormal.png"));
    m_ThemeSix->setPressBmpPath(QString(":/Images/SettingStyleWallpaperThemeSixPress.png"));
    m_ThemeSix->setCheckBmpPath(QString(":/Images/SettingStyleWallpaperThemeSixPress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 2,  51 + (51 + 90) * 1, 150, 90, m_ThemeSix);
    m_ThemeSix->setVisible(true);
    QObject::connect(m_ThemeSix, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()));
    onThemeChange(SettingPersistent::getThemeType());
}

void SettingStyleWallpaperWidgetPrivate::connectAllSlots()
{
    Q_Q(SettingStyleWallpaperWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Setting, q, ARKRECEIVER(onThemeChange(const int)));
}

void SettingStyleWallpaperWidgetPrivate::onThemeChange(const int type)
{
    m_ThemeOne->setStatus(type == TT_ThemeOne? BmpButton::B_Check: BmpButton::B_Normal);
    m_ThemeTwo->setStatus(type == TT_ThemeTwo? BmpButton::B_Check: BmpButton::B_Normal);
    m_ThemeThree->setStatus(type == TT_ThemeThree? BmpButton::B_Check: BmpButton::B_Normal);
    m_ThemeFour->setStatus(type == TT_ThemeFour? BmpButton::B_Check: BmpButton::B_Normal);
    m_ThemeFive->setStatus(type == TT_ThemeFive? BmpButton::B_Check: BmpButton::B_Normal);
    m_ThemeSix->setStatus(type == TT_ThemeSix? BmpButton::B_Check: BmpButton::B_Normal);
}

