#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QVideoWidget>
#include <QGraphicsView>

class QGraphicsScene;
class QGraphicsVideoItem;

enum VideoPlayerCropHandle
{
    VPCropHandle_None,
    VPCropHandle_TopLeft,
    VPCropHandle_TopRight,
    VPCropHandle_BottomLeft,
    VPCropHandle_BottomRight
};

class VideoPlayer : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(bool CropEnabled READ getCropEnabled WRITE setCropEnabled NOTIFY CropEnabledChanged)

public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

    QObject* getVideoOutput();
    bool getCropEnabled();
    void setCropEnabled(bool enabled);
    QPointF mapToVideo(QPointF point);
    QPointF mapFromVideo(QPointF point);

    QSize sizeHint() const override;

signals:
    void CropEnabledChanged(bool enabled);
    void VideoSizeChanged(QSizeF videoSize);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QRectF calculateVideoViewRect();
    void paintCrop(QPainter *painter);
    void paintCropRectangle(QPainter *painter);
    QRect cropHandleTLRect();
    QRect cropHandleTRRect();
    QRect cropHandleBLRect();
    QRect cropHandleBRRect();
    void paintCropHandles(QPainter *painter);
    bool isInsideCrop(QPointF point);
    std::tuple<bool, VideoPlayerCropHandle> isOverCropHandle(QPointF point);

    QGraphicsScene* theScene;
    QGraphicsVideoItem* theVideoItem;
    QSizeF theVideoSize;
    QRectF theVideoViewRectF;
    QRectF theCropRectF;

    bool theCropEnabled;
    QColor theCropColor;
    int theCropHandleSize;
};

#endif // VIDEOPLAYER_H
