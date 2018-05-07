#include "SDDiskVideoListViewWidget.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MultimediaListView.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BusinessLogic/Setting.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QVariant>
#include <QPainter>
#include <QDomDocument>

namespace SourceString {
static const QString No_Video_Resource(QObject::tr("No Video Resource..."));
}

class SDDiskVideoListViewWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskVideoListViewWidgetPrivate)
public:
    explicit SDDiskVideoListViewWidgetPrivate(SDDiskVideoListViewWidget* parent);
    ~SDDiskVideoListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_VideoListView;
    MessageBox* m_SDDiskVideoMessageBox;
    int m_Elapsed;
    int m_LastIndex;
    MultimediaType m_MultimediaType;
    bool m_Multimedia;
private:
    SDDiskVideoListViewWidget* m_Parent;
};

SDDiskVideoListViewWidget::SDDiskVideoListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskVideoListViewWidgetPrivate(this))
{
    setVisible(false);
}

SDDiskVideoListViewWidget::~SDDiskVideoListViewWidget()
{
}

void SDDiskVideoListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_SDDiskVideoMessageBox);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_VideoListView);
}

void SDDiskVideoListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDDiskVideo, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_SDDiskVideo, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (-1 != m_Private->m_LastIndex) {
                    g_Setting->setLauncherTVOUT(false);
                    g_Multimedia->videoPlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_LastIndex, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
                    g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDDiskVideo, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_SDDiskVideo, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskMusic: {
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

void SDDiskVideoListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "SDDiskVideoListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskVideoMessageBox->setVisible(false);
            m_Private->m_VideoListView->clearListView();
            m_Private->m_VideoListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskVideoMessageBox->setVisible(false);
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
            m_Private->m_SDDiskVideoMessageBox->setVisible(false);
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

void SDDiskVideoListViewWidget::onVideoPlayerFileNames(const int type, const QString& xml)
{
    if (DWT_SDDisk == type) {
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
                        if (QString("SDFileNames") == element.tagName()) {
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
                                m_Private->m_SDDiskVideoMessageBox->setText(SourceString::No_Video_Resource);
                                m_Private->m_SDDiskVideoMessageBox->setVisible(true);
                                m_Private->m_VideoListView->setVisible(false);
                            } else {
                                m_Private->m_SDDiskVideoMessageBox->setVisible(false);
                                m_Private->m_VideoListView->setVisible(true);
                            }
                        } else if (QString("SDPersistant") == element.tagName()) {
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
                        }/* else if (QString("SDType") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            if (MT_Video == node.toElement().text().toInt()) {
                                                m_Private->m_SDMultimedia = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }*/
                    }
                }
                node = node.nextSibling();
            }
        }
    }
}

void SDDiskVideoListViewWidget::onVideoPlayerPlayStatus(const DeviceWatcherType type, const VideoPlayerPlayStatus status)
{
    m_Private->m_MultimediaType = MT_Video;
}

void SDDiskVideoListViewWidget::onVideoPlayerInformation(const DeviceWatcherType type, const int index, const QString &fileName, const int endTime)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_VideoListView->setCurrentIndex(m_Private->m_VideoListView->model()->index(index, 0, QModelIndex()));
    }
}

void SDDiskVideoListViewWidget::onVideoPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    m_Private->m_Elapsed = elapsedTime;
}

void SDDiskVideoListViewWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void SDDiskVideoListViewWidget::onMusicPlayerPlayStatus(const int type, const int status)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_MultimediaType = MT_Music;
    }
}

void SDDiskVideoListViewWidget::onVideoListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        if (MT_Video == m_Private->m_MultimediaType) {
            g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDDisk, WidgetStatus::RequestShow);
        } else {
            g_Setting->setLauncherTVOUT(false);
            g_Multimedia->videoPlayerPlayListViewIndex(DWT_SDDisk, index, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
            g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDDiskVideo, WidgetStatus::RequestShow);
        }
    } else {
        m_Private->m_Elapsed = 0;
        g_Setting->setLauncherTVOUT(false);
        g_Multimedia->videoPlayerPlayListViewIndex(DWT_SDDisk, index, 0, 0, 800 * g_Widget->widthScalabilityFactor(), 480 * g_Widget->heightScalabilityFactor(), m_Private->m_Elapsed);
        g_Widget->setWidgetType(Widget::T_SDVideo, Widget::T_SDDiskVideo, WidgetStatus::RequestShow);
    }
}

SDDiskVideoListViewWidgetPrivate::SDDiskVideoListViewWidgetPrivate(SDDiskVideoListViewWidget *parent)
    : m_Parent(parent)
{
    m_VideoListView = NULL;
    m_SDDiskVideoMessageBox = NULL;
    m_Elapsed = 0;
    m_LastIndex = -1;
    m_MultimediaType = MT_Idle;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

SDDiskVideoListViewWidgetPrivate::~SDDiskVideoListViewWidgetPrivate()
{
}

void SDDiskVideoListViewWidgetPrivate::initialize()
{
    m_SDDiskVideoMessageBox = new MessageBox(m_Parent);
    m_SDDiskVideoMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_SDDiskVideoMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_SDDiskVideoMessageBox->setAutoHide(false);
    m_SDDiskVideoMessageBox->hide();
    m_VideoListView = new MultimediaListView(m_Parent);
    m_VideoListView->hide();
}

void SDDiskVideoListViewWidgetPrivate::connectAllSlots()
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
