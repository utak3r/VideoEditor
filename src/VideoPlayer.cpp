#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QPainter>
#include <QMouseEvent>

VideoPlayer::VideoPlayer(QWidget *parent)
    : QGraphicsView {parent}
    , theVideoSize(QSizeF(853,480))
    , theCurrentCropState(VPCropState_Inactive)
    , theCropEnabled(false)
    , theCropColor(QColor(Qt::white))
    , theCropHandleSize(8)
{
    theScene = new QGraphicsScene(this);
    theScene->setBackgroundBrush(Qt::black);
    theVideoItem = new QGraphicsVideoItem();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setScene(theScene);
    theScene->addItem(theVideoItem);
    setAlignment(Qt::AlignCenter);
    theVideoViewRectF = QRectF(0, 0, size().width(), size().height());
    theCropRectF = QRectF(0, 0, 0, 0);

    theCurrentCropState = (theCropEnabled) ? VPCropState_Active : VPCropState_Inactive;
    setMouseTracking(true);
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
    theCurrentCropState = (theCropEnabled) ? VPCropState_Active : VPCropState_Inactive;
    if (theCropEnabled)
    {
        theCropRectF = calculateVideoViewRect();
    }
    repaint();
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

/*!
 * \brief VideoPlayer::resizeEvent
 */
void VideoPlayer::resizeEvent(QResizeEvent *event)
{
    if (theVideoItem)
    {
        theVideoSize = theVideoItem->nativeSize();
        theVideoItem->setOffset(QPointF(0, 0));
        theVideoItem->setSize(size());
        fitInView(theScene->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
}

/*!
 * \brief VideoPlayer::paintEvent Renders video and draws a crop tool if enabled.
 */
void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    QSizeF videoSize = theVideoItem->nativeSize();
    if (videoSize != theVideoSize)
    {
        theVideoSize = videoSize;
        emit VideoSizeChanged(theVideoSize);
    }

    theVideoViewRectF = calculateVideoViewRect();
    if (theCropEnabled)
    {
        QPainter painter(this->viewport());
        paintCrop(&painter);
    }
}

/*!
 * \brief VideoPlayer::paintCrop Draws a crop tool
 */
void VideoPlayer::paintCrop(QPainter *painter)
{
    paintCropRectangle(painter);
    paintCropHandles(painter);
    //qDebug() << "Crop: " << mapToVideo(rect.topLeft()) << ", " << mapToVideo(rect.bottomRight());
}

/*!
 * \brief VideoPlayer::paintCropRectangle Draws a rectangle of a crop tool.
 */
void VideoPlayer::paintCropRectangle(QPainter *painter)
{
    QPen linePen;
    linePen.setColor(theCropColor);
    linePen.setStyle(Qt::DashLine);
    linePen.setWidth(2);

    painter->setPen(linePen);
    painter->drawRect(theCropRectF);
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a top left handle of a crop tool.
 */
QRect VideoPlayer::cropHandleTLRect()
{
    return QRect(theCropRectF.x()-theCropHandleSize/2,
                 theCropRectF.y()-theCropHandleSize/2,
                 theCropHandleSize,
                 theCropHandleSize);
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a top right handle of a crop tool.
 */
QRect VideoPlayer::cropHandleTRRect()
{
    return QRect(theCropRectF.x()+theCropRectF.width()-theCropHandleSize/2,
                 theCropRectF.y()-theCropHandleSize/2,
                 theCropHandleSize,
                 theCropHandleSize
               );
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a bottom left handle of a crop tool.
 */
QRect VideoPlayer::cropHandleBLRect()
{
    return QRect(theCropRectF.x()-theCropHandleSize/2,
                 theCropRectF.y()+theCropRectF.height()-theCropHandleSize/2,
                 theCropHandleSize,
                 theCropHandleSize
               );
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a bottom right handle of a crop tool.
 */
QRect VideoPlayer::cropHandleBRRect()
{
    return QRect(theCropRectF.x()+theCropRectF.width()-theCropHandleSize/2,
                 theCropRectF.y()+theCropRectF.height()-theCropHandleSize/2,
                 theCropHandleSize,
                 theCropHandleSize
               );
}

/*!
 * \brief VideoPlayer::paintCropHandles Draws all handles of a crop tool.
 */
void VideoPlayer::paintCropHandles(QPainter *painter)
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

    painter->drawRect(cropHandleTLRect());
    painter->drawRect(cropHandleTRRect());
    painter->drawRect(cropHandleBLRect());
    painter->drawRect(cropHandleBRRect());

}

/*!
 * \brief VideoPlayer::isInsideCrop Checks if a given point is inside a crop tool.
 * \param point Point to be checked.
 * \return True, if inside a crop tool, false otherwise.
 */
bool VideoPlayer::isInsideCrop(QPointF point)
{
    return theCropRectF.contains(point);
}

std::tuple<bool, VideoPlayerCropHandle> VideoPlayer::isOverCropHandle(QPointF point)
{
    bool isOver = false;
    VideoPlayerCropHandle which = VPCropHandle_None;

    if (cropHandleTLRect().contains(point.toPoint()))
    {
        isOver = true;
        which = VPCropHandle_TopLeft;
    }
    else if (cropHandleTRRect().contains(point.toPoint()))
    {
        isOver = true;
        which = VPCropHandle_TopRight;
    }
    else if (cropHandleBLRect().contains(point.toPoint()))
    {
        isOver = true;
        which = VPCropHandle_BottomLeft;
    }
    else if (cropHandleBRRect().contains(point.toPoint()))
    {
        isOver = true;
        which = VPCropHandle_BottomRight;
    }

    return std::make_tuple(isOver, which);
}

void VideoPlayer::mousePressEvent(QMouseEvent *event)
{
    if (theCurrentCropState == VPCropState_Active && this->rect().contains(event->pos()))
    {
        if (event->button() == Qt::LeftButton)
        {
            std::tuple<bool, VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
            if (std::get<0>(overHandle))
            {
                if (std::get<1>(overHandle) == VPCropHandle_TopLeft)
                    theCurrentCropState = VPCropState_ResizingTL;
                else if (std::get<1>(overHandle) == VPCropHandle_TopRight)
                    theCurrentCropState = VPCropState_ResizingTR;
                else if (std::get<1>(overHandle) == VPCropHandle_BottomLeft)
                    theCurrentCropState = VPCropState_ResizingBL;
                else if (std::get<1>(overHandle) == VPCropHandle_BottomRight)
                    theCurrentCropState = VPCropState_ResizingBR;
                event->accept();
            }
            else
            {
                theCurrentCropState = VPCropState_Translating;
                event->accept();
            }
        }
    }
}

void VideoPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && this->rect().contains(event->pos()))
    {
        if (theCurrentCropState == VPCropState_ResizingTL)
        {
            theCurrentCropState = VPCropState_Active;
            event->accept();
        }
        if (theCurrentCropState == VPCropState_ResizingTR)
        {
            theCurrentCropState = VPCropState_Active;
            event->accept();
        }
        if (theCurrentCropState == VPCropState_ResizingBL)
        {
            theCurrentCropState = VPCropState_Active;
            event->accept();
        }
        if (theCurrentCropState == VPCropState_ResizingBR)
        {
            theCurrentCropState = VPCropState_Active;
            event->accept();
        }
        else if (theCurrentCropState == VPCropState_Translating)
        {
            theCurrentCropState = VPCropState_Active;
            event->accept();
        }
    }
}

void VideoPlayer::mouseMoveEvent(QMouseEvent *event)
{
    if (theCurrentCropState == VPCropState_Active && this->rect().contains(event->pos()))
    {
        if (isInsideCrop(event->pos()) && event->button() == Qt::NoButton)
        {
            if (cursor().shape() != Qt::SizeAllCursor)
                setCursor(Qt::SizeAllCursor);
        }
        else
        {
            if (cursor().shape() != Qt::ArrowCursor)
                setCursor(Qt::ArrowCursor);
        }

        std::tuple<bool, VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
        if (std::get<0>(overHandle) && event->button() == Qt::NoButton)
        {
            if (std::get<1>(overHandle) == VPCropHandle_TopLeft ||
                std::get<1>(overHandle) == VPCropHandle_BottomRight)
            {
                if (cursor().shape() != Qt::SizeFDiagCursor)
                    setCursor(Qt::SizeFDiagCursor);
            }
            else
            {
                if (cursor().shape() != Qt::SizeBDiagCursor)
                    setCursor(Qt::SizeBDiagCursor);
            }
        }

        if (event->button() == Qt::LeftButton)
        {
            std::tuple<bool, VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
            if (std::get<0>(overHandle))
            {
                // resizing
            }
            else
            {
                // moving
            }
        }
    }
    if (theCurrentCropState == VPCropState_ResizingBR && this->rect().contains(event->pos()))
    {
        QRectF newCrop = theCropRectF;
        newCrop.setBottomRight(event->pos());
        theCropRectF = newCrop;
        repaint();
    }
}

