#include "MusicRoundImage.h"
#include <QPainter>

MusicRoundImage::MusicRoundImage(QWidget *parent) :
    QLabel(parent)
{
}

MusicRoundImage::~MusicRoundImage()
{
}

void MusicRoundImage::paintEvent(QPaintEvent *e)
{
    if(pixmap()){
        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        QPainterPath path;
        int round = qMin(width(), height());
        path.addEllipse(0, 0, round, round);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, width(), height(), *pixmap());
    }else{
        QLabel::paintEvent(e);
    }
}
