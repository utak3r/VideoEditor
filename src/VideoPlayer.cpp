#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QPainter>
#include <QMouseEvent>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

VideoPlayer::VideoPlayer(QWidget* parent)
	: QGraphicsView(parent)
{
	//theScene = new QGraphicsScene(parent);
	//this->setScene(theScene);
}

VideoPlayer::~VideoPlayer()
{
}

bool VideoPlayer::initFFmpeg()
{
	bool inited = false;
	AVFormatContext* pFormatCtx = nullptr;
	avformat_open_input(&pFormatCtx, "", NULL, NULL);

	return inited;
}
