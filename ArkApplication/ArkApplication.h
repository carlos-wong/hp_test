#ifndef ARKAPPLICATION_H
#define ARKAPPLICATION_H

#include <QApplication>
#include <QScopedPointer>
#include <QDateTime>

#if defined(ArkApp)
#undef ArkApp
#endif
#define ArkApp (static_cast<ArkApplication *>(QCoreApplication::instance()))

class ArkApplicationPrivate;
class ArkApplication
        : public QApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(ArkApplication)
public:
    explicit ArkApplication(int &argc, char **argv, QApplication::Type type = QApplication::Tty);
    ~ArkApplication();
    bool installTranslatorPath(const QString& path);
    int size() const;
    QStringList arguments() const;
    void startTimer(const int second = 1);
public slots:
    void onTimeout();
signals:
    void currentDateTime(const QDateTime &dateTime);
private:
    friend class ArkApplicationPrivate;
    QScopedPointer<ArkApplicationPrivate> m_Private;
};

#endif // ARKAPPLICATION_H
