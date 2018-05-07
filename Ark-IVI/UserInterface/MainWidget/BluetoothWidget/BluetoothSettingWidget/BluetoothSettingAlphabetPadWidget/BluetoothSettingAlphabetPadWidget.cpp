#include "BluetoothSettingAlphabetPadWidget.h"
#include "BusinessLogic/Widget.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QMap>

namespace SourceString {
static const QString Input_Device_Name(QObject::tr("Input Device Name: "));
}

static const unsigned short int alphabetwidth = 63;
static const unsigned short int alphabetheight = 42;
static const unsigned short int alphabetwidthdistance = 6;
static const unsigned short int alphabetheightdistance = 19;

class BluetoothSettingAlphabetPadWidgetPrivate
{
public:
    explicit BluetoothSettingAlphabetPadWidgetPrivate(BluetoothSettingAlphabetPadWidget* parent);
    ~BluetoothSettingAlphabetPadWidgetPrivate();
    void initialize();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingAlphabetPadWidget)
    BluetoothSettingAlphabetPadWidget* const q_ptr;
    BmpButton* m_LineEdit;
    BmpButton* m_PopdownBtn;
    QMap<QString, BmpButton*> m_FirstRow;
    QMap<QString, BmpButton*> m_SecondRow;
    QMap<QString, BmpButton*> m_ThirdRow;
    BmpButton* m_ConfirmBtn;
    BmpButton* m_BackspaceBtn;
};

BluetoothSettingAlphabetPadWidget::BluetoothSettingAlphabetPadWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingAlphabetPadWidgetPrivate(this))
{
}

BluetoothSettingAlphabetPadWidget::~BluetoothSettingAlphabetPadWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingAlphabetPadWidget::setDeviceName(const QString &name)
{
    Q_D(BluetoothSettingAlphabetPadWidget);
    d->m_LineEdit->setText((QObject::tr(SourceString::Input_Device_Name.toLocal8Bit().constData())) + name);
}

void BluetoothSettingAlphabetPadWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 240));
}

void BluetoothSettingAlphabetPadWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void BluetoothSettingAlphabetPadWidget::onReleased()
{
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    Q_D(BluetoothSettingAlphabetPadWidget);
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
    alphabetPadKeyType(type);
}

BluetoothSettingAlphabetPadWidgetPrivate::BluetoothSettingAlphabetPadWidgetPrivate(BluetoothSettingAlphabetPadWidget *parent)
    : q_ptr(parent)
{
    m_LineEdit = NULL;
    m_PopdownBtn = NULL;
    m_ConfirmBtn = NULL;
    m_BackspaceBtn = NULL;
    initialize();
}

BluetoothSettingAlphabetPadWidgetPrivate::~BluetoothSettingAlphabetPadWidgetPrivate()
{
}

void BluetoothSettingAlphabetPadWidgetPrivate::initialize()
{
    Q_Q(BluetoothSettingAlphabetPadWidget);
    m_FirstRow.clear();
    m_LineEdit = new BmpButton(q);
    m_LineEdit->setLanguageType(BmpButton::T_NoTranslate);
    m_LineEdit->setText(QObject::tr(SourceString::Input_Device_Name.toLocal8Bit().constData()));
    m_LineEdit->setDisabled(true);
    m_LineEdit->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadLineEditBackground.png"));
    g_Widget->geometryFit(58,
                          106 + (alphabetheight + alphabetheightdistance) * 0,
                          615,
                          alphabetheight,
                          m_LineEdit);
    m_LineEdit->setVisible(true);

    m_PopdownBtn = new BmpButton(q);
    m_PopdownBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadPopdownNormal.png"));
    m_PopdownBtn->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadPopdownNormal.png"));
    g_Widget->geometryFit(679,
                          106 + (alphabetheight + alphabetheightdistance) * 0,
                          alphabetwidth,
                          alphabetheight,
                          m_PopdownBtn);
    m_PopdownBtn->setVisible(true);
    QObject::connect(m_PopdownBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onReleased()));
    QList<QString> alphabet;
    alphabet.clear();
    alphabet.append(QString("Q"));
    alphabet.append(QString("W"));
    alphabet.append(QString("E"));
    alphabet.append(QString("R"));
    alphabet.append(QString("T"));
    alphabet.append(QString("Y"));
    alphabet.append(QString("U"));
    alphabet.append(QString("I"));
    alphabet.append(QString("O"));
    alphabet.append(QString("P"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(58 + (alphabetwidth + alphabetwidthdistance) * i,
                              125 + (alphabetheight + alphabetheightdistance) * 1,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_FirstRow.insert(alphabet.at(i), ptr);
    }
    alphabet.clear();
    alphabet.append(QString("A"));
    alphabet.append(QString("S"));
    alphabet.append(QString("D"));
    alphabet.append(QString("F"));
    alphabet.append(QString("G"));
    alphabet.append(QString("H"));
    alphabet.append(QString("J"));
    alphabet.append(QString("K"));
    alphabet.append(QString("L"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(92 + (alphabetwidth + alphabetwidthdistance) * i,
                              125 + (alphabetheight + alphabetheightdistance) * 2,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_SecondRow.insert(alphabet.at(i), ptr);
    }
    m_ConfirmBtn = new BmpButton(q);
    m_ConfirmBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadConfirmNormal.png"));
    m_ConfirmBtn->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadConfirmPress.png"));
    g_Widget->geometryFit(58,
                          125 + (alphabetheight + alphabetheightdistance) * 3,
                          98,
                          alphabetheight,
                          m_ConfirmBtn);
    m_ConfirmBtn->setVisible(true);
    QObject::connect(m_ConfirmBtn, ARKSENDER(released()),
                     q,            ARKRECEIVER(onReleased()));
    alphabet.clear();
    alphabet.append(QString("Z"));
    alphabet.append(QString("X"));
    alphabet.append(QString("C"));
    alphabet.append(QString("V"));
    alphabet.append(QString("B"));
    alphabet.append(QString("N"));
    alphabet.append(QString("M"));
    for (int i = 0; i < alphabet.count(); ++i) {
        BmpButton* ptr = new BmpButton(q);
        ptr->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadNormal.png"));
        ptr->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadPress.png"));
        ptr->setText(alphabet.at(i));
        g_Widget->geometryFit(92 + (alphabetwidth + alphabetwidthdistance) * (i + 1),
                              125 + (alphabetheight + alphabetheightdistance) * 3,
                              alphabetwidth,
                              alphabetheight,
                              ptr);
        ptr->setVisible(true);
        QObject::connect(ptr, ARKSENDER(released()),
                         q,   ARKRECEIVER(onReleased()));
        m_ThirdRow.insert(alphabet.at(i), ptr);
    }
    m_BackspaceBtn = new BmpButton(q);
    m_BackspaceBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingAlphabetPadBackspaceNormal.png"));
    m_BackspaceBtn->setPressBmpPath(QString(":/Images/BluetoothSettingAlphabetPadBackspacePress.png"));
    g_Widget->geometryFit(644,
                          125 + (alphabetheight + alphabetheightdistance) * 3,
                          98,
                          alphabetheight,
                          m_BackspaceBtn);
    QObject::connect(m_BackspaceBtn, ARKSENDER(released()),
                     q,              ARKRECEIVER(onReleased()));
    m_BackspaceBtn->setVisible(true);
}
