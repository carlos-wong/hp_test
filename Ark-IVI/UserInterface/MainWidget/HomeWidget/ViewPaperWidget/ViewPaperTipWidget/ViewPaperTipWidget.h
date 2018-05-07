#ifndef VIEWPAPERTipWidget_H
#define VIEWPAPERTipWidget_H

#include "BusinessLogic/Widget.h"
#include <QWidget>

class ViewPaperTipWidgetPrivate;
class ViewPaperTipWidget
        : private QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ViewPaperTipWidget)
public:
    explicit ViewPaperTipWidget(QWidget *parent = NULL);
    ~ViewPaperTipWidget();
protected:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);
    void paintEvent(QPaintEvent* event);
protected slots:
    void onWidgetTypeChange(const Widget::Type destinationType, const Widget::Type requestType, const QString &status);
private:
    Q_DECLARE_PRIVATE(ViewPaperTipWidget)
    ViewPaperTipWidgetPrivate* const d_ptr;
};

#endif // VIEWPAPERTipWidget_H
