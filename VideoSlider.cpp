#include "VideoSlider.h"
#include <QStyleOptionSlider>
#include <QPainter>
#include <qdrawutil.h>
#include <QCommonStyle>

VideoSlider::VideoSlider(QWidget *parent)
    : QSlider{parent}
    , theMarkIn(-1)
    , theMarkOut(-1)
{

}

int VideoSlider::MarkIn()
{
    return theMarkIn;
}

void VideoSlider::setMarkIn(int mark)
{
    if (mark >= -1)
        theMarkIn = mark;
    else
        theMarkIn = -1;
    repaint();
}

int VideoSlider::MarkOut()
{
    return theMarkOut;
}

void VideoSlider::setMarkOut(int mark)
{
    if (mark >= -1)
        theMarkOut = mark;
    else
        theMarkOut = -1;
    repaint();
}

int VideoSlider::MarkInToPixels(int width)
{
    int video_length = maximum() - minimum();
    int pg = 0;
    if (theMarkIn >= 0)
    {
        double pm = (double)theMarkIn / (double)video_length;
        pg = (int)((double)width * pm);
    }
    return pg;
}

int VideoSlider::MarkOutToPixels(int width)
{
    int video_length = maximum() - minimum();
    int pg = width;
    if (theMarkOut >= 0)
    {
        double pm = (double)theMarkOut / (double)video_length;
        pg = (int)((double)width * pm);
    }
    return pg;
}

void VideoSlider::paintEvent(QPaintEvent *ev)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);

    opt.subControls = QStyle::SC_SliderGroove | QStyle::SC_SliderHandle;
    if (tickPosition() != NoTicks)
    {
        opt.subControls |= QStyle::SC_SliderTickmarks;
    }

    QPainter painter(this);
    QStyleOptionSlider *slider = &opt;
    int thickness = style()->proxy()->pixelMetric(QStyle::PM_SliderControlThickness, slider, this);
    int len = style()->proxy()->pixelMetric(QStyle::PM_SliderLength, slider, this);
    int ticks = slider->tickPosition;
    QRect groove = style()->proxy()->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderGroove, this);
    QRect handle = style()->proxy()->subControlRect(QStyle::CC_Slider, slider, QStyle::SC_SliderHandle, this);

    if ((slider->subControls & QStyle::SC_SliderGroove) && groove.isValid())
    {
        int mid = thickness / 2;

        if (ticks & QSlider::TicksAbove)
            mid += len / 8;
        if (ticks & QSlider::TicksBelow)
            mid -= len / 8;

        // paint regular groove
        //painter.setPen(slider->palette.shadow().color());
        painter.setPen(Qt::blue);
        qDrawWinPanel(&painter, groove.x(), groove.y() + mid - 3,
                       groove.width(), 6, slider->palette, true);
        painter.drawLine(groove.x() + 1, groove.y() + mid - 1,
                    groove.x() + groove.width() - 3, groove.y() + mid - 1);
        // paint highlighted groove between marks
        painter.setPen(Qt::yellow);
        qDrawWinPanel(&painter, groove.x() + MarkInToPixels(groove.width()), groove.y() + mid - 3,
                       MarkOutToPixels(groove.width()), 6, slider->palette, true);
        painter.drawLine(groove.x() + 1 + MarkInToPixels(groove.width()), groove.y() + mid - 1,
                    groove.x() + MarkOutToPixels(groove.width()) - 3, groove.y() + mid - 1);
        // paint markers themselves
        painter.fillRect(groove.x() + MarkInToPixels(groove.width()), groove.y() + mid - 5, 2, 8, Qt::white);
        painter.fillRect(groove.x() + MarkOutToPixels(groove.width()) - 2, groove.y() + mid - 5, 2, 8, Qt::white);
        painter.setPen(Qt::black);
        painter.drawRect(groove.x() + MarkInToPixels(groove.width()), groove.y() + mid - 5, 2, 8);
        painter.drawRect(groove.x() + MarkOutToPixels(groove.width()) - 2, groove.y() + mid - 5, 2, 8);
    }

    if (slider->subControls & QStyle::SC_SliderTickmarks)
    {
        QStyleOptionSlider tmpSlider = *slider;
        tmpSlider.subControls = QStyle::SC_SliderTickmarks;
        style()->drawComplexControl(QStyle::CC_Slider, &tmpSlider, &painter, this);
    }

    if (slider->subControls & QStyle::SC_SliderHandle)
    {
        const QColor c0 = slider->palette.shadow().color();
        const QColor c1 = slider->palette.dark().color();
        // const QColor c2 = g.button();
        const QColor c3 = slider->palette.midlight().color();
        const QColor c4 = slider->palette.light().color();
        QBrush handleBrush;

        if (slider->state & QStyle::State_Enabled)
        {
            handleBrush = slider->palette.color(QPalette::Button);
        }
        else
        {
            handleBrush = QBrush(slider->palette.color(QPalette::Button), Qt::Dense4Pattern);
        }

        int x = handle.x(), y = handle.y(), wi = handle.width(), he = handle.height();

        int x1 = x;
        int x2 = x+wi-1;
        int y1 = y;
        int y2 = y+he-1;

        Qt::Orientation orient = slider->orientation;
        bool tickAbove = slider->tickPosition == QSlider::TicksAbove;
        bool tickBelow = slider->tickPosition == QSlider::TicksBelow;

        if (slider->state & QStyle::State_HasFocus)
        {
            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*slider);
            fropt.rect = style()->subElementRect(QStyle::SE_SliderFocusRect, slider, this);
            style()->proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, &painter, this);
        }

        if ((tickAbove && tickBelow) || (!tickAbove && !tickBelow))
        {
            Qt::BGMode oldMode = painter.backgroundMode();
            painter.setBackgroundMode(Qt::OpaqueMode);
            qDrawWinButton(&painter, QRect(x, y, wi, he), slider->palette, false, &handleBrush);
            painter.setBackgroundMode(oldMode);
            return;
        }

        QPolygon a;
        int d = 0;
        y2 = y2 - wi/2;
        d =  (wi + 1) / 2 - 1;
        a.setPoints(5, x1,y1, x1,y2, x1+d,y2+d, x2,y2, x2,y1);

        QBrush oldBrush = painter.brush();
        painter.setPen(Qt::NoPen);
        painter.setBrush(handleBrush);
        Qt::BGMode oldMode = painter.backgroundMode();
        painter.setBackgroundMode(Qt::OpaqueMode);
        painter.drawRect(x1, y1, x2-x1+1, y2-y1+1);
        painter.drawPolygon(a);
        painter.setBrush(oldBrush);
        painter.setBackgroundMode(oldMode);

        painter.setPen(c4);
        painter.drawLine(x1, y1, x2, y1);
        painter.setPen(c3);
        painter.drawLine(x1, y1+1, x2, y1+1);
        painter.setPen(c3);
        painter.drawLine(x1+1, y1+1, x1+1, y2);
        painter.setPen(c4);
        painter.drawLine(x1, y1, x1, y2);
        painter.setPen(c0);
        painter.drawLine(x2, y1, x2, y2);
        painter.setPen(c1);
        painter.drawLine(x2-1, y1+1, x2-1, y2-1);

        painter.setPen(c4);
        painter.drawLine(x1, y2, x1+d, y2+d);
        painter.setPen(c0);
        d = wi - d - 1;
        painter.drawLine(x2, y2, x2-d, y2+d);
        d--;
        painter.setPen(c3);
        painter.drawLine(x1+1, y2, x1+1+d, y2+d);
        painter.setPen(c1);
        painter.drawLine(x2-1, y2, x2-1-d, y2+d);
    }

}
