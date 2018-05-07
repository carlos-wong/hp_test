#include "SettingFactoryLogoWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "../../SettingMessageDialog.h"
#include "BusinessLogic/Setting.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

namespace SourceString {
static const QString Logo_Tip(QObject::tr("Change Logo Item Will Reboot!"));
static const QString Confirm(QObject::tr("Confirm"));
static const QString Cancel(QObject::tr("Cancel"));
static const QString Rebooting(QObject::tr("Rebooting..."));
}

static const QString prefix("/usr/share/Launcher/");
static const QString one("Logo/SettingFactoryLogoOne.jpg");
static const QString two("Logo/SettingFactoryLogoTwo.jpg");
static const QString three("Logo/SettingFactoryLogoThree.jpg");

class SettingFactoryLogoWidgetPrivate
{
public:
    explicit SettingFactoryLogoWidgetPrivate(SettingFactoryLogoWidget* parent);
    ~SettingFactoryLogoWidgetPrivate();
    void initializeParent();
    void initializeBasicWidget();
    void initializeLogoMessageDialog();
    void onLogoChange(const int type);
private:
    Q_DECLARE_PUBLIC(SettingFactoryLogoWidget)
    SettingFactoryLogoWidget* const q_ptr;
    BmpButton* m_LogoOne;
    BmpButton* m_LogoTwo;
    BmpButton* m_LogoThree;
    SettingMessageDialog* m_LogoDialog;
    unsigned short int m_Logo;
};

SettingFactoryLogoWidget::SettingFactoryLogoWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingFactoryLogoWidgetPrivate(this))
{
}

SettingFactoryLogoWidget::~SettingFactoryLogoWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingFactoryLogoWidget::hideDialog()
{
    Q_D(SettingFactoryLogoWidget);
    if (NULL != d->m_LogoDialog) {
        d->m_LogoDialog->setVisible(false);
    }
}

void SettingFactoryLogoWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(180, 52, 600, 334, this);
}

void SettingFactoryLogoWidget::onToolButtonRelease()
{
    Q_D(SettingFactoryLogoWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_LogoOne) {
        d->m_Logo = 1;
    } else if (ptr == d->m_LogoTwo) {
        d->m_Logo = 2;
    } else if (ptr == d->m_LogoThree) {
        d->m_Logo = 3;
    }
    d->initializeLogoMessageDialog();
    d->m_LogoDialog->setVisible(true);
}

void SettingFactoryLogoWidget::onConfirm()
{
    Q_D(SettingFactoryLogoWidget);
    QString middle("/WSVGA/");
    if (NULL != qApp->desktop()) {
        if (800 == qApp->desktop()->width()) {
            middle = QString("/WVGA/");
        }
    }
    QString logoPath = prefix + middle;
    switch (d->m_Logo) {
    case 1: {
        logoPath += one;
        break;
    }
    case 2: {
        logoPath += two;
        break;
    }
    case 3: {
        logoPath += three;
        break;
    }
    default: {
        return;
        break;
    }
    }
    d->onLogoChange(d->m_Logo);
    SettingPersistent::setLogoType(d->m_Logo);
    d->m_LogoDialog->setTitle(SourceString::Rebooting);
    d->m_LogoDialog->setBtnVisible(false);
    QString cmd = QString("changelogo ") + logoPath + QString(" bootlogo");
    if (0 != system(cmd.toLocal8Bit().constData())) {
        qDebug() << "execute changelogo failed!";
    }
    system("reboot");
}

void SettingFactoryLogoWidget::onCancel()
{
    Q_D(SettingFactoryLogoWidget);
    d->m_LogoDialog->setVisible(false);
}

SettingFactoryLogoWidgetPrivate::SettingFactoryLogoWidgetPrivate(SettingFactoryLogoWidget *parent)
    : q_ptr(parent)
{
    m_LogoOne = NULL;
    m_LogoTwo = NULL;
    m_LogoThree = NULL;
    m_LogoDialog = NULL;
    m_Logo = 1;
    initializeParent();
    initializeBasicWidget();
}

SettingFactoryLogoWidgetPrivate::~SettingFactoryLogoWidgetPrivate()
{
}

void SettingFactoryLogoWidgetPrivate::initializeParent()
{
    Q_Q(SettingFactoryLogoWidget);
    q->setVisible(false);
}

void SettingFactoryLogoWidgetPrivate::initializeBasicWidget()
{
    Q_Q(SettingFactoryLogoWidget);
    m_LogoOne = new BmpButton(q);
    m_LogoOne->setNormalBmpPath(QString(":/Images/SettingFactoryLogoOneNormal.png"));
    m_LogoOne->setPressBmpPath(QString(":/Images/SettingFactoryLogoOnePress.png"));
    m_LogoOne->setCheckBmpPath(QString(":/Images/SettingFactoryLogoOnePress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 0, 122, 150, 90, m_LogoOne);
    m_LogoOne->setVisible(true);
    QObject::connect(m_LogoOne, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()));
    m_LogoTwo = new BmpButton(q);
    m_LogoTwo->setNormalBmpPath(QString(":/Images/SettingFactoryLogoTwoNormal.png"));
    m_LogoTwo->setPressBmpPath(QString(":/Images/SettingFactoryLogoTwoPress.png"));
    m_LogoTwo->setCheckBmpPath(QString(":/Images/SettingFactoryLogoTwoPress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 1, 122, 150, 90, m_LogoTwo);
    m_LogoTwo->setVisible(true);
    QObject::connect(m_LogoTwo, ARKSENDER(released()),
                     q,          ARKRECEIVER(onToolButtonRelease()));

    m_LogoThree = new BmpButton(q);
    m_LogoThree->setNormalBmpPath(QString(":/Images/SettingFactoryLogoThreeNormal.png"));
    m_LogoThree->setPressBmpPath(QString(":/Images/SettingFactoryLogoThreePress.png"));
    m_LogoThree->setCheckBmpPath(QString(":/Images/SettingFactoryLogoThreePress.png"));
    g_Widget->geometryFit(37 + (37 + 150) * 2, 122, 150, 90, m_LogoThree);
    m_LogoThree->setVisible(true);
    QObject::connect(m_LogoThree, ARKSENDER(released()),
                     q,            ARKRECEIVER(onToolButtonRelease()));

    onLogoChange(SettingPersistent::getLogoType());
}

void SettingFactoryLogoWidgetPrivate::initializeLogoMessageDialog()
{
    if (NULL == m_LogoDialog) {
        Q_Q(SettingFactoryLogoWidget);
        m_LogoDialog = new SettingMessageDialog(q);
        m_LogoDialog->setTitle(SourceString::Logo_Tip);
        m_LogoDialog->setConfirm(SourceString::Confirm);
        m_LogoDialog->setCancel(SourceString::Cancel);
        m_LogoDialog->setVisible(false);
        QObject::connect(m_LogoDialog, ARKSENDER(confirm()),
                         q,            ARKRECEIVER(onConfirm()));
        QObject::connect(m_LogoDialog, ARKSENDER(cancel()),
                         q,            ARKRECEIVER(onCancel()));
        onLogoChange(SettingPersistent::getLogoType());
    }
}

void SettingFactoryLogoWidgetPrivate::onLogoChange(const int type)
{
    m_LogoOne->setStatus(type == 1? BmpButton::B_Check: BmpButton::B_Normal);
    m_LogoTwo->setStatus(type == 2? BmpButton::B_Check: BmpButton::B_Normal);
    m_LogoThree->setStatus(type == 3? BmpButton::B_Check: BmpButton::B_Normal);
}
