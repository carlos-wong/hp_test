#include "USBDiskImageListViewWidget.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MultimediaListView.h"
#include "BmpButton.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "UserInterface/MainWidget/DiskWidget/USBDiskWidget/USBDiskListViewWidget/USBDiskListViewWidget.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QDomDocument>

namespace SourceString {
static const QString No_Image_Resource(QObject::tr("No Image Resource..."));
}

class USBDiskImageListViewWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskImageListViewWidgetPrivate)
public:
    explicit USBDiskImageListViewWidgetPrivate(USBDiskImageListViewWidget* parent);
    ~USBDiskImageListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_ImageListView;
    MessageBox* m_USBDiskImageMessageBox;
    int m_LastIndex;
    bool m_Multimedia;
private:
    USBDiskImageListViewWidget* m_Parent;
};

USBDiskImageListViewWidget::USBDiskImageListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskImageListViewWidgetPrivate(this))
{
    setVisible(false);
}

USBDiskImageListViewWidget::~USBDiskImageListViewWidget()
{
}

void USBDiskImageListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_ImageListView);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_USBDiskImageMessageBox);
}

void USBDiskImageListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_USBDisk, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_USBDiskImage: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_USBDiskImage, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (-1 != m_Private->m_LastIndex) {
                    g_Multimedia->imagePlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_LastIndex);
                    g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_USBDiskImage, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_USBDiskImage, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskMusic:
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::Show == status) {
            setVisible(false);
        }
        break;
    }
    default: {
        break;
    }
    }
}

void USBDiskImageListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "USBDiskImageListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_USBDiskImageMessageBox->setVisible(false);
            m_Private->m_ImageListView->clearListView();
            m_Private->m_ImageListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_USBDiskImageMessageBox->setVisible(false);
            m_Private->m_ImageListView->setVisible(false);
            break;
        }
        case DWS_Ready: {
            break;
        }
        case DWS_Remove: {
            m_Private->m_LastIndex = -1;
            m_Private->m_USBDiskImageMessageBox->setVisible(false);
            m_Private->m_ImageListView->clearListView();
            m_Private->m_ImageListView->setVisible(false);
            setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void USBDiskImageListViewWidget::onImagePlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("ImagePlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            m_Private->m_ImageListView->clearListView();
                            for (int i = 0; i < nodeList.size(); ++i) {
                                QDomNode node = nodeList.at(i);
                                if (node.isElement()) {
                                    if (!node.toElement().isNull()) {
                                        if (node.isElement()) {
                                            m_Private->m_ImageListView->appendListView(node.toElement().text());
                                        }
                                    }
                                }
                            }
                            if (0 == nodeList.length()) {
                                m_Private->m_USBDiskImageMessageBox->setText(SourceString::No_Image_Resource);
                                m_Private->m_USBDiskImageMessageBox->setVisible(true);
                                m_Private->m_ImageListView->setVisible(false);
                            } else {
                                m_Private->m_USBDiskImageMessageBox->setVisible(false);
                                m_Private->m_ImageListView->setVisible(true);
                            }
                        } else if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            int index = node.toElement().text().toInt();
                                            if (m_Private->m_ImageListView->model()->rowCount(QModelIndex()) > index) {
                                                m_Private->m_ImageListView->setCurrentIndex(m_Private->m_ImageListView->model()->index(index, 0, QModelIndex()));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void USBDiskImageListViewWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_ImageListView->setCurrentIndex(m_Private->m_ImageListView->model()->index(index, 0, QModelIndex()));
    }
}

void USBDiskImageListViewWidget::onMusicPlayerPlayStatus(const int type, const int status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void USBDiskImageListViewWidget::onVideoPlayerPlayStatus(const int type, const int status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void USBDiskImageListViewWidget::onImageListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_USBDiskImage, WidgetStatus::RequestShow);
    } else {
        g_Multimedia->imagePlayerPlayListViewIndex(DWT_USBDisk, index);
    }
    g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_USBDiskImage, WidgetStatus::RequestShow);
}

USBDiskImageListViewWidgetPrivate::USBDiskImageListViewWidgetPrivate(USBDiskImageListViewWidget *parent)
    : m_Parent(parent)
{
    m_ImageListView = NULL;
    m_USBDiskImageMessageBox = NULL;
    m_LastIndex = -1;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

USBDiskImageListViewWidgetPrivate::~USBDiskImageListViewWidgetPrivate()
{
}

void USBDiskImageListViewWidgetPrivate::initialize()
{
    m_USBDiskImageMessageBox = new MessageBox(m_Parent);
    m_USBDiskImageMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_USBDiskImageMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_USBDiskImageMessageBox->setAutoHide(false);
    m_USBDiskImageMessageBox->hide();
    m_ImageListView = new MultimediaListView(m_Parent);
    m_ImageListView->hide();

#ifdef DESKTOP_AMD64
    m_ImageListView->appendListView(QString("test1.png"));
    m_ImageListView->appendListView(QString("test2.png"));
    m_ImageListView->show();
#endif
}

void USBDiskImageListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerPlayStatus(const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_ImageListView, ARKSENDER(listViewItemRelease(const int)),
                     m_Parent,        ARKRECEIVER(onImageListViewItemRelease(const int)),
                     type);
}
