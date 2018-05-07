#ifndef LINKMESSAGETIPWIDGET_H
#define LINKMESSAGETIPWIDGET_H

#include "Link.h"
#include <QWidget>

class LinkMessageTipWidgetPrivate;
class LinkMessageTipWidget
        : private QWidget
        , private Link::Interface
{
    Q_OBJECT
    Q_DISABLE_COPY(LinkMessageTipWidget)
public:
    explicit LinkMessageTipWidget(QWidget *parent = NULL);
    ~LinkMessageTipWidget();
protected:
    void resizeEvent(QResizeEvent* event) final;
protected slots:
    void onLinkStatusChange(const int type, const int status) final;
    //  void onLinkStatusChange(const Link_Type type, const Link_STATUS status);
private:
    LinkMessageTipWidgetPrivate* const d_ptr = NULL;
    Q_DECLARE_PRIVATE(LinkMessageTipWidget)
};

#endif // LINKMESSAGETIPWIDGET_H
