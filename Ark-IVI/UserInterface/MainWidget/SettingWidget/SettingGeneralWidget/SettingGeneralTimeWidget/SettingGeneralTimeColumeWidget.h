#ifndef SETTINGGENERALTIMECOLUMNWIDGET_H
#define SETTINGGENERALTIMECOLUMNWIDGET_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class SettingGeneralTimeColumeWidgetPrivate;
class SettingGeneralTimeColumeWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralTimeColumeWidget)
public:
    explicit SettingGeneralTimeColumeWidget(QWidget* parent = NULL);
    ~SettingGeneralTimeColumeWidget();
    void setText(const QString& text);
    void replaceListView(const QList<QString> &list);
    void scrollToIndex(const int index);
    int currentIndex();
    void setEnabled(const bool flag);
signals:
    void indexChange(const int index);
private slots:
    void onListViewRelease();
private:
    Q_DECLARE_PRIVATE(SettingGeneralTimeColumeWidget)
    SettingGeneralTimeColumeWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALTIMECOLUMNWIDGET_H
