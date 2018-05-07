#include "VideoPlayer.h"
#include "AutoConnect.h"
#include "Utility.h"
#include "MultimediaPersistent.h"
#include "UserInterfaceUtility.h"
#include "ark_api.h"
#include <QTimer>
#include <QList>
#include <QFileInfo>
#include <QDomDocument>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class VideoPlayerPrivate
{
    Q_DISABLE_COPY(VideoPlayerPrivate)
public:
    enum CommandType {
        CT_Undefine = -1,
        CT_PlayIndex = 0,
        CT_PlayNext = 1,
        CT_PlayPrevious = 2,
        CT_Pause = 3,
        CT_Seek = 4,
        CT_Toggle = 5,
    };
    explicit VideoPlayerPrivate(VideoPlayer *parent);
    ~VideoPlayerPrivate();
    void initialize();
    void connectAllSlots();
    void playVideoIndex(const DeviceWatcherType type, const int index, const int x, const int y, const int width, const int height, const int millesimal);
    void setPlayMode(const VideoPlayerPlayMode mode);
    void setPlayStatus(const VideoPlayerPlayStatus status);
    void pauseToggleHandler(const QString& output);
    void startHandler(const QString& output);
    void endTimePositionHandler(const QString &output);
    void millesimalHandler(const QString& output);
    void timePositionHandler(const QString &output);
    QList<QString>& getPathList(const DeviceWatcherType type);
    void endOfFileHandler();
    void unsupportHandler();
    void seekToMillesimal(const int millesimal);
    void createSDFileNamesXml();
    void createUSBFileNamesXml();
    void videoPlayerSetPlay();
    void videoPlayerSetSuspend();
    void videoPlayerSetPause();
    void videoPlayerSetExit(const DeviceWatcherType type);
    void exitVideoPlayer();
    VideoPlayerPlayMode m_PlayMode;
    VideoPlayerPlayStatus m_PlayStatus;
    QString m_SDFileNamesXml;
    QString m_USBFileNamesXml;
    QList<QString> m_SDPathList;
    QList<QString> m_USBPathList;
    QProcess* m_VideoPlayer;
    DeviceWatcherType m_DiskType;
    int m_PlayIndex;
    int m_ElapsedTime;
    int m_Millesimal;
    int m_EndTime;
    int m_X;
    int m_Y;
    int m_Width;
    int m_Height;
    bool m_Suspend;
    QTimer* m_Timer;
    VideoPlayerPrivate::CommandType m_LastCommand;
    unsigned char m_Seek;
private:
    VideoPlayer *m_Parent;
};

void VideoPlayer::videoPlayerRequestFileNames(const DeviceWatcherType type)
{
    switch (type) {
    case DWT_SDDisk: {
        emit onVideoPlayerFileNames(DWT_SDDisk, m_Private->m_SDFileNamesXml);
        break;
    }
    case DWT_USBDisk: {
        emit onVideoPlayerFileNames(DWT_USBDisk, m_Private->m_USBFileNamesXml);
        break;
    }
    default: {
        break;
    }
    }
}

void VideoPlayer::videoPlayerSetPlayModeToggle()
{
    switch (m_Private->m_PlayMode) {
    case VPPM_AllRepeat: {
        videoPlayerSetPlayMode(VPPM_Shuffle);
        break;
    }
    case VPPM_Shuffle: {
        videoPlayerSetPlayMode(VPPM_RepeatOnce);
        break;
    }
    case VPPM_RepeatOnce:
    default: {
        videoPlayerSetPlayMode(VPPM_AllRepeat);
        break;
    }
    }
}

void VideoPlayer::videoPlayerSetPlayMode(const VideoPlayerPlayMode mode)
{
    m_Private->setPlayMode(mode);
}

void VideoPlayer::videoPlayerSetPlayStatusToggle()
{
    switch (m_Private->m_PlayStatus) {
    case VPPS_Play: {
        videoPlayerSetPlayStatus(VPPS_Pause);
        break;
    }
    case VPPS_Pause: {
        videoPlayerSetPlayStatus(VPPS_Play);
        break;
    }
    case VPPS_Stop: {
        videoPlayerSetPlayStatus(VPPS_Stop);
    }
    case VPPS_Unsupport: {
        m_Private->m_Timer->stop();
        m_Private->playVideoIndex(m_Private->m_DiskType, m_Private->m_PlayIndex, m_Private->m_X, m_Private->m_Y, m_Private->m_Width, m_Private->m_Height, 0);
        break;
    }
    default: {
        break;
    }
    }
}

void VideoPlayer::videoPlayerSetPlayStatus(const VideoPlayerPlayStatus status)
{
    if (QProcess::Running == m_Private->m_VideoPlayer->state()) {
        if (VideoPlayerPrivate::CT_Undefine == m_Private->m_LastCommand) {
            m_Private->m_Timer->stop();
            switch (status) {
            case VPPS_Play: {
                m_Private->videoPlayerSetPlay();
                break;
            }
            case VPPS_Pause: {
                m_Private->videoPlayerSetPause();
                break;
            }
            case VPPS_SuspendToggle: {
                m_Private->videoPlayerSetSuspend();
                break;
            }
            case VPPS_Stop: {
                videoPlayerPlayListViewIndex(m_Private->m_DiskType, m_Private->m_PlayIndex, m_Private->m_X, m_Private->m_Y, m_Private->m_Width, m_Private->m_Height, 0);
                break;
            }
            default : {
                break;
            }
            }
        }
    }
}

void VideoPlayer::videoPlayerPlayListViewIndex(const DeviceWatcherType type, const int index, const int x, const int y, const int width, const int height, const int millesimal)
{
    if (VideoPlayerPrivate::CT_Undefine == m_Private->m_LastCommand) {
        m_Private->m_Timer->stop();
        m_Private->playVideoIndex(type, index, x, y, width, height, millesimal);
    }
}

void VideoPlayer::videoPlayerPlayPreviousListViewIndex()
{
    if (VideoPlayerPrivate::CT_Undefine == m_Private->m_LastCommand) {
        m_Private->m_Timer->stop();
        int lastIndex = m_Private->m_PlayIndex;
        QList<QString> temp = m_Private->getPathList(m_Private->m_DiskType);
        if ((lastIndex > 0)
                && (lastIndex <= temp.size() - 1)) {
            --lastIndex;
        } else {
            lastIndex = temp.size() - 1;
        }
        videoPlayerPlayListViewIndex(m_Private->m_DiskType, lastIndex, m_Private->m_X, m_Private->m_Y, m_Private->m_Width, m_Private->m_Height, 0);
    }
}

void VideoPlayer::videoPlayerPlayNextListViewIndex()
{
    if (VideoPlayerPrivate::CT_Undefine == m_Private->m_LastCommand) {
        m_Private->m_Timer->stop();
        int lastIndex = m_Private->m_PlayIndex;
        QList<QString> temp = m_Private->getPathList(m_Private->m_DiskType);
        if (((lastIndex < (temp.size() - 1)))
                && (lastIndex >= 0)) {
            ++lastIndex;
        } else {
            lastIndex = 0;
        }
        videoPlayerPlayListViewIndex(m_Private->m_DiskType, lastIndex, m_Private->m_X, m_Private->m_Y, m_Private->m_Width, m_Private->m_Height, 0);
    }
}

void VideoPlayer::videoPlayerSeekToMillesimal(const int millesimal)
{
    if (QProcess::Running == m_Private->m_VideoPlayer->state()) {
        if (VideoPlayerPrivate::CT_Undefine == m_Private->m_LastCommand) {
            m_Private->m_Timer->stop();
            m_Private->seekToMillesimal(millesimal);
        }
    }
}

void VideoPlayer::videoPlayerExit()
{
    m_Private->videoPlayerSetExit(m_Private->m_DiskType);
}

void VideoPlayer::onDeviceWatcherStatus(const DeviceWatcherType type, const DeviceWatcherStatus status)
{
    if (DWT_SDDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_SDPathList.clear();
            break;
        }
        case DWS_Busy: {
            m_Private->m_SDPathList.clear();
            break;
        }
        case DWS_Ready: {
            m_Private->createSDFileNamesXml();
            videoPlayerRequestFileNames(DWT_SDDisk);
            break;
        }
        case DWS_Remove: {
            m_Private->m_SDPathList.clear();
            if (DWT_SDDisk == m_Private->m_DiskType) {
                if (QProcess::NotRunning != m_Private->m_VideoPlayer->state()) {
                    m_Private->m_VideoPlayer->write("quit\n");
                    m_Private->m_VideoPlayer->waitForBytesWritten();
                    m_Private->m_VideoPlayer->waitForFinished();
                }
                m_Private->m_PlayStatus = VPPS_Exit;
            }
            break;
        }
        default : {
            break;
        }
        }
    } else if (DWT_USBDisk == type) {
        switch (status) {
        case DWS_Empty: {
            m_Private->m_USBPathList.clear();
            break;
        }
        case DWS_Busy: {
            m_Private->m_USBPathList.clear();
            break;
        }
        case DWS_Ready: {
            m_Private->createUSBFileNamesXml();
            videoPlayerRequestFileNames(DWT_USBDisk);
            break;
        }
        case DWS_Remove: {
            m_Private->m_USBPathList.clear();
            if (DWT_USBDisk == m_Private->m_DiskType) {
                if (QProcess::NotRunning != m_Private->m_VideoPlayer->state()) {
                    m_Private->m_VideoPlayer->write("quit\n");
                    m_Private->m_VideoPlayer->waitForBytesWritten();
                    m_Private->m_VideoPlayer->waitForFinished();
                }
                m_Private->m_PlayStatus = VPPS_Exit;
            }
        }
        default : {
            break;
        }
        }
    }
}

void VideoPlayer::onVideoFilePath(const QString &path, const DeviceWatcherType type)
{
    if (type == DWT_USBDisk) {
        m_Private->m_USBPathList.append(path);
    } else if (type == DWT_SDDisk) {
        m_Private->m_SDPathList.append(path);
    }
}

void VideoPlayer::onTimeout()
{
    m_Private->endOfFileHandler();
}

void VideoPlayer::onReadyReadStandardOutput()
{
    while (m_Private->m_VideoPlayer->canReadLine()) {
        QString output(m_Private->m_VideoPlayer->readLine());
        if (output.contains(QString("ANS_LENGTH="))) {
            m_Private->endTimePositionHandler(output);
        } else if (output.contains(QString("ANS_PERCENT_POSITION="))) {
            m_Private->millesimalHandler(output);
        } else if (output.contains(QString("ANS_TIME_POSITION="))) {
            m_Private->timePositionHandler(output);
        } else if (output.contains(QString("ANS_pause"))) {
            m_Private->pauseToggleHandler(output);
        } else if (output.contains(QString("EOF code: 1"))) {
            m_Private->setPlayStatus(VPPS_Stop);
            m_Private->m_Timer->start();
        } else if ((output.contains(QString("incorrect streams")))
                   || (output.contains(QString("No stream found")))) {
            m_Private->unsupportHandler();
        }/* else if (output.contains(QString("seek-begin"))) {
            m_Private->m_Seek = 0;
            QFile file(QString("/tmp/seek"));
            if (!file.exists()
                    && file.open(QIODevice::WriteOnly)) {
                file.close();
            }
        } else if ((output.contains(QString("seek-finish"))
                    || (output.contains(QString("seek-error"))))) {
            m_Private->m_Seek = 1;
        }*/ else {
            if (output != QString("\n")) {
                qDebug() << "output" << output;
            }
        }
    }
}

void VideoPlayer::onReadyReadStandardError()
{
    qWarning() << __PRETTY_FUNCTION__;
    while (m_Private->m_VideoPlayer->canReadLine()) {
        QString output(m_Private->m_VideoPlayer->readLine());
        qWarning() << __PRETTY_FUNCTION__ << output;
    }
}

void VideoPlayer::onStarted()
{
    qWarning() << "VideoPlayer::onStarted" << m_Private->m_VideoPlayer->state();
}

void VideoPlayer::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qWarning() << "VideoPlayer::onFinished" << exitCode << exitStatus;
}

void VideoPlayer::onError(QProcess::ProcessError error)
{
    qWarning() << "VideoPlayer::onError" << error << m_Private->m_VideoPlayer->error();
}

void VideoPlayer::onStateChanged(QProcess::ProcessState state)
{
    qWarning() << "VideoPlayer::onStateChanged" << state;
}

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent)
    , m_Private(new VideoPlayerPrivate(this))
{
}

VideoPlayer::~VideoPlayer()
{
}

VideoPlayerPrivate::VideoPlayerPrivate(VideoPlayer *parent)
    : m_Parent(parent)
{
    m_PlayMode = VPPM_AllRepeat;
    m_PlayStatus = VPPS_Exit;
    m_VideoPlayer = NULL;
    m_DiskType = DWT_Undefine;
    m_PlayIndex = -1;
    m_ElapsedTime = -1;
    m_Millesimal = 0;
    m_EndTime = -1;
    m_X = 0;
    m_Y = 0;
    m_Width = 800;
    m_Height = 480;
    m_Suspend = false;
    m_Timer = NULL;
    m_LastCommand = VideoPlayerPrivate::CT_Undefine;
    m_Seek = 0;
    initialize();
    connectAllSlots();
}

VideoPlayerPrivate::~VideoPlayerPrivate()
{
    if (QProcess::NotRunning != m_VideoPlayer->state()) {
        m_VideoPlayer->write("pausing_keep_force  quit\n");
        m_VideoPlayer->waitForBytesWritten();
        m_VideoPlayer->waitForFinished();
    }
}

void VideoPlayerPrivate::initialize()
{
    m_VideoPlayer = new QProcess(m_Parent);
    m_VideoPlayer->setProcessChannelMode(QProcess::MergedChannels);
    m_Timer = new QTimer(m_Parent);
    m_Timer->setSingleShot(true);
    m_Timer->setInterval(2000);
}

void VideoPlayerPrivate::connectAllSlots()
{
    connectSignalAndSlotByNamesake(g_DeviceWatcher, m_Parent, SLOT(onDeviceWatcherStatus(const int, const int)));
    connectSignalAndSlotByNamesake(g_DeviceWatcher, m_Parent, SLOT(onVideoFilePath(const QString &, const int)));
    Qt::ConnectionType type = static_cast<Qt::ConnectionType>(Qt::UniqueConnection | Qt::AutoConnection);
    QObject::connect(m_VideoPlayer, SIGNAL(started()),
                     m_Parent,      SLOT(onStarted()),
                     type);
    QObject::connect(m_VideoPlayer, SIGNAL(finished(int, QProcess::ExitStatus)),
                     m_Parent,      SLOT(onFinished(int, QProcess::ExitStatus)),
                     type);
    QObject::connect(m_VideoPlayer, SIGNAL(error(QProcess::ProcessError)),
                     m_Parent,      SLOT(onError(QProcess::ProcessError)),
                     type);
    QObject::connect(m_VideoPlayer, SIGNAL(stateChanged(QProcess::ProcessState)),
                     m_Parent,      SLOT(onStateChanged(QProcess::ProcessState)),
                     type);
    QObject::connect(m_VideoPlayer, SIGNAL(readyReadStandardOutput()),
                     m_Parent,      SLOT(onReadyReadStandardOutput()),
                     type);
    QObject::connect(m_VideoPlayer, SIGNAL(readyReadStandardError()),
                     m_Parent,      SLOT(onReadyReadStandardError()),
                     type);
    QObject::connect(m_Timer,  SIGNAL(timeout()),
                     m_Parent, SLOT(onTimeout()),
                     type);
}

void VideoPlayerPrivate::playVideoIndex(const DeviceWatcherType type, const int index, const int x, const int y, const int width, const int height, const int millesimal)
{
    QList<QString> temp = getPathList(type);
    if ((VideoPlayerPrivate::CT_Undefine == m_LastCommand)
            && (temp.size() > index)
            && (QFile(temp.at(index))).exists()) {
        m_DiskType = type;
        m_Seek = 0;
        m_ElapsedTime = 0;
        m_EndTime = 0;
        m_PlayIndex = index;
        m_Suspend = false;
        m_Millesimal = millesimal;
        m_X = x;
        m_Y = y;
        m_Width = width;
        m_Height = height;
        //        if (0 != millesimal) {
        //            if (QProcess::NotRunning != m_VideoPlayer->state()) {
        //                m_VideoPlayer->write("quit\n");
        //                m_VideoPlayer->waitForBytesWritten();
        //                m_VideoPlayer->waitForFinished();
        //            }
        //        }
        if (QProcess::NotRunning == m_VideoPlayer->state()) {
            QString cmd = QString("mplayer");
            QStringList arguments;
            arguments << QString("-slave");
            arguments << QString("-idle");
            arguments << QString("-quiet");
            arguments << QString("-geometry");
            arguments << QString(QString::number(m_Width)
                                 + QString("x")
                                 + QString::number(m_Height)
                                 + QString("+")
                                 + QString::number(m_X)
                                 + QString("+")
                                 + QString::number(m_Y));
            arguments << QString("-vo");
            arguments << QString("customfb");
            arguments << QString("-ao");
            arguments << QString("alsa");
            if (!qgetenv("ARKTVOUT").isEmpty()) {
                arguments << QString("-tvout");
            }
            if (0 != millesimal) {
                arguments << QString("-ss");
                arguments << QString::number(millesimal);
            }
            m_VideoPlayer->start(cmd, arguments);
            m_VideoPlayer->waitForStarted();
        }
        if (0 != access("/tmp/video", F_OK)) {
            int videofd = open("/dev/fb1", O_RDONLY);
            if (videofd >= 0) {
                arkapi_show_fb(videofd, 0);
                close(videofd);
            } else {
                perror("open fb1");
                qWarning() << "open fb1 failed!!!";
            }
            videofd = open("/dev/fb4", O_RDONLY);
            if (videofd >= 0) {
                arkapi_show_fb(videofd, 0);
                close(videofd);
            } else {
                perror("open fb4");
                qWarning() << "open fb4 failed!!!";
            }
        }
        QString cmd1 = QString("loadfile \"") + temp.at(m_PlayIndex) + QString("\"\n");
        m_VideoPlayer->write(cmd1.toLocal8Bit().constData());
        m_VideoPlayer->waitForBytesWritten();
        emit m_Parent->onVideoPlayerElapsedInformation(m_ElapsedTime, m_Millesimal);
        emit m_Parent->onVideoPlayerInformation(m_DiskType, m_PlayIndex, QFileInfo(temp.at(m_PlayIndex)).fileName(), m_EndTime);
        emit m_Parent->onVideoPlayerPlayMode(m_PlayMode);
        setPlayStatus(VPPS_Start);
        QFileInfo fileInfo(temp.at(m_PlayIndex));
        MultimediaPersistent::setVideoPathInfo(m_DiskType, fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()));
        MultimediaPersistent::setMultimediaType(m_DiskType, MT_Video);
        MultimediaPersistent::setVideoMillesmial(m_DiskType, millesimal);
    }
}

void VideoPlayerPrivate::setPlayMode(const VideoPlayerPlayMode mode)
{
    if (mode != m_PlayMode) {
        m_PlayMode = mode;
    }
    emit m_Parent->onVideoPlayerPlayMode(m_PlayMode);
}

void VideoPlayerPrivate::setPlayStatus(const VideoPlayerPlayStatus status)
{
    if (status != m_PlayStatus) {
        m_PlayStatus = status;
    }
    emit m_Parent->onVideoPlayerPlayStatus(m_DiskType, status);
}

void VideoPlayerPrivate::pauseToggleHandler(const QString &output)
{
    if (output.contains(QString("yes"))) {
        setPlayStatus(VPPS_Pause);
    } else if (output.contains(QString("no"))) {
        setPlayStatus(VPPS_Play);
        if (VideoPlayerPrivate::CT_Seek == m_LastCommand) {
        }
    }
}

void VideoPlayerPrivate::startHandler(const QString &output)
{
}

void VideoPlayerPrivate::endTimePositionHandler(const QString &output)
{
    QString keyWord("ANS_LENGTH=");
    int startPos = output.indexOf(keyWord);
    int length = keyWord.length();
    QString endTime;
    endTime.clear();
    QList<QString> temp = getPathList(m_DiskType);
    if (temp.size() > m_PlayIndex) {
        m_EndTime = 0;
        if (-1 != startPos) {
            for (int i = 0; i < (output.length() - startPos); ++i) {
                if (QChar('\n') == output.at(startPos + length + i)) {
                    m_EndTime = static_cast<int>(endTime.toFloat());
                    break;
                }
                endTime += output.at(startPos + length + i);
            }
        }
        emit m_Parent->onVideoPlayerInformation(m_DiskType, m_PlayIndex, QFileInfo(temp.at(m_PlayIndex)).fileName(), m_EndTime);
        setPlayStatus(VPPS_Play);
    } else {
        qCritical() << temp.size() << m_PlayIndex;
    }
}

void VideoPlayerPrivate::millesimalHandler(const QString &output)
{
    int start = output.indexOf(QChar('=')) + 1;
    int end = output.indexOf(QChar('\n'));
    if ((-1 != start)
            && (-1 != end)) {
        bool ok(false);
        int millesimal = static_cast<int>(output.mid(start, end - start).toFloat(&ok) * 1000);
        if (ok) {
            m_Millesimal = millesimal;
        }
    }
}

void VideoPlayerPrivate::timePositionHandler(const QString &output)
{
    if (output.startsWith(QString("ANS_TIME_POSITION="), Qt::CaseSensitive)) {
        int start = output.indexOf(QChar('=')) + 1;
        int end = output.indexOf(QChar('\n'));
        if ((-1 != start)
                && (-1 != end)) {
            bool ok = true;
            int elapsed = output.mid(start, end - start).toInt(&ok);
            if (ok) {
                if (((qAbs(m_ElapsedTime - elapsed) > 5))
                        || (elapsed % 5)) {
                    MultimediaPersistent::setVideoMillesmial(m_DiskType, elapsed);
                }
                m_ElapsedTime = elapsed;
                if (1 == m_Seek) {
                    ++m_Seek;
                    return;
                } else if (2 == m_Seek) {
                    m_Seek = 0;
                    if (QFile::exists(QString("/tmp/seek"))) {
                        QFile::remove(QString("/tmp/seek"));
                    }
                }
                emit m_Parent->onVideoPlayerElapsedInformation(m_ElapsedTime, m_Millesimal);
            }
        }
    }
}

QList<QString> &VideoPlayerPrivate::getPathList(const int type)
{
    switch (type) {
    case DWT_SDDisk: {
        return m_SDPathList;
        break;
    }
    case DWT_USBDisk:
    default: {
        return m_USBPathList;
        break;
    }
    }
}

void VideoPlayerPrivate::endOfFileHandler()
{
    switch (m_PlayStatus) {
    case VPPS_Start:
    case VPPS_Unsupport:
    case VPPS_Pause:
    case VPPS_Stop:
    case VPPS_Play: {
        switch (m_PlayMode) {
        case VPPM_AllRepeat: {
            if ((getPathList(m_DiskType).size() - 1) > m_PlayIndex) {
                ++m_PlayIndex;
            } else {
                m_PlayIndex = 0;
            }
            break;
        }
        case VPPM_RepeatOnce: {
            break;
        }
        case VPPM_Shuffle:
        default : {
            m_PlayIndex = qrand() % getPathList(m_DiskType).size();
            break;
        }
        }
        playVideoIndex(m_DiskType, m_PlayIndex, m_X, m_Y, m_Width, m_Height, 0);
        break;
    }
    default: {
        break;
    }
    }
}

void VideoPlayerPrivate::unsupportHandler()
{
    QList<QString> temp = getPathList(m_DiskType);
    if (m_PlayIndex < temp.size()) {
        m_ElapsedTime = 0;
        m_Millesimal = 0;
        emit m_Parent->onVideoPlayerElapsedInformation(m_ElapsedTime, m_Millesimal);
        m_EndTime = 0;
        emit m_Parent->onVideoPlayerInformation(m_DiskType, m_PlayIndex, QFileInfo(temp.at(m_PlayIndex)).fileName(), m_EndTime);
        setPlayStatus(VPPS_Unsupport);
        m_Timer->start();
    }
}

void VideoPlayerPrivate::seekToMillesimal(const int millesimal)
{
    if ((VPPS_Pause == m_PlayStatus)
            || (VPPS_Play == m_PlayStatus)) {
        QString cmd("seek %1.%2 1\n");
        cmd = cmd.arg(millesimal / 10, 3, 10, QChar(' ')).arg(millesimal % 10, 1, 10, QChar('0'));
        m_VideoPlayer->write(cmd.toLocal8Bit().constData());
        m_VideoPlayer->waitForBytesWritten();
    } else {
        playVideoIndex(m_DiskType, m_PlayIndex, m_X, m_Y, m_Width, m_Height, 0);
    }
}

void VideoPlayerPrivate::createSDFileNamesXml()
{
    QDomDocument domDocument;
    domDocument.clear();
    domDocument.appendChild(domDocument.createElement(QString("VideoPlayer")));
    QDomElement root = domDocument.firstChildElement(QString("VideoPlayer"));
    QDomElement fileNames;
    QDomElement info;
    fileNames = domDocument.createElement(QString("SDFileNames"));
    root.appendChild(fileNames);
    m_SDFileNamesXml.clear();
    QString sdPersistantPathInfo = MultimediaPersistent::getVideoPathInfo(DWT_SDDisk);
    QStringList stringList = sdPersistantPathInfo.split(QChar('/'));
    QString sdPersistantPath = sdPersistantPathInfo.left(sdPersistantPathInfo.length() - stringList.last().length() - 1);
    QString persistantIndex("");
    QFileInfo fileInfo;
    int i;
    for (i = 0; i < m_SDPathList.size(); ++i) {
        info = domDocument.createElement(QString("Index:" + QString::number(i)));
        fileNames.appendChild(info);
        fileInfo.setFile(m_SDPathList.at(i));
        info.appendChild(domDocument.createTextNode(fileInfo.fileName()));
        if (persistantIndex.isEmpty()) {
            if (fileInfo.filePath() == sdPersistantPath) {
                if (sdPersistantPathInfo == (fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()))) {
                    persistantIndex = QString::number(i);
                }
            }
        }
    }
    QDomElement persistant = domDocument.createElement(QString("SDPersistant"));
    root.appendChild(persistant);
    unsigned char multimediaType = MultimediaPersistent::getMultimediaType(DWT_SDDisk);
    int musicMillesmial = 0;
    if (persistantIndex.isEmpty()) {
        if (0 == i) {
            persistantIndex = QString("-1");
            multimediaType = MT_Idle;
        } else {
            if (0 == multimediaType) {
                multimediaType = MT_Video;
                MultimediaPersistent::setMultimediaType(DWT_SDDisk, multimediaType);
            }
            persistantIndex = QString("0");
        }
    } else {
        musicMillesmial = MultimediaPersistent::getVideoMillesmial(DWT_SDDisk);
    }
    QString data = persistantIndex + QString("-") + QString::number(musicMillesmial);
    persistant.appendChild(domDocument.createTextNode(data));
    QDomElement type = domDocument.createElement(QString("SDType"));
    root.appendChild(type);
    type.appendChild(domDocument.createTextNode(QString::number(multimediaType)));
    m_SDFileNamesXml = domDocument.toString();
}

void VideoPlayerPrivate::createUSBFileNamesXml()
{
    QDomDocument domDocument;
    domDocument.clear();
    domDocument.appendChild(domDocument.createElement(QString("VideoPlayer")));
    QDomElement root = domDocument.firstChildElement(QString("VideoPlayer"));
    QDomElement fileNames;
    QDomElement info;
    fileNames = domDocument.createElement(QString("USBFileNames"));
    root.appendChild(fileNames);
    m_USBFileNamesXml.clear();
    QString usbPersistantPathInfo = MultimediaPersistent::getVideoPathInfo(DWT_USBDisk);
    QStringList stringList = usbPersistantPathInfo.split(QChar('/'));
    QString usbPersistantPath = usbPersistantPathInfo.left(usbPersistantPathInfo.length() - stringList.last().length() - 1);
    QString persistantIndex("");
    QFileInfo fileInfo;
    int i;
    for (i = 0; i < m_USBPathList.size(); ++i) {
        info = domDocument.createElement(QString("Index:" + QString::number(i)));
        fileNames.appendChild(info);
        fileInfo.setFile(m_USBPathList.at(i));
        info.appendChild(domDocument.createTextNode(fileInfo.fileName()));
        if (persistantIndex.isEmpty()) {
            if (fileInfo.filePath() == usbPersistantPath) {
                if (usbPersistantPathInfo == (fileInfo.filePath() + QString("/") + fileInfo.created().toString(QString("yyyyMMddhhmmss")) + fileInfo.lastModified().toString(QString("yyyyMMddhhmmss")) + QString::number(fileInfo.size()))) {
                    persistantIndex = QString::number(i);
                }
            }
        }
    }
    QDomElement persistant = domDocument.createElement(QString("USBPersistant"));
    root.appendChild(persistant);
    unsigned char multimediaType = MultimediaPersistent::getMultimediaType(DWT_USBDisk);
    int musicMillesmial = 0;
    if (persistantIndex.isEmpty()) {
        if (0 == i) {
            persistantIndex = QString("-1");
            multimediaType = MT_Idle;
        } else {
            if (0 == multimediaType) {
                multimediaType = MT_Video;
                MultimediaPersistent::setMultimediaType(DWT_USBDisk, multimediaType);
            }
            persistantIndex = QString("0");
        }
    } else {
        musicMillesmial = MultimediaPersistent::getVideoMillesmial(DWT_USBDisk);
    }
    QString data = persistantIndex + QString("-") + QString::number(musicMillesmial);
    persistant.appendChild(domDocument.createTextNode(data));
    QDomElement type = domDocument.createElement(QString("USBType"));
    root.appendChild(type);
    type.appendChild(domDocument.createTextNode(QString::number(multimediaType)));
    m_USBFileNamesXml = domDocument.toString();
}

void VideoPlayerPrivate::videoPlayerSetPlay()
{
    switch (m_PlayStatus) {
    case VPPS_Play: {
        break;
    }
    case VPPS_Pause: {
        m_VideoPlayer->write("u\n");
        m_VideoPlayer->waitForBytesWritten();
        break;
    }
    default: {
        return;
        break;
    }
    }
}

void VideoPlayerPrivate::videoPlayerSetSuspend()
{
    if (m_Suspend) {
        m_Suspend = false;
        videoPlayerSetPlay();
    } else {
        if ((VPPS_Play == m_PlayStatus)
                || (VPPS_Start == m_PlayStatus)) {
            m_Suspend = true;
            videoPlayerSetPause();
        } else if (VPPS_Pause == m_PlayStatus) {
            m_VideoPlayer->write("pausing_keep_force pause\n");
            m_VideoPlayer->waitForBytesWritten();
        }
    }
}

void VideoPlayerPrivate::videoPlayerSetPause()
{
    switch (m_PlayStatus) {
    case VPPS_Start:
    case VPPS_Play: {
        m_VideoPlayer->write("pausing_keep_force pause\n");
        m_VideoPlayer->waitForBytesWritten();
        break;
    }
    case VPPS_Pause: {
        break;
    }
    default: {
        return;
        break;
    }
    }
}

void VideoPlayerPrivate::videoPlayerSetExit(const DeviceWatcherType type)
{
    if (type == m_DiskType) {
        exitVideoPlayer();
    }
}

void VideoPlayerPrivate::exitVideoPlayer()
{
    if (QProcess::NotRunning != m_VideoPlayer->state()) {
        m_VideoPlayer->write("quit\n");
        m_VideoPlayer->waitForBytesWritten();
        m_VideoPlayer->waitForFinished();
        setPlayStatus(VPPS_Exit);
    }
}
