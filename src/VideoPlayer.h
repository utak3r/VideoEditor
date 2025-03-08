#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QGraphicsView>
#include <QGraphicsScene>

class VideoPlayer : public QGraphicsView
{
	Q_OBJECT
public:
	VideoPlayer(QWidget* parent = nullptr);
	~VideoPlayer();

protected:
	bool initFFmpeg();

private:
	QGraphicsView* theView;
	QGraphicsScene* theScene;
};

#endif // VIDEOPLAYER_H
