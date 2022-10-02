#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
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
    setAlignment(Qt::AlignCenter);
    theVideoViewRectF = QRectF(0, 0, size().width(), size().height());
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

QSize VideoPlayer::sizeHint() const
{
    auto size = theVideoItem->videoSink()->videoSize();
    if (size.isValid())
        return size;

    return QWidget::sizeHint();
}

/*!
 * \brief VideoPlayer::calculateVideoViewRect rectangle where the video frame is being rendered.
 *
 * In general we should get this info from theScene->itemsBoundingRect()
 * But after using fitInView() somehow it becomes incorrect.
 * Thus, we have to calculate this rectangle for ourselves.
 *
 * \return Rectangle inside which the video frame is being rendered.
 */
QRectF VideoPlayer::calculateVideoViewRect()
{
    QSizeF videoViewSize = theVideoItem->nativeSize();
    videoViewSize.scale(size(), Qt::KeepAspectRatio);
    QRectF videoViewRectF = QRectF(0, 0, videoViewSize.width(), videoViewSize.height());
    videoViewRectF.moveCenter(this->rect().center());
    return videoViewRectF;
}

/*!
 * \brief VideoPlayer::mapToVideo map a point from the widget's coordinates to video's coordinates
 * \param point
 * \return mapped point in video's coordinates
 */
QPointF VideoPlayer::mapToVideo(QPointF point)
{
    QPointF mappedPoint = point;
    QRectF viewRect = calculateVideoViewRect();
    double scale = theVideoItem->nativeSize().width() / viewRect.width();
    mappedPoint -= viewRect.topLeft();
    mappedPoint *= scale;
    return mappedPoint;
}

/*!
 * \brief VideoPlayer::mapToVideo map a point from the video's coordinates to widget's coordinates
 * \param point
 * \return mapped point in widget's coordinates
 */
QPointF VideoPlayer::mapFromVideo(QPointF point)
{
    QPointF mappedPoint = point;
    QRectF viewRect = calculateVideoViewRect();
    double scale = theVideoItem->nativeSize().width() / viewRect.width();
    mappedPoint /= scale;
    mappedPoint += viewRect.topLeft();
    return mappedPoint;
}

void VideoPlayer::resizeEvent(QResizeEvent *event)
{
    if (theVideoItem)
    {
        theVideoSize = theVideoItem->nativeSize();
        theVideoItem->setOffset(QPointF(0, 0));
        fitInView(theScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    QSizeF videoSize = theVideoItem->nativeSize();
    if (videoSize != theVideoSize)
    {
        theVideoSize = videoSize;
        emit VideoSizeChanged(theVideoSize);
    }

    if (theCropEnabled)
    {
        QPainter painter(this->viewport());
        theVideoViewRectF = calculateVideoViewRect();
        paintCrop(&painter, theVideoViewRectF.toRect());

        qDebug() << "Crop: " << mapToVideo(theVideoViewRectF.topLeft()) << ", " << mapToVideo(theVideoViewRectF.bottomRight());
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

