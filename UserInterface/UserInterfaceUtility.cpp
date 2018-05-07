#include "UserInterfaceUtility.h"
#include <QFile>
#include <QDataStream>

QTime UserInterfaceUtility::time;

QBrush UserInterfaceUtility::listViewItemBrush()
{
    return QBrush(QColor(0, 94, 187));
}

QColor UserInterfaceUtility::customBlackColor()
{
    return QColor(0, 0, 1);
}

QColor UserInterfaceUtility::videoColor()
{
    return QColor(0, 0, 0);
}

QColor UserInterfaceUtility::mainWidgetBackgroundColor()
{
    return QColor(16, 6, 119);
}

QColor UserInterfaceUtility::multimediaAlphaColor()
{
    return QColor(0, 94, 187);
}

void UserInterfaceUtility::startTime()
{
    time.restart();
}

void UserInterfaceUtility::elapsed(const QString &str)
{
    qWarning() << "UserInterfaceUtility::elapsed" << time.elapsed() << str;
}

bool UserInterfaceUtility::loadFromdata(const QString &path, QByteArray &data)
{
    bool ret(false);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        data.clear();
        stream >> data;
        file.close();
        ret = true;
    }
    return ret;
}
