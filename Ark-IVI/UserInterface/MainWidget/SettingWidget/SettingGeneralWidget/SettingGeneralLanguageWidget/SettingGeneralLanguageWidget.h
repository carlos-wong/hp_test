#ifndef SETTINGGENERALLANGUAGEWIDGET_H
#define SETTINGGENERALLANGUAGEWIDGET_H

#include "BusinessLogic/Setting.h"
#include <QWidget>

class SettingGeneralLanguageWidgetPrivate;
class SettingGeneralLanguageWidget
        : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(SettingGeneralLanguageWidget)
public:
    explicit SettingGeneralLanguageWidget(QWidget* parent = NULL);
    ~SettingGeneralLanguageWidget();
protected:
    void resizeEvent(QResizeEvent* event);
protected slots:
    void onLanguageTranslateChange(const int language);
private slots:
    void onListViewItemRelease(const int index);
private:
    Q_DECLARE_PRIVATE(SettingGeneralLanguageWidget)
    SettingGeneralLanguageWidgetPrivate* const d_ptr;
};

#endif // SETTINGGENERALLANGUAGEWIDGET_H
