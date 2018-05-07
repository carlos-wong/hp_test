#include "MusicInformationWidget.h"
#include "BusinessLogic/Widget.h"
#include "TextWidget.h"
#include "BmpWidget.h"
#include "AutoConnect.h"
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <tag.h>
#include <fileref.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <mpegfile.h>
#include <mp4file.h>
#include <flac/flacfile.h>

namespace SourceString {
static const QString Unknown(QObject::tr("Unknown"));
}

class MusicInformationWidgetPrivate
{
public:
    explicit MusicInformationWidgetPrivate(MusicInformationWidget* parent);
    ~MusicInformationWidgetPrivate();
    void initializeParent();
    void initializeBasic();
    void connectAllSlots();
private:
    Q_DECLARE_PUBLIC(MusicInformationWidget)
    MusicInformationWidget* const q_ptr;
    QString m_FilePath;
    QPixmap m_CoverIcon;
    TextWidget* m_File;
    BmpWidget* m_TitleIcon;
    TextWidget* m_TitleText;
    BmpWidget* m_ArtistIcon;
    TextWidget* m_ArtistText;
    BmpWidget* m_AlbumIcon;
    TextWidget* m_AlbumText;
    BmpWidget* m_AlbumBackIcon;
};

MusicInformationWidget::MusicInformationWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MusicInformationWidgetPrivate(this))
{
}

MusicInformationWidget::~MusicInformationWidget()
{
    if (NULL != d_ptr) {
        delete d_ptr;
    }
}

void MusicInformationWidget::resizeEvent(QResizeEvent *event)
{
    g_Widget->geometryFit(66, 87, 668, 262, this);
}

void MusicInformationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    Q_D(MusicInformationWidget);
    painter.drawPixmap(g_Widget->geometryFit(25, 23, 154, 155), d->m_CoverIcon);
}

void MusicInformationWidget::onMusicPlayerID3TagChange(const int type, const int index, const QString &fileName, const QString& title, const QString& artist, const QString& album, const int endTime)
{
#ifndef DESKTOP_AMD64
    Q_D(MusicInformationWidget);
    QFileInfo fileInfo(fileName);
    if (d->m_FilePath != fileName) {
        d->m_FilePath = fileName;
        QImage cover;
        if (fileInfo.suffix().contains(QString("mp3"), Qt::CaseInsensitive)) {
            TagLib::MPEG::File mpegFile(fileName.toLocal8Bit().constData(), false);
            if (mpegFile.isValid()) {
                TagLib::ID3v2::Tag* tag = mpegFile.ID3v2Tag();
                if (NULL != tag) {
                    TagLib::ID3v2::FrameList framelist = tag->frameListMap()["APIC"];
                    if (!framelist.isEmpty()) {
                        TagLib::ID3v2::Frame* frame = framelist.front();
                        if (NULL != frame) {
                            TagLib::ID3v2::AttachedPictureFrame* attachedpictureframe = reinterpret_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame);
                            if ((NULL != attachedpictureframe)
                                    && (0 != frame->toString().size())
                                    && !(attachedpictureframe->picture().isNull())) {
                                if (cover.loadFromData(QByteArray::fromRawData(attachedpictureframe->picture().data(), attachedpictureframe->picture().size()))) {
                                    d->m_CoverIcon = d->m_CoverIcon.fromImage(cover);
                                }
                            }
                        }
                    }
                }
            }
        } else if (fileInfo.suffix().contains(QString("flac"), Qt::CaseInsensitive)) {
            TagLib::FLAC::File flacFile(fileName.toLocal8Bit().constData());
            if (flacFile.isValid()) {
                TagLib::List<TagLib::FLAC::Picture*> list = flacFile.pictureList();
                if (!list.isEmpty()) {
                    TagLib::FLAC::Picture* picture = list.front();
                    if ((NULL != picture)
                            && (!picture->data().isEmpty())) {
                        if (cover.loadFromData(QByteArray::fromRawData(picture->data().data(), picture->data().size()))) {
                            d->m_CoverIcon = d->m_CoverIcon.fromImage(cover);
                        }
                    }
                }
            }
        }
        if (cover.isNull()) {
             d->m_CoverIcon.load(QString(""));
        }
    }
    d->m_File->setText(fileInfo.fileName());
    if (title.isEmpty()) {
        d->m_TitleText->setText(QObject::tr(SourceString::Unknown.toLocal8Bit().constData()));
    } else {
        d->m_TitleText->setText(title);
    }
    if (artist.isEmpty()) {
        d->m_ArtistText->setText(QObject::tr(SourceString::Unknown.toLocal8Bit().constData()));
    } else {
        d->m_ArtistText->setText(artist);
    }
    if (album.isEmpty()) {
        d->m_AlbumText->setText(QObject::tr(SourceString::Unknown.toLocal8Bit().constData()));
    } else {
        d->m_AlbumText->setText(album);
    }
    update();
#endif
}

MusicInformationWidgetPrivate::MusicInformationWidgetPrivate(MusicInformationWidget *parent)
    : q_ptr(parent)
{
    m_File = NULL;
    m_TitleIcon = NULL;
    m_TitleText = NULL;
    m_ArtistIcon = NULL;
    m_ArtistText = NULL;
    m_AlbumIcon = NULL;
    m_AlbumText = NULL;    
    m_AlbumBackIcon = NULL;
    initializeBasic();
    initializeParent();
    connectAllSlots();
}

MusicInformationWidgetPrivate::~MusicInformationWidgetPrivate()
{
}

void MusicInformationWidgetPrivate::initializeParent()
{
    Q_Q(MusicInformationWidget);
    q->setVisible(true);
}

void MusicInformationWidgetPrivate::initializeBasic()
{
    Q_Q(MusicInformationWidget);
    m_AlbumBackIcon = new BmpWidget(q);
    m_AlbumBackIcon->setBackgroundBmpPath(":/Images/hw_01_multimedia_CD.png");
    m_AlbumBackIcon->setWidgetAdaptToImage(true);

//    m_CoverIcon.load(QString(":/Images/hw_01_theme_main_04.png"));

    m_File = new TextWidget(q);
    m_File->setLanguageType(TextWidget::T_NoTranslate);
    m_File->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    m_File->setFontPointSize(26 * g_Widget->widthScalabilityFactor());
    g_Widget->geometryFit(329, 0, 339, 81, m_File);
    m_File->setAnimation(true);
    m_File->setVisible(true);

    m_TitleIcon = new BmpWidget(q);
    m_TitleIcon->setBackgroundBmpPath(QString(":/Images/hw_01_multimedia_music_name.png"));
    g_Widget->MoveToPoint(296, 81, m_TitleIcon);
    m_TitleIcon->setWidgetAdaptToImage(true);
    m_TitleIcon->setVisible(true);

    m_TitleText = new TextWidget(q);
    m_TitleText->setLanguageType(TextWidget::T_NoTranslate);
    m_TitleText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    g_Widget->geometryFit(329, 81, 339, 31, m_TitleText);
    m_TitleText->setVisible(true);

    m_ArtistIcon = new BmpWidget(q);
    m_ArtistIcon->setBackgroundBmpPath(QString(":/Images/hw_01_multimedia_music_singer.png"));
    g_Widget->MoveToPoint(296, 120, m_ArtistIcon);
    m_ArtistIcon->setWidgetAdaptToImage(true);
    m_ArtistIcon->setVisible(true);

    m_ArtistText = new TextWidget(q);
    m_ArtistText->setLanguageType(TextWidget::T_NoTranslate);
    m_ArtistText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    g_Widget->geometryFit(329, 120, 339, 31, m_ArtistText);
    m_ArtistText->setVisible(true);

    m_AlbumIcon = new BmpWidget(q);
    m_AlbumIcon->setBackgroundBmpPath(QString(":/Images/hw_01_multimedia_music_special.png"));
    g_Widget->MoveToPoint(296, 159, m_AlbumIcon);
    m_AlbumIcon->setWidgetAdaptToImage(true);
    m_AlbumIcon->setVisible(true);

    m_AlbumText = new TextWidget(q);
    m_AlbumText->setLanguageType(TextWidget::T_NoTranslate);
    m_AlbumText->setAlignmentFlag(Qt::AlignLeft | Qt::AlignVCenter);
    g_Widget->geometryFit(329, 159, 339, 31, m_AlbumText);
    m_AlbumText->setVisible(true);
}

void MusicInformationWidgetPrivate::connectAllSlots()
{
    Q_Q(MusicInformationWidget);
    connectSignalAndSlotByNamesake(g_Multimedia, q, ARKRECEIVER(onMusicPlayerID3TagChange(const int, const int, const QString &, const QString&, const QString&, const QString&, const int)));
}
