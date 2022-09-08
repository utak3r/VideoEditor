#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>

class VideoSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(int MarkIn READ MarkIn WRITE setMarkIn NOTIFY MarkInChanged)
    Q_PROPERTY(int MarkOut READ MarkOut WRITE setMarkOut NOTIFY MarkOutChanged)

public:
    explicit VideoSlider(QWidget *parent = nullptr);

    int MarkIn();
    void setMarkIn(int mark);
    int MarkOut();
    void setMarkOut(int mark);

signals:
    void MarkInChanged(int newMark);
    void MarkOutChanged(int newMark);
    void MarksChanged(int markIn, int markOut);

protected:
    virtual void paintEvent(QPaintEvent *ev) override;

private:
    int MarkInToPixels(int width);
    int MarkOutToPixels(int width);

private:
    int theMarkIn;
    int theMarkOut;

};

#endif // VIDEOSLIDER_H
