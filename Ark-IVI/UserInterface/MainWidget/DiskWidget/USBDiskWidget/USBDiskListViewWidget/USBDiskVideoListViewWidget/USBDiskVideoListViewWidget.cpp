#include "USBDiskVideoListViewWidget.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MultimediaListView.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BusinessLogic/Setting.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QDomDocument>

namespace SourceString {
static const QString No_Video_Resource(QObject::tr("No Video Resource..."));
}

class USBDiskVideoListViewWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskVideoListViewWidgetPrivate)
public:
    explicit USBDiskVideoListViewWidgetPrivate(USBDiskVideoListViewWidget* parent);
    ~USBDiskVideoListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_VideoListView;
    MessageBox* m_USBDiskVideoMessageBox;
    int m_Elapsed;
    int m_LastIndex;
    MultimediaType m_MultimediaType;
    bool m_Multimedia;
private:
    USBDiskVideoListViewWidget* m_Parent;
};

USBDiskVideoListViewWidget::USBDiskVideoListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskVideoListViewWidgetPrivate(this))
{
    setVisible(false);
}

USBDiskVideoListViewWidget::~USBDiskVideoListViewWidget()
{
}

void USBDiskVideoListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_USBDiskVideoMessageBox);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_VideoListView);
}

void USBDiskVideoListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_USBDiskVideo, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_USBDiskVideo: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_USBDiskVideo, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (-1 != m_Private->m_LastIndex) {
                    g_Setting->setLauncherTVOUT(false);
                    g_Multimedia->videoPlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_LastIndex, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
                    g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBDiskVideo, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_USBDiskVideo, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskImage:
    case Widget::T_USBDiskMusic: {
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

void USBDiskVideoListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "USBDiskVideoListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_Multimedia = false;
            m_Private->m_USBDiskVideoMessageBox->setVisible(false);
            m_Private->m_VideoListView->clearListView();
            m_Private->m_VideoListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_Multimedia = false;
            m_Private->m_USBDiskVideoMessageBox->setVisible(false);
            m_Private->m_VideoListView->setVisible(false);
            break;
        }
        case DWS_Ready: {
            break;
        }
        case DWS_Remove: {
            m_Private->m_Multimedia = false;
            m_Private->m_MultimediaType = MT_Idle;
            m_Private->m_LastIndex = -1;
            m_Private->m_USBDiskVideoMessageBox->setVisible(false);
            m_Private->m_VideoListView->clearListView();
            m_Private->m_VideoListView->setVisible(false);
            setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void USBDiskVideoListViewWidget::onVideoPlayerFileNames(const int type, const QString& xml)
{
    if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("VideoPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            m_Private->m_VideoListView->clearListView();
                            for (int i = 0; i < nodeList.size(); ++i) {
                                QDomNode node = nodeList.at(i);
                                if (node.isElement()) {
                                    if (!node.toElement().isNull()) {
                                        if (node.isElement()) {
                                            m_Private->m_VideoListView->appendListView(node.toElement().text());
                                        }
                                    }
                                }
                            }
                            if (0 == nodeList.length()) {
                                m_Private->m_USBDiskVideoMessageBox->setText(SourceString::No_Video_Resource);
                                m_Private->m_USBDiskVideoMessageBox->setVisible(true);
                                m_Private->m_VideoListView->setVisible(false);
                            } else {
                                m_Private->m_USBDiskVideoMessageBox->setVisible(false);
                                m_Private->m_VideoListView->setVisible(true);
                            }
                        } else if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            int index = node.toElement().text().split(QChar('-')).at(0).toInt();
                                            m_Private->m_Elapsed = node.toElement().text().split(QChar('-')).at(1).toInt();
                                            if (m_Private->m_VideoListView->model()->rowCount(QModelIndex()) > index) {
                                                m_Private->m_VideoListView->setCurrentIndex(m_Private->m_VideoListView->model()->index(index, 0, QModelIndex()));
                                                m_Private->m_LastIndex = index;
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

void USBDiskVideoListViewWidget::onVideoPlayerPlayStatus(const DeviceWatcherType type, const VideoPlayerPlayStatus status)
{
    m_Private->m_MultimediaType = MT_Video;
}

void USBDiskVideoListViewWidget::onVideoPlayerInformation(const DeviceWatcherType type, const int index, const QString &fileName, const int endTime)
{
    qDebug() << "USBDiskVideoListViewWidget::onVideoPlayerInformation" << index;
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_VideoListView->setCurrentIndex(m_Private->m_VideoListView->model()->index(index, 0, QModelIndex()));
    }
}

void USBDiskVideoListViewWidget::onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    m_Private->m_Elapsed = elapsedTime;
}

void USBDiskVideoListViewWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void USBDiskVideoListViewWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_MultimediaType = MT_Music;
    }
}

void USBDiskVideoListViewWidget::onVideoListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        if (MT_Video == m_Private->m_MultimediaType) {
            g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBDisk, WidgetStatus::RequestShow);
        } else {
            g_Setting->setLauncherTVOUT(false);
            g_Multimedia->videoPlayerPlayListViewIndex(DWT_USBDisk, index, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
            g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBDiskVideo, WidgetStatus::RequestShow);
        }
    } else {
        m_Private->m_Elapsed = 0;
        g_Setting->setLauncherTVOUT(false);
        g_Multimedia->videoPlayerPlayListViewIndex(DWT_USBDisk, index, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
        g_Widget->setWidgetType(Widget::T_USBVideo, Widget::T_USBDiskVideo, WidgetStatus::RequestShow);
    }
}

USBDiskVideoListViewWidgetPrivate::USBDiskVideoListViewWidgetPrivate(USBDiskVideoListViewWidget *parent)
    : m_Parent(parent)
{
    m_VideoListView = NULL;
    m_USBDiskVideoMessageBox = NULL;
    m_Elapsed = 0;
    m_LastIndex = -1;
    m_MultimediaType = MT_Idle;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

USBDiskVideoListViewWidgetPrivate::~USBDiskVideoListViewWidgetPrivate()
{
}

void USBDiskVideoListViewWidgetPrivate::initialize()
{
    m_USBDiskVideoMessageBox = new MessageBox(m_Parent);
    m_USBDiskVideoMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_USBDiskVideoMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_USBDiskVideoMessageBox->setAutoHide(false);
    m_USBDiskVideoMessageBox->hide();
    m_VideoListView = new MultimediaListView(m_Parent);
    m_VideoListView->hide();
}

void USBDiskVideoListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerFileNames(const int, const QString&)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerInformation(const int, const int, const QString &, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerElapsedInformation(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_VideoListView, ARKSENDER(listViewItemRelease(const int)),
                     m_Parent,        ARKRECEIVER(onVideoListViewItemRelease(const int)),
                     type);
}
