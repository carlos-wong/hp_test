#include "RadioListViewWidget.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "Utility.h"
#include "BusinessLogic/Setting.h"
#include "UserInterfaceUtility.h"
#include <QPalette>
#include <QDebug>
#include <QPainter>
#include <QFile>

class RadioListViewWidgetPrivate
{
public:
    explicit RadioListViewWidgetPrivate(RadioListViewWidget* parent);
    ~RadioListViewWidgetPrivate();
    void initializeBasic();
    void initializeParent();
    void connectAllSlots();
    void onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step);
    void onRadioListChange(const QList<unsigned short>& list);
private:
    Q_DECLARE_PUBLIC(RadioListViewWidget)
    RadioListViewWidget* const q_ptr;
    BmpButton* m_FirstItem;
    BmpButton* m_SecondItem;
    BmpButton* m_ThirdItem;
    BmpButton* m_FourItem;
    BmpButton* m_FiveItem;
    BmpButton* m_SixItem;
};

RadioListViewWidget::RadioListViewWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioListViewWidgetPrivate(this))
{
}

RadioListViewWidget::~RadioListViewWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 326, 800, 64, this);
}

void RadioListViewWidget::onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    Q_D(RadioListViewWidget);
    d->onRadioInfoChange(type, frequency, index, min, max, step);
}

void RadioListViewWidget::onRadioListChange(const QList<unsigned short>& list)
{
    Q_D(RadioListViewWidget);
    d->onRadioListChange(list);
    UserInterfaceUtility::elapsed(QString(__PRETTY_FUNCTION__) + QString::number(__LINE__));
}

void RadioListViewWidget::onListViewItemRelease()
{
    Q_D(RadioListViewWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_FirstItem) {
        g_Radio->setCurrentBandIndex(1);
    } else if (ptr == d->m_SecondItem) {
        g_Radio->setCurrentBandIndex(2);
    } else if (ptr == d->m_ThirdItem) {
        g_Radio->setCurrentBandIndex(3);
    } else if (ptr == d->m_FourItem) {
        g_Radio->setCurrentBandIndex(4);
    } else if (ptr == d->m_FiveItem) {
        g_Radio->setCurrentBandIndex(5);
    } else if (ptr == d->m_SixItem) {
        g_Radio->setCurrentBandIndex(6);
    }
}

void RadioListViewWidget::onListViewItemLongPress()
{
    Q_D(RadioListViewWidget);
    BmpButton* ptr = static_cast<BmpButton*>(sender());
    if (ptr == d->m_FirstItem) {
        g_Radio->setCurrentBandFrequencyToIndex(1);
    } else if (ptr == d->m_SecondItem) {
        g_Radio->setCurrentBandFrequencyToIndex(2);
    } else if (ptr == d->m_ThirdItem) {
        g_Radio->setCurrentBandFrequencyToIndex(3);
    } else if (ptr == d->m_FourItem) {
        g_Radio->setCurrentBandFrequencyToIndex(4);
    } else if (ptr == d->m_FiveItem) {
        g_Radio->setCurrentBandFrequencyToIndex(5);
    } else if (ptr == d->m_SixItem) {
        g_Radio->setCurrentBandFrequencyToIndex(6);
    }
}

RadioListViewWidgetPrivate::RadioListViewWidgetPrivate(RadioListViewWidget *parent)
    : q_ptr(parent)
{
    m_FirstItem = NULL;
    m_SecondItem = NULL;
    m_ThirdItem = NULL;
    m_FourItem = NULL;
    m_FiveItem = NULL;
    m_SixItem = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

RadioListViewWidgetPrivate::~RadioListViewWidgetPrivate()
{

}

void RadioListViewWidgetPrivate::initializeBasic()
{
    Q_Q(RadioListViewWidget);
    int width = 120;
    int height = 64;
    QString nomalIamgePath(":/Images/hw_01_radioHZ_normal.png");
    QString pressedIamgePath(":/Images/hw_01_radioHZ_pressed.png");
    m_FirstItem = new BmpButton(q);
    m_FirstItem->setNormalBmpPath(nomalIamgePath);
    m_FirstItem->setPressBmpPath(pressedIamgePath);
    m_FirstItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 0, 0, width, height, m_FirstItem);
    m_FirstItem->setVisible(true);
    m_FirstItem->enableLongPress(true);
    m_FirstItem->enableLongPressRestore(false);
    QObject::connect(m_FirstItem, ARKSENDER(released()),
                     q,           ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_FirstItem, ARKSENDER(longPressed()),
                     q,           ARKRECEIVER(onListViewItemLongPress()));

    m_SecondItem = new BmpButton(q);
    m_SecondItem->setNormalBmpPath(nomalIamgePath);
    m_SecondItem->setPressBmpPath(pressedIamgePath);
    m_SecondItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 1, 0, width, height, m_SecondItem);
    m_SecondItem->setVisible(true);
    m_SecondItem->enableLongPress(true);
    m_SecondItem->enableLongPressRestore(false);
    QObject::connect(m_SecondItem, ARKSENDER(released()),
                     q,            ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_SecondItem, ARKSENDER(longPressed()),
                     q,            ARKRECEIVER(onListViewItemLongPress()));

    m_ThirdItem = new BmpButton(q);
    m_ThirdItem->setNormalBmpPath(nomalIamgePath);
    m_ThirdItem->setPressBmpPath(pressedIamgePath);
    m_ThirdItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 2, 0, width, height, m_ThirdItem);
    m_ThirdItem->setVisible(true);
    m_ThirdItem->enableLongPress(true);
    m_ThirdItem->enableLongPressRestore(false);
    QObject::connect(m_ThirdItem, ARKSENDER(released()),
                     q,           ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_ThirdItem, ARKSENDER(longPressed()),
                     q,           ARKRECEIVER(onListViewItemLongPress()));

    m_FourItem = new BmpButton(q);
    m_FourItem->setNormalBmpPath(nomalIamgePath);
    m_FourItem->setPressBmpPath(pressedIamgePath);
    m_FourItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 3, 0, width, height, m_FourItem);
    m_FourItem->setVisible(true);
    m_FourItem->enableLongPress(true);
    m_FourItem->enableLongPressRestore(false);
    QObject::connect(m_FourItem, ARKSENDER(released()),
                     q,          ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_FourItem, ARKSENDER(longPressed()),
                     q,          ARKRECEIVER(onListViewItemLongPress()));

    m_FiveItem = new BmpButton(q);
    m_FiveItem->setNormalBmpPath(nomalIamgePath);
    m_FiveItem->setPressBmpPath(pressedIamgePath);
    m_FiveItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 4, 0, width, height, m_FiveItem);
    m_FiveItem->setVisible(true);
    m_FiveItem->enableLongPress(true);
    m_FiveItem->enableLongPressRestore(false);
    QObject::connect(m_FiveItem, ARKSENDER(released()),
                     q,          ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_FiveItem, ARKSENDER(longPressed()),
                     q,          ARKRECEIVER(onListViewItemLongPress()));

    m_SixItem = new BmpButton(q);
    m_SixItem->setNormalBmpPath(nomalIamgePath);
    m_SixItem->setPressBmpPath(pressedIamgePath);
    m_SixItem->setCheckBmpPath(pressedIamgePath);
    g_Widget->geometryFit(11 + (11 + 120) * 5, 0, width, height, m_SixItem);
    m_SixItem->setVisible(true);
    m_SixItem->enableLongPress(true);
    m_SixItem->enableLongPressRestore(false);
    QObject::connect(m_SixItem, ARKSENDER(released()),
                     q,         ARKRECEIVER(onListViewItemRelease()));
    QObject::connect(m_SixItem, ARKSENDER(longPressed()),
                     q,         ARKRECEIVER(onListViewItemLongPress()));
    onRadioListChange(g_Radio->getCurrentBandList());
    onRadioInfoChange(g_Radio->getCurrentBand(), g_Radio->getCurrendBandFrequency(), g_Radio->getCurrendBandIndex(), g_Radio->getCurrentBandMin(), g_Radio->getCurrentBandMax(), g_Radio->getCurrentBandStep());
}

void RadioListViewWidgetPrivate::initializeParent()
{
    Q_Q(RadioListViewWidget);
    q->setVisible(true);
}

void RadioListViewWidgetPrivate::connectAllSlots()
{
    Q_Q(RadioListViewWidget);
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioInfoChange(const RadioBandType , const unsigned short , const unsigned short , const unsigned short , const unsigned short , const unsigned short)));
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioListChange(const QList<unsigned short>&)));
}

void RadioListViewWidgetPrivate::onRadioInfoChange(const RadioBandType type, const unsigned short frequency, const unsigned short index, const unsigned short min, const unsigned short max, const unsigned short step)
{
    bool ret = false;
    QString freq = QString::number(frequency);
    switch (g_Radio->getCurrentBand()) {
    case RBT_FM1:
    case RBT_FM2:
    case RBT_FM3: {
        QString integer("%1");
        integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
        QString decimal("%1");
        decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
        freq = integer + QString(".") + decimal;
        break;
    }
    default: {
        break;
    }
    }
    switch (index) {
    case 1: {
        ret = (freq == m_FirstItem->text());
        break;
    }
    case 2: {
        ret = (freq == m_SecondItem->text());
        break;
    }
    case 3: {
        ret = (freq == m_ThirdItem->text());
        break;
    }
    case 4: {
        ret = (freq == m_FourItem->text());
        break;
    }
    case 5: {
        ret = (freq == m_FiveItem->text());
        break;
    }
    case 6: {
        ret = (freq == m_SixItem->text());
        break;
    }
    default: {
        break;
    }
    }
    switch (g_Radio->getWorkType()) {
    case RWT_Idle:
    case RWT_BrowseStore: {
        if (ret) {
            m_FirstItem->setStatus((1 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            m_SecondItem->setStatus((2 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            m_ThirdItem->setStatus((3 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            m_FourItem->setStatus((4 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            m_FiveItem->setStatus((5 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            m_SixItem->setStatus((6 == index)? BmpButton::B_Check: BmpButton::B_Normal);
            return ;
        }
        BmpButton* ptr = NULL;
        if ((NULL == ptr)
                && (freq == m_FirstItem->text())) {
            ptr = m_FirstItem;
        }
        m_FirstItem->setStatus((ptr == m_FirstItem)? BmpButton::B_Check: BmpButton::B_Normal);
        if ((NULL == ptr)
                && (freq == m_SecondItem->text())) {
            ptr = m_SecondItem;
        }
        m_SecondItem->setStatus((ptr == m_SecondItem)? BmpButton::B_Check: BmpButton::B_Normal);
        if ((NULL == ptr)
                && (freq == m_ThirdItem->text())) {
            ptr = m_ThirdItem;
        }
        m_ThirdItem->setStatus((ptr == m_ThirdItem)? BmpButton::B_Check: BmpButton::B_Normal);
        if ((NULL == ptr)
                && (freq == m_FourItem->text())) {
            ptr = m_FourItem;
        }
        m_FourItem->setStatus((ptr == m_FourItem)? BmpButton::B_Check: BmpButton::B_Normal);
        if ((NULL == ptr)
                && (freq == m_FiveItem->text())) {
            ptr = m_FiveItem;
        }
        m_FiveItem->setStatus((ptr == m_FiveItem)? BmpButton::B_Check: BmpButton::B_Normal);
        if ((NULL == ptr)
                && (freq == m_SixItem->text())) {
            ptr = m_SixItem;
        }
        m_SixItem->setStatus((ptr == m_SixItem)? BmpButton::B_Check: BmpButton::B_Normal);
        break;
    }
    case RWT_SeekUp:
    case RWT_SeekDown: {
        m_FirstItem->setStatus(BmpButton::B_Normal);
        m_SecondItem->setStatus(BmpButton::B_Normal);
        m_ThirdItem->setStatus(BmpButton::B_Normal);
        m_FourItem->setStatus(BmpButton::B_Normal);
        m_FiveItem->setStatus(BmpButton::B_Normal);
        m_SixItem->setStatus(BmpButton::B_Normal);
        break;
    }
    case RWT_AutoStore: {
        m_FirstItem->setStatus((1 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        m_SecondItem->setStatus((2 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        m_ThirdItem->setStatus((3 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        m_FourItem->setStatus((4 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        m_FiveItem->setStatus((5 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        m_SixItem->setStatus((6 == index)? BmpButton::B_Check: BmpButton::B_Normal);
        break;
    }
    default: {
        break;
    }
    }
}

void RadioListViewWidgetPrivate::onRadioListChange(const QList<unsigned short> &list)
{
    RadioBandType band = g_Radio->getCurrentBand();
    qDebug() << __PRETTY_FUNCTION__ << __LINE__ << band;
    QString frequencyStr;
    unsigned short frequency;
    if (list.size() > 0) {
        frequency = list.at(0);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_FirstItem->setText(frequencyStr);
    }
    if (list.size() > 1) {
        frequency = list.at(1);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_SecondItem->setText(frequencyStr);
    }
    if (list.size() > 2) {
        frequency = list.at(2);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_ThirdItem->setText(frequencyStr);
    }
    if (list.size() > 3) {
        frequency = list.at(3);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_FourItem->setText(frequencyStr);
    }
    if (list.size() > 4) {
        frequency = list.at(4);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_FiveItem->setText(frequencyStr);
    }
    if (list.size() > 5) {
        frequency = list.at(5);
        switch (band) {
        case RBT_FM1:
        case RBT_FM2:
        case RBT_FM3: {
            QString integer("%1");
            integer = integer.arg(frequency / 100, 2, 10, QChar('0'));
            QString decimal("%1");
            decimal = decimal.arg(frequency % 100, 2, 10, QChar('0'));
            frequencyStr = integer + QString(".") + decimal;
            break;
        }
        default: {
            frequencyStr = QString::number(frequency);
            break;
        }
        }
        m_SixItem->setText(frequencyStr);
    }
}
