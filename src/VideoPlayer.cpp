#include "VideoPlayer.h"

VideoPlayer::VideoPlayer(QWidget* parent)
: QGraphicsView(parent)
{
    // The scene container
    theScene = new QGraphicsScene(this);
    theScene->setBackgroundBrush(Qt::black);
    setScene(theScene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// Media player with video and audio output
    theMediaPlayer = new QMediaPlayer(this);
    theAudioOutput = new QAudioOutput(this);
    theMediaPlayer->setAudioOutput(theAudioOutput);

    theVideoItem = new QGraphicsVideoItem();
    theScene->addItem(theVideoItem);
    theVideoItem->setZValue(1);
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
    theCropRectItem = new CropRectangle(QRectF(0, 0, 300, 300));
    theCropRectItem->setColor(Qt::blue);
    theCropRectItem->setOpacity(50);
    theCropRectItem->setEnabled(false);
	theScene->addItem(theCropRectItem);

    theScene->addItem(theTimestampBgItem);
    theScene->addItem(theTimestampItem);

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

    resizeScene();
    repositionTimestamp();
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
    QSizeF sceneSize = size();
    theScene->setSceneRect(QRectF(QPointF(0, 0), sceneSize));

    if (!theVideoItem) return;

    QSizeF videoSize(640, 360);
    qreal scaleFactor;

    if (theAspectMode == AspectMode::FitInView) {
        scaleFactor = qMin(sceneSize.width() / videoSize.width(),
            sceneSize.height() / videoSize.height());
    }
    else {
        scaleFactor = qMax(sceneSize.width() / videoSize.width(),
            sceneSize.height() / videoSize.height());
    }

    QSizeF finalSize = videoSize * scaleFactor;

    QRectF rect(
        (sceneSize.width() - finalSize.width()) / 2,
        (sceneSize.height() - finalSize.height()) / 2,
        finalSize.width(),
        finalSize.height()
    );

    theVideoItem->setPos(rect.topLeft());
    theVideoItem->setSize(rect.size());
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
		theCropRectItem->setEnabled(enabled);
        emit CropEnabledChanged(enabled);
	}
}
