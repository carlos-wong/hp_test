#include "SettingMessageDialog.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"

class SettingMessageDialogPrivate
{
    Q_DISABLE_COPY(SettingMessageDialogPrivate)
public:
    explicit SettingMessageDialogPrivate(SettingMessageDialog* parent);
    ~SettingMessageDialogPrivate();
    void initializeBackground();
    void initializeConfirm();
    void initializeCancel();
    void initializeTitle();
private:
    Q_DECLARE_PUBLIC(SettingMessageDialog)
    SettingMessageDialog* q_ptr;
    BmpWidget* m_Background;
    BmpButton* m_Confirm;
    BmpButton* m_Cancel;
    TextWidget* m_Titile;
};


SettingMessageDialog::SettingMessageDialog(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new SettingMessageDialogPrivate(this))
{
}

SettingMessageDialog::~SettingMessageDialog()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void SettingMessageDialog::setBtnEnable(const bool flag)
{
    Q_D(SettingMessageDialog);
    if(NULL != d->m_Confirm) {
        d->m_Confirm->setEnabled(flag);
    }
    if(NULL != d->m_Cancel) {
        d->m_Cancel->setEnabled(flag);
    }
}

void SettingMessageDialog::setBtnVisible(const bool flag)
{
    Q_D(SettingMessageDialog);
    if(NULL != d->m_Confirm) {
        d->m_Confirm->setVisible(flag);
    }
    if(NULL != d->m_Cancel) {
        d->m_Cancel->setVisible(flag);
    }
}

void SettingMessageDialog::setTitleTranslate(const bool flag)
{
    Q_D(SettingMessageDialog);
    d->initializeTitle();
    if (flag) {
        d->m_Titile->setLanguageType(TextWidget::T_Translate);
    } else {
        d->m_Titile->setLanguageType(TextWidget::T_NoTranslate);
    }
    d->m_Titile->update();
}

void SettingMessageDialog::setTitle(const QString &title)
{
    Q_D(SettingMessageDialog);
    d->initializeTitle();
    d->m_Titile->setText(title);
}

void SettingMessageDialog::setConfirm(const QString &confirm)
{
    Q_D(SettingMessageDialog);
    d->initializeConfirm();
    d->m_Confirm->setText(confirm);
}

void SettingMessageDialog::setCancel(const QString &cancel)
{
    Q_D(SettingMessageDialog);
    d->initializeCancel();
    d->m_Cancel->setText(cancel);
}

void SettingMessageDialog::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 600, 334, this);
}

SettingMessageDialogPrivate::SettingMessageDialogPrivate(SettingMessageDialog *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_Confirm = NULL;
    m_Cancel = NULL;
    m_Titile = NULL;
    initializeBackground();
}

SettingMessageDialogPrivate::~SettingMessageDialogPrivate()
{
}

void SettingMessageDialogPrivate::initializeBackground()
{
    if (NULL == m_Background) {
        Q_Q(SettingMessageDialog);
        m_Background = new BmpWidget(q);
        m_Background->setBackgroundBmpPath(QString(":/Images/SettingMessageDialogBackground.png"));
        g_Widget->geometryFit(70, 37, 460, 260, m_Background);
        m_Background->setVisible(true);
    }
}

void SettingMessageDialogPrivate::initializeConfirm()
{
    if (NULL == m_Confirm) {
        Q_Q(SettingMessageDialog);
        m_Confirm = new BmpButton(q);
        m_Confirm->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
        m_Confirm->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
        m_Confirm->setCheckBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
        g_Widget->geometryFit(70 + 65, 37 + 173 + 16, 100, 54, m_Confirm);
        m_Confirm->setVisible(true);
        QObject::connect(m_Confirm, ARKSENDER(released()),
                         q,         ARKSENDER(confirm()));
    }
}

void SettingMessageDialogPrivate::initializeCancel()
{
    if (NULL == m_Cancel) {
        Q_Q(SettingMessageDialog);
        m_Cancel = new BmpButton(q);
        m_Cancel->setNormalBmpPath(QString(":/Images/SettingSoundEqualizerBlankNormal.png"));
        m_Cancel->setPressBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
        m_Cancel->setCheckBmpPath(QString(":/Images/SettingSoundEqualizerBlankPress.png"));
        g_Widget->geometryFit(70 + 230 + 65, 37 + 173 + 16, 100, 54, m_Cancel);
        m_Cancel->setVisible(true);
        QObject::connect(m_Cancel, ARKSENDER(released()),
                         q,        ARKSENDER(cancel()));
    }
}

void SettingMessageDialogPrivate::initializeTitle()
{
    if (NULL == m_Titile) {
        Q_Q(SettingMessageDialog);
        m_Titile = new TextWidget(q);
        g_Widget->geometryFit(70, 37, 460, 173, m_Titile);
        m_Titile->setVisible(true);
    }
}
