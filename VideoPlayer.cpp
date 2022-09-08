#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget *parent)
    : QVideoWidget{parent}
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    setPalette(p);

#ifndef Q_OS_ANDROID // QTBUG-95723
    setAttribute(Qt::WA_OpaquePaintEvent);
#endif
}
