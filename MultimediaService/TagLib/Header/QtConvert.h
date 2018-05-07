#ifndef QTCONVERT_H
#define QTCONVERT_H

#include "toolkit/tstring.h"
#include <QString>
#include <QImage>

QString codecConvert(TagLib::String inStr);
QImage getCover(const QString& path);

#endif // QTCONVERT_H
