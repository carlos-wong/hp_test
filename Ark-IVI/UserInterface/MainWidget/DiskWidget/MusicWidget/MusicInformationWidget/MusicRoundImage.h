#ifndef MUSICROUNDIMAGE_H
#define MUSICROUNDIMAGE_H

#include <QLabel>

class MusicRoundImage : public QLabel
{
    Q_OBJECT
public:
    explicit MusicRoundImage(QWidget *parent = 0);
    ~MusicRoundImage();

protected:
    virtual void paintEvent(QPaintEvent * e);
};

#endif // MUSICROUNDIMAGE_H
