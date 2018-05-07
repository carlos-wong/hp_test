#ifndef SETTINGGENERALLISTVIEW_H
#define SETTINGGENERALLISTVIEW_H

#include "CustomListView.h"
#include <QScopedPointer>

class SettingGeneralListViewPrivate;
class SettingGeneralListView : public CustomListView
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralListView)
public:
    explicit SettingGeneralListView(QWidget* parent = NULL);
    ~SettingGeneralListView();
private:
    friend class SettingGeneralListViewPrivate;
    QScopedPointer<SettingGeneralListViewPrivate> m_Private;
};

#endif // SETTINGGENERALLISTVIEW_H
