#ifndef USERINTERFACEUTILITY_H
#define USERINTERFACEUTILITY_H

#include <QObject>
#include <QBrush>
#include <QColor>
#include <QTime>
#include <QDebug>
#include <QByteArray>
#include <QString>

class UserInterfaceUtility
{
public:
    static QBrush listViewItemBrush();
    static QColor customBlackColor();
    static QColor videoColor();
    static QColor mainWidgetBackgroundColor();
    static QColor multimediaAlphaColor();
    static void startTime();
    static void elapsed(const QString &str);
    static bool loadFromdata(const QString& path, QByteArray& data);
private:
    UserInterfaceUtility() = delete;
    ~UserInterfaceUtility() = delete;
    UserInterfaceUtility(const UserInterfaceUtility &utility) = delete;
    UserInterfaceUtility& operator =(const UserInterfaceUtility &utility) = delete;
    static QTime time;
};

#endif // USERINTERFACEUTILITY_H
