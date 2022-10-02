#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QVideoWidget>
#include <QGraphicsView>

class QGraphicsScene;
class QGraphicsVideoItem;

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

private:
    QRectF calculateVideoViewRect();
    void paintCrop(QPainter *painter, QRect rect);
    void paintCropRectangle(QPainter *painter, QRect rect);
    void paintCropHandles(QPainter *painter, QRect rect);

    QGraphicsScene* theScene;
    QGraphicsVideoItem* theVideoItem;
    QSizeF theVideoSize;
    QRectF theVideoViewRectF;

    bool theCropEnabled;
    QColor theCropColor;
    int theCropHandleSize;
};

#endif // VIDEOPLAYER_H
