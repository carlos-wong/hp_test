#include "SDDiskImageListViewWidget.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MultimediaListView.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "AutoConnect.h"
#include "BmpButton.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QDomDocument>

namespace SourceString {
static const QString No_Image_Resource(QObject::tr("No Image Resource..."));
}

class SDDiskImageListViewWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskImageListViewWidgetPrivate)
public:
    explicit SDDiskImageListViewWidgetPrivate(SDDiskImageListViewWidget* parent);
    ~SDDiskImageListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_ImageListView;
    MessageBox* m_SDDiskImageMessageBox;
    int m_LastIndex;
    bool m_Multimedia;
private:
    SDDiskImageListViewWidget* m_Parent;
};

SDDiskImageListViewWidget::SDDiskImageListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskImageListViewWidgetPrivate(this))
{
    setVisible(false);
}

SDDiskImageListViewWidget::~SDDiskImageListViewWidget()
{
}

void SDDiskImageListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_SDDiskImageMessageBox);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_ImageListView);
}

void SDDiskImageListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDDiskImage, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDDiskImage: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_SDDiskImage, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (-1 != m_Private->m_LastIndex) {
                    g_Multimedia->imagePlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_LastIndex);
                    g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_SDDiskImage, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_SDDiskImage, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskMusic:
    case Widget::T_SDDiskVideo: {
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

void SDDiskImageListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "SDDiskImageListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskImageMessageBox->setVisible(false);
            m_Private->m_ImageListView->clearListView();
            m_Private->m_ImageListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskImageMessageBox->setVisible(false);
            m_Private->m_ImageListView->setVisible(false);
            break;
        }
        case DWS_Ready: {
            break;
        }
        case DWS_Remove: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskImageMessageBox->setVisible(false);
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

void SDDiskImageListViewWidget::onImagePlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_SDDisk == type) {
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
                        if (QString("SDFileNames") == element.tagName()) {
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
                                m_Private->m_SDDiskImageMessageBox->setText(SourceString::No_Image_Resource);
                                m_Private->m_SDDiskImageMessageBox->setVisible(true);
                                m_Private->m_ImageListView->setVisible(false);
                            } else {
                                m_Private->m_SDDiskImageMessageBox->setVisible(false);
                                m_Private->m_ImageListView->setVisible(true);
                            }
                        } else if (QString("SDPersistant") == element.tagName()) {
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
                        } else if (QString("SDType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Image == node.toElement().text().toInt()) {
                                                m_Private->m_ImageListView->setVisible(true);
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

void SDDiskImageListViewWidget::onImagePlayerChange(const DeviceWatcherType type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_ImageListView->setCurrentIndex(m_Private->m_ImageListView->model()->index(index, 0, QModelIndex()));
    }
}

void SDDiskImageListViewWidget::onMusicPlayerPlayStatus(const int type, const int status)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void SDDiskImageListViewWidget::onVideoPlayerPlayStatus(const int type, const int status)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void SDDiskImageListViewWidget::onImageListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_SDDiskImage, WidgetStatus::RequestShow);
    } else {
        g_Multimedia->imagePlayerPlayListViewIndex(DWT_SDDisk, index);
    }
    g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_SDDiskImage, WidgetStatus::RequestShow);
}

SDDiskImageListViewWidgetPrivate::SDDiskImageListViewWidgetPrivate(SDDiskImageListViewWidget *parent)
    : m_Parent(parent)
{
    m_ImageListView = NULL;
    m_SDDiskImageMessageBox = NULL;
    m_LastIndex = -1;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

SDDiskImageListViewWidgetPrivate::~SDDiskImageListViewWidgetPrivate()
{
}

void SDDiskImageListViewWidgetPrivate::initialize()
{
    m_SDDiskImageMessageBox = new MessageBox(m_Parent);
    m_SDDiskImageMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_SDDiskImageMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_SDDiskImageMessageBox->setAutoHide(false);
    m_SDDiskImageMessageBox->hide();
    m_ImageListView = new MultimediaListView(m_Parent);
    m_ImageListView->hide();

#ifdef DESKTOP_AMD64
    m_ImageListView->appendListView(QString("test1.png"));
    m_ImageListView->appendListView(QString("test2.png"));
    m_ImageListView->setVisible(true);
#endif
}

void SDDiskImageListViewWidgetPrivate::connectAllSlots()
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
