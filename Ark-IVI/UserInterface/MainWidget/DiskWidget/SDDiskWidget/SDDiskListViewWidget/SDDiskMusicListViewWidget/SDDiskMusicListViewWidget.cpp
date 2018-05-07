#include "SDDiskMusicListViewWidget.h"
#include "MessageBox.h"
#include "UserInterface/MainWidget/DiskWidget/MultimediaListView.h"
#include "UserInterfaceUtility.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "BmpButton.h"
#include "AutoConnect.h"
#include "UserInterfaceUtility.h"
#include <QPainter>
#include <QDomDocument>

namespace SourceString {
static const QString No_Music_Resource(QObject::tr("No Music Resource..."));
}

class SDDiskMusicListViewWidgetPrivate
{
    Q_DISABLE_COPY(SDDiskMusicListViewWidgetPrivate)
public:
    explicit SDDiskMusicListViewWidgetPrivate(SDDiskMusicListViewWidget* parent);
    ~SDDiskMusicListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_MusicListView;
    MessageBox* m_SDDiskMusicMessageBox;
    int m_Elapsed;
    int m_LastIndex;
    MultimediaType m_MultimediaType;
    bool m_Multimedia;
private:
    SDDiskMusicListViewWidget* m_Parent;
};

SDDiskMusicListViewWidget::SDDiskMusicListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new SDDiskMusicListViewWidgetPrivate(this))
{
    setVisible(false);
}

SDDiskMusicListViewWidget::~SDDiskMusicListViewWidget()
{
}

void SDDiskMusicListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_SDDiskMusicMessageBox);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_MusicListView);
}

void SDDiskMusicListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDDiskMusic: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_SDDiskMusic, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                if (-1 != m_Private->m_LastIndex) {
                    g_Multimedia->musicPlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_LastIndex, m_Private->m_Elapsed);
                    g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_SDDiskMusic, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDDiskImage:
    case Widget::T_SDDiskVideo: {
        if (WidgetStatus::Show == status) {
            //            m_Private->m_RequestToSDDiskWidget = false;
            setVisible(false);
        }
        break;
    }
        //    case Widget::T_Music: {
        //        if (WidgetStatus::Show == status) {
        ////            if (isVisible()
        ////                    || !m_Private->m_ListBtn->isHidden()) {
        ////                m_Private->m_RequestToSDDiskWidget = true;
        ////            }
        //        }
        //        break;
        //    }
        //    case Widget::T_SDDisk: {
        //        if (WidgetStatus::Show == status) {
        //            m_Private->m_RequestToSDDiskWidget = false;
        //            if (!isHidden()
        //                    && (!m_Private->m_SDMultimedia)) {
        //                //                g_Widget->setWidgetType(Widget::T_SDDiskMusic, WidgetStatus::Show);
        //            }
        //        }
        //        break;
        //    }
    default: {
        //        if (WidgetStatus::Show == status) {
        //            m_Private->m_RequestToSDDiskWidget = false;
        //        }
        break;
    }
    }
}

void SDDiskMusicListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "SDDiskMusicListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->clearListView();
            m_Private->m_MusicListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_Multimedia = false;
            m_Private->m_SDDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->setVisible(false);
            break;
        }
        case DWS_Ready: {
            break;
        }
        case DWS_Remove: {
            m_Private->m_Multimedia = false;
            m_Private->m_MultimediaType = MT_Idle;
            m_Private->m_LastIndex = -1;
            m_Private->m_SDDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->clearListView();
            m_Private->m_MusicListView->setVisible(false);
            setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void SDDiskMusicListViewWidget::onMusicPlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_SDDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("MusicPlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDFileNames") == element.tagName()) {
                            QDomNodeList nodeList = element.childNodes();
                            m_Private->m_MusicListView->clearListView();
                            for (int i = 0; i < nodeList.size(); ++i) {
                                QDomNode node = nodeList.at(i);
                                if (node.isElement()) {
                                    if (!node.toElement().isNull()) {
                                        if (node.isElement()) {
                                            QFileInfo fileInfo(node.toElement().text());
                                            m_Private->m_MusicListView->appendListView(fileInfo.fileName());
                                        }
                                    }
                                }
                            }
                            if (0 == nodeList.length()) {
                                m_Private->m_SDDiskMusicMessageBox->setText(SourceString::No_Music_Resource);
                                m_Private->m_SDDiskMusicMessageBox->setVisible(true);
                                m_Private->m_MusicListView->setVisible(false);
                            } else {
                                m_Private->m_SDDiskMusicMessageBox->setVisible(false);
                                m_Private->m_MusicListView->setVisible(true);
                            }
                        } else if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            int index = node.toElement().text().split(QChar('-')).at(0).toInt();
                                            m_Private->m_Elapsed = node.toElement().text().split(QChar('-')).at(1).toInt();
                                            if (m_Private->m_MusicListView->model()->rowCount(QModelIndex()) > index) {
                                                m_Private->m_MusicListView->setCurrentIndex(m_Private->m_MusicListView->model()->index(index, 0, QModelIndex()));
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

void SDDiskMusicListViewWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
}

void SDDiskMusicListViewWidget::onMusicPlayerID3TagChange(const DeviceWatcherType type, const int index, const QString &fileName, const QString& title, const QString& artist, const QString& album, const int endTime)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_MusicListView->setCurrentIndex(m_Private->m_MusicListView->model()->index(index, 0, QModelIndex()));
    }
}

void SDDiskMusicListViewWidget::onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    m_Private->m_Elapsed = elapsedTime;
}

void SDDiskMusicListViewWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void SDDiskMusicListViewWidget::onVideoPlayerPlayStatus(const DeviceWatcherType type, const VideoPlayerPlayStatus status)
{
    if (DWT_SDDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_MultimediaType = MT_Video;
    }
}

void SDDiskMusicListViewWidget::onMusicListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        if (MT_Music == m_Private->m_MultimediaType) {
            g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
        } else {
            g_Multimedia->musicPlayerPlayListViewIndex(DWT_SDDisk, index, m_Private->m_Elapsed);
            g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
        }
    } else {
        m_Private->m_Elapsed = 0;
        g_Multimedia->musicPlayerPlayListViewIndex(DWT_SDDisk, index, m_Private->m_Elapsed);
        g_Widget->setWidgetType(Widget::T_SDMusic, Widget::T_SDDiskMusic, WidgetStatus::RequestShow);
    }
}

SDDiskMusicListViewWidgetPrivate::SDDiskMusicListViewWidgetPrivate(SDDiskMusicListViewWidget *parent)
    : m_Parent(parent)
{
    m_MusicListView = NULL;
    m_SDDiskMusicMessageBox = NULL;
    m_Elapsed = 0;
    m_LastIndex = -1;
    m_MultimediaType = MT_Idle;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

SDDiskMusicListViewWidgetPrivate::~SDDiskMusicListViewWidgetPrivate()
{
}

void SDDiskMusicListViewWidgetPrivate::initialize()
{
    m_SDDiskMusicMessageBox = new MessageBox(m_Parent);
    m_SDDiskMusicMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_SDDiskMusicMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_SDDiskMusicMessageBox->setAutoHide(false);
    m_SDDiskMusicMessageBox->hide();
    m_MusicListView = new MultimediaListView(m_Parent);
    m_MusicListView->hide();

#ifdef DESKTOP_AMD64
    m_MusicListView->appendListView(QString("test1.mp3"));
    m_MusicListView->appendListView(QString("test2.mp3"));
    m_MusicListView->setVisible(true);
#endif
}

void SDDiskMusicListViewWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onMusicPlayerElapsedInformation(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onVideoPlayerPlayStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_MusicListView, ARKSENDER(listViewItemRelease(const int)),
                     m_Parent,        ARKRECEIVER(onMusicListViewItemRelease(const int)),
                     type);
    QObject::connect(m_MusicListView, ARKSENDER(listViewItemRelease(const int)),
                     m_Parent,        ARKRECEIVER(onMusicListViewItemRelease(const int)),
                     type);
}
