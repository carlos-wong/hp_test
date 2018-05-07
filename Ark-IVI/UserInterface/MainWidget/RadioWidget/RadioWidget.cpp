#include "RadioWidget.h"
#include "AutoConnect.h"
#include "RadioSelectorWidget/RadioSelectorWidget.h"
#include "RadioUtilityWidget/RadioUtilityWidget.h"
#include "RadioListViewWidget/RadioListViewWidget.h"
#include "RadioListViewWidget/RadioListViewWidget.h"
#include "RadioToolWidget/RadioToolWidget.h"
#include "MessageBox.h"
#include <QMouseEvent>
#include <QDebug>
#include "BmpWidget.h"

class RadioWidgetPrivate
{
    Q_DISABLE_COPY(RadioWidgetPrivate)
public:
    explicit RadioWidgetPrivate(RadioWidget* parent);
    ~RadioWidgetPrivate();
    void initializeParent();
    void connectAllSlots();
    void initializeRadioSelectorWidget();
    void initializeRadioUtilityWidget();
    void initializeRadioListViewWidget();
    void initializeRadioToolWidget();
    void initialBackPanel();
private:
    Q_DECLARE_PUBLIC(RadioWidget)
    RadioWidget* const q_ptr;
    RadioSelectorWidget* m_RadioSelectorWidget;
    RadioUtilityWidget* m_RadioUtilityWidget;
    RadioListViewWidget* m_RadioListViewWidget;
    RadioToolWidget* m_RadioToolWidget;
    unsigned char m_Counter;
    TestControl* m_FMSmeter;
    TestControl* m_FMAdjchannel;
    TestControl* m_FMMultipath;
    TestControl* m_FMDetuning;
    TestControl* m_AMSmeter;
    TestControl* m_AMDetuning;
    BmpWidget* m_BackPanel;
};

RadioWidget::RadioWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new RadioWidgetPrivate(this))
{

}

RadioWidget::~RadioWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void RadioWidget::makeSureComplete()
{
    if (g_Radio->isStartComplete()) {
        Q_D(RadioWidget);
        d->initializeRadioSelectorWidget();
        d->initializeRadioUtilityWidget();
        d->initializeRadioListViewWidget();
        d->initializeRadioToolWidget();
        QObject::connect(d->m_RadioSelectorWidget, ARKSENDER(sliderMoved(int)),
                         d->m_RadioUtilityWidget,  ARKRECEIVER(onSliderMoved(int)));
        d->initializeParent();
        d->connectAllSlots();
    }
}

void RadioWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void RadioWidget::mousePressEvent(QMouseEvent *event)
{
    Q_D(RadioWidget);
    if (QString(qgetenv("PROTOCOL_ID")).contains(QString("ark169")) && !qgetenv("RADIO_DEBUG").isEmpty()) {
        if (g_Widget->geometryFit(134, 122, 134, 225).contains(event->pos())) {
            static QTime time;
            static bool flag = true;
            if (flag) {
                flag = false;
                time.start();
            }
            if (time.restart() > 500) {
                d->m_Counter = 0;
            }
            if (d->m_Counter++ > 5) {
                if (NULL == d->m_FMSmeter) {
                    d->m_FMSmeter = new TestControl(this);
                    d->m_FMSmeter->setTitle(QString("FMSmeter"));
                    d->m_FMSmeter->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 0, 140, 40);
                    d->m_FMSmeter->setVisible(true);
                    connect(d->m_FMSmeter, ARKSENDER(minus()),
                            this,        ARKRECEIVER(onMinus()));
                    connect(d->m_FMSmeter, ARKSENDER(plus()),
                            this,         ARKRECEIVER(onPlus()));
                }
                if (NULL == d->m_FMAdjchannel) {
                    d->m_FMAdjchannel = new TestControl(this);
                    d->m_FMAdjchannel->setTitle(QString("FMAdjchannel"));
                    d->m_FMAdjchannel->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 1, 140, 40);
                    d->m_FMAdjchannel->setVisible(true);
                    connect(d->m_FMAdjchannel, ARKSENDER(minus()),
                            this,              ARKRECEIVER(onMinus()));
                    connect(d->m_FMAdjchannel, ARKSENDER(plus()),
                            this,              ARKRECEIVER(onPlus()));
                }
                if (NULL == d->m_FMMultipath) {
                    d->m_FMMultipath = new TestControl(this);
                    d->m_FMMultipath->setTitle(QString("FMMultipath"));
                    d->m_FMMultipath->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 2, 140, 40);
                    d->m_FMMultipath->setVisible(true);
                    connect(d->m_FMMultipath, ARKSENDER(minus()),
                            this,             ARKRECEIVER(onMinus()));
                    connect(d->m_FMMultipath, ARKSENDER(plus()),
                            this,             ARKRECEIVER(onPlus()));
                }
                if (NULL == d->m_FMDetuning) {
                    d->m_FMDetuning = new TestControl(this);
                    d->m_FMDetuning->setTitle(QString("FMDetuning"));
                    d->m_FMDetuning->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 3, 140, 40);
                    d->m_FMDetuning->setVisible(true);
                    connect(d->m_FMDetuning, ARKSENDER(minus()),
                            this,            ARKRECEIVER(onMinus()));
                    connect(d->m_FMDetuning, ARKSENDER(plus()),
                            this,            ARKRECEIVER(onPlus()));
                }
                if (NULL == d->m_AMSmeter) {
                    d->m_AMSmeter = new TestControl(this);
                    d->m_AMSmeter->setTitle(QString("AMSmeter"));
                    d->m_AMSmeter->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 4, 140, 40);
                    d->m_AMSmeter->setVisible(true);
                    connect(d->m_AMSmeter, ARKSENDER(minus()),
                            this,          ARKRECEIVER(onMinus()));
                    connect(d->m_AMSmeter, ARKSENDER(plus()),
                            this,          ARKRECEIVER(onPlus()));
                }
                if (NULL == d->m_AMDetuning) {
                    d->m_AMDetuning = new TestControl(this);
                    d->m_AMDetuning->setTitle(QString("AMDetuning"));
                    d->m_AMDetuning->setGeometry(134 * g_Widget->widthScalabilityFactor(), 122 + 45 * 5, 140, 40);
                    d->m_AMDetuning->setVisible(true);
                    connect(d->m_AMDetuning, ARKSENDER(minus()),
                            this,          ARKRECEIVER(onMinus()));
                    connect(d->m_AMDetuning, ARKSENDER(plus()),
                            this,          ARKRECEIVER(onPlus()));
                }
                QByteArray byteArray = g_Radio->getRadioSmeter();
                if (6 == byteArray.length()) {
                    d->m_FMSmeter->setValue(byteArray.at(0));
                    d->m_FMAdjchannel->setValue(byteArray.at(1));
                    d->m_FMMultipath->setValue(byteArray.at(2));
                    d->m_FMDetuning->setValue(byteArray.at(3));
                    d->m_AMSmeter->setValue(byteArray.at(4));
                    d->m_AMDetuning->setValue(byteArray.at(5));
                }
            }
        }
    }
}

void RadioWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    Q_D(RadioWidget);
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Radio, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_Radio: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(Widget::T_Radio, Widget::T_Radio, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            if (!isVisible()) {
                setVisible(true);
            }
            d->m_Counter = 0;
            g_Radio->initialize();
        }
        break;
    }
    default: {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                setVisible(false);
            }
        }
        break;
    }
    }
}

void RadioWidget::onRadioSmeter(const QByteArray &byteArray)
{
    Q_D(RadioWidget);
    if (6 == byteArray.length()) {
        if (NULL != d->m_FMSmeter) {
            d->m_FMSmeter->setValue(byteArray.at(0));
        }
        if(NULL != d->m_FMAdjchannel) {
            d->m_FMAdjchannel->setValue(byteArray.at(1));
        }
        if (NULL != d->m_FMMultipath) {
            d->m_FMMultipath->setValue(byteArray.at(2));
        }
        if (NULL != d->m_FMDetuning) {
            d->m_FMDetuning->setValue(byteArray.at(3));
        }
        if (NULL != d->m_AMSmeter) {
            d->m_AMSmeter->setValue(byteArray.at(4));
        }
        if (NULL != d->m_AMDetuning) {
            d->m_AMDetuning->setValue(byteArray.at(5));
        }
    }
}

void RadioWidget::onMinus()
{
    Q_D(RadioWidget);
    TestControl* ptr = static_cast<TestControl*>(sender());
    QByteArray byteArray = g_Radio->getRadioSmeter();
    if (byteArray.length() == 6) {
        int index = 0;
        if (ptr == d->m_FMSmeter) {
            index = 0;
        } else if (ptr == d->m_FMAdjchannel) {
            index = 1;
        } else if (ptr == d->m_FMMultipath) {
            index = 2;
        } else if (ptr == d->m_FMDetuning) {
            index = 3;
        } else if (ptr == d->m_AMSmeter) {
            index = 4;
        } else if (ptr == d->m_AMDetuning) {
            index = 5;
        }
        byteArray[index] = byteArray.at(index) - 1;
        g_Radio->setRadioSmeter(byteArray);
    }
}

void RadioWidget::onPlus()
{
    Q_D(RadioWidget);
    TestControl* ptr = static_cast<TestControl*>(sender());
    QByteArray byteArray = g_Radio->getRadioSmeter();
    if (byteArray.length() == 6) {
        int index = 0;
        if (ptr == d->m_FMSmeter) {
            index = 0;
        } else if (ptr == d->m_FMAdjchannel) {
            index = 1;
        } else if (ptr == d->m_FMMultipath) {
            index = 2;
        } else if (ptr == d->m_FMDetuning) {
            index = 3;
        } else if (ptr == d->m_AMSmeter) {
            index = 4;
        } else if (ptr == d->m_AMDetuning) {
            index = 5;
        }
        byteArray[index] = byteArray.at(index) + 1;
        g_Radio->setRadioSmeter(byteArray);
    }
}

RadioWidgetPrivate::RadioWidgetPrivate(RadioWidget *parent)
    : q_ptr(parent)
{
    m_RadioSelectorWidget = NULL;
    m_RadioUtilityWidget = NULL;
    m_RadioListViewWidget = NULL;
    m_RadioToolWidget = NULL;
    m_Counter = 0;
    m_FMSmeter = NULL;
    m_FMAdjchannel = NULL;
    m_FMMultipath = NULL;
    m_FMDetuning = NULL;
    m_AMSmeter = NULL;
    m_AMDetuning = NULL;
    m_BackPanel = NULL;

    bool isOk = g_Radio->isStartComplete();
#ifdef DESKTOP_AMD64
    isOk = true;
#endif
    if (isOk) {
        initialBackPanel();
        initializeRadioSelectorWidget();
        initializeRadioUtilityWidget();
        initializeRadioListViewWidget();
        initializeRadioToolWidget();
        QObject::connect(m_RadioSelectorWidget, ARKSENDER(sliderMoved(int)),
                         m_RadioUtilityWidget,  ARKRECEIVER(onSliderMoved(int)));
        initializeParent();
        connectAllSlots();
    }
}

RadioWidgetPrivate::~RadioWidgetPrivate()
{
}

void RadioWidgetPrivate::initializeParent()
{
    Q_Q(RadioWidget);
    q->setVisible(true);
}

void RadioWidgetPrivate::connectAllSlots()
{
    Q_Q(RadioWidget);
    connectSignalAndSlotByNamesake(g_Widget, q, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Radio, q, ARKRECEIVER(onRadioSmeter(const QByteArray&)));
}

void RadioWidgetPrivate::initializeRadioSelectorWidget()
{
    if (NULL == m_RadioSelectorWidget) {
        Q_Q(RadioWidget);
        m_RadioSelectorWidget = new RadioSelectorWidget(q);
    }
}

void RadioWidgetPrivate::initializeRadioUtilityWidget()
{
    if (NULL == m_RadioUtilityWidget) {
        Q_Q(RadioWidget);
        m_RadioUtilityWidget = new RadioUtilityWidget(q);
    }
}

void RadioWidgetPrivate::initializeRadioListViewWidget()
{
    if (NULL == m_RadioListViewWidget) {
        Q_Q(RadioWidget);
        m_RadioListViewWidget = new RadioListViewWidget(q);
    }
}

void RadioWidgetPrivate::initializeRadioToolWidget()
{
    if (NULL == m_RadioToolWidget) {
        Q_Q(RadioWidget);
        m_RadioToolWidget = new RadioToolWidget(q);
    }
}

void RadioWidgetPrivate::initialBackPanel()
{
    if (NULL == m_BackPanel) {
        Q_Q(RadioWidget);
        m_BackPanel = new BmpWidget(q);
        m_BackPanel->setBackgroundBmpPath(QString(":/Images/hw_01_main_background.png"));
        g_Widget->geometryFit(0, 0, 800, 480, m_BackPanel);
        m_BackPanel->setVisible(true);
    }
}


TestControl::TestControl(QWidget *parent)
    : QWidget(parent)
{
    m_Title = NULL;
    m_Minus = NULL;
    m_Value = NULL;
    m_Plus = NULL;
    m_Title = new QPushButton(this);
    m_Title->setFont(QFont(QString(), 8));
    m_Title->setGeometry(0, 0, 140, 15);
    m_Title->setVisible(true);
    m_Minus = new QPushButton(this);
    m_Minus->setGeometry(0, 15, 40, 25);
    m_Minus->setText(QString("-"));
    m_Minus->setVisible(true);
    m_Value = new QPushButton(this);
    m_Value->setGeometry(40, 15, 60, 25);
    m_Value->setText(QString("0"));
    m_Value->setVisible(true);
    m_Plus = new QPushButton(this);
    m_Plus->setGeometry(100, 15, 40, 25);
    m_Plus->setText(QString("+"));
    m_Plus->setVisible(true);
    connect(m_Minus, ARKSENDER(released()),
            this,    ARKSENDER(minus()));
    connect(m_Plus, ARKSENDER(released()),
            this,   ARKSENDER(plus()));
}

TestControl::~TestControl()
{
}

void TestControl::setValue(const unsigned char value)
{
    m_Value->setText(QString::number(value));
}

void TestControl::setTitle(const QString &title)
{
    m_Title->setText(title);
}
