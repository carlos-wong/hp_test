#include "BluetoothMessageDialog.h"
#include "BmpWidget.h"
#include "BmpButton.h"
#include "TextWidget.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"

class BluetoothMessageDialogPrivate
{
    Q_DISABLE_COPY(BluetoothMessageDialogPrivate)
public:
    explicit BluetoothMessageDialogPrivate(BluetoothMessageDialog* parent);
    ~BluetoothMessageDialogPrivate();
    void initializeBackground();
    void initializeConfirm();
    void initializeCancel();
    void initializeTitle();
private:
    Q_DECLARE_PUBLIC(BluetoothMessageDialog)
    BluetoothMessageDialog* q_ptr;
    BmpWidget* m_Background;
    BmpButton* m_Confirm;
    BmpButton* m_Cancel;
    TextWidget* m_Title;
    QString m_TitleText;
};


BluetoothMessageDialog::BluetoothMessageDialog(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothMessageDialogPrivate(this))
{
}

BluetoothMessageDialog::~BluetoothMessageDialog()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothMessageDialog::setConfirmVisible(const bool flag)
{
    Q_D(BluetoothMessageDialog);
    if(NULL != d->m_Confirm) {
        d->m_Confirm->setVisible(flag);
    }
}

void BluetoothMessageDialog::setTitle(const QString &title)
{
    Q_D(BluetoothMessageDialog);
    d->initializeTitle();
    d->m_TitleText = title;
    d->m_Title->setText(d->m_TitleText);
}

QString BluetoothMessageDialog::getTitle()
{
    Q_D(BluetoothMessageDialog);
    return d->m_TitleText;
}

void BluetoothMessageDialog::setConfirm(const QString &confirm)
{
    Q_D(BluetoothMessageDialog);
    d->initializeConfirm();
    d->m_Confirm->setText(confirm);
}

void BluetoothMessageDialog::setCancel(const QString &cancel)
{
    Q_D(BluetoothMessageDialog);
    d->initializeCancel();
    d->m_Cancel->setText(cancel);
}

void BluetoothMessageDialog::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(175, 54, 600, 334, this);
}

BluetoothMessageDialogPrivate::BluetoothMessageDialogPrivate(BluetoothMessageDialog *parent)
    : q_ptr(parent)
{
    m_Background = NULL;
    m_Confirm = NULL;
    m_Cancel = NULL;
    m_Title = NULL;
    initializeBackground();
}

BluetoothMessageDialogPrivate::~BluetoothMessageDialogPrivate()
{
}

void BluetoothMessageDialogPrivate::initializeBackground()
{
    if (NULL == m_Background) {
        Q_Q(BluetoothMessageDialog);
        m_Background = new BmpWidget(q);
        m_Background->setBackgroundBmpPath(QString(":/Images/SettingMessageDialogBackground.png"));
        g_Widget->geometryFit(70, 37, 460, 260, m_Background);
        m_Background->setVisible(true);
    }
}

void BluetoothMessageDialogPrivate::initializeConfirm()
{
    if (NULL == m_Confirm) {
        Q_Q(BluetoothMessageDialog);
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

void BluetoothMessageDialogPrivate::initializeCancel()
{
    if (NULL == m_Cancel) {
        Q_Q(BluetoothMessageDialog);
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

void BluetoothMessageDialogPrivate::initializeTitle()
{
    if (NULL == m_Title) {
        Q_Q(BluetoothMessageDialog);
        m_Title = new TextWidget(q);
        m_Title->setLanguageType(TextWidget::T_NoTranslate);
        g_Widget->geometryFit(70, 37, 460, 173, m_Title);
        m_Title->setVisible(true);
    }
}
