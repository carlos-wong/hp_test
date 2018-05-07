#include "BluetoothSettingNumberPadWidget.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QMap>

namespace SourceString {
static const QString Input_4_Character_Pincode(QObject::tr("Input 4 Character Pincode: "));
}

static const unsigned short int alphabetwidth = 97;
static const unsigned short int alphabetheight = 42;
static const unsigned short int alphabetwidthdistance = 12;
static const unsigned short int alphabetheightdistance = 19;

class BluetoothSettingNumberPadWidgetPrivate
{
public:
    explicit BluetoothSettingNumberPadWidgetPrivate(BluetoothSettingNumberPadWidget* parent);
    ~BluetoothSettingNumberPadWidgetPrivate();
    void initialize();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingNumberPadWidget)
    BluetoothSettingNumberPadWidget* const q_ptr;
    BmpButton* m_LineEdit;
    BmpButton* m_PopdownBtn;
    QMap<QString, BmpButton*> m_FirstRow;
    QMap<QString, BmpButton*> m_SecondRow;
    QMap<QString, BmpButton*> m_ThirdRow;
    BmpButton* m_ConfirmBtn;
    BmpButton* m_BackspaceBtn;
};

BluetoothSettingNumberPadWidget::BluetoothSettingNumberPadWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingNumberPadWidgetPrivate(this))
{
}

BluetoothSettingNumberPadWidget::~BluetoothSettingNumberPadWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingNumberPadWidget::setPincode(const QString &pincode)
{
    Q_D(BluetoothSettingNumberPadWidget);
    d->m_LineEdit->setText((QObject::tr(SourceString::Input_4_Character_Pincode.toLocal8Bit().constData())) + pincode);
}

void BluetoothSettingNumberPadWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 240));
}

void BluetoothSettingNumberPadWidget::onReleased()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothSettingNumberPadWidget);
    QString type;
    if (ptr == d->m_PopdownBtn) {
        type = QString("Popdown");
    } else if (ptr == d->m_ConfirmBtn) {
        type = QString("Confirm");
    } else if (ptr == d->m_BackspaceBtn) {
        type = QString("Backspace");
    } else {
        type = ptr->text();
    }
    numberPadKeyType(type);
}


BluetoothSettingNumberPadWidgetPrivate::BluetoothSettingNumberPadWidgetPrivate(BluetoothSettingNumberPadWidget *parent)
    : q_ptr(parent)
{
    m_LineEdit = NULL;
    m_PopdownBtn = NULL;
    m_ConfirmBtn = NULL;
    m_BackspaceBtn = NULL;
    initialize();
}

BluetoothSettingNumberPadWidgetPrivate::~BluetoothSettingNumberPadWidgetPrivate()
{
}

void BluetoothSettingNumberPadWidgetPrivate::initialize()
{
    Q_Q(BluetoothSettingNumberPadWidget);
    m_LineEdit = new BmpButton(q);
    m_LineEdit->setLanguageType(BmpButton::T_NoTranslate);
    m_LineEdit->setText(QObject::tr(SourceString::Input_4_Character_Pincode.toLocal8Bit().constData()));
    m_LineEdit->setDisabled(true);
    m_LineEdit->setNormalBmpPath(QString(":/Images/BluetoothSettingNumberPadLineEditBackground.png"));
    g_Widget->geometryFit(182,
                          106 + (alphabetheight + alphabetheightdistance) * 0,
                          315,
                          alphabetheight,
                          m_LineEdit);
    m_LineEdit->setVisible(true);

    m_PopdownBtn = new BmpButton(q);
    m_PopdownBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingNumberPadPopdownNormal.png"));
    m_PopdownBtn->setPressBmpPath(QString(":/Images/BluetoothSettingNumberPadPopdownNormal.png"));
    g_Widget->geometryFit(509,
                          106 + (alphabetheight + alphabetheightdistance) * 0,
                          alphabetwidth,
                          alphabetheight,
                          m_PopdownBtn);
    m_PopdownBtn->setVisible(true);
    QObject::connect(m_PopdownBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onReleased()));
    QList<QString> alphabet;
    alphabet.clear();
    alphabet.append(QString("7"));
    alphabet.append(QString("8"));
    alphabet.append(QString("9"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingNumberPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingNumberPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(182 + (alphabetwidth + alphabetwidthdistance) * i,
                              125 + (alphabetheight + alphabetheightdistance) * 1,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_FirstRow.insert(alphabet.at(i), ptr);
    }
    m_BackspaceBtn = new BmpButton(q);
    m_BackspaceBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadBackspaceNormal.png"));
    m_BackspaceBtn->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadBackspacePress.png"));
    g_Widget->geometryFit(508,
                          185,
                          alphabetwidth,
                          alphabetheight,
                          m_BackspaceBtn);
    m_BackspaceBtn->setVisible(true);
    QObject::connect(m_BackspaceBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onReleased()));

    alphabet.clear();
    alphabet.append(QString("4"));
    alphabet.append(QString("5"));
    alphabet.append(QString("6"));
    alphabet.append(QString("0"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingNumberPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingNumberPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(182 + (alphabetwidth + alphabetwidthdistance) * i,
                              125 + (alphabetheight + alphabetheightdistance) * 2,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_SecondRow.insert(alphabet.at(i), ptr);
    }
    alphabet.clear();
    alphabet.append(QString("1"));
    alphabet.append(QString("2"));
    alphabet.append(QString("3"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingNumberPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingNumberPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(182 + (alphabetwidth + alphabetwidthdistance) * i,
                              125 + (alphabetheight + alphabetheightdistance) * 3,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_ThirdRow.insert(alphabet.at(i), ptr);
    }
    m_ConfirmBtn = new BmpButton(q);
    m_ConfirmBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadConfirmNormal.png"));
    m_ConfirmBtn->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadConfirmPress.png"));
    g_Widget->geometryFit(508,
                          308,
                          alphabetwidth,
                          alphabetheight,
                          m_ConfirmBtn);
    m_ConfirmBtn->setVisible(true);
    QObject::connect(m_ConfirmBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onReleased()));
}
