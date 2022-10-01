#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QPainter>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QGraphicsView {parent}
    , theVideoSize(QSizeF(853,480))
    , theCropEnabled(true)
    , theCropColor(QColor(Qt::white))
    , theCropHandleSize(8)
{
    theScene = new QGraphicsScene(this);
    theScene->setBackgroundBrush(Qt::black);
    theVideoItem = new QGraphicsVideoItem();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setScene(theScene);
    theScene->addItem(theVideoItem);
}

VideoPlayer::~VideoPlayer()
{
    delete theVideoItem;
    delete theScene;
}

QObject* VideoPlayer::getVideoOutput()
{
    return theVideoItem;
}

bool VideoPlayer::getCropEnabled()
{
    return theCropEnabled;
}

void VideoPlayer::setCropEnabled(bool enabled)
{
    theCropEnabled = enabled;
    emit CropEnabledChanged(theCropEnabled);
}

void VideoPlayer::resizeEvent(QResizeEvent *event)
{
    if (theVideoItem)
    {
        // There's still an issue, when resizing the window.
        // The rendered video frame can become not centered,
        // despite the itemsBoundingRect() is proper.
        // If we use fitInView without setSize, it works,
        // but itemsBoundingRect will return 0
        // we need it for proper cropping tool drawing.

        theVideoSize = theVideoItem->nativeSize();
        theVideoItem->setSize(size());
        fitInView(theScene->itemsBoundingRect(), Qt::KeepAspectRatio);
        centerOn(theVideoItem);
    }
}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    QPainter painter(this->viewport());
    QSizeF videoSize = theVideoItem->nativeSize();

    qDebug() << "paintEvent: itemsBoundingRect: " << theScene->itemsBoundingRect();
    QRectF videoViewRectF = theScene->itemsBoundingRect();
    QRect videoViewRect = QRect(videoViewRectF.x(), videoViewRectF.y(),
                                videoViewRectF.width(), videoViewRectF.height());

    if (videoSize != theVideoSize)
    {
        theVideoSize = videoSize;
        emit VideoSizeChanged(theVideoSize);
    }

    if (theCropEnabled)
    {
        paintCrop(&painter, videoViewRect);
    }
}

void VideoPlayer::paintCrop(QPainter *painter, QRect rect)
{
    paintCropRectangle(painter, rect);
    paintCropHandles(painter, rect);
}

void VideoPlayer::paintCropRectangle(QPainter *painter, QRect rect)
{
    QPen linePen;
    linePen.setColor(theCropColor);
    linePen.setStyle(Qt::DashLine);
    linePen.setWidth(2);

    painter->setPen(linePen);
    painter->drawRect(rect);
}

void VideoPlayer::paintCropHandles(QPainter *painter, QRect rect)
{
    QPen handlePen;
    handlePen.setColor(theCropColor);
    handlePen.setStyle(Qt::SolidLine);
    handlePen.setWidth(2);
    QBrush handleBrush;
    handleBrush.setColor(theCropColor);
    handleBrush.setStyle(Qt::SolidPattern);
    painter->setPen(handlePen);
    painter->setBrush(handleBrush);

    painter->drawRect(rect.x()-theCropHandleSize/2, rect.y()-theCropHandleSize/2, theCropHandleSize, theCropHandleSize);
    painter->drawRect(rect.x()+rect.width()-theCropHandleSize/2, rect.y()-theCropHandleSize/2, theCropHandleSize, theCropHandleSize);
    painter->drawRect(rect.x()-theCropHandleSize/2, rect.y()+rect.height()-theCropHandleSize/2, theCropHandleSize, theCropHandleSize);
    painter->drawRect(rect.x()+rect.width()-theCropHandleSize/2, rect.y()+rect.height()-theCropHandleSize/2, theCropHandleSize, theCropHandleSize);

}

