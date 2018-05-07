#ifndef IMAGEPLAYER_H
#define IMAGEPLAYER_H

#include "DeviceWatcher/DeviceWatcher.h"
#include <QObject>
#include <QImage>
#include <QScopedPointer>

class ImagePlayerPrivate;
class ImagePlayer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ImagePlayer)
#ifdef g_ImagePlayer
#undef g_ImagePlayer
#endif
#define g_ImagePlayer (ImagePlayer::instance())
public:
    inline static ImagePlayer* instance() {
        static ImagePlayer* imagePlayer(new ImagePlayer(qApp));
        return imagePlayer;
    }
    void imagePlayerRequestFileNames(const int type);
    void imagePlayerPlayListViewIndex(const int type, const int index);
    void imagePlayerPlayPreviousListViewIndex();
    void imagePlayerPlayNextListViewIndex();
    void imagePlayerRotateImage();
    void imagePlayerZoomInImage();
    void imagePlayerZoomOutImage();
signals:
    void onImagePlayerFileNames(const int type, const QString &xml);
    void onImagePlayerChange(const int type, const QString &filePath, const int index, const int percent, const int rotate);
protected slots:
    void onDeviceWatcherStatus(const int type, const int status);
    void onImageFilePath(const QString &path, const int type);
private:
    explicit ImagePlayer(QObject* parent = NULL);
    ~ImagePlayer();
    friend class ImagePlayerPrivate;
    QScopedPointer<ImagePlayerPrivate> m_Private;
};

#endif // IMAGEPLAYER_H
