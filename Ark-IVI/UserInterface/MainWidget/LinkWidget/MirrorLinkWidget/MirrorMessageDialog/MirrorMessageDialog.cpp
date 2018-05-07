#include "MirrorMessageDialog.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "TextWidget.h"
#include "BmpButton.h"
#include "BmpWidget.h"
#include "BusinessLogic/Widget.h"
#include "UserInterfaceUtility.h"
#include <QPainter>

namespace SourceString {
static const QString Connect_Bluetooth(QObject::tr("Bluetooth Information!"));
static const QString Phone(QObject::tr("Phone:"));
static const QString Disconnect(QObject::tr("Disconnect"));
static const QString Connect(QObject::tr("Connect"));
static const QString Device_Name(QObject::tr("Device Name:"));
static const QString Device_Code(QObject::tr("Device Code:"));
}

class MirrorMessageDialogPrivate
{
    Q_DISABLE_COPY(MirrorMessageDialogPrivate)
public:
    explicit MirrorMessageDialogPrivate(MirrorMessageDialog* parent);
    ~MirrorMessageDialogPrivate();
    void initialize();
    void connectAllSlots();
    TextWidget* m_TitleTextWidget = NULL;
    TextWidget* m_NameTextWidget = NULL;
    TextWidget* m_CodeTextWidget = NULL;
    TextWidget* m_PhoneStatus = NULL;
    BmpWidget* m_Background = NULL;
private:
    MirrorMessageDialog* m_Parent = NULL;
};

MirrorMessageDialog::MirrorMessageDialog(QWidget *parent)
    : QWidget(parent)
    , m_Private(new MirrorMessageDialogPrivate(this))
{
}

MirrorMessageDialog::~MirrorMessageDialog()
{
}

void MirrorMessageDialog::setDeviceName(const QString &name)
{
    QString text = QString(QObject::tr(SourceString::Device_Name.toLocal8Bit().constData())) + name;
    m_Private->m_NameTextWidget->setText(text);
}

void MirrorMessageDialog::setDeviceCode(const QString &code)
{
    QString text = QString(QObject::tr(SourceString::Device_Name.toLocal8Bit().constData())) + code;
    m_Private->m_CodeTextWidget->setText(text);
}

void MirrorMessageDialog::setDeviceStatus(const int flag)
{
//    if (flag == BT_CONNECTED) {
//        m_Private->m_PhoneStatus->setText(QObject::tr(SourceString::Phone.toLocal8Bit().constData()) + QObject::tr(SourceString::Connect.toLocal8Bit().constData()));
//    } else {
//        m_Private->m_PhoneStatus->setText(QObject::tr(SourceString::Phone.toLocal8Bit().constData()) + QObject::tr(SourceString::Disconnect.toLocal8Bit().constData()));
//    }
}

void MirrorMessageDialog::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
//    int width(853);
//    int height(360);
//    g_Widget->geometryFit(40 + (g_Widget->baseWindowWidth() - width) * 0.5, (g_Widget->baseWindowHeight() - height) * 0.5, width, height, m_Private->m_Background);
//    g_Widget->geometryFit(40 + (g_Widget->baseWindowWidth() - width) * 0.5, 120 * 0 + (g_Widget->baseWindowHeight() - height) * 0.5, width, 120, m_Private->m_TitleTextWidget);
//    g_Widget->geometryFit(40 + (g_Widget->baseWindowWidth() - width) * 0.5, 120 * 1 + (g_Widget->baseWindowHeight() - height) * 0.5, width * 0.5, 120, m_Private->m_NameTextWidget);
//    g_Widget->geometryFit(width * 0.5 + 40 + (g_Widget->baseWindowWidth() - width) * 0.5, 120 * 1 + (g_Widget->baseWindowHeight() - height) * 0.5, width * 0.5, 120, m_Private->m_CodeTextWidget);
//    g_Widget->geometryFit(40 + (g_Widget->baseWindowWidth() - width) * 0.5, 120 * 2 + (g_Widget->baseWindowHeight() - height) * 0.5, width, 120, m_Private->m_PhoneStatus);
}

void MirrorMessageDialog::mouseReleaseEvent(QMouseEvent *event)
{
    setVisible(false);
}

bool MirrorMessageDialog::event(QEvent *event)
{
    bool ret(false);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
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

MirrorMessageDialogPrivate::MirrorMessageDialogPrivate(MirrorMessageDialog *parent)
    : m_Parent(parent)
{
    initialize();
    connectAllSlots();
}

MirrorMessageDialogPrivate::~MirrorMessageDialogPrivate()
{
}

void MirrorMessageDialogPrivate::initialize()
{
    m_Background = new BmpWidget(m_Parent);
    m_Background->setBackgroundBmpPath(QString(":/Images/Resources/Images/MessageBackground"));
    m_Background->setVisible(true);
    m_TitleTextWidget = new TextWidget(m_Parent);
    int fontPointSize(20 * g_Widget->widthScalabilityFactor());
    m_TitleTextWidget->setFontPointSize(fontPointSize);
    m_TitleTextWidget->setText(SourceString::Connect_Bluetooth);
    m_TitleTextWidget->setVisible(true);
    m_NameTextWidget = new TextWidget(m_Parent);
    m_NameTextWidget->setFontPointSize(fontPointSize);
    m_NameTextWidget->setLanguageType(TextWidget::T_NoTranslate);
    m_NameTextWidget->setText(QObject::tr(SourceString::Device_Name.toLocal8Bit().constData()));
    m_NameTextWidget->setVisible(true);
    m_CodeTextWidget = new TextWidget(m_Parent);
    m_CodeTextWidget->setFontPointSize(fontPointSize);
    m_CodeTextWidget->setLanguageType(TextWidget::T_NoTranslate);
    m_CodeTextWidget->setText(QObject::tr(SourceString::Device_Code.toLocal8Bit().constData()));
    m_CodeTextWidget->setVisible(true);
    m_PhoneStatus = new TextWidget(m_Parent);
    m_PhoneStatus->setFontPointSize(fontPointSize);
    m_PhoneStatus->setLanguageType(TextWidget::T_NoTranslate);
    m_PhoneStatus->setText(QObject::tr(SourceString::Phone.toLocal8Bit().constData()) + QObject::tr(SourceString::Disconnect.toLocal8Bit().constData()));
    m_PhoneStatus->setVisible(true);
}

void MirrorMessageDialogPrivate::connectAllSlots()
{
}
