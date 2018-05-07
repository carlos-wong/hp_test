#include "RadioPersistent.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QList<QVariant> list;
    list.clear();
    list.append(880);
    list.append(990);
    list.clear();
    list = RadioPersistent::getFrequencyList(RBT_FM1);
    qDebug() << list.size();
    for (int i = 0; i < list.size(); ++i) {
        qDebug() << list.at(i);
    }
    return app.exec();
}
