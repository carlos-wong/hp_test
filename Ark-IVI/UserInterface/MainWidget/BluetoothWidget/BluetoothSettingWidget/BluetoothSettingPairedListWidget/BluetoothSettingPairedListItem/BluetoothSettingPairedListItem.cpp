#include "BluetoothSettingPairedListItem.h"
#include "BmpButton.h"
#include "BusinessLogic/Widget.h"
#include "BmpWidget.h"
#include "AutoConnect.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include <QFontMetrics>

class BluetoothSettingPairedListItemPrivate
{
public:
    explicit BluetoothSettingPairedListItemPrivate(BluetoothSettingPairedListItem* parent);
    ~BluetoothSettingPairedListItemPrivate();
    void initializeBasicWidget();
private:
    Q_DECLARE_PUBLIC(BluetoothSettingPairedListItem)
    BluetoothSettingPairedListItem* q_ptr;
    BmpButton* m_DeleteBtn;
    QColor m_Color;
    QString m_Text;
    BmpWidget* m_Background;
    bool m_Restore;
};

BluetoothSettingPairedListItem::BluetoothSettingPairedListItem(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new BluetoothSettingPairedListItemPrivate(this))
{
}

BluetoothSettingPairedListItem::~BluetoothSettingPairedListItem()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void BluetoothSettingPairedListItem::setDeleteVisible(const bool flag)
{
    Q_D(BluetoothSettingPairedListItem);
    if (NULL != d->m_DeleteBtn) {
        d->m_DeleteBtn->setVisible(flag);
    }
}

void BluetoothSettingPairedListItem::setItemText(const QString &text)
{
    Q_D(BluetoothSettingPairedListItem);
    d->m_Text = text;
    if (isVisible()) {
        update();
    }
}

void BluetoothSettingPairedListItem::setEnabled(bool flag)
{
    Q_D(BluetoothSettingPairedListItem);
    setEnabled(flag);
    d->m_DeleteBtn->setVisible(flag);
}

void BluetoothSettingPairedListItem::resizeEvent(QResizeEvent *event)
{
    Q_D(BluetoothSettingPairedListItem);
    d->m_Background->move(0, height() - 2);
    d->m_Background->resize(width(), 2);
}

void BluetoothSettingPairedListItem::mousePressEvent(QMouseEvent *event)
{
    Q_D(BluetoothSettingPairedListItem);
    if (d->m_DeleteBtn->isVisible()) {
        if (g_Widget->geometryFit(0, 0, 219, 65).contains(event->pos())) {
            d->m_Restore = true;
            d->m_Color = QColor(0, 162, 255, 191);
            if (isVisible()) {
                update();
            }
        }
    }
}

void BluetoothSettingPairedListItem::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(BluetoothSettingPairedListItem);
    if (d->m_Restore) {
        d->m_Restore = false;
        d->m_Color = QColor(255, 255, 255, 255);
        if (isVisible()) {
            update();
        }
        connectBtnRelease();
    }
}

void BluetoothSettingPairedListItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    Q_D(BluetoothSettingPairedListItem);
    painter.setPen(d->m_Color);
    QFontMetrics fontMetrics(painter.font());
    d->m_Text = fontMetrics.elidedText(d->m_Text, Qt::ElideRight, 219 * g_Widget->widthScalabilityFactor());
    painter.drawText(g_Widget->geometryFit(0, 0, 219, 65), Qt::AlignCenter, d->m_Text);
}

BluetoothSettingPairedListItemPrivate::BluetoothSettingPairedListItemPrivate(BluetoothSettingPairedListItem *parent)
    : q_ptr(parent)
{
    m_DeleteBtn = NULL;
    m_Color = QColor(255, 255, 255, 255);
    m_Background = NULL;
    m_Restore = false;
    initializeBasicWidget();
}

BluetoothSettingPairedListItemPrivate::~BluetoothSettingPairedListItemPrivate()
{
}

void BluetoothSettingPairedListItemPrivate::initializeBasicWidget()
{
    Q_Q(BluetoothSettingPairedListItem);
    m_Background = new BmpWidget(q);
    m_Background->setBackgroundBmpPath(QString(":/Images/MultimediaListViewRules.png"));
    m_Background->setVisible(true);
    m_DeleteBtn = new BmpButton(q);
    m_DeleteBtn->setNormalBmpPath(QString(":/Images/BluetoothSettingPairedListDeleteItemNormal.png"));
    m_DeleteBtn->setPressBmpPath(QString(":/Images/BluetoothSettingPairedListDeleteItemPress.png"));
    m_DeleteBtn->setCheckBmpPath(QString(":/Images/BluetoothSettingPairedListDeleteItemPress.png"));
    g_Widget->geometryFit(219, 0, 81, 65, m_DeleteBtn);
    m_DeleteBtn->setVisible(true);
    QObject::connect(m_DeleteBtn, ARKSENDER(released()),
                     q,           ARKSENDER(deleteBtnRelease()));
}
