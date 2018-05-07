#include "ImageWidget.h"
#include "BusinessLogic/BusinessLogicUtility.h"
#include "UserInterfaceUtility.h"
#include "PixmapWidget/PixmapWidget.h"
#include "ImageToolBarWidget/ImageToolBarWidget.h"
#include "UserInterfaceUtility.h"
#include "TextWidget.h"
#include "RunnableThread.h"
#include "BmpWidget.h"
#include "MessageBox.h"
#include <QPainter>
#include <QImageReader>
#include <QMouseEvent>
#include <QDomDocument>
#include <QTimer>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>

namespace SourceString {
static const QString Unsupport(QObject::tr("Unsupport..."));
}

class ImageWidgetPrivate
{
public:
    explicit ImageWidgetPrivate(ImageWidget *parent);
    ~ImageWidgetPrivate();
    void initializeTextWidget();
    void initializeMessageBox();
    void initializeTimer();
    void initializePreviewTimer();
    void initializeAnimationTimer();
    void initializePixmapWidget();
    void initializeStartLoading();
    void initializeToolBarWidget();
    void initializeParent();
    void connectAllSlots();
    void startLoading();
    void updateImage();
    ImageToolBarWidget* m_ToolBarWidget;
    BmpWidget* m_StartLoading;
    MessageBox* m_MessageBox;
    PixmapWidget* m_PixmapWidget;
    TextWidget* m_TextWidget;
    QTimer* m_Timer;
    QTimer* m_PreviewTimer;
    QTimer* m_AnimationTimer;
    QRect m_TouchRect;
    QImage m_Image;
    QImageReader m_ImageReader;
    QString m_FilePath;
    bool m_Failed;
    bool m_Restart;
    bool m_Exit;
    bool m_GIF;
    Widget::Type m_Type;
    DeviceWatcherType m_DeviceWatcherType;
    int m_SDDiskLastIndex;
    int m_USBDiskLastIndex;
    QMutex m_Mutex;
    QWaitCondition m_WaitCondition;
    CustomThread* m_CustomThread;
private:
    ImageWidget* m_Parent;
};

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
    , m_Private(new ImageWidgetPrivate(this))
{
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
}

void ImageWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(0, 0, 800, 480, this);
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
    m_Private->initializeTimer();
    m_Private->m_Timer->stop();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_Private->m_TouchRect.contains(event->pos())) {
        if (g_Widget->geometryFit(0, 0, 800, 480) == m_Private->m_TouchRect) {
            if (Widget::T_SDImage == m_Private->m_Type) {
                g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_SDImageFullScreen, WidgetStatus::Show);
            } else if (Widget::T_USBImage == m_Private->m_Type) {
                g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_USBImageFullScreen, WidgetStatus::Show);
            }
        } else {
            if (Widget::T_SDImage == m_Private->m_Type) {
                g_Widget->setWidgetType(Widget::T_SDImageFullScreen, Widget::T_SDImage, WidgetStatus::Show);
            } else if (Widget::T_USBImage == m_Private->m_Type) {
                g_Widget->setWidgetType(Widget::T_USBImageFullScreen, Widget::T_USBImage, WidgetStatus::Show);
            }
        }
    }
}

void ImageWidget::onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status)
{
    switch (destinationType) {
    case Widget::T_Back: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_SDDiskImage, Widget::T_SDImage, WidgetStatus::RequestShow);
                } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                    g_Widget->setWidgetType(Widget::T_USBDiskImage, Widget::T_USBImage, WidgetStatus::RequestShow);
                }
            }
        }
        break;
    }
    case Widget::T_Home: {
        if (WidgetStatus::RequestShow == status) {
            if (isVisible()) {
                g_Widget->setWidgetType(Widget::T_ViewPaper0, Widget::T_Undefine, WidgetStatus::RequestShow);
            }
        }
        break;
    }
    case Widget::T_SDImage: {
        if (WidgetStatus::RequestShow == status) {
            if ((Widget::T_SDDisk == requestType)
                    && (-1 != m_Private->m_SDDiskLastIndex)) {
                if (DWT_SDDisk != m_Private->m_DeviceWatcherType) {
                    if (-1 == m_Private->m_SDDiskLastIndex) {
                        g_Multimedia->imagePlayerPlayListViewIndex(DWT_SDDisk, 0);
                    } else {
                        g_Multimedia->imagePlayerPlayListViewIndex(DWT_SDDisk, m_Private->m_SDDiskLastIndex);
                    }
                    m_Private->m_DeviceWatcherType = DWT_SDDisk;
                    g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_Undefine, WidgetStatus::Show);
                } else {
                    g_Widget->setWidgetType(Widget::T_SDImage, requestType, WidgetStatus::Show);
                }
            } else {
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    if (Widget::T_SDDiskImage != requestType) {
                        if (Widget::T_SDImage == m_Private->m_Type) {
                            g_Widget->setWidgetType(Widget::T_SDDisk, requestType, WidgetStatus::Show);
                        }
                        m_Private->m_Type = Widget::T_Undefine;
                    } else {
                        g_Widget->setWidgetType(Widget::T_SDImage, Widget::T_Undefine, WidgetStatus::Show);
                    }
                } else {
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 74, 800, 480 - 74 - 103);
            m_Private->initializeToolBarWidget();
            m_Private->initializeTimer();
            m_Private->m_Timer->start();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(true);
            g_Multimedia->imageInitliaze();
            if (m_Private->m_DeviceWatcherType == DWT_SDDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
            } else if (m_Private->m_DeviceWatcherType == DWT_USBDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
            }
            m_Private->m_Type = Widget::T_SDImage;
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBImage: {
        if (WidgetStatus::RequestShow == status) {
            if ((Widget::T_USBDisk == requestType)
                    && (-1 != m_Private->m_USBDiskLastIndex)) {
                if (DWT_USBDisk != m_Private->m_DeviceWatcherType) {
                    if (-1 == m_Private->m_USBDiskLastIndex) {
                        g_Multimedia->imagePlayerPlayListViewIndex(DWT_USBDisk, 0);
                    } else {
                        g_Multimedia->imagePlayerPlayListViewIndex(DWT_USBDisk, m_Private->m_USBDiskLastIndex);
                    }
                    m_Private->m_DeviceWatcherType = DWT_USBDisk;
                    g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_Undefine, WidgetStatus::Show);
                } else {
                    g_Widget->setWidgetType(Widget::T_USBImage, requestType, WidgetStatus::Show);
                }
            } else {
                if (m_Private->m_DeviceWatcherType == DWT_Undefine) {
                    if (Widget::T_USBDiskImage != requestType) {
                        if (Widget::T_USBImage == m_Private->m_Type) {
                            g_Widget->setWidgetType(Widget::T_USBDisk, requestType, WidgetStatus::Show);
                        }
                        m_Private->m_Type = Widget::T_Undefine;
                    } else {
                        g_Widget->setWidgetType(Widget::T_USBImage, Widget::T_Undefine, WidgetStatus::Show);
                    }
                } else {
                    g_Widget->setWidgetType(m_Private->m_Type, requestType, WidgetStatus::Show);
                }
            }
        } else if (WidgetStatus::Show == status) {
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 74, 800, 480 - 74 - 103);
            m_Private->initializeToolBarWidget();
            m_Private->initializeTimer();
            m_Private->m_Timer->start();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(true);
            g_Multimedia->imageInitliaze();
            if (m_Private->m_DeviceWatcherType == DWT_SDDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
            } else if (m_Private->m_DeviceWatcherType == DWT_USBDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
            }
            m_Private->m_Type = Widget::T_USBImage;
            setVisible(true);
        }
        break;
    }
    case Widget::T_SDImageFullScreen: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(destinationType, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 0, 800, 480);
            m_Private->initializeTimer();
            m_Private->m_Timer->stop();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(false);
            g_Multimedia->imageInitliaze();
            if (m_Private->m_DeviceWatcherType == DWT_SDDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
            } else if (m_Private->m_DeviceWatcherType == DWT_USBDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
            }
            g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
            m_Private->m_Type = Widget::T_SDImage;
            setVisible(true);
        }
        break;
    }
    case Widget::T_USBImageFullScreen: {
        if (WidgetStatus::RequestShow == status) {
            g_Widget->setWidgetType(destinationType, requestType, WidgetStatus::Show);
        } else if (WidgetStatus::Show == status) {
            m_Private->m_TouchRect = g_Widget->geometryFit(0, 0, 800, 480);
            m_Private->initializeTimer();
            m_Private->m_Timer->stop();
            m_Private->initializeTextWidget();
            m_Private->m_TextWidget->setVisible(false);
            g_Multimedia->imageInitliaze();
            if (m_Private->m_DeviceWatcherType == DWT_SDDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
            } else if (m_Private->m_DeviceWatcherType == DWT_USBDisk) {
                g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
            }
            g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
            m_Private->m_Type = Widget::T_USBImage;
            setVisible(true);
        }
        break;
    }
    default : {
        if (WidgetStatus::Show == status) {
            if (isVisible()) {
                m_Private->initializePreviewTimer();
                if (m_Private->m_PreviewTimer->isActive()) {
                    onToolButtonRelease(ImageToolBarWidget::BT_Toggle);
                }
                m_Private->initializeTimer();
                m_Private->m_Timer->stop();
                g_Multimedia->setImageFocus(Multimedia::F_ImageUnfocus);
                setVisible(false);
            }
        }
        break;
    }
    }
}

void ImageWidget::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    if ((m_Private->m_DeviceWatcherType == type)
            && (DWS_Remove == status)) {
        if (!isHidden()) {
            if (DWT_SDDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_SDDisk, Widget::T_SDImage, WidgetStatus::RequestShow);
            } else if (DWT_USBDisk == m_Private->m_DeviceWatcherType) {
                g_Widget->setWidgetType(Widget::T_USBDisk, Widget::T_USBImage, WidgetStatus::RequestShow);
            }
        }
        m_Private->m_Restart = true;
        m_Private->m_DeviceWatcherType = DWT_Undefine;
        if (NULL != m_Private->m_PixmapWidget) {
            m_Private->m_PixmapWidget->setVisible(false);
        }
    }
}

void ImageWidget::onImagePlayerFileNames(const DeviceWatcherType type, const QString &xml)
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
            m_Private->m_SDDiskLastIndex = -1;
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("SDPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            m_Private->m_SDDiskLastIndex = node.toElement().text().toInt();
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
    } else if (DWT_USBDisk == type) {
        QDomDocument document;
        document.setContent(xml);
        QDomElement root = document.documentElement();
        if ((!root.isNull())
                && (root.isElement())
                && (QString("ImagePlayer") == root.toElement().tagName())
                && (root.hasChildNodes())) {
            QDomNode node = root.firstChild();
            m_Private->m_USBDiskLastIndex = -1;
            while (!node.isNull()) {
                if (node.isElement()) {
                    QDomElement element = node.toElement();
                    if (!element.isNull()) {
                        if (QString("USBPersistant") == element.tagName()) {
                            QDomElement node = element.toElement();
                            if (node.isElement()) {
                                if (!node.toElement().isNull()) {
                                    if (node.isElement()) {
                                        if (!node.toElement().text().isEmpty()) {
                                            m_Private->m_USBDiskLastIndex = node.toElement().text().toInt();
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

static void renderThread(void *paramater)
{
    ImageWidgetPrivate* ptr = static_cast<ImageWidgetPrivate*>(paramater);
    if (ptr != NULL) {
        QImageReader imageReader;
        QImage image;
        do {
            ptr->m_Mutex.lock();
            if ((ptr->m_Restart)
                    || (imageReader.fileName() != ptr->m_ImageReader.fileName())) {
                ptr->startLoading();
                imageReader.setFileName(ptr->m_ImageReader.fileName());
            }
            if (NULL != ptr->m_AnimationTimer
                    && ptr->m_AnimationTimer->isActive()) {
                if (!ptr->m_Restart) {
                    if (imageReader.supportsAnimation()) {
                        if (imageReader.currentImageNumber() >= (imageReader.imageCount() - 1)) {
                            imageReader.setFileName(imageReader.fileName());
                        }
                    }
                }
            }
            ptr->m_Mutex.unlock();
            if (!ptr->m_Restart) {
                imageReader.read(&image);
            }
            if (!ptr->m_Restart) {
                ptr->m_Image = image;
            }
            if (!ptr->m_Restart) {
                ptr->updateImage();
            }
            ptr->m_Mutex.lock();
            bool flag = ptr->m_Restart;
            ptr->m_Restart = false;
            if (!flag) {
                ptr->m_WaitCondition.wait(&ptr->m_Mutex);
            }
            ptr->m_Mutex.unlock();
        } while (!ptr->m_Exit);
    }
}

void ImageWidget::onImagePlayerChange(const DeviceWatcherType type, const QString &filePath, const int index, const int percent, const int rotate)
{
    if (NULL != m_Private->m_MessageBox) {
        m_Private->m_MessageBox->setVisible(false);
    }
    m_Private->initializeTextWidget();
    m_Private->m_TextWidget->setText(QFileInfo(filePath).fileName());
    m_Private->initializePixmapWidget();
    m_Private->m_PixmapWidget->setDisabled(true);
    m_Private->m_PixmapWidget->onImageChange(QImage(), true);
    QMutexLocker mutexLocker(&m_Private->m_Mutex);
    m_Private->m_DeviceWatcherType = type;
    if (m_Private->m_DeviceWatcherType == DWT_SDDisk) {
        g_Multimedia->setImageFocus(Multimedia::F_ImageSDFocus);
    } else if (m_Private->m_DeviceWatcherType == DWT_USBDisk) {
        g_Multimedia->setImageFocus(Multimedia::F_ImageUSBFocus);
    }
    if (DWT_SDDisk == type) {
        m_Private->m_SDDiskLastIndex = index;
    } else if (DWT_USBDisk == type) {
        m_Private->m_USBDiskLastIndex = index;
    }
    m_Private->m_ImageReader.setFileName(filePath);
    m_Private->m_Failed = !m_Private->m_ImageReader.canRead();
    QString tmpPath(filePath);
    if (!m_Private->m_ImageReader.canRead()) {
        //        tmpPath = QString(":/Images/ImageLoadFailedBackground.png");
        tmpPath.clear();
        m_Private->initializeMessageBox();
        m_Private->m_MessageBox->setVisible(true);
    }
    m_Private->m_ImageReader.setFileName(tmpPath);
    m_Private->m_Restart = true;
    m_Private->initializeAnimationTimer();
    if ((tmpPath == filePath)
            && (m_Private->m_ImageReader.supportsAnimation())) {
        m_Private->m_AnimationTimer->setSingleShot(false);
    } else {
        m_Private->m_AnimationTimer->setSingleShot(true);
    }
    if (NULL == m_Private->m_CustomThread) {
        m_Private->m_CustomThread = new CustomThread(this);
        m_Private->m_CustomThread->setCallbackFunction(renderThread, static_cast<void*>(m_Private.data()));
        QObject::connect(this, ARKSENDER(imageChange()),
                         this, ARKRECEIVER(onImageChange()),
                         Qt::UniqueConnection);
        QObject::connect(this, ARKSENDER(startLoading()),
                         this, ARKRECEIVER(onStartLoading()),
                         Qt::UniqueConnection);
        m_Private->m_Restart = true;
        m_Private->m_CustomThread->start(QThread::LowestPriority);
    } else {
        m_Private->m_WaitCondition.wakeOne();
    }
    if (m_Private->m_ImageReader.supportsAnimation()) {
        m_Private->m_AnimationTimer->setInterval(m_Private->m_ImageReader.nextImageDelay());
        m_Private->m_GIF = true;
        m_Private->m_AnimationTimer->start();
    }
}

void ImageWidget::onArkProtocol(const ArkProtocolWrapper &protocol)
{
    switch (protocol.type) {
    case AT_Key: {
        struct ArkKey* key = arkProtocolWrapperToArkKey(protocol);
        if ((NULL != key)
                && (!protocol.handler)
                && (isVisible())
                && (Multimedia::F_ImageUnfocus != g_Multimedia->getImageFocus())) {
            if ((AKS_Release == key->status)
                    || (AKS_Active == key->status)) {
                switch (key->type) {
                case AKT_CommonPrevious: {
                    g_Multimedia->imagePlayerPlayPreviousListViewIndex();
                    break;
                }
                case AKT_CommonNext: {
                    g_Multimedia->imagePlayerPlayNextListViewIndex();
                    break;
                }
                default: {
                    break;
                }
                }
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

void ImageWidget::onStartTimer()
{
    if (g_Widget->geometryFit(0, 0, 800, 480) != m_Private->m_TouchRect) {
        m_Private->initializeTimer();
        m_Private->m_Timer->start();
    }
}

void ImageWidget::onStopTimer()
{
    m_Private->initializeTimer();
    m_Private->m_Timer->stop();
}

void ImageWidget::onImageChange()
{
    if (NULL != m_Private->m_StartLoading) {
        m_Private->m_StartLoading->setVisible(false);
    }
    if (!m_Private->m_Restart) {
        m_Private->initializeMessageBox();
        m_Private->m_MessageBox->setVisible(m_Private->m_Image.isNull());
        if (m_Private->m_Image.isNull()) {
            m_Private->initializePixmapWidget();
            m_Private->m_PixmapWidget->onImageChange(QImage(QString(/*":/Images/ImageLoadFailedBackground.png"*/)), true);
        } else {
            m_Private->initializePixmapWidget();
            bool flag = true;
            if (m_Private->m_AnimationTimer->isActive()) {
                if (m_Private->m_GIF) {
                    m_Private->m_GIF = false;
                } else {
                    flag = false;
                }
            }
            m_Private->m_PixmapWidget->onImageChange(m_Private->m_Image, flag, m_Private->m_AnimationTimer->isActive());
        }
        m_Private->initializePixmapWidget();
        m_Private->m_PixmapWidget->setVisible(true);
    }
    m_Private->initializePixmapWidget();
    m_Private->m_PixmapWidget->setDisabled(false);
}

void ImageWidget::onStartLoading()
{
    m_Private->initializeStartLoading();
    m_Private->m_StartLoading->raise();
    m_Private->m_StartLoading->setVisible(true);
}

ImageWidgetPrivate::ImageWidgetPrivate(ImageWidget *parent)
    : m_Parent(parent)
{
    m_ToolBarWidget = NULL;
    m_StartLoading = NULL;
    m_MessageBox = NULL;
    m_PixmapWidget = NULL;
    m_TextWidget = NULL;
    m_Timer = NULL;
    m_PreviewTimer = NULL;
    m_AnimationTimer = NULL;
    m_TouchRect = g_Widget->geometryFit(0, 74, 800, 480 - 74 - 103);
    m_Failed = false;
    m_Restart = true;
    m_Exit = false;
    m_GIF = true;
    m_Type = Widget::T_Undefine;
    m_DeviceWatcherType = DWT_Undefine;
    m_SDDiskLastIndex = -1;
    m_USBDiskLastIndex = -1;
    m_CustomThread = NULL;
    initializeParent();
    connectAllSlots();
}

ImageWidgetPrivate::~ImageWidgetPrivate()
{
    if (NULL != m_CustomThread) {
        m_Mutex.lock();
        m_Exit = true;
        m_WaitCondition.wakeOne();
        m_Mutex.unlock();
    }
}

void ImageWidgetPrivate::initializeTextWidget()
{
    if (NULL == m_TextWidget) {
        m_TextWidget = new TextWidget(m_Parent);
        m_TextWidget->setLanguageType(TextWidget::T_NoTranslate);
        m_TextWidget->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
        g_Widget->geometryFit(0, 52, 800, 30, m_TextWidget);
        m_TextWidget->setVisible(true);
    }
}

void ImageWidgetPrivate::initializeMessageBox()
{
    if (NULL == m_MessageBox) {
        m_MessageBox = new MessageBox(m_Parent);
        m_MessageBox->setFontPointSize(15 * g_Widget->widthScalabilityFactor());
        m_MessageBox->setBackgroundBmpPath(QString(":/Images/MessageBoxBackground.png"));
        m_MessageBox->setText(SourceString::Unsupport);
        g_Widget->geometryFit(0, 52, 800, 348, m_MessageBox);
        m_MessageBox->setAutoHide(false);
        m_MessageBox->raise();
    }
}

void ImageWidgetPrivate::initializeTimer()
{
    if (NULL == m_Timer) {
        m_Timer = new QTimer(m_Parent);
        m_Timer->setSingleShot(true);
        m_Timer->setInterval(7000);
        QObject::connect(m_Timer,  ARKSENDER(timeout()),
                         m_Parent, ARKRECEIVER(onTimeout()));
    }
}

void ImageWidgetPrivate::initializePreviewTimer()
{
    if (NULL == m_PreviewTimer) {
        m_PreviewTimer = new QTimer(m_Parent);
        m_PreviewTimer->setInterval(5 * 1000);
        QObject::connect(m_PreviewTimer,  ARKSENDER(timeout()),
                         m_Parent,        ARKRECEIVER(onTimeout()));
    }
}

void ImageWidgetPrivate::initializeAnimationTimer()
{
    if (NULL == m_AnimationTimer) {
        m_AnimationTimer = new QTimer(m_Parent);
        m_AnimationTimer->setSingleShot(false);
        QObject::connect(m_AnimationTimer,  ARKSENDER(timeout()),
                         m_Parent,          ARKRECEIVER(onTimeout()));
    }
}

void ImageWidgetPrivate::initializePixmapWidget()
{
    if (NULL == m_PixmapWidget) {
        m_PixmapWidget = new PixmapWidget(m_Parent);
        m_PixmapWidget->lower();
    }
}

void ImageWidgetPrivate::initializeStartLoading()
{
    if (NULL == m_StartLoading) {
        m_StartLoading = new BmpWidget(m_Parent);
        m_StartLoading->setVisible(false);
        m_StartLoading->setBackgroundBmpPath(QString(":/Images/ImageLoadingBackground.png"));
        g_Widget->geometryFit(353, 190, 93, 100, m_StartLoading);
    }
}

void ImageWidgetPrivate::initializeToolBarWidget()
{
    if (NULL == m_ToolBarWidget) {
        m_ToolBarWidget = new ImageToolBarWidget(m_Parent);
        m_ToolBarWidget->raise();
        m_ToolBarWidget->setVisible(true);
        QObject::connect(m_ToolBarWidget, ARKSENDER(toolButtonRelease(const int)),
                         m_Parent,        ARKRECEIVER(onToolButtonRelease(const int)));
    }
}

void ImageWidgetPrivate::initializeParent()
{
    m_Parent->setVisible(false);
}

void ImageWidgetPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_Setting, m_Parent, ARKRECEIVER(onArkProtocol(const ArkProtocolWrapper&)));
    connectSignalAndSlotByNamesake(g_Widget, m_Parent, ARKRECEIVER(onWidgetTypeChange(const Widget::Type, const Widget::Type, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerFileNames(const int, const QString &)));
    connectSignalAndSlotByNamesake(g_Multimedia, m_Parent, ARKRECEIVER(onImagePlayerChange(const int, const QString &, const int, const int, const int)));
}

void ImageWidgetPrivate::startLoading()
{
    m_Parent->startLoading();
}

void ImageWidgetPrivate::updateImage()
{
    m_Parent->imageChange();
}

void ImageWidget::onTimeout()
{
    QTimer* ptr = qobject_cast<QTimer*>(sender());
    m_Private->initializeTimer();
    m_Private->initializePreviewTimer();
    m_Private->initializeAnimationTimer();
    if (ptr == m_Private->m_Timer) {
        if (Widget::T_SDImage == m_Private->m_Type) {
            g_Widget->setWidgetType(Widget::T_SDImageFullScreen, Widget::T_SDImage, WidgetStatus::Show);
        } else if (Widget::T_USBImage == m_Private->m_Type) {
            g_Widget->setWidgetType(Widget::T_USBImageFullScreen, Widget::T_USBImage, WidgetStatus::Show);
        }
    } else if (ptr == m_Private->m_AnimationTimer) {
        if (isVisible()) {
            QMutexLocker mutexLocker(&m_Private->m_Mutex);
            m_Private->m_WaitCondition.wakeOne();
        }
    } else if (ptr == m_Private->m_PreviewTimer) {
        g_Multimedia->imagePlayerPlayNextListViewIndex();
    }
}

void ImageWidget::onToolButtonRelease(const int type)
{
    switch (type) {
    case ImageToolBarWidget::BT_Rotate: {
        m_Private->initializeStartLoading();
        if (!m_Private->m_StartLoading->isVisible()) {
            m_Private->m_Restart = false;
            m_Private->m_PixmapWidget->rotate();
        }
        break;
    }
    case ImageToolBarWidget::BT_Previous: {
        m_Private->initializeStartLoading();
        if (!m_Private->m_StartLoading->isVisible()) {
            m_Private->m_StartLoading->raise();
            m_Private->m_StartLoading->setVisible(true);
            m_Private->m_Restart = true;
            g_Multimedia->imagePlayerPlayPreviousListViewIndex();
        }
        break;
    }
    case ImageToolBarWidget::BT_Toggle: {
        m_Private->initializeStartLoading();
        if (!m_Private->m_StartLoading->isVisible()) {
            m_Private->initializePreviewTimer();
            m_Private->initializeToolBarWidget();
            if (m_Private->m_PreviewTimer->isActive()) {
                m_Private->m_PreviewTimer->stop();
                m_Private->m_ToolBarWidget->setToolButtonStatus(ImageToolBarWidget::PS_Pause);
            } else {
                m_Private->m_PreviewTimer->start();
                m_Private->m_ToolBarWidget->setToolButtonStatus(ImageToolBarWidget::PS_Play);
            }
        }
        break;
    }
    case ImageToolBarWidget::BT_Next: {
        m_Private->initializeStartLoading();
        if (!m_Private->m_StartLoading->isVisible()) {
            m_Private->m_StartLoading->raise();
            m_Private->m_StartLoading->setVisible(true);
            m_Private->m_Restart = true;
            g_Multimedia->imagePlayerPlayNextListViewIndex();
        }
        break;
    }
    case ImageToolBarWidget::BT_ZoomIn: {
        if (NULL != m_Private->m_StartLoading) {
            if (!m_Private->m_StartLoading->isVisible()) {
                m_Private->m_Restart = false;
                m_Private->m_PixmapWidget->zoomIn();
            }
        }
        break;
    }
    case ImageToolBarWidget::BT_ZoomOut: {
        if (NULL != m_Private->m_StartLoading) {
            if (!m_Private->m_StartLoading->isVisible()) {
                m_Private->m_Restart = false;
                m_Private->m_PixmapWidget->zoomOut();
            }
        }
        break;
    }
    default: {
        break;
    }
    }
}

