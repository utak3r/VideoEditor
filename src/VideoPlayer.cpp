#include "VideoPlayer.h"
#include <QMediaMetaData>
#include <QVideoSink>

// https://doc.qt.io/qt-6/qgraphicsview.html
// https://doc.qt.io/qt-6/qgraphicsscene.html
// https://doc.qt.io/qt-6/qmediaplayer.html
// https://doc.qt.io/qt-6/qgraphicsvideoitem.html
// https://doc.qt.io/qt-6/qgraphicsitem.html
// https://doc.qt.io/qt-6/qgraphicsrectitem.html

VideoPlayer::VideoPlayer(QWidget* parent)
: QGraphicsView(parent)
{
    // The scene container
    theScene = new QGraphicsScene(this);
    theScene->setBackgroundBrush(Qt::black);
    setScene(theScene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	// Media player with video and audio output
    theMediaPlayer = new QMediaPlayer(this);
    theAudioOutput = new QAudioOutput(this);
    theMediaPlayer->setAudioOutput(theAudioOutput);

    theVideoItem = new QGraphicsVideoItem();
	theVideoItem->setOffset(QPointF(0, 0));
	theVideoItem->setAspectRatioMode(Qt::KeepAspectRatio);
    theScene->addItem(theVideoItem);
    theMediaPlayer->setVideoOutput(theVideoItem);

	// Timestamp item
    theTimestampItem = new QGraphicsTextItem("00:00:00 / 00:00:00");
    theTimestampItem->setDefaultTextColor(Qt::white);
    QFont f = theTimestampItem->font();
    f.setPointSize(12);
    f.setBold(true);
    theTimestampItem->setFont(f);

	// Background for timestamp
    theTimestampBgItem = new QGraphicsRectItem();
    theTimestampBgItem->setBrush(QColor(0, 0, 0, 100));
    theTimestampBgItem->setPen(Qt::NoPen);

	// Crop rectangle
    theCropRectItem = new CropRectangle(QRectF(0, 0, 0, 0));
    theCropRectItem->setColor(Qt::blue);
    theCropRectItem->setOpacity(50);
    theCropRectItem->setEnabled(false);
	theScene->addItem(theCropRectItem);

    theScene->addItem(theTimestampBgItem);
    theScene->addItem(theTimestampItem);

	// Set Z-values for proper stacking order
    theVideoItem->setZValue(1);
    theTimestampBgItem->setZValue(2);
    theTimestampItem->setZValue(3);
    theCropRectItem->setZValue(4);

    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::NoDrag);

    theAspectMode = AspectMode::FitInView;
    theDuration = 0;

    theMarksRange = "---";

    connect(theMediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::updatePosition);
    connect(theMediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::updateDuration);

	theMediaPlayer->setLoops(QMediaPlayer::Infinite);

    //resizeScene();
    repositionTimestamp();

	// properly resize the scene when the video size changes (or it's being loaded for the first time)
    connect(theMediaPlayer->videoSink(), &QVideoSink::videoSizeChanged, this, &VideoPlayer::resizeScene);

    connect(theCropRectItem, &CropRectangle::rectChanged, this, [=](const QRectF& rect)
        {
            emit CropWindowChanged(sceneRectToVideoRect(rect));
        });

}

void VideoPlayer::openFile(const QString& filename)
{
    theMediaPlayer->setSource(QUrl::fromLocalFile(filename));
}

void VideoPlayer::play()
{
    theMediaPlayer->play();
}
void VideoPlayer::pause()
{
    theMediaPlayer->pause();
}
void VideoPlayer::stop()
{
    theMediaPlayer->stop();
}

QMediaPlayer* VideoPlayer::player() const
{
    return theMediaPlayer;
}

VideoPlayer::PlaybackState VideoPlayer::playbackState() const
{
    switch (theMediaPlayer->playbackState())
	{
        case QMediaPlayer::StoppedState:
			return VideoPlayer::StoppedState;
		case QMediaPlayer::PlayingState:
			return VideoPlayer::PlayingState;
		case QMediaPlayer::PausedState:
			return VideoPlayer::PausedState;
		default:
			qWarning() << "Unknown playback state!";
			return VideoPlayer::StoppedState;
	}
}

QAudioOutput* VideoPlayer::audioOutput() const
{
    return theAudioOutput;
}

void VideoPlayer::setAspectMode(AspectMode mode)
{
    if (theAspectMode != mode)
    {
        theAspectMode = mode;
        resizeScene();
    }
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    resizeScene();
    repositionTimestamp();
}

void VideoPlayer::updatePosition(qint64 position)
{
	emit positionChanged(position);
	updateTimestamp(position);
}

void VideoPlayer::updateTimestamp(qint64 ms)
{
    QTime current(0, 0);
    current = current.addMSecs(static_cast<int>(ms));

    QTime total(0, 0);
    total = total.addMSecs(static_cast<int>(theDuration));

    QString text = QString("%1 / %2")
        .arg(current.toString("hh:mm:ss"))
        .arg(total.toString("hh:mm:ss"));

    theTimestampItem->setPlainText(text);
    repositionTimestamp();
}

qint64 VideoPlayer::duration() const
{
	return theDuration;
}

void VideoPlayer::updateDuration(qint64 duration)
{
    theDuration = duration;
	emit durationChanged(duration);
    updateTimestamp(theMediaPlayer->position());
}

qint64 VideoPlayer::position() const
{
    return theMediaPlayer->position();
}

void VideoPlayer::setPosition(qint64 position)
{
    theMediaPlayer->setPosition(position);
}

void VideoPlayer::resizeScene()
{
    QSizeF viewSize = viewport()->size();
    QSize videoSize = theMediaPlayer->videoSink()->videoSize();

    if (!videoSize.isValid())
    {
        theScene->setSceneRect(QRectF(QPointF(0, 0), viewSize));
        return;
    }

    qreal videoAspect = qreal(videoSize.width()) / qreal(videoSize.height());
    qreal viewAspect = viewSize.width() / viewSize.height();

    QRectF targetRect;
    if (videoAspect > viewAspect)
    {
        // fit to width
        qreal w = viewSize.width();
        qreal h = w / videoAspect;
        qreal y = (viewSize.height() - h) / 2.0;
        targetRect = QRectF(0, y, w, h);
    }
    else
    {
        // fit to height
        qreal h = viewSize.height();
        qreal w = h * videoAspect;
        qreal x = (viewSize.width() - w) / 2.0;
        targetRect = QRectF(x, 0, w, h);
    }

    theVideoItem->setSize(targetRect.size());
    theVideoItem->setPos(targetRect.topLeft());

    theScene->setSceneRect(QRectF(QPointF(0, 0), viewSize));
}

void VideoPlayer::repositionTimestamp()
{
    if (!theTimestampItem || !theTimestampBgItem) return;

    QRectF sceneRect = theScene->sceneRect();
    QRectF textRect = theTimestampItem->boundingRect();

    const qreal margin = 10;
    qreal x = sceneRect.left() + margin;
    qreal y = sceneRect.bottom() - textRect.height() - margin;

    theTimestampItem->setPos(x, y);

    QRectF bgRect = theTimestampItem->boundingRect();
    bgRect.translate(x, y);
    bgRect.adjust(-5, -2, +5, +2);
    theTimestampBgItem->setRect(bgRect);
}

void VideoPlayer::setMarkers(const QString& range)
{
    theMarksRange = range;
}

bool VideoPlayer::getCropEnabled()
{
	return theCropEnabled;
}

void VideoPlayer::setCropEnabled(bool enabled)
{
    if (theCropEnabled != enabled)
    {
        theCropEnabled = enabled;
        if (theCropEnabled)
            if (theCropRectItem->rect().isEmpty())
            {
                // If crop rectangle is empty, set it to the video item size
                QRectF videoRect = QRectF(theVideoItem->pos() /* + theVideoItem->offset()*/, theVideoItem->size());
                theCropRectItem->setRect(videoRect);
                emit CropWindowChanged(sceneRectToVideoRect(videoRect));
		    }
        theCropRectItem->setBoundingBorders(QRectF(theVideoItem->pos() /* + theVideoItem->offset()*/, theVideoItem->size()));
		theCropRectItem->setEnabled(enabled);
        emit CropEnabledChanged(enabled);
	}
}

QRect VideoPlayer::sceneRectToVideoRect(QRectF sceneRect) const
{
    QRect retrect = sceneRect.toRect();

    QPointF videoPos = theVideoItem->pos() + theVideoItem->offset();
    QRectF videoRect = QRectF(videoPos, theVideoItem->size());

    retrect.moveTopLeft(QPoint(retrect.x() - videoPos.x(), retrect.y() - videoPos.y()));

    QSize videoSize = theVideoItem->nativeSize().toSize();
    if (videoSize.isValid() && !videoRect.isEmpty())
    {
        double scaleX = static_cast<double>(videoSize.width()) / videoRect.width();
        double scaleY = static_cast<double>(videoSize.height()) / videoRect.height();
        double scale = qMax(scaleX, scaleY);
        int width = static_cast<int>(retrect.width() * scaleX);
        int height = static_cast<int>(retrect.height() * scaleY);
        retrect.setX(static_cast<int>(retrect.x() * scaleX));
        retrect.setY(static_cast<int>(retrect.y() * scaleY));
        retrect.setWidth(width);
        retrect.setHeight(height);
    }

    return retrect;
}

QRectF VideoPlayer::videoRectToSceneRect(QRect videoRect) const
{
    QRect retrect = videoRect;

    QSize videoSize = theVideoItem->nativeSize().toSize();
    QSize videoSceneSize = theVideoItem->size().toSize();
    double scaleX = static_cast<double>(videoSize.width()) / videoSceneSize.width();
    double scaleY = static_cast<double>(videoSize.height()) / videoSceneSize.height();
    int width = static_cast<int>(retrect.width() / scaleX);
    int height = static_cast<int>(retrect.height() / scaleY);
    retrect.setX(static_cast<int>(retrect.x() / scaleX) + theVideoItem->pos().x());
    retrect.setY(static_cast<int>(retrect.y() / scaleY) + theVideoItem->pos().y());
    retrect.setWidth(width);
    retrect.setHeight(height);

    return QRectF(retrect);
}

QRect VideoPlayer::getCropWindow() const
{
	this->scene()->update();
    return sceneRectToVideoRect(QRectF(theCropRectItem->rect().topLeft(), theCropRectItem->rect().size()));
}

void VideoPlayer::setCropWindow(const QRect& cropRect)
{
    if (theCropRectItem)
        theCropRectItem->setRect(videoRectToSceneRect(cropRect));
}