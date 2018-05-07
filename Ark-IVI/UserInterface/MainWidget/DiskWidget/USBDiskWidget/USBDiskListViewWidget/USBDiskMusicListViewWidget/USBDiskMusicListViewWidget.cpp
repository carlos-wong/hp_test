#include "USBDiskMusicListViewWidget.h"
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

class USBDiskMusicListViewWidgetPrivate
{
    Q_DISABLE_COPY(USBDiskMusicListViewWidgetPrivate)
public:
    explicit USBDiskMusicListViewWidgetPrivate(USBDiskMusicListViewWidget* parent);
    ~USBDiskMusicListViewWidgetPrivate();
    void initialize();
    void connectAllSlots();
    MultimediaListView* m_MusicListView;
    MessageBox* m_USBDiskMusicMessageBox;
    int m_Elapsed;
    int m_LastIndex;
    MultimediaType m_MultimediaType;
    bool m_Multimedia;
private:
    USBDiskMusicListViewWidget* m_Parent;
};

USBDiskMusicListViewWidget::USBDiskMusicListViewWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new USBDiskMusicListViewWidgetPrivate(this))
{
    setVisible(false);
}

USBDiskMusicListViewWidget::~USBDiskMusicListViewWidget()
{
}

void USBDiskMusicListViewWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 18, 578, 360, this);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_MusicListView);
    g_Widget->geometryFit(0, 0, 578, 360, m_Private->m_USBDiskMusicMessageBox);
}

void USBDiskMusicListViewWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Home:
    case Widget::T_Back: {
        if (isVisible()) {
            if (WidgetStatus::RequestShow == status) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_USBDiskMusic, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_USBDiskMusic: {
        if (WidgetStatus::RequestShow == status) {
            if (m_Private->m_Multimedia) {
                g_Widget->setWidgetType(Widget::T_USBDiskMusic, Widget::T_Undefine, WidgetStatus::Show);
            } else {
                qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_Private->m_LastIndex;
                if (-1 != m_Private->m_LastIndex) {
                    g_Multimedia->musicPlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_LastIndex, m_Private->m_Elapsed);
                    g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_USBDiskMusic, WidgetStatus::RequestShow);
                } else {
                    g_Widget->setWidgetType(Widget::T_USBDiskMusic, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBDiskImage:
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

void USBDiskMusicListViewWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    qDebug() << "USBDiskMusicListViewWidget::onDeviceWatcherStatus" << type << status;
    if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_Multimedia = false;
            m_Private->m_USBDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->clearListView();
            m_Private->m_MusicListView->setVisible(false);
            break;
        }
        case DWS_Busy: {
            m_Private->m_Multimedia = false;
            m_Private->m_USBDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->setVisible(false);
            break;
        }
        case DWS_Ready: {
            //            if (isVisible()) {
            //                g_Widget->setWidgetType(Widget::T_USBDiskMusic, WidgetStatus::Show);
            //            }
            break;
        }
        case DWS_Remove: {
            m_Private->m_Multimedia = false;
            m_Private->m_MultimediaType = MT_Idle;
            m_Private->m_LastIndex = -1;
            m_Private->m_USBDiskMusicMessageBox->setVisible(false);
            m_Private->m_MusicListView->setVisible(false);
            m_Private->m_MusicListView->clearListView();
            setVisible(false);
            break;
        }
        default: {
            break;
        }
        }
    }
}

void USBDiskMusicListViewWidget::onMusicPlayerFileNames(const DeviceWatcherType type, const QString &xml)
{
    if (DWT_USBDisk == type) {
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
                        if (QString("USBFileNames") == element.tagName()) {
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
                                m_Private->m_USBDiskMusicMessageBox->setText(SourceString::No_Music_Resource);
                                m_Private->m_USBDiskMusicMessageBox->setVisible(true);
                                m_Private->m_MusicListView->setVisible(false);
                            } else {
                                m_Private->m_USBDiskMusicMessageBox->setVisible(false);
                                m_Private->m_MusicListView->setVisible(true);
                            }
                        } else if (QString("USBPersistant") == element.tagName()) {
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

void USBDiskMusicListViewWidget::onMusicPlayerPlayStatus(const DeviceWatcherType type, const MusicPlayerPlayStatus status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_MultimediaType = MT_Music;
        //        if ((MPPS_Start == status)
        //                || (MPPS_Unsupport == status)) {
        //        } else if (MPPS_Exit == status) {
        //            m_Private->m_ListBtn->setVisible(false);
        //        }
    }
}

void USBDiskMusicListViewWidget::onMusicPlayerID3TagChange(const DeviceWatcherType type, const int index, const QString &fileName, const QString& title, const QString& artist, const QString& album, const int endTime)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_LastIndex = index;
        m_Private->m_MusicListView->setCurrentIndex(m_Private->m_MusicListView->model()->index(index, 0, QModelIndex()));
    }
}

void USBDiskMusicListViewWidget::onMusicPlayerElapsedInformation(const int elapsedTime, const int elapsedMillesimal)
{
    m_Private->m_Elapsed = elapsedTime;
}

void USBDiskMusicListViewWidget::onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
    }
}

void USBDiskMusicListViewWidget::onVideoPlayerPlayStatus(const int type, const int status)
{
    if (DWT_USBDisk == type) {
        m_Private->m_Multimedia = true;
        m_Private->m_MultimediaType = MT_Video;
    }
}

void USBDiskMusicListViewWidget::onMusicListViewItemRelease(const int index)
{
    if (index == m_Private->m_LastIndex) {
        if (MT_Music == m_Private->m_MultimediaType) {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_USBDiskMusic, WidgetStatus::RequestShow);
        } else {
            qDebug() << __PRETTY_FUNCTION__ << __LINE__;
            g_Multimedia->musicPlayerPlayListViewIndex(DWT_USBDisk, index, m_Private->m_Elapsed);
            g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_USBDiskMusic, WidgetStatus::RequestShow);
        }
    } else {
        m_Private->m_Elapsed = 0;
        g_Multimedia->musicPlayerPlayListViewIndex(DWT_USBDisk, index, m_Private->m_Elapsed);
        g_Widget->setWidgetType(Widget::T_USBMusic, Widget::T_USBDiskMusic, WidgetStatus::RequestShow);
    }
}

USBDiskMusicListViewWidgetPrivate::USBDiskMusicListViewWidgetPrivate(USBDiskMusicListViewWidget *parent)
    : m_Parent(parent)
{
    m_MusicListView = NULL;
    m_USBDiskMusicMessageBox = NULL;
    m_Elapsed = 0;
    m_LastIndex = -1;
    m_MultimediaType = MT_Idle;
    m_Multimedia = false;
    initialize();
    connectAllSlots();
}

USBDiskMusicListViewWidgetPrivate::~USBDiskMusicListViewWidgetPrivate()
{
}

void USBDiskMusicListViewWidgetPrivate::initialize()
{
    m_USBDiskMusicMessageBox = new MessageBox(m_Parent);
    m_USBDiskMusicMessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
    m_USBDiskMusicMessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
    m_USBDiskMusicMessageBox->setAutoHide(false);
    m_USBDiskMusicMessageBox->hide();
    m_MusicListView = new MultimediaListView(m_Parent);
    m_MusicListView->hide();
}

void USBDiskMusicListViewWidgetPrivate::connectAllSlots()
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
