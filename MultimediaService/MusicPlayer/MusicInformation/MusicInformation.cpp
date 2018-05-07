#include "MusicInformation.h"
#include "attachedpictureframe.h"
#include "mpegfile.h"
#include "id3v2tag.h"
#include "fileref.h"
#include "tag.h"
#include "flacmetadatablock.h"
#include "flacfile.h"
#include "tstring.h"
#include "mp4file.h"
#include "mp4item.h"
#include "mp4coverart.h"
#include "mp4tag.h"
#include "apefile.h"
#include "apetag.h"
#include "apeitem.h"
#include "oggfile.h"
#include "vorbisfile.h"
#include "xiphcomment.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QImage>

//QString codecConvert(TagLib::String inStr)
//{
//    return QString::fromStdString(inStr.toCString(true));
//}

class MusicInformationPrivate
{
    Q_DISABLE_COPY(MusicInformationPrivate)
public:
    MusicInformationPrivate();
    ~MusicInformationPrivate();
    void parseString(const QString &path);
    QString m_Title;
    QString m_Artist;
    QString m_Album;
};

MusicInformation::MusicInformation()
    : m_Private(new MusicInformationPrivate())
{
}

MusicInformation::~MusicInformation()
{
}

void MusicInformation::parserFilePath(const QString &path)
{
    m_Private->parseString(path);
}

QString MusicInformation::getTitle()
{
    return m_Private->m_Title;
}

QString MusicInformation::getArtist()
{
    return m_Private->m_Artist;
}

QString MusicInformation::getAlbum()
{
    return m_Private->m_Album;
}

MusicInformationPrivate::MusicInformationPrivate()
{
    m_Title.clear();
    m_Artist.clear();
    m_Album.clear();
}

MusicInformationPrivate::~MusicInformationPrivate()
{
}

void MusicInformationPrivate::parseString(const QString &path)
{

}
